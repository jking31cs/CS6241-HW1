#include "llvm/Pass.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
/*
 * This detects the number of edges of a CFG and prints it to errs.
 */
using namespace llvm;

namespace {
   
  struct CFGDetector : public FunctionPass  {
    static char ID;
    CFGDetector() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      int edgeCount = 0;
      for (Function::iterator bi=F.begin(), be = F.end(); bi != be; bi++) {
        BasicBlock *BB = bi;
	//This allows me to see all paths from BB to other Basic Blocks.
        for (succ_iterator pi = succ_begin(BB), e = succ_end(BB); pi != e; ++pi) {
          edgeCount++;
        }
      }
      errs() << "Function Name: " << F.getName() << "| CFG Edge Count: " << edgeCount << "\n";
      return false;
    }
  };
}

char CFGDetector::ID = 0;
static RegisterPass<CFGDetector> X("cfgDetector", "CFGDetector Pass", false, false);

