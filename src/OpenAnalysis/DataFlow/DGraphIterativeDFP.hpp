/*! \file
  
  \brief Declaration of Kildall iterative data flow solver for directed graphs.

  \authors John Mellor-Crummey (July 1994), 
           adapted for OA by Michelle Strout (April 2004)
  \version $Id: DGraphIterativeDFP.hpp,v 1.7 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef DGraphIterativeDFP_h
#define DGraphIterativeDFP_h

//#ifndef DirectedGraph_h
//#include <libs/support/graphs/directedGraph/DirectedGraph.h>
//#endif
// Going to attempt to use DGraph instead
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/Utils/DGraph/Interface.hpp>
#include <OpenAnalysis/CFG/Interface.hpp>
#include <OpenAnalysis/CFG/CFGStandard.hpp>

#include <iostream>

namespace OA {
  namespace DataFlow {

//*********************************************************************
// class DGraphIterativeDFP
//*********************************************************************
class DGraphIterativeDFP {
public:
  //--------------------------------------------------------
  // constructor/destructor
  //--------------------------------------------------------
  DGraphIterativeDFP();
  virtual ~DGraphIterativeDFP();

  //--------------------------------------------------------
  // Kildall iterative solver 
  //--------------------------------------------------------
  void solve(OA_ptr<DGraph::Interface> dg, 
             DGraph::DGraphEdgeDirection alongFlow);

  int afterSolve_getNumIter() {return numIter;}

private:

  int numIter;

  //--------------------------------------------------------
  // initialization callback
  //--------------------------------------------------------
  virtual void initialize(OA_ptr<DGraph::Interface> dg)  = 0;

  //--------------------------------------------------------
  // solver callbacks 
  //--------------------------------------------------------
  //   atDGraphNode and atDGraphEdge return true if change
  //   in dataflow information at the graph element
  virtual bool atDGraphNode(OA_ptr<DGraph::Interface::Node>, 
                            DGraph::DGraphEdgeDirection);
  virtual bool atDGraphEdge(OA_ptr<DGraph::Interface::Edge>, 
                            DGraph::DGraphEdgeDirection);

  //--------------------------------------------------------
  // finalization callbacks
  //--------------------------------------------------------
  virtual void finalizeNode(OA_ptr<DGraph::Interface::Node> node);
  virtual void finalizeEdge(OA_ptr<DGraph::Interface::Edge> edge);
};

  } // end of DataFlow
}  // end of OA namespace

#endif
