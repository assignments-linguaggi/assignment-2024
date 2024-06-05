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
#include <llvm/Analysis/DependenceAnalysis.h>
#include <llvm/Analysis/ScalarEvolutionExpressions.h>
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

bool LoopFusion::haveSameIterationCount(const Loop *L1, const Loop *L2, ScalarEvolution &SE) const {
    // Ottieni i conteggi delle iterazioni per i loop
    auto loop1TripCount = SE.getSmallConstantTripCount(L1);
    auto loop2TripCount = SE.getSmallConstantTripCount(L2);
    outs() << "Loop1 Trip Count: " << loop1TripCount << "\n";   //Una iterazione in più perchè conta la iterazione di uscita dei loop
    outs() << "Loop2 Trip Count: " << loop2TripCount << "\n";

    // Verifica se i conteggi delle iterazioni sono uguali
    if (loop1TripCount != loop2TripCount) {
        outs() << "Iteration Count Different\n";
        return false;
    }

    outs() << "Trip Count Checked\n";
    return true;
}



bool LoopFusion::noNegativeDistanceDependencies(const Loop* loop1, const Loop* loop2, DependenceInfo &DI) const{
    for (auto *BB1 : loop1->getBlocks()) {
        for (auto &I1 : *BB1) {
            for (auto *BB2 : loop2->getBlocks()) {
                for (auto &I2 : *BB2) {
                    if (auto DepResult = DI.depends(&I1, &I2, true)) {
                        if (DepResult->isLoopIndependent()) continue;

                        // Check for flow, output, or anti-dependence
                        if (DepResult->isFlow() || DepResult->isOutput() || DepResult->isAnti()) {
                            // Get the distance between the instructions
                            const SCEV *Distance = DepResult->getDistance(1);
                            if (!Distance) continue;

                            // Check if the distance is non-negative
                            if (const SCEVConstant *C = dyn_cast<SCEVConstant>(Distance)) {
                                const APInt& Val = C->getAPInt();
                                if (Val.isNegative()) {
                                    return false; // Negative distance dependency found
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true; // No negative distance dependencies found
}

void LoopFusion::performLoopFusion(Loop* firstLoop, Loop* secondLoop) {
    // 1. Modifica degli usi della variabile di induzione
    Value* firstIndVar = firstLoop->getCanonicalInductionVariable();
    Value* secondIndVar = secondLoop->getCanonicalInductionVariable();
    
    for (auto& U : secondIndVar->uses()) {
        if (auto* User = dyn_cast<Instruction>(U.getUser())) {
            User->replaceUsesOfWith(secondIndVar, firstIndVar);
        }
    }

    // 2. Modifica del CFG
    BasicBlock* firstExitBlock = firstLoop->getExitBlock();
    BasicBlock* secondPreheader = secondLoop->getLoopPreheader();
    BasicBlock* secondHeader = secondLoop->getHeader();
    
    BranchInst* secondLatchBranch = dyn_cast<BranchInst>(secondHeader->getTerminator());
    if (secondLatchBranch && secondLatchBranch->isUnconditional()) {
        BasicBlock* secondLatchTarget = secondLatchBranch->getSuccessor(0);
        
        // Rimuovi il salto all'uscita del primo loop
        firstExitBlock->removePredecessor(secondLatchTarget);

        // Rimuovi l'header del secondo loop dai predecessori dell'uscita del primo loop
        secondPreheader->getTerminator()->eraseFromParent();
        secondPreheader->getTerminator()->removeFromParent();
        secondPreheader->replaceSuccessorsPhiUsesWith(secondHeader, firstExitBlock);

        // Collega il corpo del secondo loop al corpo del primo loop
        secondLatchBranch->setSuccessor(0, firstExitBlock);
        BranchInst::Create(firstExitBlock, secondHeader);
    }
}


PreservedAnalyses LoopFusion::run(Function& function, FunctionAnalysisManager& analysisManager) {
    outs() << "Running LoopFusion Pass\n";
    PostDominatorTree& postDomTree = analysisManager.getResult<PostDominatorTreeAnalysis>(function);
    DominatorTree& domTree = analysisManager.getResult<DominatorTreeAnalysis>(function);
    LoopInfo& loopInfo = analysisManager.getResult<LoopAnalysis>(function);
    ScalarEvolution& scalarEvolution = analysisManager.getResult<ScalarEvolutionAnalysis>(function);
    DependenceInfo& dependenceInfo = analysisManager.getResult<DependenceAnalysis>(function);

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

        if (!equivalentControlFlow) continue;

        bool sameIterationCount = haveSameIterationCount(loop1, loop2, scalarEvolution);
        outs() << "Iteration Count check:\n";
        outs() << (sameIterationCount ? "Same\n" : "Different\n");

        if (!sameIterationCount) continue;

        bool noNegDependencies = noNegativeDistanceDependencies(loop1, loop2, dependenceInfo);
        outs() << "Negative Distance Dependencies check:\n";
        outs() << (noNegDependencies ? "None\n" : "Found\n");

        if (!noNegDependencies) continue;

        // If all checks pass, perform the fusion
        outs() << "Loops can be fused:\n";
        outs() << " - Loop ";
        loop1->print(outs(), /*Verbose=*/false);
        outs() << "\n - Loop ";
        loop2->print(outs(), /*Verbose=*/false);
        outs() << "\n";

        performLoopFusion(loop1, loop2);

        // Aggiungi un output di debug per vedere il nuovo loop dopo la fusione
        outs() << "New loop after fusion:\n";
        loop1->print(outs(), /*Verbose=*/true);
        outs() << "\n";
    }

    return PreservedAnalyses::all();
}
