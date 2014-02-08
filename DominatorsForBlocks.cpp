#include "llvm/Pass.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

  struct DominatorDetector : public FunctionPass  {
    static char ID;
    DominatorDetector() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      functionName(&F);
      getInfo(F);
      return false;
    }

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
    }

    void getInfo(const Function& F) const {
      DominatorTree *DT = &getAnalysis<DominatorTree>();
      Function::const_iterator BB1 = F.begin(), E1 = F.end();
      Function::const_iterator BB2 = F.begin(), E2 = F.end();
      for(; BB1 != E1; ++BB1) {
        int dominateCount = 0;
        for(; BB2 != E2; ++BB2) {
           if (DT->dominates(&*BB1, &*BB2)) dominateCount++;
        }
        errs() << BB1->getName() << " dominates " << dominateCount << '\n';
      }
    };

    bool functionName(Function *F) {
      errs().write_escaped(F->getName()) << '\n';
      return false;
    }
  };
}

char DominatorDetector::ID = 0;
static RegisterPass<DominatorDetector> X("dominatorDetector", "DominatorDetector Pass", false, false);
