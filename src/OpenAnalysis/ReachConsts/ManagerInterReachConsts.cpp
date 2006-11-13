/*! \file
  
  \brief The AnnotationManager that generates InterReachConsts
  INTERprocedurally.

  \authors Michelle Strout, Barbara Kreaseck
  \version $Id: ManagerInterReachConsts.cpp,v 1.2 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerInterReachConsts.hpp"


namespace OA {
  namespace ReachConsts {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerInterReachConsts)
static bool debug = true;
#else
static bool debug = false;
#endif

ManagerInterReachConsts::ManagerInterReachConsts(
    OA_ptr<ReachConstsIRInterface> _ir) : mIR(_ir)
{
}

/*!
   Just calls ManagerReachConstStandard::performAnalysis on each procedure
*/
OA_ptr<InterReachConsts> 
ManagerInterReachConsts::performAnalysis(
        OA_ptr<CallGraph::Interface> callGraph,
        // OA_ptr<DataFlow::ParamBindings> paramBind,
        OA_ptr<Alias::InterAliasInterface> interAlias,
        OA_ptr<SideEffect::InterSideEffectInterface> interSE,
        OA_ptr<CFG::EachCFGInterface> eachCFG)
{
  OA_ptr<InterReachConsts> retval;
  retval = new InterReachConsts;


  // Iterate over the procedures in the call graph
  OA_ptr<CallGraph::Interface::NodesIterator> procIter
      = callGraph->getNodesIterator();
  for ( ; procIter->isValid(); ++(*procIter)) { 
    
    ProcHandle proc = procIter->current()->getProc();

    // if this procedure isn't defined then move on
    if (proc==ProcHandle(0)) { 
        continue;
    }

    //set currentProc()
    //mIR->currentProc(proc);

    // get Alias::Interface for this proc
    OA_ptr<Alias::Interface> alias;
    alias = interAlias->getAliasResults(proc);

    // get CFG for this proc
    OA_ptr<CFG::Interface> cfg;
    cfg = eachCFG->getCFGResults(proc);

    // create ReachConsts manager
    OA_ptr<ManagerStandard> rcman;
    rcman = new ReachConsts::ManagerStandard(mIR);

    OA_ptr<ReachConsts::ReachConstsStandard> rcs 
        = rcman->performAnalysis(proc, cfg,
                alias,interSE);

    // put reachconsts results in InterReachConsts
    retval->mapProcToReachConsts(proc,rcs);

  }
 
  return retval;
}
 

  } // end of namespace Activity
} // end of namespace OA
