/*! \file
  
  \brief The AnnotationManager that generates InterVary using ICFGDFSolver..

  \authors Michelle Strout
  \version $Id: ManagerICFGVary.cpp,v 1.2 2005/06/10 02:32:02 mstrout Exp $

  Copyright (c) 2002-2005, Rice University <br>
  Copyright (c) 2004-2005, University of Chicago <br>
  Copyright (c) 2006, Contributors <br>
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "ManagerICFGVary.hpp"
#include <Utils/Util.hpp>


namespace OA {
  namespace Activity {

static bool debug = false;

/*!
*/
ManagerICFGVary::ManagerICFGVary( OA_ptr<ActivityIRInterface> _ir) : mIR(_ir)
{
    OA_DEBUG_CTRL_MACRO("DEBUG_ManagerICFGVary:ALL", debug);
    mSolver = new DataFlow::ICFGDFSolver(DataFlow::ICFGDFSolver::Forward,*this);
}

OA_ptr<Activity::InterVary> 
ManagerICFGVary::performAnalysis(
        OA_ptr<ICFG::ICFGInterface> icfg,
        OA_ptr<DataFlow::ParamBindings> paramBind,
        OA_ptr<Alias::InterAliasInterface> interAlias,
        OA_ptr<ICFGDep> icfgDep,
        DataFlow::DFPImplement algorithm)
{
  // store results that will be needed in callbacks
  mICFG = icfg;
  mParamBind = paramBind;
  mICFGDep = icfgDep;
  mInterAlias = interAlias;

  // create an empty InterVary
  mInterVary = new InterVary();

  // call iterative data-flow solver for ICFG
  mSolver->solve(icfg,algorithm);

  mInterVary->setNumIter(mSolver->getNumIter());
    
  if (debug) {
      std:: cout << std::endl << "%+%+%+%+ Vary numIter = " 
       << mSolver->getNumIter() 
       << " +%+%+%+%" << std::endl << std::endl;
  }
    
  // assign dep results for each procedure into InterVary
  std::map<ProcHandle,OA_ptr<VaryStandard> >::iterator mapIter;
  for (mapIter=mVaryMap.begin(); mapIter!=mVaryMap.end(); mapIter++) {
    mInterVary->mapProcToVary(mapIter->first,mapIter->second);
  }

  return mInterVary;
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
OA_ptr<DataFlow::DataFlowSet> ManagerICFGVary::initializeTop()
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    return retval;
}

OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGVary::initializeNodeIN(OA_ptr<ICFG::NodeInterface> n)
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    
    return retval;
}

OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGVary::initializeNodeOUT(OA_ptr<ICFG::NodeInterface> n)
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
ManagerICFGVary::meet(OA_ptr<DataFlow::DataFlowSet> set1, 
                      OA_ptr<DataFlow::DataFlowSet> set2)
{
    if (debug) {
        std::cout << "ManagerICFGVary::meet" << std::endl;
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
ManagerICFGVary::transfer(ProcHandle proc,
                          OA_ptr<DataFlow::DataFlowSet> in, OA::StmtHandle stmt)
{    
    // convert incoming set to more specific subclass
    OA_ptr<DataFlow::LocDFSet> inRecast = in.convert<DataFlow::LocDFSet>();
    OA_ptr<DataFlow::LocDFSet> outVary;
    outVary = new DataFlow::LocDFSet;
    if (debug) {
        std::cout << "In transfer, stmt(hval=" << stmt.hval() << ")= ";
        mIR->dump(stmt,std::cout);
        std::cout << "\tinRecast = ";
        inRecast->dump(std::cout,mIR);
    }

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
    
    // store outVary for stmt into procedure's Vary results
    if (mVaryMap[proc].ptrEqual(0)) {
        mVaryMap[proc] = new VaryStandard(proc);
    }
    mVaryMap[proc]->copyIntoOutVary(stmt,outVary);

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
ManagerICFGVary::entryTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> in)
{
    // create retval as copy of in
    OA_ptr<DataFlow::LocDFSet> retval;
    OA_ptr<DataFlow::LocDFSet> inRecast = in.convert<DataFlow::LocDFSet>();
    retval = new DataFlow::LocDFSet(*inRecast);

    // get iterator over indep MemRefExpr for procedure
    OA_ptr<MemRefExprIterator> indepIter = mIR->getIndepMemRefExprIter(proc);
    // get alias results for this procedure
    OA_ptr<Alias::Interface> alias = mInterAlias->getAliasResults(proc);

    if (mVaryMap[proc].ptrEqual(0)) {
        mVaryMap[proc] = new VaryStandard(proc);
    }
    for ( indepIter->reset(); indepIter->isValid(); (*indepIter)++ ) {
      OA_ptr<MemRefExpr> memref = indepIter->current();

      // get may locs for memref
      OA_ptr<LocIterator> locIter = alias->getMayLocs(*memref,proc);
      for (locIter->reset(); locIter->isValid(); (*locIter)++ ) {
        retval->insert(locIter->current());
        mVaryMap[proc]->insertIndepLoc(locIter->current());
      }
    }
    if (debug) {
        std::cout << "\tManagerICFGVary, Indep locations for proc "
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
ManagerICFGVary::exitTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> out)
{
    assert(0);
    return out;
}


//! Propagate a data-flow set from caller to callee
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGVary::callerToCallee(ProcHandle caller,
    OA_ptr<DataFlow::DataFlowSet> dfset, CallHandle call, ProcHandle callee)
{
    OA_ptr<DataFlow::LocDFSet> inRecast = dfset.convert<DataFlow::LocDFSet>();
    return inRecast->callerToCallee(caller,call,callee,mInterAlias,
                                    mParamBind,mIR);
}
  
//! Propagate a data-flow set from callee to caller
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGVary::calleeToCaller(ProcHandle callee,
    OA_ptr<DataFlow::DataFlowSet> dfset, CallHandle call, ProcHandle caller)
{
    OA_ptr<DataFlow::LocDFSet> inRecast = dfset.convert<DataFlow::LocDFSet>();
    return inRecast->calleeToCaller(callee,call,caller,mInterAlias,
                                    mParamBind,mIR);
}

  } // end of namespace Activity
} // end of namespace OA
