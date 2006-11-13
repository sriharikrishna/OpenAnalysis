/*! \file
  
  \brief The AnnotationManager that generates InterUseful using ICFGDFSolver.

  \authors Michelle Strout
  \version $Id: ManagerICFGUseful.cpp,v 1.2 2005/06/10 02:32:02 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerICFGUseful.hpp"


namespace OA {
  namespace Activity {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerICFGUseful)
static bool debug = true;
#else
static bool debug = false;
#endif

/*!
*/
ManagerICFGUseful::ManagerICFGUseful( OA_ptr<ActivityIRInterface> _ir) : mIR(_ir)
{
  mSolver = new DataFlow::ICFGDFSolver(DataFlow::ICFGDFSolver::Backward,*this);
}

OA_ptr<Activity::InterUseful> 
ManagerICFGUseful::performAnalysis(
        OA_ptr<ICFG::ICFGStandard> icfg,
        OA_ptr<DataFlow::ParamBindings> paramBind,
        OA_ptr<Alias::InterAliasInterface> interAlias,
        OA_ptr<ICFGDep> icfgDep)
{
  // store results that will be needed in callbacks
  mICFG = icfg;
  mParamBind = paramBind;
  mICFGDep = icfgDep;
  mInterAlias = interAlias;

  // create an empty InterUseful
  mInterUseful = new InterUseful();

  // call iterative data-flow solver for ICFG
  mSolver->solve(icfg);

  if (debug) {
      std::cout << std::endl << "%+%+%+%+ Useful numIter = " 
                << mSolver->getNumIter() 
                << " +%+%+%+%" << std::endl << std::endl;
  }

  mInterUseful->setNumIter(mSolver->getNumIter());
    
  // assign results for each procedure into InterUseful
  std::map<ProcHandle,OA_ptr<UsefulStandard> >::iterator mapIter;
  for (mapIter=mUsefulMap.begin(); mapIter!=mUsefulMap.end(); mapIter++) {
    mInterUseful->mapProcToUseful(mapIter->first,mapIter->second);
  }

  return mInterUseful;
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
OA_ptr<DataFlow::DataFlowSet> ManagerICFGUseful::initializeTop()
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    return retval;
}

OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGUseful::initializeNodeIN(OA_ptr<ICFG::ICFGStandard::Node> n)
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    
    return retval;
}

OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGUseful::initializeNodeOUT(OA_ptr<ICFG::ICFGStandard::Node> n)
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    return retval;
 }


//--------------------------------------------------------
// solver callbacks 
//--------------------------------------------------------
  
/*! Unions location sets
 */
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGUseful::meet(OA_ptr<DataFlow::DataFlowSet> set1, 
                      OA_ptr<DataFlow::DataFlowSet> set2)
{
    if (debug) {
        std::cout << "ManagerICFGUseful::meet" << std::endl;
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

//! OK to modify in set and return it again as result because
//! solver clones the BB in sets
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGUseful::transfer(ProcHandle proc,
                        OA_ptr<DataFlow::DataFlowSet> out, OA::StmtHandle stmt)
{    
    // convert incoming set to more specific subclass
    OA_ptr<DataFlow::LocDFSet> outRecast = out.convert<DataFlow::LocDFSet>();
    OA_ptr<DataFlow::LocDFSet> inUseful;
    inUseful = new DataFlow::LocDFSet;
    if (debug) {
        std::cout << "In transfer, stmt(hval=" << stmt.hval() << ")= ";
        mIR->dump(stmt,std::cout);
        std::cout << "\toutRecast = ";
        outRecast->dump(std::cout,mIR);
    }

    // use dep pairs to determine what locations should be in OutUseful
    DataFlow::LocDFSetIterator outLocIter(*outRecast);
    for ( ; outLocIter.isValid(); ++outLocIter ) {
      OA_ptr<Location> outLoc = outLocIter.current();
      if (debug) {
          std::cout << "\toutLoc = ";
          outLoc->dump(std::cout, mIR);
      }
      OA_ptr<LocIterator> locIterPtr;
      locIterPtr = mICFGDep->getDiffUseIterator(stmt,outLoc);
      for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
          inUseful->insert(locIterPtr->current());
      }
    }
    
    // store inUseful for stmt into procedure's Useful results
    if (mUsefulMap[proc].ptrEqual(0)) {
        mUsefulMap[proc] = new UsefulStandard(proc);
    }
    mUsefulMap[proc]->copyIntoInUseful(stmt,inUseful);

    if (debug) {
        std::cout << "\tinUseful = ";
        inUseful->dump(std::cout,mIR);
    }

    return inUseful;
}

/*!
   Will get the dep vars if this procedure has any and add them
   to incoming LocDFSet
*/
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGUseful::exitTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> out)
{
    // create retval as copy of out
    OA_ptr<DataFlow::LocDFSet> retval;
    OA_ptr<DataFlow::LocDFSet> outRecast = out.convert<DataFlow::LocDFSet>();
    retval = new DataFlow::LocDFSet(*outRecast);

    // get iterator over dep  locations for procedure
    OA_ptr<LocIterator> depIter = mIR->getDepLocIter(proc);

    if (mUsefulMap[proc].ptrEqual(0)) {
        mUsefulMap[proc] = new UsefulStandard(proc);
    }
    for ( ; depIter->isValid(); (*depIter)++ ) {
        retval->insert(depIter->current());
        mUsefulMap[proc]->insertDepLoc(depIter->current());
    }
    if (debug) {
        std::cout << "\tManagerICFGUseful, Dep locations for proc "
                  << mIR->toString(proc) << ": " << std::endl;
        retval->dump(std::cout, mIR);
        std::cout << "----" << std::endl;
    }

    return retval;
}

/*!
   Just pass along out because this won't be called since we are a Backward
   analysis
*/
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGUseful::entryTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> in)
{
    assert(0);
    return in;
}


//! Propagate a data-flow set from caller to callee
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGUseful::callerToCallee(ProcHandle caller,
    OA_ptr<DataFlow::DataFlowSet> dfset, ExprHandle call, ProcHandle callee)
{
    OA_ptr<DataFlow::LocDFSet> inRecast = dfset.convert<DataFlow::LocDFSet>();
    return inRecast->callerToCallee(call, caller, mInterAlias, mParamBind);
}
  
//! Propagate a data-flow set from callee to caller
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGUseful::calleeToCaller(ProcHandle callee,
    OA_ptr<DataFlow::DataFlowSet> dfset, ExprHandle call, ProcHandle caller)
{
    OA_ptr<DataFlow::LocDFSet> inRecast = dfset.convert<DataFlow::LocDFSet>();
    return inRecast->calleeToCaller(call, caller, mInterAlias, mParamBind);
}

  } // end of namespace Activity
} // end of namespace OA
