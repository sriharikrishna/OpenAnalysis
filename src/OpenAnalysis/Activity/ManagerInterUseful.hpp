/*! \file
  
  \brief Declarations of the AnnotationManager that generates an InterUseful.

  \authors Michelle Strout
  \version $Id: ManagerInterUseful.hpp,v 1.2 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef ManagerInterUseful_H
#define ManagerInterUseful_H

//--------------------------------------------------------------------
#include <cassert>

// OpenAnalysis headers
#include <OpenAnalysis/Alias/InterAliasInterface.hpp>
#include <OpenAnalysis/SideEffect/InterSideEffectInterface.hpp>
#include <OpenAnalysis/IRInterface/ActivityIRInterface.hpp>
#include <OpenAnalysis/ExprTree/DifferentiableLocsVisitor.hpp>
#include <OpenAnalysis/ExprTree/EvalToMemRefVisitor.hpp>
#include <OpenAnalysis/DataFlow/CallGraphDFProblemNew.hpp>
#include <OpenAnalysis/DataFlow/ParamBindings.hpp>
#include <OpenAnalysis/DataFlow/LocDFSet.hpp>
#include <OpenAnalysis/Activity/InterUseful.hpp>
#include <OpenAnalysis/Activity/InterDep.hpp>
#include <OpenAnalysis/DataFlow/CalleeToCallerVisitor.hpp>
#include <OpenAnalysis/CFG/EachCFGInterface.hpp>

#include "ManagerUsefulStandard.hpp"

namespace OA {
  namespace Activity {

/*! 
    Creates InterUseful, which can be queried for a VaryStandard for
    each procedure.  Computes the vary set for each statement doing
    a top-down traversal of the CallGraph so the set of locations that
    vary at the beginning of a procedure are known.
*/
class ManagerInterUseful : private DataFlow::CallGraphDFProblemNew
{ 
public:
  ManagerInterUseful(OA_ptr<ActivityIRInterface> _ir);
  ~ManagerInterUseful () {}

  OA_ptr<InterUseful> 
  performAnalysis(OA_ptr<CallGraph::Interface> callGraph,
                  OA_ptr<DataFlow::ParamBindings> paramBind,
                  OA_ptr<Alias::InterAliasInterface> interAlias,
                  OA_ptr<CFG::EachCFGInterface> eachCFG,
                  OA_ptr<InterDep> interDep);

private:
  //========================================================
  // implementation of CallGraphDFProblemNew callbacks
  //========================================================
  //--------------------------------------------------------
  // initialization callbacks
  //--------------------------------------------------------

  //! Return an initialized top set
  OA_ptr<DataFlow::DataFlowSet>  initializeTop();

  //! Return an initialized bottom set
  OA_ptr<DataFlow::DataFlowSet>  initializeBottom();

  //! Should generate an initial DataFlowSet for a procedure
  OA_ptr<DataFlow::DataFlowSet> initializeNode(ProcHandle proc);
  //! Should generate an initial DataFlowSet, use if for a call if both caller
  //! and callee are defined
  OA_ptr<DataFlow::DataFlowSet> initializeEdge(ExprHandle call, 
                                     ProcHandle caller,
                                     ProcHandle callee);
  //! Should generate an initial DataFlowSet for a call,
  //! called when callee is not defined in call graph and therefore
  //! doesn't have a procedure definition handle
  OA_ptr<DataFlow::DataFlowSet> initializeEdge(ExprHandle call, 
                                     ProcHandle caller,
                                     SymHandle callee);

  //--------------------------------------------------------
  // solver callbacks 
  //--------------------------------------------------------
  
  //! OK to modify set1 and return it as result, because solver
  //! only passes a tempSet in as set1
  OA_ptr<DataFlow::DataFlowSet> meet(OA_ptr<DataFlow::DataFlowSet> set1, 
                           OA_ptr<DataFlow::DataFlowSet> set2); 

  //! What the analysis does for the particular procedure
  OA_ptr<DataFlow::DataFlowSet>
  atCallGraphNode(OA_ptr<DataFlow::DataFlowSet> inSet, OA::ProcHandle proc);

  //! What the analysis does for a particular call
  OA_ptr<DataFlow::DataFlowSet>
  atCallGraphEdge(OA_ptr<DataFlow::DataFlowSet> inSet, OA::ExprHandle call,
                  ProcHandle caller, ProcHandle callee);

  //! translate results from procedure node to callee edge if top-down
  //! or to caller edge if bottom-up
  OA_ptr<DataFlow::DataFlowSet>  nodeToEdge(ProcHandle proc, 
          OA_ptr<DataFlow::DataFlowSet> procDFSet, ExprHandle call);

  //! translate results from caller edge to procedure node if top-down
  //! or from callee edge if bottom-up
  OA_ptr<DataFlow::DataFlowSet>  edgeToNode(ExprHandle call, 
          OA_ptr<DataFlow::DataFlowSet> callDFSet, ProcHandle proc);


private:
  OA_ptr<InterUseful> mInterUseful;
  OA_ptr<ActivityIRInterface> mIR;
  OA_ptr<DataFlow::ParamBindings> mParamBind;
  OA_ptr<CFG::EachCFGInterface> mEachCFG;
  OA_ptr<InterDep> mInterDep;
  OA_ptr<Alias::InterAliasInterface> mInterAlias;

};

  } // end of Activity namespace
} // end of OA namespace

#endif
