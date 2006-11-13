/*! \file
  
  \brief The AnnotationManager that generates InterVary.

  \authors Michelle Strout
  \version $Id: ManagerInterVary.cpp,v 1.2 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerInterVary.hpp"


namespace OA {
  namespace Activity {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerInterVary)
static bool debug = true;
#else
static bool debug = false;
#endif

/*!
*/
ManagerInterVary::ManagerInterVary(
  OA_ptr<ActivityIRInterface> _ir) 
    : DataFlow::CallGraphDFProblemNew(DataFlow::CallGraphDFProblemNew::TopDown),
      mIR(_ir)
{
}

OA_ptr<Activity::InterVary> 
ManagerInterVary::performAnalysis(
        OA_ptr<CallGraph::Interface> callGraph,
        OA_ptr<DataFlow::ParamBindings> paramBind,
        OA_ptr<Alias::InterAliasInterface> interAlias,
        OA_ptr<CFG::EachCFGInterface> eachCFG,
        OA_ptr<InterDep> interDep)
{
  // store results that will be needed in callbacks
  mParamBind = paramBind;
  mEachCFG = eachCFG;
  mInterDep = interDep;
  mInterAlias = interAlias;

  // create an empty InterVary
  mInterVary = new InterVary();

  // call iterative data-flow solver for CallGraph
  CallGraphDFProblemNew::solve(callGraph);
    
  return mInterVary;
}

//========================================================
// implementation of CallGraphDFProblemNew callbacks
//========================================================
//--------------------------------------------------------
// initialization callbacks
//--------------------------------------------------------

/*!
   Data-flow set passed around on the call graph is an
   LocationDataFlowSet.  The top value for this is an empty set.
*/
OA_ptr<DataFlow::DataFlowSet> ManagerInterVary::initializeTop()
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    return retval;
}

/*! Shouldn't be called.  
 */
OA_ptr<DataFlow::DataFlowSet>  
ManagerInterVary::initializeBottom()
{
    assert(0);
    // have different bottom based on what procedure we are
    // specificallly have to have all accessible locs
}

//! Should generate an initial DataFlowSet for a procedure
OA_ptr<DataFlow::DataFlowSet> 
ManagerInterVary::initializeNode(ProcHandle proc)
{
    return initializeTop();
}

//! Should generate an initial DataFlowSet, use if for a call if both caller
//! and callee are defined
OA_ptr<DataFlow::DataFlowSet> 
ManagerInterVary::initializeEdge(ExprHandle call, 
                                                 ProcHandle caller,
                                                 ProcHandle callee)
{
    return initializeTop();
}

//! Should generate an initial DataFlowSet for a call,
//! called when callee is not defined in call graph and therefore
//! doesn't have a procedure definition handle
OA_ptr<DataFlow::DataFlowSet> 
ManagerInterVary::initializeEdge(ExprHandle call, 
                                                 ProcHandle caller,
                                                 SymHandle callee)
{
    return initializeTop();
}

//--------------------------------------------------------
// solver callbacks 
//--------------------------------------------------------
  
//! OK to modify set1 and return it as result, because solver
//! only passes a tempSet in as set1
OA_ptr<DataFlow::DataFlowSet> 
ManagerInterVary::meet(OA_ptr<DataFlow::DataFlowSet> set1, 
                      OA_ptr<DataFlow::DataFlowSet> set2)
{
    if (debug) {
        std::cout << "ManagerInterVary::meet" << std::endl;
    }
    OA_ptr<DataFlow::LocDFSet> remapSet1 = set1.convert<DataFlow::LocDFSet>();
    OA_ptr<DataFlow::LocDFSet> remapSet2 = set2.convert<DataFlow::LocDFSet>();
    if (debug) {
        std::cout << "\tremapSet1 = ";
        remapSet1->dump(std::cout, mIR);
        std::cout << "\tremapSet2 = ";
        remapSet2->dump(std::cout, mIR);
    }

    // making sure we don't trounce set1 just in case
    OA_ptr<DataFlow::LocDFSet> retval;
    OA_ptr<DataFlow::DataFlowSet> temp = remapSet1->clone();
    retval = temp.convert<DataFlow::LocDFSet>();
    *retval = retval->setUnion(*remapSet2);
    if (debug) {
        std::cout << "\tretval = ";
        OA_ptr<DataFlow::LocDFSet> temp = retval.convert<DataFlow::LocDFSet>();
        temp->dump(std::cout, mIR);
    }
    return retval;
}

//! What the analysis does for the particular procedure
OA_ptr<DataFlow::DataFlowSet>
ManagerInterVary::atCallGraphNode(
        OA_ptr<DataFlow::DataFlowSet> inSet, 
        OA::ProcHandle proc)
{
    if (debug) {
        std::cout << "In ManagerInterVary::atCallGraphNode for proc = " 
                  << mIR->toString(proc) << std::endl;
    }
    
    // get iterator over indep  locations for procedure
    OA_ptr<LocIterator> indepIter = mIR->getIndepLocIter(proc);

    // going to be determining indep LocDFSet
    // to pass to ManagerVaryStandard
    OA_ptr<DataFlow::LocDFSet> inVary;
    inVary = new DataFlow::LocDFSet;

    // if the Unknown Loc is in the indep set then use inSet
    // FIXME: should we check that this is not the top procedure?
    for ( indepIter->reset(); indepIter->isValid(); (*indepIter)++ ) {
        if (debug) { 
          std::cout << "indepIter->current() = "; 
          indepIter->current()->dump(std::cout,mIR);
          std::cout << std::endl;
        }
        if (indepIter->current()->isaUnknown()) {
            inVary = inSet.convert<DataFlow::LocDFSet>();
            break;
        }
        inVary->insert(indepIter->current());
    }
    if (debug) {
      std::cout << "\tManagerInterVary, Indep locations: " << std::endl;
      inVary->dump(std::cout, mIR);
      std::cout << "----" << std::endl;
    }
 
    // get vary results from ManagerVary
    OA_ptr<ManagerVaryStandard> varyman;
    varyman = new ManagerVaryStandard(mIR);
    OA_ptr<VaryStandard> vary 
        = varyman->performAnalysis(proc, mEachCFG->getCFGResults(proc),
                                   mInterDep->getDepResults(proc), inVary);

    // assign dep results to this procedure
    mInterVary->mapProcToVary(proc,vary);
    if (debug) {
      std::cout << "\tManagerInterVary, just called performAnalysis on proc = ";
      std::cout << mIR->toString(proc) << std::endl;
    }

    // data flow set for each call is set up in nodeToEdge where
    // node is caller, so this is just a dummy set being passed
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    return retval;
}

//! What the analysis does for a particular call
OA_ptr<DataFlow::DataFlowSet>
ManagerInterVary::atCallGraphEdge(
        OA_ptr<DataFlow::DataFlowSet> inSet, 
        OA::ExprHandle call, ProcHandle caller, ProcHandle callee)
{
    if (debug) {
        std::cout << "atCallGraphEdge::inSet = ";
        inSet->dump(std::cout, mIR);
    }

    // pass on inSet
    return inSet;
}

OA_ptr<DataFlow::DataFlowSet>  
ManagerInterVary::nodeToEdge(ProcHandle proc, 
        OA_ptr<DataFlow::DataFlowSet> procDFSet, ExprHandle call)
{
  if (debug) {
      std::cout << "In ManagerInterVary::nodeToEdge" << std::endl;
  }

  // get locations that vary according to independents coming into
  // the statement for this call
  OA_ptr<VaryStandard> vary = mInterVary->getVaryResults(proc);
  OA_ptr<DataFlow::LocDFSet> inVarySet = vary->getInVarySet(call);
  if (debug) { std::cout << "\tinVarySet = ";
      inVarySet->dump(std::cout,mIR);
  }
  OA_ptr<DataFlow::LocDFSet> retval;
  retval = new DataFlow::LocDFSet;

  // get alias results for caller procedure
  OA_ptr<Alias::Interface> alias = mInterAlias->getAliasResults(proc);

  // iterate over the reference parameters for the call
  OA_ptr<MemRefHandleIterator> memrefIter = mParamBind->getActualIterator(call);
  for (; memrefIter->isValid(); (*memrefIter)++ ) {
      MemRefHandle memref = memrefIter->current();
      if (debug) {
          std::cout << "\tmemref = " << mIR->toString(memref) << std::endl;
      }

      // loop over may locs for the memref
      OA_ptr<LocIterator> locIter = alias->getMayLocs(memref);
      for ( ; locIter->isValid(); (*locIter)++ ) {
        OA_ptr<Location> loc = locIter->current();
        if (inVarySet->hasOverlapLoc(loc)) {
          SymHandle sym = mParamBind->getCalleeFormal(call,memref);
          OA_ptr<Location> nloc;
          nloc = new NamedLoc(sym,true);
          retval->insert(nloc);
        }
      }
  }

  return retval;
}

//! translate results from caller edge to procedure node if top-down
//! or from callee edge if bottom-up
OA_ptr<DataFlow::DataFlowSet>  
ManagerInterVary::edgeToNode(ExprHandle call, 
        OA_ptr<DataFlow::DataFlowSet> callDFSet, ProcHandle proc)
{
    if (debug) {
        std::cout << "edgeToNode::callDFSet = ";
        callDFSet->dump(std::cout, mIR);
    }

    return callDFSet;
}



  } // end of namespace Activity
} // end of namespace OA
