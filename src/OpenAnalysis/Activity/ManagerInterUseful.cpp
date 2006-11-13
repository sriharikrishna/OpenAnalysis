/*! \file
  
  \brief The AnnotationManager that generates InterUseful.

  \authors Michelle Strout
  \version $Id: ManagerInterUseful.cpp,v 1.2 2005/06/10 02:32:02 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerInterUseful.hpp"


namespace OA {
  namespace Activity {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerInterUseful)
static bool debug = true;
#else
static bool debug = false;
#endif

/*!
*/
ManagerInterUseful::ManagerInterUseful(
  OA_ptr<ActivityIRInterface> _ir) 
    : DataFlow::CallGraphDFProblemNew(DataFlow::CallGraphDFProblemNew::TopDown),
      mIR(_ir)
{
}

OA_ptr<Activity::InterUseful> 
ManagerInterUseful::performAnalysis(
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

  // create an empty InterUseful
  mInterUseful = new InterUseful();

  // call iterative data-flow solver for CallGraph
  CallGraphDFProblemNew::solve(callGraph);
    
  return mInterUseful;
}

//========================================================
// implementation of CallGraphDFProblemNew callbacks
//========================================================
//--------------------------------------------------------
// initialization callbacks
//--------------------------------------------------------

/*!
   Data-flow set passed around on the call graph is an
   DataFlow::LocDFSet.  The top value for this is an empty set.
*/
OA_ptr<DataFlow::DataFlowSet> ManagerInterUseful::initializeTop()
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    return retval;
}

/*! Shouldn't be called.  
 */
OA_ptr<DataFlow::DataFlowSet>  
ManagerInterUseful::initializeBottom()
{
    assert(0);
    // have different bottom based on what procedure we are
    // specificallly have to have all accessible locs
}

//! Should generate an initial DataFlowSet for a procedure
OA_ptr<DataFlow::DataFlowSet> 
ManagerInterUseful::initializeNode(ProcHandle proc)
{
    return initializeTop();
}

//! Should generate an initial DataFlowSet, use if for a call if both caller
//! and callee are defined
OA_ptr<DataFlow::DataFlowSet> 
ManagerInterUseful::initializeEdge(ExprHandle call, 
                                                 ProcHandle caller,
                                                 ProcHandle callee)
{
    return initializeTop();
}

//! Should generate an initial DataFlowSet for a call,
//! called when callee is not defined in call graph and therefore
//! doesn't have a procedure definition handle
OA_ptr<DataFlow::DataFlowSet> 
ManagerInterUseful::initializeEdge(ExprHandle call, 
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
ManagerInterUseful::meet(OA_ptr<DataFlow::DataFlowSet> set1, 
                      OA_ptr<DataFlow::DataFlowSet> set2)
{
    if (debug) {
        std::cout << "ManagerInterUseful::meet" << std::endl;
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
ManagerInterUseful::atCallGraphNode(
        OA_ptr<DataFlow::DataFlowSet> inSet, 
        OA::ProcHandle proc)
{
    if (debug) {
        std::cout << "In ManagerInterUseful::atCallGraphNode" 
                  << std::endl;
    }
    
    // get iterator over dep  locations for procedure
    OA_ptr<LocIterator> depIter = mIR->getDepLocIter(proc);

    // going to be determining useful DataFlow::LocDFSet
    // to pass to ManagerUsefulStandard
    OA_ptr<DataFlow::LocDFSet> outUseful;
    outUseful = new DataFlow::LocDFSet;

    // if the Unknown Loc is in the dep set then use inSet
    // FIXME: should we check that this is not the top procedure?
    for ( depIter->reset(); depIter->isValid(); (*depIter)++ ) {
        if (debug) { 
          std::cout << "depIter->current() = "; 
          depIter->current()->dump(std::cout,mIR);
          std::cout << std::endl;
        }
        if (depIter->current()->isaUnknown()) {
            outUseful = inSet.convert<DataFlow::LocDFSet>();
            break;
        }
        outUseful->insert(depIter->current());
    }
    if (debug) {
      std::cout << "\tManagerInterUseful, Dep locations: " << std::cout;
      outUseful->dump(std::cout, mIR);
      std::cout << "----" << std::endl;
    }
 
    // get Useful results from ManagerUsefulStandard
    OA_ptr<ManagerUsefulStandard> usefulman;
    usefulman = new ManagerUsefulStandard(mIR);
    OA_ptr<UsefulStandard> useful 
        = usefulman->performAnalysis(proc, mEachCFG->getCFGResults(proc),
                                   mInterDep->getDepResults(proc), outUseful);

    // assign dep results to this procedure
    mInterUseful->mapProcToUseful(proc,useful);
    if (debug) { std::cout << "%%%%%% ManagerInterUseful, useful = ";
        useful->dump(std::cout,mIR);
    }

    // resulting DataFlow::LocDFSet for whole procedure will be sent to each
    // of the call edges
    return useful->getFinalUseful();
}

//! What the analysis does for a particular call
OA_ptr<DataFlow::DataFlowSet>
ManagerInterUseful::atCallGraphEdge(
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
ManagerInterUseful::nodeToEdge(ProcHandle proc, 
        OA_ptr<DataFlow::DataFlowSet> procDFSet, ExprHandle call)
{
  if (debug) {
      std::cout << "In ManagerInterUseful::nodeToEdge" << std::endl;
  }
  
  // get locations that are useful according to dependents after
  // the statement for this call
  OA_ptr<UsefulStandard> useful = mInterUseful->getUsefulResults(proc);
  OA_ptr<DataFlow::LocDFSet> outUsefulSet = useful->getOutUsefulSet(call);
  OA_ptr<DataFlow::LocDFSet> retval;
  retval = new DataFlow::LocDFSet;

  // get alias results for caller procedure
  OA_ptr<Alias::Interface> alias = mInterAlias->getAliasResults(proc);

  // iterate over the reference parameters for the call
  OA_ptr<MemRefHandleIterator> memrefIter = mParamBind->getActualIterator(call);
  for (; memrefIter->isValid(); (*memrefIter)++ ) {
      MemRefHandle memref = memrefIter->current();

      // loop over may locs for the memref
      OA_ptr<LocIterator> locIter = alias->getMayLocs(memref);
      for ( ; locIter->isValid(); (*locIter)++ ) {
        OA_ptr<Location> loc = locIter->current();
        if (outUsefulSet->hasOverlapLoc(loc)) {
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
ManagerInterUseful::edgeToNode(ExprHandle call, 
        OA_ptr<DataFlow::DataFlowSet> callDFSet, ProcHandle proc)
{
    if (debug) {
        std::cout << "edgeToNode::callDFSet = ";
        callDFSet->dump(std::cout, mIR);
    }

/* need this for side-effect analysis
    // cast to more specific subclass
    OA_ptr<DataFlow::LocDFSet> recastDFSet 
        = callDFSet.convert<DataFlow::LocDFSet>();

    // create a new set that is empty
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;

    OA_ptr<LocIterator> locIter = recastDFSet->getLocIterator();
    for ( ; locIter->isValid(); (*locIter)++ ) {
        OA_ptr<Location> loc = locIter->current();
        DataFlow::CalleeToCallerVisitor v(call,proc,mInterAlias,mParamBind);
        loc->acceptVisitor(v);
        OA_ptr<LocIterator> usefulIter = v.getCallerLocIterator();
        for ( ; usefulIter->isValid(); (*usefulIter)++ ) {
            retval->insert(usefulIter->current());
        }
    }
*/
    return callDFSet;
}



  } // end of namespace Activity
} // end of namespace OA
