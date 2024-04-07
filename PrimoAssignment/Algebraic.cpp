#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/IR/Constants.h>
#include "llvm/IR/IRBuilder.h"
#include "cmath"
#include "vector"

using namespace llvm;

bool runOnBasicBlockAlgebraic(BasicBlock &B) {

   std::vector<Instruction*> daEliminare;
    int ElementoNeutro;
    for (Instruction &iter : B){
      ElementoNeutro = 2;
      //Controllo elemento neutro somma
        if(iter.getOpcode() == Instruction::Add){ //se è una somma
            if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(0))){ //è una costante, se si è zero? primo operando
              if(C->getValue().isZero()){
                ElementoNeutro=0;
              }
            }else if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(1))){//è una costante, se si è zero? secondo operando
              if(C->getValue().isZero()){
                ElementoNeutro=1;
              }
            }
          }
          if(iter.getOpcode() == Instruction::Mul){//analogo a prima ma con la moltiplicazione
            if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(0))){//è una costante, se si è zero? primo operando
              if(C->getValue().isOne()){
                ElementoNeutro=0;
              }
            }else if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(1))){//è una costante, se si è zero? secondo operando
              if(C->getValue().isOne()){
                ElementoNeutro=1;
              }
            }
          }
          if(ElementoNeutro != 2){
            iter.replaceAllUsesWith(iter.getOperand(1-ElementoNeutro)); //rimpiazzo con i suoi users l'indirizzo in questione
            daEliminare.push_back(&iter);
            //printf("Sostituzione avvenuta con successo\n");
          }

    };
    for(Instruction *i: daEliminare)
      i->eraseFromParent();

  return true;
}

