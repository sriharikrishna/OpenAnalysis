/*! \file
  
  \brief Abstract class for specifying dataflow analysis problems on ICFGs.

  \authors Michelle Strout 
  \version $Id: ICFGDFProblem.hpp,v 1.2 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef ICFGDFProblem_h
#define ICFGDFProblem_h

#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/DataFlow/DataFlowSet.hpp>
#include <OpenAnalysis/ICFG/ICFGStandard.hpp>
//#include <OpenAnalysis/IRInterface/CFGIRInterface.hpp>

namespace OA {
  namespace DataFlow {

//*********************************************************************
// class ICFGDFProblem
//*********************************************************************
//template <class DataFlowSet>
class ICFGDFProblem  {
public:
  //--------------------------------------------------------
  // constructor/destructor
  //--------------------------------------------------------
  ICFGDFProblem() {}
  virtual ~ICFGDFProblem() {}

  //--------------------------------------------------------
  // initialization callbacks
  //--------------------------------------------------------

  //! Return an initialized top set
  virtual OA_ptr<DataFlowSet>  initializeTop() = 0;

  //! Return an initialized bottom set
  //virtual OA_ptr<DataFlowSet>  initializeBottom() = 0;

  //! Should generate an in and out DataFlowSet for node
  virtual OA_ptr<DataFlowSet> 
      initializeNodeIN(OA_ptr<ICFG::ICFGStandard::Node> n)  = 0;
  virtual OA_ptr<DataFlowSet> 
      initializeNodeOUT(OA_ptr<ICFG::ICFGStandard::Node> n)  = 0;
  //virtual void initializeEdge(DGraph::Interface::Edge *e)  = 0;

  //--------------------------------------------------------
  // solver callbacks 
  //--------------------------------------------------------
  
  //! OK to modify set1 and return it as result, because solver
  //! only passes a tempSet in as set1
  virtual OA_ptr<DataFlowSet> meet(OA_ptr<DataFlowSet> set1, 
                                   OA_ptr<DataFlowSet> set2) = 0; 

  //! OK to modify in set and return it again as result because
  //! solver clones the BB in sets. Proc is procedure that
  //! contains the statement.
  virtual OA_ptr<DataFlowSet> transfer(ProcHandle proc,
                                       OA_ptr<DataFlowSet> in, 
                                       OA::StmtHandle stmt) = 0; 
  
  //! transfer function for the entry node of the given procedure
  //! should manipulate incoming data-flow set in any special ways
  //! for procedure and return outgoing data-flow set for node
  virtual OA_ptr<DataFlowSet> entryTransfer(ProcHandle proc,
                                            OA_ptr<DataFlowSet> in) = 0; 

  //! transfer function for the exit node of the given procedure
  //! should manipulate outgoing data-flow set in any special ways
  //! for procedure and return incoming data-flow set for node
  virtual OA_ptr<DataFlowSet> exitTransfer(ProcHandle proc,
                                           OA_ptr<DataFlowSet> out) = 0; 

  //! Propagate a data-flow set from caller to callee
  virtual OA_ptr<DataFlowSet> callerToCallee(ProcHandle caller,
    OA_ptr<DataFlowSet> dfset, ExprHandle call, ProcHandle callee) = 0;
  
  //! Propagate a data-flow set from callee to caller
  virtual OA_ptr<DataFlowSet> calleeToCaller(ProcHandle callee,
    OA_ptr<DataFlowSet> dfset, ExprHandle call, ProcHandle caller) = 0;

};
                            
  } // end of DataFlow namespace
}  // end of OA namespace

#endif
