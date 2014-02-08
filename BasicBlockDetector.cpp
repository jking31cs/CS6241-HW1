#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
/*
 * This file runs a pass that detects the number of basic blocks in the system.
 */
using namespace llvm;

namespace {
  struct BasicBlockDetector : public LoopInfo  {
    static char ID;
    BasicBlockDetector() : LoopInfo() {}

    virtual bool runOnFunction(Function &F) {
      getLoopInfo(F); 
      return false;
    }

    void getLoopInfo(const Function& F) const {
      errs() << F.getName() << " # of BasicBlocks: " << F.size() << '\n';
    };
    
  };
}

char BasicBlockDetector::ID = 0;
static RegisterPass<BasicBlockDetector> X("basicBlockDetector", "BasicBlockDetector Pass", false, false);

