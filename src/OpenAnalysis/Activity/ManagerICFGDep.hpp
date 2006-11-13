/*! \file
  
  \brief Declarations of the AnnotationManager that generates a ICFGDep

  \authors Michelle Strout
  \version $Id: ManagerICFGDep.hpp,v 1.2 2005/06/10 02:32:02 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef ManagerICFGDep_H
#define ManagerICFGDep_H

//--------------------------------------------------------------------
// OpenAnalysis headers
#include "ICFGDep.hpp"
#include "DepDFSet.hpp"
#include <OpenAnalysis/IRInterface/ActivityIRInterface.hpp>
#include <OpenAnalysis/ExprTree/DifferentiableLocsVisitor.hpp>
#include <OpenAnalysis/ExprTree/EvalToMemRefVisitor.hpp>

#include <OpenAnalysis/DataFlow/ICFGDFProblem.hpp>
#include <OpenAnalysis/DataFlow/ICFGDFSolver.hpp>
#include <OpenAnalysis/DataFlow/ParamBindings.hpp>
#include <OpenAnalysis/DataFlow/LocDFSet.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>


namespace OA {
  namespace Activity {

/*! 
   The AnnotationManager for a ICFGDep.
   Just posing as an ICFGDFProblem because the ICFGDep results are only 
   valid for ManagerICFGVary and ManagerICFGUseful because they propagate
   over the ICFG.
   FIXME: could be useful to other representations that handle
   propagation to function calls as well and doesn't need to have the
   stmt with a function call include the Dep relations for the call
*/
class ManagerICFGDep : public virtual DataFlow::ICFGDFProblem
{
public:
  ManagerICFGDep(OA_ptr<ActivityIRInterface> _ir);
  ~ManagerICFGDep () {}

  OA_ptr<ICFGDep> 
  performAnalysis(OA_ptr<ICFG::ICFGStandard> icfg,
                  OA_ptr<DataFlow::ParamBindings> paramBind,
                  OA_ptr<Alias::InterAliasInterface> interAlias);
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
      initializeNodeIN(OA_ptr<ICFG::ICFGStandard::Node> n);
  OA_ptr<DataFlow::DataFlowSet> 
      initializeNodeOUT(OA_ptr<ICFG::ICFGStandard::Node> n); 
 
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
      entryTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> in)
        { return initializeTop(); }

  //! transfer function for the exit node of the given procedure
  //! should manipulate outgoing data-flow set in any special ways
  //! for procedure and return incoming data-flow set for node
  OA_ptr<DataFlow::DataFlowSet> 
      exitTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> out)
        { return initializeTop(); }

  //! Propagate a data-flow set from caller to callee
  OA_ptr<DataFlow::DataFlowSet> callerToCallee(ProcHandle caller,
    OA_ptr<DataFlow::DataFlowSet> dfset, ExprHandle call, ProcHandle callee)
        { return initializeTop(); }
  
  //! Propagate a data-flow set from callee to caller
  OA_ptr<DataFlow::DataFlowSet> calleeToCaller(ProcHandle callee,
    OA_ptr<DataFlow::DataFlowSet> dfset, ExprHandle call, ProcHandle caller)
        { return initializeTop(); }

private: // member variables

  OA_ptr<ActivityIRInterface> mIR;
  OA_ptr<ICFGDep> mDep;
  OA_ptr<Alias::InterAliasInterface> mInterAlias;
  OA_ptr<DataFlow::ParamBindings> mParamBind;
  OA_ptr<DataFlow::ICFGDFSolver> mSolver;
  OA_ptr<ICFG::ICFGStandard> mICFG;

};

  } // end of Activity namespace
} // end of OA namespace

#endif
