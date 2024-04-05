#include "llvm/Transforms/Utils/LocalOpts.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/IR/Constants.h>
#include "llvm/IR/IRBuilder.h"
#include "cmath"
#include "vector"


using namespace llvm;

bool isAlgebric(unsigned tipoistr){
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
      if(isAlgebric(tipoistr)){

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

bool runOnBasicBlockAlgebra(BasicBlock &B) {

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

bool runOnFunction(Function &F) {
  bool Transformed = false;
  int scelta = 2;
  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    switch(scelta){
      case(1):
        if (runOnBasicBlockMulti(*Iter)) 
          Transformed = true;
        break;
      case(2):
        if (runOnBasicBlockAlgebra(*Iter)) 
          Transformed = true;
        break;
      /*case(3):
        if (runOnBasicBlockStrenght(*Iter)) 
          Transformed = true;
        break;*/
    }
  }

  return Transformed;
}

PreservedAnalyses LocalOpts::run(Module &M,
                                      ModuleAnalysisManager &AM) {
  for (auto Fiter = M.begin(); Fiter != M.end(); ++Fiter)
    if (runOnFunction(*Fiter))
      return PreservedAnalyses::none();
  
  return PreservedAnalyses::all();
}

