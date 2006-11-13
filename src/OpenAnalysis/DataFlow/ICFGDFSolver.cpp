/*! \file
  
  \brief Definition of a dataflow algorithm for ICFGs.

  \authors Michelle Strout
  \version $Id: ICFGDFSolver.cpp,v 1.2 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "ICFGDFSolver.hpp"

namespace OA {
  namespace DataFlow {

#if defined(DEBUG_ALL) || defined(DEBUG_ICFGDFSolver)
static bool debug = true;
#else
static bool debug = false;
#endif

void
ICFGDFSolver::solve(OA_ptr<ICFG::ICFGStandard> icfg)
{
    // remove all mappings of nodes to data flow sets 
    mNodeInSetMap.clear();
    mNodeOutSetMap.clear();
    mNodeInitTransApp.clear();

    mTop = mDFProb.initializeTop();

    DataFlow::DGraphIterativeDFP::solve(icfg, 
            ((mDirection==Forward) ? DGraph::DEdgeOrg : DGraph::DEdgeRev));

    /*
    // if forward then return DataFlowSet for exit
    if (mDirection==Forward) {
        OA_ptr<ICFG::ICFGStandard::Node> exitnode = icfg->getExit();
        return mNodeInSetMap[exitnode];

    // if backward then return DataFlowSet for entry
    } else {
        OA_ptr<ICFG::ICFGStandard::Node> entrynode = icfg->getEntry();
        return mNodeOutSetMap[entrynode];
    }
    */
}
  
//========================================================
// implementation of DGraphIterativeDFP callbacks
//========================================================
  
//--------------------------------------------------------
// initialization upcall 
//--------------------------------------------------------
void ICFGDFSolver::initialize(OA_ptr<DGraph::Interface> dg)
{
    OA_ptr<ICFG::ICFGStandard> cfg = dg.convert<ICFG::ICFGStandard>();

    // iterate over all nodes and call initialization routine
    // that sets up DataFlowSets
    OA_ptr<ICFG::ICFGStandard::NodesIterator> nodeIterPtr;
    nodeIterPtr = cfg->getNodesIterator();
    for ( ;nodeIterPtr->isValid(); ++(*nodeIterPtr) ) {
        mNodeInSetMap[nodeIterPtr->current()] 
            = mDFProb.initializeNodeIN(nodeIterPtr->current());
        mNodeOutSetMap[nodeIterPtr->current()] 
            = mDFProb.initializeNodeOUT(nodeIterPtr->current());
        mNodeInitTransApp[nodeIterPtr->current()] = false;
    }
}

//--------------------------------------------------------
// solver upcalls
//--------------------------------------------------------
bool ICFGDFSolver::atDGraphNode( OA_ptr<DGraph::Interface::Node> pNode, 
                                 DGraph::DGraphEdgeDirection pOrient)
{
    bool changed = false;
    OA_ptr<ICFG::ICFGStandard::Node> node 
        = pNode.convert<OA::ICFG::ICFGStandard::Node>();

    if (debug) {
        std::cout << "ICFGDFSolver::atDGraphNode: ICFG node = ";
        std::cout << node->getId() << std::endl;
    }

    //-----------------------------------------------------
    // do a meet of all out information from nodes that are
    // predecessors with current input for this node
    // based on the flow direction, edge type,
    // and node type
    // FIXME: Have to meet with current input for this node because
    // of ReachConsts?
    //-----------------------------------------------------
    OA_ptr<DataFlowSet> meetPartialResult = mTop->clone();
    if (pOrient==DGraph::DEdgeOrg) {
      meetPartialResult = mDFProb.meet(meetPartialResult,mNodeInSetMap[node]);
    } else {
      meetPartialResult = mDFProb.meet(meetPartialResult,mNodeOutSetMap[node]);
    }

    // set up iterator for predecessor edges
    OA_ptr<ICFG::ICFGStandard::EdgesIterator> predIterPtr;
    if (pOrient==DGraph::DEdgeOrg) {
      predIterPtr = node->getIncomingEdgesIterator();
    } else {
      predIterPtr = node->getOutgoingEdgesIterator();
    }
    // iterate over predecessors and do meet operation
    for (; predIterPtr->isValid(); ++(*predIterPtr)) {
      OA_ptr<ICFG::ICFGStandard::Edge> predEdge = predIterPtr->current();
      OA_ptr<ICFG::ICFGStandard::Node> predNode;
      OA_ptr<DataFlowSet> inSet;
      if (pOrient==DGraph::DEdgeOrg) {
        predNode = predEdge->source();
        switch(predEdge->getType()) {
            case (ICFG::CALL_EDGE):
                // mNodeInSet because only get to use data-flow set before call
                // in caller
                inSet = mDFProb.callerToCallee(predEdge->getSourceProc(),
                            mNodeInSetMap[predNode], predEdge->getCall(),
                            predEdge->getSinkProc());
                break;
            case (ICFG::RETURN_EDGE):
                inSet = mDFProb.calleeToCaller(predEdge->getSourceProc(),
                            mNodeOutSetMap[predNode], predEdge->getCall(),
                            predEdge->getSinkProc());
                break;
            case (ICFG::CFLOW_EDGE):
                inSet = mNodeOutSetMap[predNode];
                break;
        }
        meetPartialResult = mDFProb.meet(meetPartialResult, inSet);

      // backward flow
      } else {
        predNode = predEdge->sink();
        switch(predEdge->getType()) {
            case (ICFG::CALL_EDGE):
                inSet = mDFProb.calleeToCaller(predEdge->getSinkProc(),
                            mNodeInSetMap[predNode], predEdge->getCall(),
                            predEdge->getSourceProc());
                break;
            case (ICFG::RETURN_EDGE):
                // use outset for RETURN_NODE in caller
                inSet = mDFProb.callerToCallee(predEdge->getSinkProc(),
                            mNodeOutSetMap[predNode], predEdge->getCall(),
                            predEdge->getSourceProc());
                break;
            case (ICFG::CFLOW_EDGE):
                inSet = mNodeInSetMap[predNode];
                break;
        }
        meetPartialResult = mDFProb.meet(meetPartialResult, inSet);
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

        // call transfer methods based on what kind of node 
        if (node->getType()==ICFG::ENTRY_NODE) {
            prevOut = mDFProb.entryTransfer(node->getProc(), prevOut);

        // otherwise it is a normal node and should apply regular transfer
        // loop through statements in forward order
        } else {
            OA_ptr<CFG::Interface::NodeStatementsIterator> stmtIterPtr 
                = node->getNodeStatementsIterator();
            for (; stmtIterPtr->isValid(); ++(*stmtIterPtr)) {
                OA::StmtHandle stmt = stmtIterPtr->current();
                prevOut = mDFProb.transfer(node->getProc(), prevOut, stmt);
            }
        }

        if (prevOut != mNodeOutSetMap[node] ) {
          changed = true;
          mNodeOutSetMap[node] = prevOut;
        }
      
      // Reverse direction
      } else { 
        OA_ptr<DataFlowSet> prevIn = mNodeOutSetMap[node]->clone();
        
        // if it is an exit node call transfer method for exit nodes
        if (node->getType()==ICFG::EXIT_NODE) {
            prevIn = mDFProb.exitTransfer(node->getProc(), prevIn);

        // otherwise it is a normal node and should apply regular transfer
        } else {
            // loop through statements in reverse order
            OA_ptr<CFG::Interface::NodeStatementsRevIterator> stmtIterPtr 
                = node->getNodeStatementsRevIterator();
            for (; stmtIterPtr->isValid(); ++(*stmtIterPtr)) {
                OA::StmtHandle stmt = stmtIterPtr->current();
                prevIn = mDFProb.transfer(node->getProc(), prevIn, stmt);
            }
        }

        if (prevIn != mNodeInSetMap[node] ) {
          changed = true;
          mNodeInSetMap[node] = prevIn;
        }
      }

    }
   
    if (debug) {
      std::cout << "ICFGDFSolver::atDGraphNode: changed = " << changed << std::endl;
    }
    return changed;
}
  
//--------------------------------------------------------
// finalization upcalls
//--------------------------------------------------------
void 
ICFGDFSolver::finalizeNode(OA_ptr<DGraph::Interface::Node> node)
{
}



//--------------------------------------------------------
// debugging upcall 
//--------------------------------------------------------
void ICFGDFSolver::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    // iterate over all entries in mNodeInSetMap and mNodeOutSetMap
    std::map<OA_ptr<ICFG::ICFGStandard::Node>,
             OA_ptr<DataFlowSet> >::iterator mapIter;
    for (mapIter=mNodeInSetMap.begin(); mapIter!=mNodeInSetMap.end(); mapIter++)
    {
        OA_ptr<DataFlowSet> dfset = mapIter->second;
        os << "Node (" << mapIter->first << ") In: ";
        dfset->dump(os,ir);
        os << std::endl;
    }
    for (mapIter=mNodeOutSetMap.begin(); mapIter!=mNodeOutSetMap.end(); mapIter++)
    {
        OA_ptr<DataFlowSet> dfset = mapIter->second;
        os << "Node (" << mapIter->first << ") Out: ";
        dfset->dump(os,ir);
        os << std::endl;
    }
}

  } // end of DataFlow namespace
}  // end of OA namespace

