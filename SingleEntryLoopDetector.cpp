#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
/**
 * This finds all single entry loops by checking purely for back edges.
 */
namespace {
  struct SingleEntryLoopDetector : public LoopInfo  {
    static char ID;
    SingleEntryLoopDetector() : LoopInfo() {}

    virtual bool runOnFunction(Function &F) {
      functionName(&F);
      getLoopInfo(F); 
      return false;
    }

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
      AU.addRequired<LoopInfo>();
    }

    void getLoopInfo(const Function& F) const {
      const LoopInfo *LI = &getAnalysis<LoopInfo>();
      Function::const_iterator BB = F.begin(), E = F.end();
      int singleEntryLoops = 0;
      for(; BB != E; ++BB) {
        int curLevel = LI->getLoopDepth(&*BB);
        for (succ_const_iterator child = succ_begin(BB), e = succ_end(BB); child != e; ++child) {
           int childLevel = LI->getLoopDepth(*child);
           if (curLevel > childLevel) {
             singleEntryLoops++;
           }
	}
        
      }
      errs() << '\t' << F.getName() << " # of SingleEntryLoops: " << singleEntryLoops << '\n';
    };

    bool functionName(Function *F) {
      errs().write_escaped(F->getName()) << '\n';
      return false;
    }
  };
}

char SingleEntryLoopDetector::ID = 0;
static RegisterPass<SingleEntryLoopDetector> X("singleEntryloopDetector", "SingleEntryLoopDetector Pass", false, false);

