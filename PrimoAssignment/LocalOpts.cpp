#include "llvm/Transforms/Utils/Algebraic.h"
#include "llvm/Transforms/Utils/SR.h"
#include "llvm/Transforms/Utils/MultiInstr.h"
#include "llvm/Transforms/Utils/LocalOpts.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/IR/Constants.h>
#include "llvm/IR/IRBuilder.h"
#include "cmath"
#include "vector"
#include <iostream>

using namespace llvm;

void printOptTable () {
    std::cout << "Numero\tOttimizzazione\n\n";
    
    std::cout << "1\tAlgebraic Identity\n";
    std::cout << "2\tStrength reduction\n";
    std::cout << "3\tMulti Instruction\n\n";
    
}

bool runOnFunction(Function &F) {
  bool Transformed = false;
  int scelta = 0;
  printOptTable();
  std::cin >> scelta;
  for (auto &BB : F) {
    switch(scelta){
      case(1):
        if (runOnBasicBlockAlgebraic(BB)) 
          Transformed = true;
        break;
      case(2):
        if (runOnBasicBlockSR(BB)) 
          Transformed = true;
        break;
      case(3):
        if (runOnBasicBlockMulti(BB)) 
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
