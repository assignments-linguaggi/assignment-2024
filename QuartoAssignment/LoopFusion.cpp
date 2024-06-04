#include "llvm/Transforms/Utils/LoopFusion.h"
#include <llvm/ADT/STLExtras.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Support/Casting.h>
#include <llvm/Transforms/Utils/LoopSimplify.h>
#include <vector>

using namespace llvm;
bool LoopFusion::areLoopsSequential(const Loop* firstLoop, const Loop* secondLoop) const {
    const BasicBlock* firstExitBlock = firstLoop->getExitBlock();
    const BasicBlock* secondPreheader = secondLoop->getLoopPreheader();
    if (firstExitBlock && secondPreheader && secondPreheader->size() == 1 && firstExitBlock == secondPreheader) {
        return true;
    }
    return false;
}

void LoopFusion::findSequentialLoops(const std::vector<Loop*>& loopList, std::vector<std::pair<Loop*, Loop*>>& sequentialLoopPairs) const {
    for (size_t i = 0; i < loopList.size(); ++i) {
        const std::vector<Loop*>& subLoops = loopList[i]->getSubLoopsVector();
        if (subLoops.size() > 1) {
            findSequentialLoops(subLoops, sequentialLoopPairs);
        }
    }

    for (size_t i = 1; i < loopList.size(); ++i) {
        Loop* previousLoop = loopList[i - 1];
        Loop* currentLoop = loopList[i];

        if (areLoopsSequential(previousLoop, currentLoop)) {
            outs() << "Found sequential loops:\n";
            outs() << " - Loop ";
            previousLoop->print(outs(), /*Verbose=*/false);
            outs() << "\n - Loop ";
            currentLoop->print(outs(), /*Verbose=*/false);
            outs() << "\n";
            sequentialLoopPairs.emplace_back(previousLoop, currentLoop);
        }
    }
}

bool LoopFusion::verifyControlFlow(DominatorTree& domTree, PostDominatorTree& postDomTree, const Loop* loop1, const Loop* loop2) const {
    return domTree.dominates(loop1->getHeader(), loop2->getHeader()) && postDomTree.dominates(loop2->getHeader(), loop1->getHeader());
}

PreservedAnalyses LoopFusion::run(Function& function, FunctionAnalysisManager& analysisManager) {
    outs() << "Running LoopFusion Pass\n";

    PostDominatorTree& postDomTree = analysisManager.getResult<PostDominatorTreeAnalysis>(function);
    DominatorTree& domTree = analysisManager.getResult<DominatorTreeAnalysis>(function);
    LoopInfo& loopInfo = analysisManager.getResult<LoopAnalysis>(function);

    const std::vector<Loop*>& topLevelLoops = loopInfo.getTopLevelLoops();
    std::vector<Loop*> reversedLoops(topLevelLoops.rbegin(), topLevelLoops.rend());
    std::vector<std::pair<Loop*, Loop*>> sequentialLoopPairs;

    findSequentialLoops(reversedLoops, sequentialLoopPairs);

    for (const auto& loopPair : sequentialLoopPairs) {
        Loop* loop1 = loopPair.first;
        Loop* loop2 = loopPair.second;
        bool equivalentControlFlow = verifyControlFlow(domTree, postDomTree, loop1, loop2);
        outs() << "Control Flow Equivalence check:\n";
        outs() << " - Loop ";
        loop1->print(outs(), /*Verbose=*/false);
        outs() << "\n - Loop ";
        loop2->print(outs(), /*Verbose=*/false);
        outs() << "\n";
        outs() << (equivalentControlFlow ? "Equivalent\n" : "Not Equivalent\n");
    }

    return PreservedAnalyses::none();
}