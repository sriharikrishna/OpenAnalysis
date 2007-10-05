/*! \file
  
  \brief Declarations of an AnnotationManager that generates an InterVary
         using the ICFGDFSolver.

  \authors Michelle Strout
  \version $Id: ManagerICFGVary.hpp,v 1.2 2005/06/10 02:32:02 mstrout Exp $

  Copyright (c) 2002-2005, Rice University <br>
  Copyright (c) 2004-2005, University of Chicago <br>
  Copyright (c) 2006, Contributors <br>
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef ManagerICFGVary_H
#define ManagerICFGVary_H

//--------------------------------------------------------------------
#include <cassert>

// OpenAnalysis headers
#include <OpenAnalysis/Alias/InterAliasInterface.hpp>
#include <OpenAnalysis/SideEffect/InterSideEffectInterface.hpp>
#include <OpenAnalysis/IRInterface/ActivityIRInterface.hpp>

//??? needed???
//#include <OpenAnalysis/ExprTree/DifferentiableLocsVisitor.hpp>
//#include <OpenAnalysis/ExprTree/EvalToMemRefVisitor.hpp>
//#include <OpenAnalysis/DataFlow/CalleeToCallerVisitor.hpp>

#include <OpenAnalysis/DataFlow/ICFGDFProblem.hpp>
#include <OpenAnalysis/DataFlow/ICFGDFSolver.hpp>
#include <OpenAnalysis/DataFlow/ParamBindings.hpp>
#include <OpenAnalysis/DataFlow/LocDFSet.hpp>
#include <OpenAnalysis/Activity/InterVary.hpp>
#include <OpenAnalysis/Activity/ICFGDep.hpp>

#include "ManagerVaryStandard.hpp"

namespace OA {
  namespace Activity {

/*! 
    Creates InterVary, which can be queried for a VaryStandard for
    each procedure.  
*/
class ManagerICFGVary : public virtual DataFlow::ICFGDFProblem
{ 
public:
  ManagerICFGVary(OA_ptr<ActivityIRInterface> _ir);
  ~ManagerICFGVary () {}

  OA_ptr<InterVary> 
  performAnalysis(OA_ptr<ICFG::ICFGInterface> icfg,
                  OA_ptr<DataFlow::ParamBindings> paramBind,
                  OA_ptr<Alias::InterAliasInterface> interAlias,
                  OA_ptr<ICFGDep> icfgDep,
                  DataFlow::DFPImplement algorithm);

private:
  //========================================================
  // implementation of ICFGDFProblem interface
  //========================================================
  //--------------------------------------------------------
  // initialization callbacks
  //--------------------------------------------------------

  //! Return an initialized top set
  OA_ptr<DataFlow::DataFlowSet>  initializeTop();
  
  //! Should generate an in and out DataFlowSet for node
  OA_ptr<DataFlow::DataFlowSet> 
      initializeNodeIN(OA_ptr<ICFG::NodeInterface> n);
  OA_ptr<DataFlow::DataFlowSet> 
      initializeNodeOUT(OA_ptr<ICFG::NodeInterface> n);
 
  //--------------------------------------------------------
  // solver callbacks 
  //--------------------------------------------------------
  
  //! OK to modify set1 and return it as result, because solver
  //! only passes a tempSet in as set1
  OA_ptr<DataFlow::DataFlowSet> meet(OA_ptr<DataFlow::DataFlowSet> set1, 
                                     OA_ptr<DataFlow::DataFlowSet> set2); 

  //! OK to modify in set and return it again as result because
  //! solver clones the BB in sets
  OA_ptr<DataFlow::DataFlowSet> 
      transfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> in, 
               OA::StmtHandle stmt);
  
  //! transfer function for the entry node of the given procedure
  //! should manipulate incoming data-flow set in any special ways
  //! for procedure and return outgoing data-flow set for node
  OA_ptr<DataFlow::DataFlowSet> 
      entryTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> in);

  //! transfer function for the exit node of the given procedure
  //! should manipulate outgoing data-flow set in any special ways
  //! for procedure and return incoming data-flow set for node
  OA_ptr<DataFlow::DataFlowSet> 
      exitTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> out);

  //! Propagate a data-flow set from caller to callee
  OA_ptr<DataFlow::DataFlowSet> callerToCallee(ProcHandle caller,
    OA_ptr<DataFlow::DataFlowSet> dfset, CallHandle call, ProcHandle callee);
  
  //! Propagate a data-flow set from callee to caller
  OA_ptr<DataFlow::DataFlowSet> calleeToCaller(ProcHandle callee,
    OA_ptr<DataFlow::DataFlowSet> dfset, CallHandle call, ProcHandle caller);

 //! Propagate a data-flow set from call node to return node
 OA_ptr<DataFlow::DataFlowSet> callToReturn(ProcHandle caller,
   OA_ptr<DataFlow::DataFlowSet> dfset, CallHandle call, ProcHandle callee)
  { assert(0); // has not been updated recently
    return initializeTop(); }

private:
  OA_ptr<InterVary> mInterVary;
  OA_ptr<ActivityIRInterface> mIR;
  OA_ptr<DataFlow::ParamBindings> mParamBind;
  OA_ptr<ICFGDep> mICFGDep;
  OA_ptr<Alias::InterAliasInterface> mInterAlias;
  OA_ptr<DataFlow::ICFGDFSolver> mSolver;
  OA_ptr<ICFG::ICFGInterface> mICFG;

  std::map<ProcHandle,OA_ptr<VaryStandard> > mVaryMap;

};

  } // end of Activity namespace
} // end of OA namespace

#endif
