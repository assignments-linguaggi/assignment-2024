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

//controllo che l'istruzione sia una moltiplicazione o una divisione (signed/unsigned)
bool ottimizzabileConSR(const Instruction& instr) 
{
    return instr.getOpcode() == Instruction::Mul || instr.getOpcode() == Instruction::UDiv || instr.getOpcode() == Instruction::SDiv;
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

bool runOnBasicBlockSR(BasicBlock &B) {
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
  int scelta = 3;
  for (auto &BB : F) {
    switch(scelta){
      case(1):
        if (runOnBasicBlockMulti(BB)) 
          Transformed = true;
        break;
      case(2):
        if (runOnBasicBlockAlgebra(BB)) 
          Transformed = true;
        break;
      case(3):
        if (runOnBasicBlockSR(BB)) 
          Transformed = true;
        break;
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
