#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include <list>
#include <set>
using namespace llvm;
/*
 * This detects total number of loops, total number of outer loops, and number of back edges via 
 * the LoopInfo Analysis.
 */
namespace {

  class Edge {
    public:
      BasicBlock *U;
      BasicBlock *V;
      Edge(BasicBlock*, BasicBlock*);
    
  };

  Edge::Edge(BasicBlock *U, BasicBlock *V) {
    this->U = U;
    this->V = V;
  }
  struct BackEdgeLoopDetector : public FunctionPass  {
    static char ID;
    BackEdgeLoopDetector() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      functionName(&F);
      std::list<Edge> edgeList = getLoopInfo(&F);
      std::set<BasicBlock*> visited;
      std::list<BasicBlock*> queue;
      int numBackEdges = 0;
      queue.push_back(F.begin());
      /*
       * The idea here is to start at the root, add to the queue
       * each node we approach in the graph, and if we re-approach
       * a visited node, we have a loop.
       */
      while (queue.size() > 0) {
        BasicBlock *b = queue.front();
        std::list<Edge>::iterator edge_it = edgeList.begin(), edge_end = edgeList.end();
        for (; edge_it != edge_end; ++edge_it) {
          if (edge_it->U == b) {
            if (visited.find(edge_it->V) != visited.end()) {
              numBackEdges++;
            } else { //To Prevent infinite loop, put this in else.
              queue.push_back(edge_it->V);
            }
          }
        }
        visited.insert(b);
        queue.pop_front();        
      }
      errs() << '\t' << "Number of Loops based on Back Edges: " << numBackEdges << '\n'; 
      return false;
    }

    std::list<Edge> getLoopInfo(Function *F) {
      std::list<Edge> edgeList;
      for (Function::iterator bi = F->begin(), be = F->end(); bi != be; bi++) {
        BasicBlock *BB = bi;
        //This allows me to see all paths from BB to other Basic Blocks.
        for (succ_iterator pi = succ_begin(BB), e = succ_end(BB); pi != e; ++pi) {
          Edge E(BB, *pi);
          edgeList.push_back(E);
        }
      }
      return edgeList;
    };

    bool functionName(Function *F) {
      errs().write_escaped(F->getName()) << '\n';
      return false;
    }
  };

  char BackEdgeLoopDetector::ID = 0;
  static RegisterPass<BackEdgeLoopDetector> X("loop-backedge", "Loop Detection via Back edge", false, false);
}
