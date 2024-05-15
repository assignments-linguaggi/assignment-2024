#pragma once
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/IR/Use.h"
#include <set>
#include "llvm/IR/BasicBlock.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include <llvm/IR/Dominators.h>





using namespace llvm;

struct LoopLICM : public PassInfoMixin<LoopLICM> {
    llvm::Loop* currentLoop;
    llvm::DominatorTree* DT;
    std::set<llvm::Instruction*> loopInvInstructions;
    bool isLoopInv(const llvm::Instruction&);
    bool isOperandLoopInv(const llvm::Use&);
    void analyze(llvm::Loop*);
    void findLoopInv();
    void displayLoopInvInstr();
    std::set<llvm::Instruction*> getLoopInvInstructions(llvm::BasicBlock*);
    void moveLoopInvariantInstructions();
    void applyCodeMotion();

    PreservedAnalyses run(Loop&, LoopAnalysisManager&, LoopStandardAnalysisResults&, LPMUpdater&);
};
