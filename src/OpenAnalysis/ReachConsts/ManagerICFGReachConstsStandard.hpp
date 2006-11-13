/*! \file
  
  \brief Declarations of an AnnotationManager that generates an 
         InterReachConsts using the ICFGDFSolver.

  \authors Michelle Strout
  \version $Id: ManagerICFGReachConstsStandard.hpp,v 1.2 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
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
#include <OpenAnalysis/Activity/InterDep.hpp>

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
  performAnalysis(OA_ptr<ICFG::ICFGStandard> icfg,
                  OA_ptr<DataFlow::ParamBindings> paramBind,
                  OA_ptr<Alias::InterAliasInterface> interAlias,
                  OA_ptr<InterDep> interDep);

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

  //! Propagate a data-flow set from caller to callee
  OA_ptr<DataFlow::DataFlowSet> callerToCallee(ProcHandle caller,
    OA_ptr<DataFlow::DataFlowSet> dfset, ExprHandle call, ProcHandle callee);
  
  //! Propagate a data-flow set from callee to caller
  OA_ptr<DataFlow::DataFlowSet> calleeToCaller(ProcHandle callee,
    OA_ptr<DataFlow::DataFlowSet> dfset, ExprHandle call, ProcHandle caller);


private:
  OA_ptr<InterVary> mInterVary;
  OA_ptr<ActivityIRInterface> mIR;
  OA_ptr<DataFlow::ParamBindings> mParamBind;
  OA_ptr<InterDep> mInterDep;
  OA_ptr<Alias::InterAliasInterface> mInterAlias;
  OA_ptr<DataFlow::ICFGDFSolver> mSolver;
  OA_ptr<ICFG::ICFGStandard> mICFG;

  std::map<ProcHandle,OA_ptr<VaryStandard> > mVaryMap;

};

  } // end of Activity namespace
} // end of OA namespace

#endif
