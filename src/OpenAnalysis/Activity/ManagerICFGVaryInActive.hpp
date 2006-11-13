/*! \file
  
  \brief Declarations of an AnnotationManager that generates an
         InActivePerStmt by doing Vary analysis and using results
         of Useful analysis.

  \authors Michelle Strout
  \version $Id: ManagerICFGVaryInActive.hpp,v 1.1 2005/07/01 02:49:56 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef ManagerICFGVaryInActive_H
#define ManagerICFGVaryInActive_H

//--------------------------------------------------------------------
#include <cassert>

// OpenAnalysis headers
#include <OpenAnalysis/Alias/InterAliasInterface.hpp>
#include <OpenAnalysis/SideEffect/InterSideEffectInterface.hpp>
#include <OpenAnalysis/IRInterface/ActivityIRInterface.hpp>

#include <OpenAnalysis/DataFlow/ICFGDFProblem.hpp>
#include <OpenAnalysis/DataFlow/ICFGDFSolver.hpp>
#include <OpenAnalysis/DataFlow/ParamBindings.hpp>
#include <OpenAnalysis/DataFlow/LocDFSet.hpp>
#include <OpenAnalysis/Activity/InterVary.hpp>
#include <OpenAnalysis/Activity/InterUseful.hpp>
#include <OpenAnalysis/Activity/ICFGDep.hpp>

#include "ManagerUsefulStandard.hpp"
#include "InActivePerStmt.hpp"

namespace OA {
  namespace Activity {

/*! 
    Creates InActivePerStmt, which can be queried for  the set of locs
    that are InActive for each stmt.
*/
class ManagerICFGVaryInActive : public virtual DataFlow::ICFGDFProblem
{ 
public:
  ManagerICFGVaryInActive(OA_ptr<ActivityIRInterface> _ir);
  ~ManagerICFGVaryInActive () {}

  OA_ptr<InActivePerStmt> 
  performAnalysis(OA_ptr<ICFG::ICFGStandard> icfg,
                  OA_ptr<DataFlow::ParamBindings> paramBind,
                  OA_ptr<Alias::InterAliasInterface> interAlias,
                  OA_ptr<ICFGDep> icfgDep,
                  OA_ptr<InterUseful> interUseful);

private:
  //========================================================
  // helper routines
  //========================================================
  OA_ptr<DataFlow::LocDFSet> calculateActive(
        OA_ptr<LocIterator> varyIter,
        OA_ptr<LocIterator> usefulIter, StmtHandle stmt);

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
      entryTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> in);

  //! transfer function for the exit node of the given procedure
  //! should manipulate outgoing data-flow set in any special ways
  //! for procedure and return incoming data-flow set for node
  OA_ptr<DataFlow::DataFlowSet> 
      exitTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> out);

  //! Propagate a data-flow set from caller to callee
  OA_ptr<DataFlow::DataFlowSet> callerToCallee(ProcHandle caller,
    OA_ptr<DataFlow::DataFlowSet> dfset, ExprHandle call, ProcHandle callee);
  
  //! Propagate a data-flow set from callee to caller
  OA_ptr<DataFlow::DataFlowSet> calleeToCaller(ProcHandle callee,
    OA_ptr<DataFlow::DataFlowSet> dfset, ExprHandle call, ProcHandle caller);


private:
  OA_ptr<InActivePerStmt> mInActive;
  OA_ptr<InterUseful> mInterUseful;
  OA_ptr<ActivityIRInterface> mIR;
  OA_ptr<DataFlow::ParamBindings> mParamBind;
  OA_ptr<ICFGDep> mICFGDep;
  OA_ptr<Alias::InterAliasInterface> mInterAlias;
  OA_ptr<DataFlow::ICFGDFSolver> mSolver;
  OA_ptr<ICFG::ICFGStandard> mICFG;

};

  } // end of Activity namespace
} // end of OA namespace

#endif
