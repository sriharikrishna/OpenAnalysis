/*! \file
  
  \brief Definition of Kildall iterative data flow solver for directed graphs.

  \authors John Mellor-Crummey (July 1994), 
           adapted for OA by Michelle Strout (April 2004)
  \version $Id: DGraphIterativeDFP.cpp,v 1.22 2005/06/15 03:32:01 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "DGraphIterativeDFP.hpp"

#include <iostream>

namespace OA {
  namespace DataFlow {

#if defined(DEBUG_ALL) || defined(DEBUG_DGraphIterativeDFP)
static bool debug = true;
#else
static bool debug = false;
#endif

//*********************************************************************
// class DGraphIterativeDFP
//*********************************************************************

DGraphIterativeDFP::DGraphIterativeDFP()
{ numIter = 0;
}


DGraphIterativeDFP::~DGraphIterativeDFP()
{
}


//-----------------------------------------------------------------------
// general default solver 
//-----------------------------------------------------------------------
void DGraphIterativeDFP::solve(OA_ptr<DGraph::Interface> dg, 
                               DGraph::DGraphEdgeDirection alongFlow) 
{

  //---------------------------------------------------------------
  // initialize dataflow information at each of the nodes and edges
  //---------------------------------------------------------------
  initialize(dg);

  //---------------------------------------------------------------
  // Kildall style iterative solver: iterate until dataflow 
  // information at each node and edge stabilizes
  //---------------------------------------------------------------
  unsigned int changed;
  OA_ptr<DGraph::Interface::Node> node;

  // get iterator for desired direction
  OA_ptr<DGraph::Interface::NodesIterator> nodeIterPtr 
    = dg->getReversePostDFSIterator(alongFlow);

  // loop until there are no changes
  int iterCnt = 0;
  do {
    changed = 0;
    iterCnt++;
    if (debug) {
        std::cout << "DGraphIterativeDFP (in loop), iterCnt = " << iterCnt << std::endl;
    }
    for (; nodeIterPtr->isValid(); ++(*nodeIterPtr)) {
      node = nodeIterPtr->current();
      if (debug) {
        std::cout << "node: Id = " << node->getId();
        std::cout << ", num_outgoing = " << node->num_outgoing();
        std::cout << ", OA_ptr::dump = ";
        node.dump(std::cout);
        std::cout << std::endl;
      }

      //--------------------------------------------------
      // compute dataflow information at node based on
      // what is coming into the node along the dataflow
      // direction
      //--------------------------------------------------
      changed |= atDGraphNode(node, alongFlow);

      //--------------------------------------------------
      // compute dataflow information going out of the node
      // along dataflow direction
      //--------------------------------------------------
      OA::OA_ptr<DGraph::Interface::EdgesIterator> edgeIterPtr;
      if (alongFlow==DGraph::DEdgeOrg) {  // follows original orientation of edges
        OA_ptr<DGraph::Interface::OutgoingEdgesIterator> it = 
          node->getOutgoingEdgesIterator(); // Stupid Sun CC 5.4
        edgeIterPtr = it.convert<DGraph::Interface::EdgesIterator>();
      } else {
        OA_ptr<DGraph::Interface::IncomingEdgesIterator> it = 
          node->getIncomingEdgesIterator(); // Stupid Sun CC 5.4
        edgeIterPtr = it.convert<DGraph::Interface::EdgesIterator>();
      }
      for (; edgeIterPtr->isValid(); ++(*edgeIterPtr)) {
        changed |= atDGraphEdge(edgeIterPtr->current(), alongFlow);
      }

    } // end for all nodes

    nodeIterPtr->reset();

  } while (changed);


  //---------------------------------------------------------------
  // finalize dataflow information at each of the nodes and edges
  //---------------------------------------------------------------
  for (; nodeIterPtr->isValid(); ++(*nodeIterPtr)) {
    node = nodeIterPtr->current();
    finalizeNode(node);


    OA::OA_ptr<DGraph::Interface::EdgesIterator> edgeIterPtr;
    if (alongFlow==DGraph::DEdgeOrg) {  // follows original orientation of edges
      OA_ptr<DGraph::Interface::OutgoingEdgesIterator> it = 
	node->getOutgoingEdgesIterator(); // Stupid Sun CC 5.4
      edgeIterPtr = it.convert<DGraph::Interface::EdgesIterator>();
    } else {
      OA_ptr<DGraph::Interface::IncomingEdgesIterator> it = 
	node->getIncomingEdgesIterator(); // Stupid Sun CC 5.4
      edgeIterPtr = it.convert<DGraph::Interface::EdgesIterator>();
    }
    for (; edgeIterPtr->isValid(); ++(*edgeIterPtr)) {
      finalizeEdge(edgeIterPtr->current());
    }

  }

  numIter = iterCnt; // set value for retrieval
  if (debug) { 
      std::cout << "DGraphIterativeDFP: iterCnt = " << iterCnt << std::endl;
  }
  return;

}


//-----------------------------------------------------------------------
// solver callbacks
//-----------------------------------------------------------------------
bool DGraphIterativeDFP::atDGraphNode
    (OA_ptr<DGraph::Interface::Node>, DGraph::DGraphEdgeDirection)
{
  return false;
}



bool DGraphIterativeDFP::atDGraphEdge
    (OA_ptr<DGraph::Interface::Edge>, DGraph::DGraphEdgeDirection)
{
  return false;
}



//-----------------------------------------------------------------------
// finalization callbacks
//-----------------------------------------------------------------------
void DGraphIterativeDFP::finalizeEdge(OA_ptr<DGraph::Interface::Edge>)
{
}



void DGraphIterativeDFP::finalizeNode(OA_ptr<DGraph::Interface::Node>)
{
}

  } // end of DataFlow namespace
}  // end of OA namespace

