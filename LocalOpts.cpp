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
            //Controlli per addizione e moltiplicazione
            if ((op->getOpcode() == Instruction::Add && //somma 
                isa<ConstantInt>(op->getOperand(1)) && //verifica che il secondo operando sia una costante (il template isa<> è come in java instanceof)
                cast<ConstantInt>(op->getOperand(1))->isZero()) //verifica che sia uguale a 0
                || 
                (op->getOpcode() == Instruction::Mul && //mult
                isa<ConstantInt>(op->getOperand(1)) && 
                cast<ConstantInt>(op->getOperand(1))->isOne())) { //verifica che sia uguale a 1
                    op->replaceAllUsesWith(op->getOperand(0)); //rimpiazzo gli usi dell'istruzione con il primo operando
                    op->eraseFromParent(); // rimuovo l'istruzione dal BB
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