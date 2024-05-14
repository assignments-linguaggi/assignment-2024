#pragma once
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/IR/Use.h"
#include <set>
#include "llvm/IR/BasicBlock.h"

using namespace llvm;

struct LoopWalk : public PassInfoMixin<LoopWalk> {
    llvm::Loop* currentLoop;
    llvm::DominatorTree* DT;
    std::set<llvm::Instruction*> loopInvInstructions;
    bool isLoopInv(const llvm::Instruction&);
    bool isOperandLoopInv(const llvm::Use&);
    void analyze(llvm::Loop*);
    void findLoopInv();
    void displayLoopInvInstr();
    std::set<llvm::Instruction*> getLoopInvInstructions(llvm::BasicBlock*);

    PreservedAnalyses run(Loop&, LoopAnalysisManager&, LoopStandardAnalysisResults&, LPMUpdater&);
};
