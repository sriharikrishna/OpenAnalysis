/*! \file
  
  \brief Definition of the AnnotationManager that generates a CallGraphStandard.

  \authors Arun Chauhan (2001 as part of Mint), Nathan Tallent, Michelle Strout
  \version $Id: ManagerCallGraphStandard.cpp,v 1.13 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

// standard headers

#ifdef NO_STD_CHEADERS
# include <stdlib.h>
# include <string.h>
# include <assert.h>
#else
# include <cstdlib>
# include <cstring>
# include <cassert>
using namespace std; // For compatibility with non-std C headers
#endif

#include <iostream>

#include "ManagerCallGraphStandard.hpp"

namespace OA {
  namespace CallGraph {

static bool debug = false;

//--------------------------------------------------------------------
OA_ptr<CallGraphStandard> ManagerStandard::performAnalysis (
        OA_ptr<IRProcIterator> procIter) 
{
  mCallGraph = new CallGraphStandard();
  build_graph(procIter);
  return mCallGraph;
}

//--------------------------------------------------------------------
void
ManagerStandard::build_graph(OA_ptr<IRProcIterator> procIter)
{
  // Iterate over all the procedures in the program
  for ( ; procIter->isValid(); ++(*procIter)) { 
    
    // Create a node for this procedure
    ProcHandle currProc = procIter->current();
    SymHandle currProcSym = mIR->getProcSymHandle(currProc);
    if (debug) {
      std::cout << "currProc = " << mIR->toString(currProc);
      std::cout << ", currProcSym = " << mIR->toString(currProcSym) << std::endl;
    }

    OA_ptr<CallGraphStandard::Node> currProcNode 
      = mCallGraph->findOrAddNode(currProcSym);
    currProcNode->add_def(currProc); 
    
    // Iterate over the statements of this procedure
    OA_ptr<IRStmtIterator> stmtIterPtr = mIR->getStmtIterator(currProc);
    // Iterate over the statements of this block adding procedure references
    for ( ; stmtIterPtr->isValid(); ++(*stmtIterPtr)) {

      StmtHandle stmt = stmtIterPtr->current();

      // Iterate over procedure calls of a statement
      OA_ptr<IRCallsiteIterator> callsiteItPtr = mIR->getCallsites(stmt);
      for ( ; callsiteItPtr->isValid(); ++(*callsiteItPtr)) {
        ExprHandle expr = callsiteItPtr->current();
        SymHandle sym = mIR->getSymHandle(expr);
        if (debug) {
          std::cout << "sym for callee = " << mIR->toString(sym) << std::endl;
        }
	
        // Add a node (if nonexistent) and edge
        OA_ptr<CallGraphStandard::Node> node 
          = mCallGraph->findOrAddNode(sym);
        node->add_call(expr);
        mCallGraph->connect(currProcNode, node, 
                            CallGraph::Interface::NORMAL_EDGE, expr);
      }
    }
  } // iterating over procedures

}

  } // end CallGraph namespace
} // end OA namespace
