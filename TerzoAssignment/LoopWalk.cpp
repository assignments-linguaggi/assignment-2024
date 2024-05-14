
#include  "llvm/Transforms/Utils/LoopWalk.h"

using namespace llvm;



void LoopWalk::displayLoopInvInstr() {

    outs() << "Istruzioni loop invariant:\n";

    for (auto* instruction : loopInvInstructions)
    {
        outs() << *instruction << '\n';
    }

}

void LoopWalk::findLoopInv() {
    for (auto* bb : this->currentLoop->getBlocks()) {
        std::set<Instruction*> bbLoopInvInstructions = getLoopInvInstructions(bb);
        this->loopInvInstructions.insert(bbLoopInvInstructions.begin(), bbLoopInvInstructions.end());
    }
}

std::set<Instruction*> LoopWalk::getLoopInvInstructions(BasicBlock* bb) {
    std::set<Instruction*> loopInvInstructions;

    for (auto& instr : *bb) {
        if (instr.isBinaryOp() && isLoopInv(instr)) {
            loopInvInstructions.insert(&instr);
        }
    }

    return loopInvInstructions;
}

bool LoopWalk::isLoopInv(const Instruction& instr) {
    for (const Use& operand : instr.operands()) {
        if (!isOperandLoopInv(operand)) {
            return false;
        }
    }
    return true;
}

bool LoopWalk::isOperandLoopInv(const Use& operand) {
    Value* reachingDefValue = operand.get();

    if (isa<Constant>(reachingDefValue) || isa<Argument>(reachingDefValue)) {
        return true;
    }
    
    if (Instruction* operandDefInstruction = dyn_cast<Instruction>(reachingDefValue)) {
        return !this->currentLoop->contains(operandDefInstruction) || this->loopInvInstructions.count(operandDefInstruction) > 0;
    }

    return false;
}


PreservedAnalyses LoopWalk::run(Loop &l, LoopAnalysisManager &lam, LoopStandardAnalysisResults &lar, LPMUpdater &lu) {

    this->currentLoop = &l;

    findLoopInv();
    displayLoopInvInstr();

  return PreservedAnalyses::all();
}
