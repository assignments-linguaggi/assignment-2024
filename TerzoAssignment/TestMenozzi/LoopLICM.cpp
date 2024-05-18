#include  "llvm/Transforms/Utils/LoopLICM.h"


using namespace llvm;

void LoopLICM::displayLoopInvInstr() {
    outs() << "Istruzioni loop invariant:\n";
    for (auto* instruction : loopInvInstructions) {
        outs() << *instruction << " (" << static_cast<void*>(instruction) << ")\n";
    }
}

void LoopLICM::findLoopInv() {
    for (auto* bb : this->currentLoop->getBlocks()) {
        std::set<Instruction*> bbLoopInvInstructions = getLoopInvInstructions(bb);
        this->loopInvInstructions.insert(bbLoopInvInstructions.begin(), bbLoopInvInstructions.end());
    }
}

std::set<Instruction*> LoopLICM::getLoopInvInstructions(BasicBlock* bb) {
    std::set<Instruction*> loopInvInstructions;
    for (auto& instr : *bb) {
        if (instr.isBinaryOp() && isLoopInv(instr)) {
            loopInvInstructions.insert(&instr);
        }
    }
    return loopInvInstructions;
}

bool LoopLICM::isLoopInv(const Instruction& instr) {
    for (const Use& operand : instr.operands()) {
        if (!isOperandLoopInv(operand)) {
            return false;
        }
    }
    return true;
}

bool LoopLICM::isOperandLoopInv(const Use& operand) {
    Value* reachingDefValue = operand.get();
    if (isa<Constant>(reachingDefValue) || isa<Argument>(reachingDefValue)) {
        return true;
    }
    if (Instruction* operandDefInstruction = dyn_cast<Instruction>(reachingDefValue)) {
        return !this->currentLoop->contains(operandDefInstruction) || this->loopInvInstructions.count(operandDefInstruction) > 0;
    }
    return false;
}

void LoopLICM::applyCodeMotion() {
    BasicBlock *preheader = this->currentLoop->getLoopPreheader();
    if (!preheader) {
        LLVMContext &Ctx = this->currentLoop->getHeader()->getContext();
        preheader = BasicBlock::Create(Ctx, "preheader", this->currentLoop->getHeader()->getParent(), this->currentLoop->getHeader());
        BranchInst::Create(this->currentLoop->getHeader(), preheader);
    }

    std::set<Instruction*> movedInstructions;
    std::set<Value*> assignedValues;
    for (BasicBlock *BB : this->currentLoop->blocks()) {
        for (Instruction &I : *BB) {
            if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
                assignedValues.insert(SI->getPointerOperand());
            }
        }
    }

    for (Instruction *instr : loopInvInstructions) {
        bool assignsToUnassigned = false;
        for (Use &operand : instr->operands()) {
            if (assignedValues.find(operand.get()) == assignedValues.end()) {
                assignsToUnassigned = true;
                break;
            }
        }

        bool dominatesExits = true;
        SmallVector<BasicBlock*, 8> ExitBlocks;
        this->currentLoop->getExitBlocks(ExitBlocks);
        for (BasicBlock *exitBlock : ExitBlocks) {
            if (!this->DT->dominates(instr->getParent(), exitBlock)) {
                dominatesExits = false;
                break;
            }
        }

        bool dominatesUsers = true;
        for (User *user : instr->users()) {
            if (Instruction *userInstr = dyn_cast<Instruction>(user)) {
                BasicBlock *userBlock = userInstr->getParent();
                if (userBlock && !this->DT->dominates(instr->getParent(), userBlock)) {
                    dominatesUsers = false;
                    break;
                }
            }
        }

        bool isDeadAtExit = true;
        for (User *user : instr->users()) {
            if (Instruction *userInstr = dyn_cast<Instruction>(user)) {
                if (this->currentLoop->contains(userInstr)) {
                    isDeadAtExit = false;
                    break;
                }
            }
        }

        if ((assignsToUnassigned && (dominatesExits || isDeadAtExit)) && dominatesUsers) {
            instr->moveBefore(preheader->getTerminator());
            movedInstructions.insert(instr);
        }
    }

    outs() << "Istruzioni loop invariant spostate al preheader:\n";
    for (auto* instruction : movedInstructions) {
        outs() << *instruction << " (" << static_cast<void*>(instruction) << ")\n";
    }

    for (Instruction *instr : movedInstructions) {
        loopInvInstructions.erase(instr);
    }

    outs() << "Code motion applicata alle loop invariant instruction.\n";
}

PreservedAnalyses LoopLICM::run(Loop &l, LoopAnalysisManager &lam, LoopStandardAnalysisResults &lar, LPMUpdater &lu) {
    this->currentLoop = &l;
    this->DT = &lar.DT;

    findLoopInv();
    displayLoopInvInstr();
    applyCodeMotion();
    return PreservedAnalyses::all();
}
