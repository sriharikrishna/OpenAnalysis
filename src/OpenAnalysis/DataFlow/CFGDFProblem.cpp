/*! \file
  
  \brief Definition of a dataflow algorithm for CFGs.

  \authors Michelle Strout (April 2004)
           adapted from John Mellor-Crummey's dsystem CallGraphDFProblem
  \version $Id: CFGDFProblem.cpp,v 1.24 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "CFGDFProblem.hpp"

namespace OA {
  namespace DataFlow {

#if defined(DEBUG_ALL) || defined(DEBUG_CFGDFProblem)
static bool debug = true;
#else
static bool debug = false;
#endif

/*
template <class DataFlowSet> 
CFGDFProblem<DataFlowSet>::~CFGDFProblem() 
{
}
*/


OA_ptr<DataFlowSet> CFGDFProblem::solve(OA_ptr<CFG::Interface> cfg)
{
    // remove all mappings from statements to in and out sets
//    mStmtInSetMap.clear();
//    mStmtOutSetMap.clear();
    mNodeInSetMap.clear();
    mNodeOutSetMap.clear();
    mNodeInitTransApp.clear();

    mTop = initializeTop();
    //mBottom = initializeBottom();

    DataFlow::DGraphIterativeDFP::solve(cfg, 
            ((mDirection==Forward) ? DGraph::DEdgeOrg : DGraph::DEdgeRev));

    // if forward then return DataFlowSet for exit
    if (mDirection==Forward) {
        OA_ptr<CFG::Interface::Node> exitnode = cfg->getExit();
        return mNodeInSetMap[exitnode];

    // if backward then return DataFlowSet for entry
    } else {
        OA_ptr<CFG::Interface::Node> entrynode = cfg->getEntry();
        return mNodeOutSetMap[entrynode];
    }
}
  
//========================================================
// implementation of DGraphIterativeDFP callbacks
//========================================================
  
//--------------------------------------------------------
// initialization upcall 
//--------------------------------------------------------
void CFGDFProblem::initialize(OA_ptr<DGraph::Interface> dg)
{
    OA_ptr<CFG::Interface> cfg = dg.convert<CFG::Interface>();

    // iterate over all nodes and call initialization routine
    // that sets up DataFlowSets
    OA_ptr<CFG::Interface::NodesIterator> nodeIterPtr;
    nodeIterPtr = cfg->getNodesIterator();
    for ( ;nodeIterPtr->isValid(); ++(*nodeIterPtr) ) {
        initializeNode(nodeIterPtr->current());
        mNodeInitTransApp[nodeIterPtr->current()] = false;
    }
}

//--------------------------------------------------------
// solver upcalls
//--------------------------------------------------------
bool CFGDFProblem::atDGraphNode( OA_ptr<DGraph::Interface::Node> pNode, 
                                 DGraph::DGraphEdgeDirection pOrient)
{
    bool changed = false;
    OA_ptr<CFG::Interface::Node> node 
        = pNode.convert<OA::CFG::Interface::Node>();

    if (debug) {
        std::cout << "CFGDFProblem::atDGraphNode: CFG node = ";
        std::cout << node->getId() << std::endl;
    }

    //-----------------------------------------------------
    // do a meet of all out information from nodes that are
    // predecessors based on the flow direction
    //-----------------------------------------------------
    OA_ptr<DataFlowSet> meetPartialResult = mTop->clone();
    // added following for ReachConsts, should not bother other flows
    // because DFProblem has monotonicity
    meetPartialResult = meet(meetPartialResult,mNodeInSetMap[node]);

    // set up iterator for predecessor nodes
    OA_ptr<CFG::Interface::NodesIterator> predIterPtr;
    if (pOrient==DGraph::DEdgeOrg) {
      OA_ptr<CFG::Interface::PredNodesIterator> it = 
        node->getPredNodesIterator(); // Stupid Sun CC 5.4
      predIterPtr = it.convert<CFG::Interface::NodesIterator>();
    } else {
      OA_ptr<CFG::Interface::SuccNodesIterator> it = 
        node->getSuccNodesIterator(); // Stupid Sun CC 5.4
      predIterPtr = it.convert<CFG::Interface::NodesIterator>();
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
        if (debug) {
            std::cout << "%%%%%%%% CFGDFProblem:  There was a change" 
                      << std::endl;
            std::cout << "\tmNodeInSetMap != meetPartialResult" << std::endl;
        }
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
            std::cout << "CFGDFProblem: iterating over statements in node";
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
            std::cout << "CFGDFProblem: done iterating over statements";
            std::cout << std::endl;
        }

        if (prevOut != mNodeOutSetMap[node] ) {
          changed = true;
          mNodeOutSetMap[node] = prevOut;
          if (debug) {
            std::cout << "%%%%%%%% CFGDFProblem:  There was a change" 
                      << std::endl;
            std::cout << "\tmNodeOutSetMap != prevOut" << std::endl;
          }
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
      std::cout << "CFGDFProblem::atDGraphNode: changed = " << changed << std::endl;
    }
    return changed;
}
  
//--------------------------------------------------------
// finalization upcalls
//--------------------------------------------------------
void 
CFGDFProblem::finalizeNode(OA_ptr<DGraph::Interface::Node> node)
{
}



//DataFlowSet* CFGDFProblem::transfer(DataFlowSet* in, DGraph::Interface::Node* n)
//{
//}

//--------------------------------------------------------
// debugging upcall 
//--------------------------------------------------------
void CFGDFProblem::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    // iterate over all entries in mNodeInSetMap and mNodeOutSetMap
    std::map<OA_ptr<CFG::Interface::Node>,
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

