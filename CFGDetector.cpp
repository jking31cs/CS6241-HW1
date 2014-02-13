#include "llvm/Pass.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include <limits>

/*
 * This detects the number of edges of a CFG and prints it to errs.
 */
using namespace llvm;

namespace {
   
  struct CFGDetector : public ModulePass  {
    static char ID;
    CFGDetector() : ModulePass(ID) {}

    virtual bool runOnModule(Module &M) {
      int max = 0; 
      int min = std::numeric_limits<int>::max();
      double total = 0;
      for (Module::iterator F = M.begin(); F != M.end(); F++) {
        int count = edgeCount(F);
        total += count;
        if (max < count) { max = count; }
        if (min > count) { min = count; }
      }
      errs() << "Min edge count for function: " << min << '\n';
      errs() << "Max edge count for function: " << max << '\n';
      errs() << "Average edge count per function: " << total / M.size() << '\n';
      
      return false;
    }

    int edgeCount (Function *F) {
      int edgeCount = 0;
      for (Function::iterator bi=F->begin(), be = F->end(); bi != be; bi++) {
        BasicBlock *BB = bi;
	//This allows me to see all paths from BB to other Basic Blocks.
        for (succ_iterator pi = succ_begin(BB), e = succ_end(BB); pi != e; ++pi) {
          edgeCount++;
        }
      }
      return edgeCount;
    }
  };
}

char CFGDetector::ID = 0;
static RegisterPass<CFGDetector> X("cfgDetector", "CFGDetector Pass", false, false);

