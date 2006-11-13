/*! \file
  
  \brief The AnnotationManager that generates ParamBindings.

  \authors Michelle Strout
  \version $Id: ManagerParamBindings.cpp,v 1.5 2005/08/15 19:29:55 utke Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerParamBindings.hpp"


namespace OA {
  namespace DataFlow {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerParamBindings)
static bool debug = true;
#else
static bool debug = false;
#endif

ManagerParamBindings::ManagerParamBindings(
    OA_ptr<DataFlow::ParamBindingsIRInterface> _ir) : mIR(_ir)
{
}

/*!
*/
OA_ptr<ParamBindings> 
ManagerParamBindings::performAnalysis( OA_ptr<CallGraph::Interface> callGraph )
{
  // empty set of parameter bindings that we are going to fill
  OA_ptr<ParamBindings> retval;
  retval = new ParamBindings;

  // for each node in the call graph get the set of formal 
  // parameters (that are actually refefenced)
  OA_ptr<CallGraph::Interface::NodesIterator> nodeIter=callGraph->getNodesIterator();
  for (;nodeIter->isValid();++(*nodeIter)) {
    OA_ptr<CallGraph::Interface::Node> aCGNode=nodeIter->current();
    // don't bother with things we don't have a definition for 
    if (!aCGNode->isDefined()) 
      continue;
    ProcHandle aProcHandle=aCGNode->getProc();
    // should I do this and why ? 
    mIR->currentProc(aProcHandle);
    // get all the referenced symbols
    OA_ptr<IRSymIterator> aRefSymIterator=mIR->getRefSymIterator(aProcHandle);
    for (;aRefSymIterator->isValid();++(*aRefSymIterator)) {
      SymHandle aReferencedSymbolHandle=aRefSymIterator->current();
      if (mIR->isParam(aReferencedSymbolHandle)) { 
        // associate formal parameter with  the procedure
	retval->mapFormalToProc(aReferencedSymbolHandle,aProcHandle);
	if (debug) { 
	  std::cout << "formal = " << mIR->toString(aReferencedSymbolHandle) << std::endl;
	  std::cout.flush();
	}
      }
    }
  }

  // for each edge in the call graph get parameter binding
  OA_ptr<CallGraph::Interface::EdgesIterator> 
      edgeIter = callGraph->getEdgesIterator();
  for ( ; edgeIter->isValid(); ++(*edgeIter) ) {
    OA_ptr<CallGraph::Interface::Edge> edge = edgeIter->current();

    // only do all this if the callee is defined
    OA_ptr<CallGraph::Interface::Node> callee = edge->sink();
    if (!callee->isDefined()) {
      continue;
    }

    // get procedure handle for caller
    OA_ptr<CallGraph::Interface::Node> caller = edge->source();
    ProcHandle callerProc = caller->getProc();

    // get procedure handle for callee
    ProcHandle calleeProc = callee->getProc();

    // iterate over parameters at call site
    ExprHandle call = edge->getCallHandle();
    OA_ptr<IRCallsiteParamIterator> paramIterPtr = mIR->getCallsiteParams(call);
    for ( ; paramIterPtr->isValid(); (*paramIterPtr)++ ) {
        ExprHandle param = paramIterPtr->current();

        if (debug) {
          std::cout << "==== param = " << mIR->toString(param) << std::endl;
          std::cout << "callerProc = " << mIR->toString(callerProc) 
                    << ", hval = " << callerProc.hval() << std::endl;
        }


        // if the parameter is a memory reference then get the memory reference
        mIR->currentProc(callerProc);
        OA_ptr<ExprTree> eTreePtr = mIR->getExprTree(param);
        EvalToMemRefVisitor evalVisitor;
        eTreePtr->acceptVisitor(evalVisitor);
        if (debug) { eTreePtr->dump(std::cout,mIR); }
        MemRefHandle memref;
        if ( evalVisitor.isMemRef() ) {
          memref = evalVisitor.getMemRef();
          // associate param memref with call
          retval->mapActualToCall(memref, call);
        } else {
          continue;
        }

        // get formal associated with callsite param
        SymHandle formal = mIR->getFormalForActual(callerProc, call, 
                                                   calleeProc, param);
      
        // for now only do mapping if the formal parameter is pass by reference
        // NOPE need it if it isn't pass by reference as well
        mIR->currentProc(calleeProc);
        retval->mapMemRefToFormal(call, memref,formal);
        if (mIR->isRefParam(formal)) {
          retval->setRefParam(formal);
        }

    } // loop over parameters at callsite

  }

  return retval;
}
 

  } // end of namespace DataFlow
} // end of namespace OA
