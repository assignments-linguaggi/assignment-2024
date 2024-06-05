#pragma once

#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Transforms/Scalar/LoopRotation.h>
#include "llvm/Analysis/DependenceAnalysis.h"
#include <llvm/Analysis/DependenceAnalysis.h>

namespace llvm {

class LoopFusion : public PassInfoMixin<LoopFusion> {
public:
    bool areLoopsSequential(const Loop*, const Loop*) const;
    void findSequentialLoops(const std::vector<Loop*>& loops, std::vector<std::pair<Loop*, Loop*>>& seqLoopPairs) const;
    bool verifyControlFlow(DominatorTree& DT, PostDominatorTree& PT, const Loop* loop1, const Loop* loop2) const;
    bool haveSameIterationCount(const Loop* loop1, const Loop* loop2, ScalarEvolution &SE) const;
    bool noNegativeDistanceDependencies(const Loop* loop1, const Loop* loop2, DependenceInfo &DI) const;
    void performLoopFusion(Loop* firstLoop, Loop* secondLoop);
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // namespace llvm
