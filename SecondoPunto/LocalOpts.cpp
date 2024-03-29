#include "llvm/Transforms/Utils/LocalOpts.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

static bool ottimizzabileConSR(const Instruction& instr)
{
    return instr.getOpcode() == Instruction::Mul || instr.getOpcode() == Instruction::UDiv || instr.getOpcode() == Instruction::SDiv;
}

bool runOnBasicBlock(BasicBlock &B) {
    std::vector<std::pair<Instruction*, Value*>> istrDaCambiare; //vettore di coppie <istruzione / valore >
    bool Trasformato = false;

    for (auto& Inst : B) { //itero sulle istruzioni del BB
        if (ottimizzabileConSR(Inst)) {
            Value* primoOperando = Inst.getOperand(0);
            Value* secondoOperando = Inst.getOperand(1);
            
            //(es a = b * 5)
            Value* operando = primoOperando;
            ConstantInt* constante = dyn_cast<ConstantInt>(secondoOperando);
            //se il secondo operando non è costante scambio (es: a = 5 * b)
            if (constante == nullptr && Inst.isCommutative()) {
                operando = secondoOperando;
                constante = dyn_cast<ConstantInt>(primoOperando);
            }
            // se non ci sono costanti l'ottimizzazione SR non continua
            if (constante != nullptr) {
                APInt constVal = constante->getValue();
                //controllo diretto per potenze di due
                if (constVal.isPowerOf2()) {
                    IRBuilder<> builder(&Inst); 
                    ConstantInt* shiftAmount = ConstantInt::get(constante->getType(), constVal.logBase2());
                    Value* istrShift = nullptr;

                    if (Inst.getOpcode() == Instruction::Mul) {
                    // Se l'istruzione è una moltiplicazione, creiamo uno shift a sx
                        istrShift = builder.CreateShl(operando, shiftAmount, "shl"); 
                    } else {
                    // Se l'istruzione è una moltiplicazione, creiamo uno shift a dx
                        istrShift = builder.CreateAShr(operando, shiftAmount, "shr"); 
                    }
                    istrDaCambiare.push_back(std::make_pair(&Inst, istrShift));
                
                } else if (Inst.getOpcode() == Instruction::Mul) {
                    unsigned int nearestLog2Val = constVal.nearestLogBase2();
                    if (constVal == (1 << nearestLog2Val) - 1) {
                        //Ramo per costante uguale a potenza di due -1
                        IRBuilder<> builder(&Inst);
                        ConstantInt* shiftAmount = ConstantInt::get(constante->getType(), nearestLog2Val);
                        Value* istrShift = builder.CreateShl(operando, shiftAmount, "shl");
                        Value* istrSub = builder.CreateSub(istrShift, operando, "sub");
                        istrDaCambiare.push_back(std::make_pair(&Inst, istrSub));
                    } else if (constVal == (1 << nearestLog2Val) + 1) {
                        //Ramo per costante uguale a potenza di due + 1 
                        IRBuilder<> builder(&Inst);
                        ConstantInt* shiftAmount = ConstantInt::get(constante->getType(), nearestLog2Val);
                        Value* istrShift = builder.CreateShl(operando, shiftAmount, "shl");
                        Value* istrAdd = builder.CreateAdd(istrShift, operando, "add");
                        istrDaCambiare.push_back(std::make_pair(&Inst, istrAdd));
                    }
                }
            }
        }
    }
    //cambio tutte le istruzioni che ho salvato nel vector 
    for (auto& pair : istrDaCambiare) {
        Instruction* oldInst = pair.first;
        Value* newInst = pair.second;
        oldInst->replaceAllUsesWith(newInst);
        oldInst->eraseFromParent();
        Trasformato = true;
    }

    return Trasformato;
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