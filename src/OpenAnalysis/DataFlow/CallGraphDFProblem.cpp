/*! \file
  
  \brief Definition of a dataflow algorithm for CallGraphs

  \authors Michelle Strout (Nov 2004)
           adapted from John Mellor-Crummey's dsystem CallGraphDFProblem
  \version $Id: CallGraphDFProblem.cpp,v 1.13 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "CallGraphDFProblem.hpp"

namespace OA {
  namespace DataFlow {
static bool debug = false;

void CallGraphDFProblem::solve(OA_ptr<CallGraph::Interface> callGraph,
                               OA_ptr<Alias::InterAliasInterface> interAlias)
{
    // remove all mappings from statements to in and out sets
    //mNodeInSetMap.clear();
    //mNodeOutSetMap.clear();
    //mNodeInitTransApp.clear();

    //mTop = initializeTop();
    //mBottom = initializeBottom();

  // save for use within atDGraphEdge
  mInterAlias = interAlias;

  if (debug) {
    std::cout << "CallGraphDFProblem::solve: ";
    std::cout << "callgraph = " << std::endl;
    callGraph->dump(std::cout,mIR);
  }
  DataFlow::DGraphIterativeDFP::solve(callGraph, 
          ((mDirection==TopDown) ? DGraph::DEdgeOrg : DGraph::DEdgeRev));
}

//--------------------------------------------------------
// Methods for subclasses to get information about mapping
// of locations in Callee to locations in Caller
//--------------------------------------------------------
OA_ptr<LocIterator> 
CallGraphDFProblem::getCalleeToCallerMayLocs(ExprHandle call,
                                             OA_ptr<Location> loc)
{
    OA_ptr<LocSet> retSet;

    // determine base location
    OA_ptr<Location> baseLoc = loc->getBaseLoc();

    // if a named location and a reference param then return iterator over
    // location set this location maps to in caller
    // assumes that the CallGraphDFProblem::atDGraphEdge method has already
    // been called
    if (!mFormalToActualLocMay[call][baseLoc].empty()) {
        retSet = new LocSet(mFormalToActualLocMay[call][baseLoc]);

    // if a named location that is local to the callee and not a reference
    // param then return iterator over an empty list
    } else if (baseLoc->isaNamed() && baseLoc->isLocal()) {
        retSet = new LocSet;

    // if a named location that is not local then return that location
    // if location is unknown then return an iterator over a set with the
    // unknown location
    } else {
        retSet = new LocSet;
        retSet->insert(baseLoc);
    }

    // FIXME: not handling invisible locations or unnamed locations yet

    // if original location was a subset then change everything in retSet
    // to be same subset type
    if (loc->isaSubSet()) {
        OA_ptr<LocSubSet> locSub = loc.convert<LocSubSet>();

        OA_ptr<LocSet> tempSet;
        tempSet = new LocSet(*retSet);
        retSet->clear();
        LocSetIterator locIter(tempSet);
        for ( ; locIter.isValid(); ++locIter) {
            OA_ptr<Location> callerLoc = locIter.current();
            if (! locSub->isSubClassOfLocSubSet()) {
                OA_ptr<Location> newLoc;
                newLoc = new LocSubSet(callerLoc,false);
                retSet->insert(newLoc);
            } else {
                assert(0); // not handling other subsets yet
                // FIXME: all this code needs to somehow go in a visitor
            }
        }
    }
            
    // create iterator for set of locations in caller
    OA_ptr<LocIterator> retIter;
    retIter = new LocSetIterator(retSet);
    return retIter;
}

OA_ptr<LocIterator> 
CallGraphDFProblem::getCalleeToCallerMustLocs(ExprHandle call,
                                              OA_ptr<Location> loc)
{
    OA_ptr<LocSet> retSet;

    // if a named location and a reference param then return iterator over
    // location set this location maps to in caller
    // assumes that the CallGraphDFProblem::atDGraphEdge method has already
    // been called
    if (!mFormalToActualLocMust[call][loc].empty()) {
        retSet = new LocSet(mFormalToActualLocMust[call][loc]);

    // if a named location that is local to the callee and not a reference
    // param then return iterator over an empty list
    } else if (loc->isaNamed() && loc->isLocal()) {
        retSet = new LocSet;

    // if a named location that is not local then return that location
    } else if (loc->isaNamed() && !loc->isLocal()) {
        retSet = new LocSet;
        retSet->insert(loc);

    // if location is unknown then return an iterator over empty set
    } else {
        retSet = new LocSet;
    }

    // FIXME: not handling invisible locations or unnamed locations yet

    OA_ptr<LocIterator> retIter;
    retIter = new LocSetIterator(retSet);
    return retIter;
}
  
//========================================================
// implementation of DGraphIterativeDFP callbacks
//========================================================
  
//--------------------------------------------------------
// initialization upcall 
//--------------------------------------------------------
void CallGraphDFProblem::initialize(OA_ptr<DGraph::Interface> dg)
{
    /*
    OA_ptr<CallGraph::Interface> callGraph = dg.convert<CallGraph::Interface>();

    // iterate over all nodes and call initialization routine
    // that sets up DataFlowSets
    OA_ptr<CFG::Interface::NodesIterator> nodeIterPtr;
    nodeIterPtr = cfg->getNodesIterator();
    for ( ;nodeIterPtr->isValid(); ++(*nodeIterPtr) ) {
        initializeNode(nodeIterPtr->current());
        mNodeInitTransApp[nodeIterPtr->current()] = false;
    }
    */
}

//--------------------------------------------------------
// solver upcalls
//--------------------------------------------------------

//! make it so that all locations that the formal maps to will
//! map to the set of locations the passed in memref maps to
//! FIXME: is this just relevant to the backward direction?
bool CallGraphDFProblem::atDGraphEdge( OA_ptr<DGraph::Interface::Edge> pEdge, 
                                       DGraph::DGraphEdgeDirection pOrient)
{
    if (debug) {
      std::cout << "CallGraphDFProblem::atDGraphEdge" << std::endl;
    }

    // cast edge to a CallGraph edge
    OA_ptr<CallGraph::Interface::Edge> edge 
        = pEdge.convert<CallGraph::Interface::Edge>();
    
    // only do all this if the callee is defined
    OA_ptr<CallGraph::Interface::Node> callee = edge->sink();
    if (!callee->isDefined()) {
      return false;
    }

    // get procedure handle for caller
    OA_ptr<CallGraph::Interface::Node> caller = edge->source();
    ProcHandle callerProc = caller->getProc();

    // get procedure handle for callee
    ProcHandle calleeProc = callee->getProc();

    // get alias analysis results for caller
    OA_ptr<Alias::Interface> callerAlias 
      = mInterAlias->getAliasResults(callerProc);
    //OA_ptr<Alias::Interface> calleeAlias 
    //    = mInterAlias->getAliasResults(calleeProc);

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
        } else {
          continue;
        }

        // get formal associated with callsite param
        SymHandle formal = mIR->getFormalForActual(callerProc, call, 
                                                   calleeProc, param);
        if (debug) { 
          std::cout << "formal = " << mIR->toString(formal) << std::endl;
          std::cout.flush();
        }
      
        // for now only do mapping if the formal parameter is pass by reference
        mIR->currentProc(calleeProc);
        if (mIR->isRefParam(formal)) {

          // get location for this formal
          OA_ptr<Location> formalLoc 
              = mIR->getLocation(calleeProc,formal);

          // FIXME: loop over locations that the formal aliases
          // For now just assuming that formals don't alias anything statically
          // due to equivalences, unions, etc.
          //OA_ptr<LocIterator> locIterPtr;
          //locIterPtr = alias->getMayAliases(formalLoc);

          // loop over may and must locations for memref
          mIR->currentProc(callerProc);
          OA_ptr<LocIterator> locIterPtr;
          locIterPtr = callerAlias->getMayLocs(memref);
          if (debug) {
            std::cout << "memref = " << mIR->toString(memref) << std::endl;
            std::cout.flush();
          }
          for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
              OA_ptr<Location> loc = locIterPtr->current();
              mapFormalToActualMay(call, formalLoc, loc);
              if (debug) {
                std::cout << "loc = ";
                loc->dump(std::cout,mIR);
                std::cout << std::endl;
                std::cout.flush();
              }
          }
          locIterPtr = callerAlias->getMustLocs(memref);
          for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
              OA_ptr<Location> loc = locIterPtr->current();
              mapFormalToActualMust(call, formalLoc, loc);
          }
       } 

    } // loop over parameters at callsite

    // FIXME: this should only be called once so say that things haven't 
    // changed? if doing interprocedural alias analysis where locations 
    // a memref maps to changes then need to iterate this step
    return false;
}

    /* FIXME: Might need this and if do then probably want to refactor between 
     * CFGDFProblem and CallGraphDFProblem::atGraphNode.
     * For now going to implement atDGraphNode in subclasses for specific
     * analyses.
bool CallGraphDFProblem::atDGraphNode( OA_ptr<DGraph::Interface::Node> pNode, 
                                       DGraph::DGraphEdgeDirection pOrient)
{
    bool debug = false;
    bool changed = false;
    OA_ptr<CFG::Interface::Node> node 
        = pNode.convert<OA::CFG::Interface::Node>();

    if (debug) {
        std::cout << "CallGraphDFProblem::atDGraphNode: CFG node = ";
        std::cout << node->getId() << std::endl;
    }

    //-----------------------------------------------------
    // do a meet of all out information from nodes that are
    // predecessors based on the flow direction
    //-----------------------------------------------------
    OA_ptr<DataFlowSet> meetPartialResult = mTop->clone();
    meetPartialResult = meet(meetPartialResult,mNodeInSetMap[node]);

    // set up iterator for predecessor nodes
    OA_ptr<CFG::Interface::NodesIterator> predIterPtr;
    if (pOrient==DGraph::DEdgeOrg) {
      predIterPtr = node->getPredNodesIterator();
    } else {
      predIterPtr = node->getSuccNodesIterator();
    }
    // iterate over predecessors and do meet operation
    for (; predIterPtr->isValid(); ++(*predIterPtr)) {
      OA_ptr<CFG::Interface::Node> predNode = predIterPtr->current();
      if (pOrient==DGraph::DEdgeOrg) {
        meetPartialResult = meet(meetPartialResult, mNodeOutSetMap[predNode]);
      } else {
        meetPartialResult = meet(meetPartialResult, mNodeInSetMap[predNode]);
      }
    }

    // update the appropriate set for this node
    if (pOrient==DGraph::DEdgeOrg) { // forward
      if ( mNodeInSetMap[node] != meetPartialResult ) {
        mNodeInSetMap[node] = meetPartialResult;
        changed = true;
      }
    } else { // reverse
      if ( mNodeOutSetMap[node] != meetPartialResult ) {
        mNodeOutSetMap[node] = meetPartialResult;
        changed = true;
      }
    }

    // if the data flowing into this node has changed or if the
    // transfer functions have never been applied, then
    // loop through statements in the CFG node/(Basic Block) 
    // calculating the new node out
    if (debug) {
      std::cout << "\tchanged = " << changed << ", mNITA[node]=" 
                << mNodeInitTransApp[node] << std::endl;
    }
    if (changed || !mNodeInitTransApp[node]) {
      changed = false;  // reuse to determine if there is a change based
                        // on the block transfer function
      mNodeInitTransApp[node] = true;

      // Forward direction
      if (pOrient==DGraph::DEdgeOrg) {
        OA_ptr<DataFlowSet> prevOut = mNodeInSetMap[node]->clone();
        // loop through statements in forward order
        if (debug) {
            std::cout << "CallGraphDFProblem: iterating over statements in node";
            std::cout << std::endl;
        }
        OA_ptr<CFG::Interface::NodeStatementsIterator> stmtIterPtr 
            = node->getNodeStatementsIterator();
        for (; stmtIterPtr->isValid(); ++(*stmtIterPtr)) {
          OA::StmtHandle stmt = stmtIterPtr->current();
          if (debug) {
              std::cout << "\tstmt(hval=" << stmt.hval() << ")" << std::endl;
          }
          prevOut = transfer(prevOut, stmt);
        }
        if (debug) {
            std::cout << "CallGraphDFProblem: done iterating over statements";
            std::cout << std::endl;
        }

        if (prevOut != mNodeOutSetMap[node] ) {
          changed = true;
          mNodeOutSetMap[node] = prevOut;
        }
      
      // Reverse direction
      } else { 
        OA_ptr<DataFlowSet> prevIn = mNodeOutSetMap[node]->clone();
        // loop through statements in reverse order
        OA_ptr<CFG::Interface::NodeStatementsRevIterator> stmtIterPtr 
            = node->getNodeStatementsRevIterator();
        for (; stmtIterPtr->isValid(); ++(*stmtIterPtr)) {
          OA::StmtHandle stmt = stmtIterPtr->current();
          prevIn = transfer(prevIn, stmt);
        }
        if (prevIn != mNodeInSetMap[node] ) {
          changed = true;
          mNodeInSetMap[node] = prevIn;
        }
      }

    }
   
    if (debug) {
      std::cout << "CallGraphDFProblem::atDGraphNode: changed = " << changed << std::endl;
    }
    return changed;
}
    */
  
//--------------------------------------------------------
// finalization upcalls
//--------------------------------------------------------
void 
CallGraphDFProblem::finalizeNode(OA_ptr<DGraph::Interface::Node> node)
{
}

void 
CallGraphDFProblem::finalizeEdge(OA_ptr<DGraph::Interface::Edge> edge)
{
}

//--------------------------------------------------------
// debugging upcall 
//--------------------------------------------------------
void CallGraphDFProblem::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
}
 


  } // end of DataFlow namespace
}  // end of OA namespace

