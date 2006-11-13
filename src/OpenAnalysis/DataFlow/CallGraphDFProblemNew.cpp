/*! \file
  
  \brief Definition of a dataflow algorithm for CallGraphs

  \authors Michelle Strout (Nov 2004)
           adapted from John Mellor-Crummey's dsystem 
           CallGraphFlowInsensitiveDFProblem
  \version $Id: CallGraphDFProblemNew.cpp,v 1.3 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "CallGraphDFProblemNew.hpp"

namespace OA {
  namespace DataFlow {
#if defined(DEBUG_ALL) || defined(DEBUG_CallGraphDFProblemNew)
static bool debug = true;
#else
static bool debug = false;
#endif

void 
CallGraphDFProblemNew::solve(OA_ptr<CallGraph::Interface> callGraph)
{
  if (debug) {
    //std::cout << "CallGraphDFProblem::solve: ";
    //std::cout << "callgraph = " << std::endl;
    //callGraph->dump(std::cout,mIR);
  }

  mTop = initializeTop();

  DataFlow::DGraphIterativeDFP::solve(callGraph, 
          ((mDirection==TopDown) ? DGraph::DEdgeOrg : DGraph::DEdgeRev));
}

//========================================================
// implementation of DGraphIterativeDFP callbacks
//========================================================
  
//--------------------------------------------------------
// initialization upcall 
//--------------------------------------------------------
void CallGraphDFProblemNew::initialize(OA_ptr<DGraph::Interface> dg)
{
    
    OA_ptr<CallGraph::Interface> callGraph = dg.convert<CallGraph::Interface>();

    // iterate over all nodes and call initialization routine
    // that sets up initial DataFlowSets so we know if things have changed
    OA_ptr<CallGraph::Interface::NodesIterator> nodeIterPtr;
    nodeIterPtr = callGraph->getNodesIterator();
    for ( ;nodeIterPtr->isValid(); ++(*nodeIterPtr) ) {
        OA_ptr<CallGraph::Interface::Node> node = nodeIterPtr->current();
        // In and Out in this context refer to the data-flow direction
        // which in the case of bottom-up is in the reverse direction
        // of edges in the graph
        mNodeInSetMap[node] = initializeTop();
        mNodeOutSetMap[node] = initializeNode(node->getProc());
    }

    // iterate over all edges and call initialization routine
    // that sets up initial DataFlowSets so we know if things have changed
    OA_ptr<CallGraph::Interface::EdgesIterator> edgeIterPtr;
    edgeIterPtr = callGraph->getEdgesIterator();
    for ( ;edgeIterPtr->isValid(); ++(*edgeIterPtr) ) {
        OA_ptr<CallGraph::Interface::Edge> edge = edgeIterPtr->current();

        // In is in direction of data-flow
        mEdgeInSetMap[edge] = initializeTop();

        // get procedure handle for caller
        OA_ptr<CallGraph::Interface::Node> caller = edge->source();

        // get procedure handle for callee if there is one
        OA_ptr<CallGraph::Interface::Node> callee = edge->sink();
        if (callee->isDefined()) {
            mEdgeOutSetMap[edge] = initializeEdge(edge->getCallHandle(),
                                                  caller->getProc(),
                                                  callee->getProc());
        } else {
            mEdgeOutSetMap[edge] = initializeEdge(edge->getCallHandle(),
                                                  caller->getProc(),
                                                  callee->getProcSym());
        }
    }

}

//--------------------------------------------------------
// solver upcalls
//--------------------------------------------------------

bool CallGraphDFProblemNew::atDGraphEdge( OA_ptr<DGraph::Interface::Edge> pEdge, 
                                       DGraph::DGraphEdgeDirection pOrient)
{
    if (debug) {
      std::cout << "CallGraphDFProblemNew::atDGraphEdge" << std::endl;
    }

    // cast edge to a CallGraph edge
    OA_ptr<CallGraph::Interface::Edge> edge 
        = pEdge.convert<CallGraph::Interface::Edge>();

    // get procedure handle for caller
    OA_ptr<CallGraph::Interface::Node> caller = edge->source();

    // get procedure handle for callee if there is one
    OA_ptr<CallGraph::Interface::Node> callee = edge->sink();

    // get data-flow set for edge based on procedure
    // whether getting it from caller or callee depends on 
    // the direction of the analysis
    OA_ptr<DataFlowSet> edgeIn;
    if (pOrient==DGraph::DEdgeOrg) { // top down
      edgeIn = nodeToEdge(caller->getProc(), mNodeOutSetMap[caller],
                          edge->getCallHandle());
    } else { // bottom up
      edgeIn = nodeToEdge(callee->getProc(), mNodeOutSetMap[callee],
                          edge->getCallHandle());
    }

    // do computation at actual call
    OA_ptr<DataFlowSet> edgeOut 
        = atCallGraphEdge(edgeIn,
                edge->getCallHandle(), caller->getProc(), callee->getProc());
    mEdgeInSetMap[edge] = edgeIn;

    bool changed = false;
    // check if a change occured
    if (edgeOut != mEdgeOutSetMap[edge]) {
      if (debug) { 
          std::cout << "%%% CallGraphDFProblemNew::atDGraphEdge, "
                    << "mEdgeOutSetMap != edgeOut" << std::endl;
      }
      mEdgeOutSetMap[edge] = edgeOut;
      changed = true;
    } 
     
    return changed;
}

/*!
   NOTE: The CallGraphDFProblemNew code treats nodeIn, nodeOut, edgeIn, and
         edgeOut differently than the other data-flow frameworks.
         FIXME: to make maintenance easier they should all use similar logic
*/
bool CallGraphDFProblemNew::atDGraphNode( 
                                       OA_ptr<DGraph::Interface::Node> pNode, 
                                       DGraph::DGraphEdgeDirection pOrient)
{
    OA_ptr<CallGraph::Interface::Node> node 
        = pNode.convert<OA::CallGraph::Interface::Node>();

    if (debug) {
        std::cout << "CallGraphDFProblemNew::atDGraphNode: CallGraph node = ";
        std::cout << node->getId() << std::endl;
    }

    //-----------------------------------------------------
    // for each incoming edge translate edgeOut to nodeIn
    // and then do meet of all nodeIns with each other and
    // current nodeIn 
    //-----------------------------------------------------
    OA_ptr<DataFlowSet> nodeInSet = mNodeInSetMap[node]->clone();

    // set up iterator for predecessor nodes
    OA_ptr<CallGraph::Interface::EdgesIterator> predIterPtr;
    if (pOrient==DGraph::DEdgeOrg) {
      OA_ptr<CallGraph::Interface::IncomingEdgesIterator> it
        = node->getIncomingEdgesIterator(); // stupid Sun compiler
      predIterPtr = it.convert<CallGraph::Interface::EdgesIterator>();
    } else {
      OA_ptr<CallGraph::Interface::OutgoingEdgesIterator> it
        = node->getOutgoingEdgesIterator(); // stupid Sun compiler
      predIterPtr = it.convert<CallGraph::Interface::EdgesIterator>();
    }
    // iterate over predecessors, translate from edge to node, 
    // and do meet operation
    for (; predIterPtr->isValid(); ++(*predIterPtr)) {
      OA_ptr<CallGraph::Interface::Edge> edge = predIterPtr->current();
      OA_ptr<DataFlowSet> temp  = edgeToNode(edge->getCallHandle(),
 					              mEdgeOutSetMap[edge], node->getProc());
      nodeInSet = meet(nodeInSet, temp);
    }

    if (debug) { 
      std::cout << "%%% CallGraphDFProblemNew::atDGraphNode, ";
      std::cout << "nodeInSet = ";
      nodeInSet->dump(std::cout);
      std::cout << "mNodeInSetMap[node] = ";
      mNodeInSetMap[node]->dump(std::cout);
    }
    // calculate the output data-flow set for this procedure
    OA_ptr<DataFlowSet> nodeOutSet;
    if (node->getProc() != ProcHandle(0)) {
        nodeOutSet = atCallGraphNode(nodeInSet, node->getProc());
    } else {
        nodeOutSet = mNodeOutSetMap[node];
    }
    mNodeInSetMap[node] = nodeInSet;

    // update the out set for this node if has changed
    bool changed = false;
    if ( mNodeOutSetMap[node] != nodeOutSet ) {
        if (debug) { 
          std::cout << "%%% CallGraphDFProblemNew::atDGraphNode, "
                    << "mNodeOutSetMap != nodeOutSet" << std::endl;
          std::cout << "mNodeOutSetMap = ";
          mNodeOutSetMap[node]->dump(std::cout);
          std::cout << "nodeOutSet = ";
          nodeOutSet->dump(std::cout);
        }
        mNodeOutSetMap[node] = nodeOutSet;
        changed = true;
    }
    if (debug) { std::cout << "CallGraphDFProblemNew::atDGraphNode, changed = "
                           << changed << std::endl;
    }

    return changed;
}
  
//--------------------------------------------------------
// finalization upcalls
//--------------------------------------------------------
void 
CallGraphDFProblemNew::finalizeNode(
    OA_ptr<DGraph::Interface::Node> node)
{
}

void 
CallGraphDFProblemNew::finalizeEdge(
    OA_ptr<DGraph::Interface::Edge> edge)
{
}

//--------------------------------------------------------
// debugging upcall 
//--------------------------------------------------------
void CallGraphDFProblemNew::dump(std::ostream& os, 
    OA_ptr<IRHandlesIRInterface> ir)
{
}
 


  } // end of DataFlow namespace
}  // end of OA namespace

