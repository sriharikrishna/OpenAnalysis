/*! \file
  
  \brief Declarations of the AnnotationManager that generates an 
         InterSideEffectStandard.

  \authors Michelle Strout
  \version $Id: ManagerInterSideEffectStandard.hpp,v 1.5 2005/06/10 02:32:05 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef ManagerInterSideEffectStandard_h
#define ManagerInterSideEffectStandard_h

//--------------------------------------------------------------------
// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/InterSideEffectIRInterface.hpp>
#include <OpenAnalysis/SideEffect/InterSideEffectStandard.hpp>
#include <OpenAnalysis/SideEffect/ManagerSideEffectStandard.hpp>
#include <OpenAnalysis/Alias/InterAliasInterface.hpp>
#include <OpenAnalysis/DataFlow/CallGraphDFProblemNew.hpp>
#include <OpenAnalysis/DataFlow/CountDFSet.hpp>
#include <OpenAnalysis/DataFlow/ParamBindings.hpp>
#include <OpenAnalysis/DataFlow/CalleeToCallerVisitor.hpp>
//#include <OpenAnalysis/CallGraph/Interface.hpp>


namespace OA {
  namespace SideEffect {


/*! 
   The AnnotationManager for InterSideEffectStandard.
   This class can build an InterSideEffectStandard. 
*/
class ManagerInterSideEffectStandard : private DataFlow::CallGraphDFProblemNew {
public:
  ManagerInterSideEffectStandard(OA_ptr<InterSideEffectIRInterface> _ir);
  ~ManagerInterSideEffectStandard () {}

  OA_ptr<InterSideEffectStandard> 
  performAnalysis(OA_ptr<CallGraph::Interface> callGraph,
                  OA_ptr<DataFlow::ParamBindings> paramBind,
                  OA_ptr<Alias::InterAliasInterface> interAlias,
                  OA_ptr<ManagerStandard> intraMan);

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
          OA_ptr<DataFlow::DataFlowSet> procDFSet, ExprHandle call)
  {
    return procDFSet;
  }

  //! translate results from caller edge to procedure node if top-down
  //! or from callee edge if bottom-up
  OA_ptr<DataFlow::DataFlowSet>  edgeToNode(ExprHandle call, 
          OA_ptr<DataFlow::DataFlowSet> callDFSet, ProcHandle proc)
  {
      return callDFSet;
  }


private:
  OA_ptr<InterSideEffectIRInterface> mIR;
  OA_ptr<Alias::InterAliasInterface> mInterAlias;
  OA_ptr<InterSideEffectStandard> mInterSideEffect;
  OA_ptr<DataFlow::ParamBindings> mParamBind;

  OA_ptr<ManagerStandard> mIntraMan;
  
};

  } // end of SideEffect namespace
} // end of OA namespace

#endif
