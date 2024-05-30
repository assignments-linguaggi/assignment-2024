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

bool LoopFusion::isAdjacencyRespected(const Loop* firstLoop, const Loop* secondLoop) const {
    const BasicBlock* firstLoopExitBlock = firstLoop->getExitBlock();
    const BasicBlock* secondLoopPreheader = secondLoop->getLoopPreheader();
    if (firstLoopExitBlock != nullptr && secondLoopPreheader != nullptr && secondLoopPreheader->size() == 1 && firstLoopExitBlock == secondLoopPreheader)
        return true;
    return false;   
}

void LoopFusion::findAdjacentLoops(const std::vector<Loop*>& loops, std::vector<std::pair<Loop*, Loop*>>& adjLoopPairs) const {
    std::vector<const std::vector<Loop*>*> subLoopsVec;

    for (const Loop* loop : loops) {
        const std::vector<Loop*>& subLoops = loop->getSubLoops();
        if (subLoops.size() > 1) {
            subLoopsVec.push_back(&subLoops);
        }
    }

    for (const auto* subLoops : subLoopsVec) {
        findAdjacentLoops(*subLoops, adjLoopPairs);
    }

    for (size_t i = 0; i+1 < loops.size(); i++) {
        Loop* firstLoop = loops[i];
        Loop* secondLoop = loops[i+1];
        if (isAdjacencyRespected(firstLoop, secondLoop)) {
            outs() << "Pair of adjacent loops detected:\n";
            
            outs() << "1st: ";
            firstLoop->print(outs(), false, false);
            outs() << "\n";

            outs() << "2nd: ";
            secondLoop->print(outs(), false, false);
            outs() << "\n";

            adjLoopPairs.emplace_back(firstLoop, secondLoop);
        }
    }
}

bool LoopFusion::isPairCFEquivalent(DominatorTree& domTree, PostDominatorTree& postDomTree, const Loop* firstLoop, const Loop* secondLoop) const {
    if (domTree.dominates(firstLoop->getHeader(), secondLoop->getHeader()) && postDomTree.dominates(secondLoop->getHeader(), firstLoop->getHeader()))
        return true;
    return false;
}

PreservedAnalyses LoopFusion::run(Function& function, FunctionAnalysisManager& fam) {
    LoopInfo& loopInfo = fam.getResult<LoopAnalysis>(function);
    DominatorTree& domTree = fam.getResult<DominatorTreeAnalysis>(function);
    PostDominatorTree& postDomTree = fam.getResult<PostDominatorTreeAnalysis>(function);
    const std::vector<Loop*>& topLevelLoops = loopInfo.getTopLevelLoops();
    std::vector<Loop*> topLevelLoopsInPreorder(topLevelLoops.rbegin(), topLevelLoops.rend());
    std::vector<std::pair<Loop*, Loop*>> adjacentLoops;

    findAdjacentLoops(topLevelLoops, adjacentLoops);

    for (const auto& pair : adjacentLoops) {
        Loop* firstLoop = pair.first;
        Loop* secondLoop = pair.second;
        bool cfe = isPairCFEquivalent(domTree, postDomTree, firstLoop, secondLoop);
        outs() << "Control Flow Equivalence for loops:\n";
        outs() << "1st: ";
        firstLoop->print(outs(), false, false);
        outs() << "\n";
        outs() << "2nd: ";
        secondLoop->print(outs(), false, false);
        outs() << "\n";
        outs() << (cfe ? "Equivalent\n" : "Not Equivalent\n");
    }

    return PreservedAnalyses::none();
}
