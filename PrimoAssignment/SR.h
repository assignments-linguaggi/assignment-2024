#ifndef SR_H
#define SR_H

#include "llvm/IR/BasicBlock.h"

using namespace llvm;

bool runOnBasicBlockSR(BasicBlock &B);

#endif // SR_H

