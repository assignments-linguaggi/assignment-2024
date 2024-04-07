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


using namespace llvm;

bool runOnFunction(Function &F) {
  bool Transformed = false;
  int scelta = 1;
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
