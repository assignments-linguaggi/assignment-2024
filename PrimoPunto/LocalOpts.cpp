#include "llvm/Transforms/Utils/LocalOpts.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"

using namespace llvm;

bool runOnBasicBlock(BasicBlock &B) {
    for (auto I = B.begin(), E = B.end(); I != E; ++I) { //Itero sulle istruzioni del BB
        Instruction *Inst = &(*I);
        if (BinaryOperator *op = dyn_cast<BinaryOperator>(Inst)) {
            //Controlli per add e molt
        if (((op->getOpcode() == Instruction::Add || op->getOpcode() == Instruction::Mul) && // Considera sia l'addizione che la moltiplicazione
            isa<ConstantInt>(op->getOperand(1)) && 
            cast<ConstantInt>(op->getOperand(1))->isZero()) 
            ||
            (isa<ConstantInt>(op->getOperand(0)) &&  // Controlla anche il primo operando
            cast<ConstantInt>(op->getOperand(0))->isZero())) { 
                op->replaceAllUsesWith(op->getOperand(0)); 
                op->eraseFromParent(); 
                return true;
        }
        }
    }
    return false;
}

bool runOnFunction(Function &F) {
    bool Trasformato = false;
    for (auto &BB : F) {
        if (runOnBasicBlock(BB)) {
            Trasformato = true;
        }
    }
    return Trasformato;
}

PreservedAnalyses LocalOpts::run(Module &M, ModuleAnalysisManager &AM) {
    bool Cambiato = false;
    for (auto &F : M) {
        if (runOnFunction(F)) {
            Cambiato = true;
        }
    }
    if (Cambiato) {
        return PreservedAnalyses::none();
    } else {
        return PreservedAnalyses::all();
    }
}
