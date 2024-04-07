#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/IR/Constants.h>
#include "llvm/IR/IRBuilder.h"
#include "cmath"
#include "vector"

using namespace llvm;


bool isAlgebraic(unsigned tipoistr){
  switch (tipoistr){
  case Instruction::Add :
  case Instruction::Sub :
  case Instruction::Mul :
  case Instruction::SDiv :  
    return true;
    break;
  default:
    return false;
    break;
  }

  return false;
}


//controllo che la seconda istruzione sia del "tipo" opposto della prima: add - sub; mul - Sdiv
bool isOpOpposta(unsigned primaInstr, unsigned secondaInstr){
  bool risposta = false;
  if(primaInstr == Instruction::Add && secondaInstr == Instruction::Sub)
    risposta = true;
  if(primaInstr == Instruction::Sub && secondaInstr == Instruction::Add)
    risposta = true;
  if(primaInstr == Instruction::Mul && secondaInstr == Instruction::SDiv)
    risposta = true;
  if(primaInstr == Instruction::SDiv && secondaInstr == Instruction::Mul)
    risposta = true;
  
  return risposta;
}

bool runOnBasicBlockMulti(BasicBlock &B) {

    std::vector<Instruction*> daEliminare;
    int COnst1, numeroConst, valoreIntero, Const2;
    numeroConst = 0;
    Value* operatore0;
    Value* operatore1;
    Value* opRegistro; //puntatore al registro utilizzato nelle istruzioni
    unsigned tipoistr;
    

    for (Instruction &iter : B){
      COnst1 = -1;
      numeroConst = 0; //indica se c'è un numero e un operando
      tipoistr = iter.getOpcode();
      //Controllo elemento neutro somma/sottrazione
      if(isAlgebraic(tipoistr)){

          operatore0 = iter.getOperand(0);
          operatore1 = iter.getOperand(1);

          //Controllo gli operandi, seleziono il valore numerico se c'è e preparo un puntatore
          //opRegistro al registro da utilizzare dopo se un operatore è un numero e l'altro è un registro
          //trovo l'operando che è una costante
          if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(0))){ //è operando 0 una costante?
            COnst1=0;
            valoreIntero= C->getSExtValue(); //long int o unsigned int  
            opRegistro=operatore1; // puntatore alla variabile-registro andando a esclusione
            numeroConst++;
          }else if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(1))){
            COnst1=1;
            valoreIntero= C->getSExtValue();
            opRegistro=operatore0;
            numeroConst++;
          }
        
        //Controllo ci sia una e una variabile e una costante altrimenti non faccio nulla
        if(numeroConst != 1)
          continue;
        else{
          //Controllo tutti i suoi utilizzatori
          if(Instruction* I = &iter){
            for(auto uso = I->user_begin(); uso != I->user_end(); ++uso){
              //for per prendere gli utilizzatori, Usando Instruction mi da errore perchè user è un value
              //se è così allora posso propagare opRegistro al posto del valore del nuovo utilizzatore

              //a=b+1, c=a-1 > a=b+1; c=b che verrà sucessivamente eliminata
              Const2=-1;
              //controllo che sia l'operazione opposta all'istruzione
              if(Instruction *userInst = dyn_cast<Instruction>(*uso)){
                /*Devo controllare se l'istruzione ha un operando numerico, e nel caso se è lo stesso
                  intero dell'uso di riferimento*/
                if(isOpOpposta(I->getOpcode(),userInst->getOpcode())){
                  //se sono operazione opposte allora controllo gli operandi
                  if(ConstantInt *C = dyn_cast<ConstantInt>(userInst->getOperand(0))){
                    Const2=0;
                  }else if(ConstantInt *C = dyn_cast<ConstantInt>(userInst->getOperand(1))){
                    Const2=1;
                  }
                  if(Const2 != -1){
                    //se costante della prima istruzione analizzata è uguale nella operazione opposta del suo utilizzatore allora replace
                    if(I->getOperand(COnst1) == userInst->getOperand(Const2)){

                      userInst->replaceAllUsesWith(opRegistro);
                      daEliminare.push_back(userInst);
                    }
                  }

                }
              }
            }
          }
        }
      }
    }

    for(Instruction *i: daEliminare)
      i->eraseFromParent();

    return true;
}
