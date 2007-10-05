/*! \file
  
  \brief Declarations of the AnnotationManager that generates Activity
         analysis results for each procedure using interprocedural results.

  \authors Michelle Strout
  \version $Id: ManagerICFGActive.hpp,v 1.3 2005/07/01 02:49:56 mstrout Exp $

  Copyright (c) 2002-2005, Rice University <br>
  Copyright (c) 2004-2005, University of Chicago <br>
  Copyright (c) 2006, Contributors <br>
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef ManagerICFGActive_h
#define ManagerICFGActive_h

//--------------------------------------------------------------------
// OpenAnalysis headers

#include <OpenAnalysis/Activity/ManagerActiveStandard.hpp>
#include <OpenAnalysis/Activity/ManagerICFGDep.hpp>
#include <OpenAnalysis/Activity/ManagerICFGUseful.hpp>
#include <OpenAnalysis/Activity/ManagerICFGVaryActive.hpp>
#include <OpenAnalysis/Activity/ActivePerStmt.hpp>
#include <OpenAnalysis/IRInterface/ActivityIRInterface.hpp>

#include <OpenAnalysis/Activity/InterActive.hpp>
//#include <OpenAnalysis/CFG/EachCFGInterface.hpp>
#include <OpenAnalysis/Alias/InterAliasInterface.hpp>
#include <OpenAnalysis/SideEffect/InterSideEffectInterface.hpp>
#include <OpenAnalysis/CallGraph/CallGraphInterface.hpp>

namespace OA {
  namespace Activity {


/*! 
   Generates InterActive.
*/
class ManagerICFGActive : public virtual DataFlow::ICFGDFProblem {
public:
  ManagerICFGActive(OA_ptr<Activity::ActivityIRInterface> _ir);
  ~ManagerICFGActive () {}

  OA_ptr<InterActive> performAnalysis(
          OA_ptr<ICFG::ICFGInterface> icfg,
          OA_ptr<DataFlow::ParamBindings> paramBind,
          OA_ptr<Alias::InterAliasInterface> interAlias,
          OA_ptr<SideEffect::InterSideEffectInterface> interSE,
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
    OA_ptr<DataFlow::DataFlowSet> dfset, CallHandle call, ProcHandle callee);

private: // member variables

  OA_ptr<Activity::ActivityIRInterface> mIR;
  std::map<ProcHandle,OA_ptr<ActiveStandard> > mActiveMap;

  OA_ptr<DataFlow::ParamBindings> mParamBind;
  OA_ptr<ICFGDep> mICFGDep;
  OA_ptr<Alias::InterAliasInterface> mInterAlias;
  OA_ptr<DataFlow::ICFGDFSolver> mSolver;
  OA_ptr<ICFG::ICFGInterface> mICFG;
  OA_ptr<ActivePerStmt> mActive;
};

  } // end of Activity namespace
} // end of OA namespace

#endif
