/*! \file
  
  \brief The AnnotationManager that generates InActivePerStmt.

  \authors Michelle Strout
  \version $Id: ManagerICFGVaryInActive.cpp,v 1.1 2005/07/01 02:49:56 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerICFGVaryInActive.hpp"


namespace OA {
  namespace Activity {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerICFGVaryInActive)
static bool debug = true;
#else
static bool debug = false;
#endif

/*!
*/
ManagerICFGVaryInActive::ManagerICFGVaryInActive( 
        OA_ptr<ActivityIRInterface> _ir) : mIR(_ir)
{
    mSolver = new DataFlow::ICFGDFSolver(DataFlow::ICFGDFSolver::Forward,*this);
}

OA_ptr<Activity::InActivePerStmt> 
ManagerICFGVaryInActive::performAnalysis(
        OA_ptr<ICFG::ICFGStandard> icfg,
        OA_ptr<DataFlow::ParamBindings> paramBind,
        OA_ptr<Alias::InterAliasInterface> interAlias,
        OA_ptr<ICFGDep> icfgDep,
        OA_ptr<InterUseful> interUseful)
{
  // store results that will be needed in callbacks
  mICFG = icfg;
  mParamBind = paramBind;
  mICFGDep = icfgDep;
  mInterAlias = interAlias;
  mInterUseful = interUseful;

  // create an empty InActivePerStmt
  mInActive = new InActivePerStmt();

  // call iterative data-flow solver for ICFG
  mSolver->solve(icfg);

  mInActive->setNumIter(mSolver->getNumIter());
    
  if (debug) {
      std:: cout << std::endl << "%+%+%+%+ Vary numIter = " 
       << mSolver->getNumIter() 
       << " +%+%+%+%" << std::endl << std::endl;
  }
    
  return mInActive;
}

//========================================================
// implementation of ICFGDFProblem interface
//========================================================
//--------------------------------------------------------
// initialization callbacks
//--------------------------------------------------------

/*!
   Data-flow set passed around on the call graph is an
   LocDFSet.  The top value for this is an empty set.
*/
OA_ptr<DataFlow::DataFlowSet> ManagerICFGVaryInActive::initializeTop()
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    return retval;
}

OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGVaryInActive::initializeNodeIN(OA_ptr<ICFG::ICFGStandard::Node> n)
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    
    return retval;
}

OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGVaryInActive::initializeNodeOUT(OA_ptr<ICFG::ICFGStandard::Node> n)
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    
    return retval;
 }


//--------------------------------------------------------
// solver callbacks 
//--------------------------------------------------------
  
//! OK to modify set1 and return it as result, because solver
//! only passes a tempSet in as set1
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGVaryInActive::meet(OA_ptr<DataFlow::DataFlowSet> set1, 
                      OA_ptr<DataFlow::DataFlowSet> set2)
{
    if (debug) {
        std::cout << "ManagerICFGVaryInActive::meet" << std::endl;
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

/*! 
    A helper function that determines active locations, whether
    the previous stmt was active, and which memory references in the
    previous and current stmt are active
*/
OA_ptr<DataFlow::LocDFSet> 
ManagerICFGVaryInActive::calculateActive(
        OA_ptr<LocIterator> varyIter,
        OA_ptr<LocIterator> usefulIter, StmtHandle stmt)
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;

    if (debug) {
        std::cout << "\tcalculateActive ---------------------" << std::endl;
    }

    // get set of active locations
    varyIter->reset();
    for ( ; varyIter->isValid(); (*varyIter)++ ) {
        OA_ptr<Location> loc = varyIter->current();
        if (debug) { 
            std::cout << "\t\tinVary loc = ";
            loc->dump(std::cout,mIR);
        }
        usefulIter->reset();
        for ( ; usefulIter->isValid(); (*usefulIter)++ ) {
            if (debug) { 
                std::cout << "\t\tusefulIter loc = ";
                usefulIter->current()->dump(std::cout,mIR);
            }
            if (loc->mayOverlap(*(usefulIter->current())) ) {
                retval->insert(loc);
                retval->insert(usefulIter->current());
                if (debug) {
                  std::cout << "\t\tinserting active loc = ";
                  loc->dump(std::cout,mIR);
                  std::cout << "\t\tinserting active loc = ";
                  usefulIter->current()->dump(std::cout,mIR);
                }
            }
        }
    }

    return retval;
}


//! OK to modify in set and return it again as result because
//! solver clones the BB in sets
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGVaryInActive::transfer(ProcHandle proc,
                          OA_ptr<DataFlow::DataFlowSet> in, OA::StmtHandle stmt)
{    
    // convert incoming set to more specific subclass
    OA_ptr<DataFlow::LocDFSet> inRecast = in.convert<DataFlow::LocDFSet>();
    if (debug) {
        std::cout << "In transfer, stmt(hval=" << stmt.hval() << ")= ";
        mIR->dump(stmt,std::cout);
        std::cout << "\tinRecast = ";
        inRecast->dump(std::cout,mIR);
    }

    // determine the set of incoming active locations based on InUseful
    // and InVary
    OA_ptr<UsefulStandard> useful = mInterUseful->getUsefulResults(proc);
    OA_ptr<LocIterator> usefulIter = useful->getInUsefulIterator(stmt);
    OA_ptr<LocIterator> varyIter = inRecast->getLocIterator();
    OA_ptr<DataFlow::LocDFSet> inActive 
        = calculateActive(varyIter, usefulIter, stmt);
    mInActive->copyIntoInActive(stmt, inActive);

    // create outVary set
    OA_ptr<DataFlow::LocDFSet> outVary;
    outVary = new DataFlow::LocDFSet;

    // use dep pairs to determine what locations should be in OutVary
    DataFlow::LocDFSetIterator inLocIter(*inRecast);
    for ( ; inLocIter.isValid(); ++inLocIter ) {
      OA_ptr<Location> inLoc = inLocIter.current();
      if (debug) {
          std::cout << "\tinLoc = ";
          inLoc->dump(std::cout, mIR);
      }
      OA_ptr<LocIterator> locIterPtr;
      locIterPtr = mICFGDep->getMayDefIterator(stmt,inLoc);
      for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
          outVary->insert(locIterPtr->current());
      }
    }
    
    /*
    // store outVary for stmt into procedure's Vary results
    if (mVaryMap[proc].ptrEqual(0)) {
        mVaryMap[proc] = new VaryStandard(proc);
    }
    mVaryMap[proc]->copyIntoOutVary(stmt,outVary);
*/
    if (debug) {
        std::cout << "\toutVary = ";
        outVary->dump(std::cout,mIR);
    }

    return outVary;
}

/*!
   Will get the indep vars if this procedure has any and add them
   to incoming LocDFSet
*/
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGVaryInActive::entryTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> in)
{
    // create retval as copy of in
    OA_ptr<DataFlow::LocDFSet> retval;
    OA_ptr<DataFlow::LocDFSet> inRecast = in.convert<DataFlow::LocDFSet>();
    retval = new DataFlow::LocDFSet(*inRecast);

    // get iterator over indep  locations for procedure
    OA_ptr<LocIterator> indepIter = mIR->getIndepLocIter(proc);

    for ( indepIter->reset(); indepIter->isValid(); (*indepIter)++ ) {
        retval->insert(indepIter->current());
    }
    if (debug) {
        std::cout << "\tManagerICFGVaryInActive, Indep locations for proc "
                  << mIR->toString(proc) << ": " << std::endl;
        retval->dump(std::cout, mIR);
        std::cout << "----" << std::endl;
    }

    return retval;
}

/*!
   Just pass along out because this won't be called since we are a Forward
   analysis
*/
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGVaryInActive::exitTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> out)
{
    assert(0);
    return out;
}


//! Propagate a data-flow set from caller to callee
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGVaryInActive::callerToCallee(ProcHandle caller,
    OA_ptr<DataFlow::DataFlowSet> dfset, ExprHandle call, ProcHandle callee)
{
    OA_ptr<DataFlow::LocDFSet> inRecast = dfset.convert<DataFlow::LocDFSet>();
    return inRecast->callerToCallee(call, caller, mInterAlias, mParamBind);
}
  
//! Propagate a data-flow set from callee to caller
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGVaryInActive::calleeToCaller(ProcHandle callee,
    OA_ptr<DataFlow::DataFlowSet> dfset, ExprHandle call, ProcHandle caller)
{
    OA_ptr<DataFlow::LocDFSet> inRecast = dfset.convert<DataFlow::LocDFSet>();
    return inRecast->calleeToCaller(call, caller, mInterAlias, mParamBind);
}

  } // end of namespace Activity
} // end of namespace OA
