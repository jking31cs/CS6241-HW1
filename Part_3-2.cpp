#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include <list>
#include <set>
#include <limits>
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

  class Path {
    public:
      BasicBlock *U;
      BasicBlock *V;
      int dist;
      Path(BasicBlock*, BasicBlock*);
      bool operator==(const Path& that) { return this->U == that.U && this->V == that.V; }
  };

  Path::Path(BasicBlock *U, BasicBlock *V) {
    this->U = U;
    this->V = V;
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
  }

  bool functionName(Function *F) {
    errs().write_escaped(F->getName()) << '\n';
    return false;
  }

  struct WarshallLoopDetector : public FunctionPass {
    static char ID;
    WarshallLoopDetector() : FunctionPass(ID) {}
    
    virtual bool runOnFunction(Function &F) {
      functionName(&F);
      std::list<Edge> edgeList = getLoopInfo(&F);
      std::list<Path*> paths;
      //Initialize the distance between any 2 points to be at max, 0 for distance to itself.
      errs() << "checkpoint";
      for (Function::iterator it1 = F.begin(), e1 = F.end(); it1 != e1; it1++) {
        errs() << "hi";
        for (Function::iterator it2 = F.begin(), e2 = F.end(); it2 != e2; it2++) {
          Path path(it1, it2);
          path.dist = std::numeric_limits<int>::max();
          errs() << "hello";
          paths.push_front(&path);
        }
      } 
      for (Function::iterator it1 = F.begin(); it1 != F.end(); it1++) {
         Path path(it1, it1);
         for (std::list<Path*>::iterator p = paths.begin(); p != paths.end(); p++) {
           if ((*p) == &path) {
             (*p)->dist = 0;
             
           }
         }
         paths.push_back(&path);
      }
      errs() << "checkpoint"; 
      //Set up edges in the paths
      for (std::list<Edge>::iterator edge_it = edgeList.begin(), e = edgeList.end(); edge_it != e; ++edge_it) {
        Path path(edge_it->U, edge_it->V);
        for (std::list<Path*>::iterator p = paths.begin(); p != paths.end(); p++) {
          if ((*p) == &path) {
            (*p)->dist = 1;    
          }
        }
      }
       errs() << "checkpoint";
      //Now it's time to find shortest path from all points to every other point.
      Function::iterator i = F.begin(), e1 = F.end();
      Function::iterator j = F.begin(), e2 = F.end();
      Function::iterator k = F.begin(), e3 = F.end();
      errs() << "Let's find shortest paths guys\n";
      for (; k != e3; ++k) {
        for (; i != e1; ++i) {
          for (; j != e2; ++j) {
            Path IJ(i, j);
            Path JK(j, k);
            Path IK(i, k);
            for (std::list<Path*>::iterator P = paths.begin(); P != paths.end(); P++) {
              if ((*P) == &IJ) {
                IJ = *(*P); 
              }
              if ((*P) == &JK) {
                JK = *(*P);
                
              }
              if ((*P) == &IK) {
                IK = *(*P);
              }
            }
            
            if (IK.dist > IJ.dist + IK.dist) {
              IK.dist = IJ.dist + JK.dist;
            }
          }
        }
      }
      //Now loop detection time
      int numLoops = 0;
      for (Function::iterator BB1 = F.begin(); BB1 != F.end(); BB1++) {
        for (Function::iterator BB2 = F.begin(); BB2 != F.end(); BB2++) {
          if (BB1 == BB2) { continue; }
          Path *to_path;
          Path *from_path;
          for (std::list<Path*>::iterator path = paths.begin(); path != paths.end(); path++) {
            if ((*path)->U == BB1 && (*path)->V == BB2) {
              to_path = *path;
            }
            if ((*path)->V == BB1 && (*path)->U == BB2) {
              from_path = *path;
            }
          }
          if (to_path->dist < std::numeric_limits<int>::max() || from_path->dist < std::numeric_limits<int>::max()) {
            numLoops++;
          }
        }
      }
      errs() << '\t' << "Num of Loops with Warshall's Alg: " << numLoops << '\n';
      return false; 
    }
  };

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

    
  };

  char BackEdgeLoopDetector::ID = 0;
  char WarshallLoopDetector::ID = 0;
  static RegisterPass<BackEdgeLoopDetector> X("loop-backedge", "Loop Detection via Back edge", false, false);
  static RegisterPass<WarshallLoopDetector> X1("loop-warshall", "Loop Detection via Warshall's Alg", false, false);

}
