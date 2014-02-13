#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include <limits>
/*
 * This file runs a pass that detects the number of basic blocks in the system.
 */
using namespace llvm;

namespace {
  struct BasicBlockDetector : public ModulePass  {
    static char ID;
    BasicBlockDetector() : ModulePass(ID) {}

    virtual bool runOnModule(Module &M) {
      Module::iterator F = M.begin(), E = M.end();
      int min = std::numeric_limits<int>::max();
      int max = 0;
      int total = 0;
      for (; F != E; F++) {
        int numBlocks = F->size();
        total += numBlocks;
        if (max < numBlocks) { max = numBlocks; }
        if (min > numBlocks) { min = numBlocks; }
      } 
      double average = total / M.size();
      errs() << "Min Blocks in function: " << min << '\n';
      errs() << "Max Blocks in function: " << max << '\n';
      errs() << "Average Blocks per Function: " << average << '\n';
      return false;
    }

    void getLoopInfo(Function *F) const {
      errs() << F->getName() << " # of BasicBlocks: " << F->size() << '\n';
    };
    
  };
}

char BasicBlockDetector::ID = 0;
static RegisterPass<BasicBlockDetector> X("basicBlockDetector", "BasicBlockDetector Pass", false, false);

