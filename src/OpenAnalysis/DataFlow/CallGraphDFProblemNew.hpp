/*! \file
  
  \brief Class for solving dataflow analysis problems on CallGraphs in a flow
         and context insensitive fashion.

  \authors Michelle Strout (Jan 2005)
           similar to John Mellor-Crummey's CallGraphFlowInsensitiveDFProblem.h
  \version $Id: CallGraphDFProblemNew.hpp,v 1.3 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  To use this for performing interprocedural dataflow analysis,
  privately inherit from the CallGraphDFProblemNew class and
  implement the callbacks.
  It is flow insensitive because for top down analyses it uses the analysis
  results for the whole caller at each of the caller's callsites.
  It is context-insensitive because it meets all the data flow information
  for callers when working top-down and callees when working bottom-up.
*/

#ifndef CallGraphDFProblemNew_h
#define CallGraphDFProblemNew_h

#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/DataFlow/DGraphIterativeDFP.hpp>
#include <OpenAnalysis/DataFlow/DataFlowSet.hpp>
#include <OpenAnalysis/CallGraph/Interface.hpp>
#include <map>
#include <iostream>

namespace OA {
  namespace DataFlow {

  
//*********************************************************************
// class CallGraphDFProblemNew
//*********************************************************************
class CallGraphDFProblemNew  : private DataFlow::DGraphIterativeDFP {
public:
  typedef enum { TopDown, BottomUp } CallGraphDirectionType;
private:
  OA_ptr<DataFlowSet> mTop;
  OA_ptr<DataFlowSet> mBottom;
  CallGraphDirectionType mDirection;

public:
  //--------------------------------------------------------
  // constructor/destructor
  //--------------------------------------------------------
  CallGraphDFProblemNew(CallGraphDirectionType pDirection) 
      : mDirection(pDirection) {} 
  virtual ~CallGraphDFProblemNew() {}

  void solve(OA_ptr<CallGraph::Interface> callGraph);

private:
  //--------------------------------------------------------
  // initialization callbacks
  //--------------------------------------------------------

  //! Return an initialized top set
  virtual OA_ptr<DataFlowSet>  initializeTop() = 0;

  //! Return an initialized bottom set
  virtual OA_ptr<DataFlowSet>  initializeBottom() = 0;

  //! Should generate an initial DataFlowSet for a procedure
  //! Nodes that we don't have procedure definition for will get ProcHandle(0)
  virtual OA_ptr<DataFlowSet> initializeNode(ProcHandle proc)  = 0;
  //! Should generate an initial DataFlowSet, use if for a call if both caller
  //! and callee are defined
  virtual OA_ptr<DataFlowSet> initializeEdge(ExprHandle call, 
                                             ProcHandle caller,
                                             ProcHandle callee)  = 0;
  //! Should generate an initial DataFlowSet for a call,
  //! called when callee is not defined in call graph and therefore
  //! doesn't have a procedure definition handle
  virtual OA_ptr<DataFlowSet> initializeEdge(ExprHandle call, 
                                             ProcHandle caller,
                                             SymHandle callee)  = 0;

  //--------------------------------------------------------
  // solver callbacks 
  //--------------------------------------------------------
  
  //! OK to modify set1 and return it as result, because solver
  //! only passes a tempSet in as set1
  virtual OA_ptr<DataFlowSet> meet(OA_ptr<DataFlowSet> set1, 
                                   OA_ptr<DataFlowSet> set2) = 0; 

  //! OK to modify in set and return it again as result because
  //! solver clones the BB in sets
  //virtual OA_ptr<DataFlowSet> transfer(OA_ptr<DataFlowSet> in, 
  //                                     OA::StmtHandle stmt) = 0; 

  //! What the analysis does for the particular procedure
  virtual OA_ptr<DataFlowSet>
  atCallGraphNode(OA_ptr<DataFlowSet> inSet, OA::ProcHandle proc) = 0;

  //! What the analysis does for a particular call
  virtual OA_ptr<DataFlowSet>
  atCallGraphEdge(OA_ptr<DataFlowSet> inSet, ExprHandle call,
                  ProcHandle caller, ProcHandle callee) = 0;

  //! translate results from procedure node to callee edge if top-down
  //! or to caller edge if bottom-up using data-flow set at procedure node
  //! the proc could be ProcHandle(0) if the called procedure has not
  //! been defined
  virtual OA_ptr<DataFlowSet>  nodeToEdge(ProcHandle proc, 
            OA_ptr<DataFlow::DataFlowSet> procDFSet, ExprHandle call) = 0;

  //! translate results from caller edge to procedure node if top-down
  //! or from callee edge if bottom-up using call data flow set
  virtual OA_ptr<DataFlowSet>  edgeToNode(ExprHandle call, 
            OA_ptr<DataFlow::DataFlowSet> callDFSet, ProcHandle proc) = 0;

                            
protected:
  //========================================================
  // implementation of DGraphIterativeDFP callbacks
  // These are implemented in CallGraphDFProblem.cpp for a 
  // CallGraph.
  //========================================================
  
  //--------------------------------------------------------
  // initialization upcall 
  //--------------------------------------------------------
  void initialize(OA_ptr<DGraph::Interface> dg);
  
  //--------------------------------------------------------
  // solver upcalls
  //--------------------------------------------------------
  bool atDGraphNode(OA_ptr<DGraph::Interface::Node> node, 
                    DGraph::DGraphEdgeDirection pOrient);
  bool atDGraphEdge(OA_ptr<DGraph::Interface::Edge>, 
                    DGraph::DGraphEdgeDirection);
  
  //--------------------------------------------------------
  // finalization upcalls
  //--------------------------------------------------------
  void finalizeNode(OA_ptr<DGraph::Interface::Node> node);
  void finalizeEdge(OA_ptr<DGraph::Interface::Edge> edge);


  //--------------------------------------------------------
  // debugging
  //--------------------------------------------------------
public:
  void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);


private:
  // In and Out in this context refer to the data-flow direction
  // which in the case of bottom-up is in the reverse direction
  // of edges in the graph
  std::map<OA_ptr<CallGraph::Interface::Node>,OA_ptr<DataFlowSet> > 
      mNodeOutSetMap;
  std::map<OA_ptr<CallGraph::Interface::Node>,OA_ptr<DataFlowSet> > 
      mNodeInSetMap;
  std::map<OA_ptr<CallGraph::Interface::Edge>,OA_ptr<DataFlowSet> > 
      mEdgeOutSetMap;
  std::map<OA_ptr<CallGraph::Interface::Edge>,OA_ptr<DataFlowSet> > 
      mEdgeInSetMap;

};

  } // end of DataFlow namespace
}  // end of OA namespace

#endif
