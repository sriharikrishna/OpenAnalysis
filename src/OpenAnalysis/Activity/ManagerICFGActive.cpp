/*! \file
  
  \brief The AnnotationManager that generates InterActive INTERprocedurally.

  \authors Michelle Strout
  \version $Id: ManagerICFGActive.cpp,v 1.3 2005/07/01 02:49:56 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include <iostream>
#include "ManagerICFGActive.hpp"



namespace OA {
  namespace Activity {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerICFGActive)
static bool debug = true;
#else
static bool debug = false;
#endif

ManagerICFGActive::ManagerICFGActive(
    OA_ptr<Activity::ActivityIRInterface> _ir) : mIR(_ir)
{
    mSolver = new DataFlow::ICFGDFSolver(DataFlow::ICFGDFSolver::Backward,*this);
}

/*!
   First calls Manager's to get ICFGDep and ICFGUseful results.

   Then does Vary and Activity analysis at same time over the ICFG using
   an ICFGDFSolver.
*/
OA_ptr<InterActive> 
ManagerICFGActive::performAnalysis(
        OA_ptr<CallGraph::Interface> callGraph,
        OA_ptr<ICFG::ICFGStandard> icfg,
        OA_ptr<DataFlow::ParamBindings> paramBind,
        OA_ptr<Alias::InterAliasInterface> interAlias,
        OA_ptr<SideEffect::InterSideEffectInterface> interSE,
        OA_ptr<CFG::EachCFGInterface> eachCFG)
{
  OA_ptr<InterActive> retval;
  retval = new InterActive;

  // create a Manager that generates dep information for each statement in
  // ICFG
  OA_ptr<ManagerICFGDep> depman;
  depman = new ManagerICFGDep(mIR);
  OA_ptr<ICFGDep> icfgDep = depman->performAnalysis(icfg, paramBind,
                                                    interAlias);

  if (debug) { icfgDep->dump(std::cout, mIR); }

 
  if (debug) {
      std::cout << "Calling usefulman->performAnalysis() ...\n";
      std::cout.flush();
  }

  OA_ptr<ManagerICFGUseful> usefulman;
  usefulman = new ManagerICFGUseful(mIR);
  OA_ptr<InterUseful> interUseful = usefulman->performAnalysis(icfg, 
          paramBind, interAlias, icfgDep);

  retval->setNumIterUseful(interUseful->getNumIter());
  if (debug) { interUseful->dump(std::cout, mIR); }
  
  // ManagerICFGVary does vary analysis and determines which
  // locations are active coming into a stmt
  if (debug) {
      std::cout << "Calling varyman->performAnalysis() ...\n";
      std::cout.flush();
  }

  OA_ptr<ManagerICFGVaryInActive> varyman;
  varyman = new ManagerICFGVaryInActive(mIR);
  OA_ptr<InActivePerStmt> inActive = varyman->performAnalysis(icfg, paramBind,
          interAlias, icfgDep, interUseful);

  //FIXME: there is no getNumIter method to in InActivePerStmt
  //retval->setNumIterVary(inActive->getNumIter());
  if (debug) { inActive->dump(std::cout, mIR); }
  
  
  //-------------------------------------------------------------
  // Do backward dataflow analysis to determine which def memrefs
  // and stmts are active
  
  // store results that will be needed in callbacks
  mICFG = icfg;
  mParamBind = paramBind;
  mICFGDep = icfgDep;
  mInterAlias = interAlias;
  mInActive = inActive;

  // create an empty InterActive
  mInterActive = new InterActive();

  // call iterative data-flow solver for ICFG
  mSolver->solve(icfg);

  // then iterate over each statement to find active use memrefs

  // assign activity results for each procedure into InterActive
  std::map<ProcHandle,OA_ptr<ActiveStandard> >::iterator mapIter;
  for (mapIter=mActiveMap.begin(); mapIter!=mActiveMap.end(); mapIter++) {
    retval->mapProcToActive(mapIter->first,mapIter->second);
  }

  // iterate over all symbols to determine the size
  OA_ptr<SymHandleIterator> symIter = retval->getActiveSymIterator();
  int bytes = 0;
  for ( ; symIter->isValid(); (*symIter)++ ) {
      SymHandle sym = symIter->current();

      bytes += mIR->getSizeInBytes(sym);

      if (debug) {
          std::cout << "ManagerICFGActive: sym = " << mIR->toString(sym)
                    << ", size = " << mIR->getSizeInBytes(sym) << ", bytes = "
                    << bytes << std::endl;
      }
  }
  retval->setActiveSizeInBytes(bytes);
 
  return retval;
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
OA_ptr<DataFlow::DataFlowSet> ManagerICFGActive::initializeTop()
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    return retval;
}

OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGActive::initializeNodeIN(OA_ptr<ICFG::ICFGStandard::Node> n)
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;
    
    return retval;
}

OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGActive::initializeNodeOUT(OA_ptr<ICFG::ICFGStandard::Node> n)
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
ManagerICFGActive::meet(OA_ptr<DataFlow::DataFlowSet> set1, 
                      OA_ptr<DataFlow::DataFlowSet> set2)
{
    if (debug) {
        std::cout << "ManagerICFGActive::meet" << std::endl;
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
ManagerICFGActive::transfer(ProcHandle proc,
                        OA_ptr<DataFlow::DataFlowSet> out, OA::StmtHandle stmt)
{    
    // convert incoming set to more specific subclass
    OA_ptr<DataFlow::LocDFSet> outRecast = out.convert<DataFlow::LocDFSet>();
    if (debug) {
        std::cout << "In transfer, stmt(hval=" << stmt.hval() << ")= ";
        mIR->dump(stmt,std::cout);
        std::cout << "\toutRecast = ";
        outRecast->dump(std::cout,mIR);
    }


    // will be storing activity results for stmt and memrefs in
    // ActiveStandard for current procedure
    if (mActiveMap[proc].ptrEqual(0)) {
        mActiveMap[proc] = new ActiveStandard(proc);
    }
        
    // get alias and active results for current procedure
    OA_ptr<Alias::Interface> alias = mInterAlias->getAliasResults(proc);
    OA_ptr<ActiveStandard> active = mActiveMap[proc];

    // iterate over def mem refs and if any of them access a location
    // that is in the out active set then the mem ref is active and so
    // is the stmt
    bool activeStmt = false;
    OA_ptr<MemRefHandleIterator> defIter = mIR->getDefMemRefs(stmt);
    for (; defIter->isValid(); (*defIter)++ ) {
        MemRefHandle def = defIter->current();
        if (debug) {
          std::cout << "\tdef = ";
          mIR->dump(def,std::cout);
        }

        // loop over may locs for the memref
        OA_ptr<LocIterator> locIter = alias->getMayLocs(def);
        for ( ; locIter->isValid(); (*locIter)++ ) {
          OA_ptr<Location> loc = locIter->current();
          if (debug) { 
            std::cout << "\t\t mayloc for def = ";
            loc->dump(std::cout,mIR);
          }
 
          if (outRecast->hasOverlapLoc(loc)) {
            active->insertMemRef(def);
            active->insertStmt(stmt);
            activeStmt = true;
          }
        }
    } 

    // if this is an active statement then check the uses to
    // see if they are active
    if (activeStmt) {
      OA_ptr<MemRefHandleIterator> useIter = mIR->getUseMemRefs(stmt);
      for (; useIter->isValid(); (*useIter)++ ) {
          MemRefHandle use = useIter->current();
          if (debug) {
            std::cout << "\tuse = ";
            mIR->dump(use,std::cout);
          }

          // iterate over locations that are active anywhere
          // in procedure, flow-insensitive active locations
          OA_ptr<LocIterator> procActiveLocIter 
              = active->getActiveLocsIterator();
          for ( ; procActiveLocIter->isValid(); (*procActiveLocIter)++ ) {
            OA_ptr<Location> activeLoc = procActiveLocIter->current();
            if (debug) { 
              std::cout << "\t activeLoc = ";
              activeLoc->dump(std::cout,mIR);
            }
                  
            // iterate over may locs for use
            OA_ptr<LocIterator> mayLocIter = alias->getMayLocs(use);
            for ( ; mayLocIter->isValid(); (*mayLocIter)++ ) {
              OA_ptr<Location> useLoc = mayLocIter->current();
              if (debug) { 
                std::cout << "\t\t may loc for use, useLoc = ";
                useLoc->dump(std::cout,mIR);
              }
              if (activeLoc->mayOverlap(*useLoc)) {
                  active->insertMemRef(use);
              }
            }
          } 
      } // iterate over uses

    } // if activestmt

    // iterate through all of the active locations that come into this
    // statement and insert those locations as active within the
    // procedure
    OA_ptr<DataFlow::LocDFSet> inActiveSet = mInActive->getInActiveSet(stmt);
    OA_ptr<LocIterator> locIter = inActiveSet->getLocIterator();
    for ( ; locIter->isValid(); (*locIter)++ ) {
        active->insertLoc(locIter->current());
    } 

    // return the inActive for this stmt
    return mInActive->getInActiveSet(stmt);
}

/*!
   Will get the indep vars if this procedure has any and add them
   to incoming LocDFSet
*/
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGActive::entryTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> in)
{
    return in;
}

/*!
*/
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGActive::exitTransfer(ProcHandle proc, OA_ptr<DataFlow::DataFlowSet> out)
{
    return out;
}


//! Propagate a data-flow set from caller to callee
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGActive::callerToCallee(ProcHandle caller,
    OA_ptr<DataFlow::DataFlowSet> dfset, ExprHandle call, ProcHandle callee)
{
    OA_ptr<DataFlow::LocDFSet> inRecast = dfset.convert<DataFlow::LocDFSet>();
    return inRecast->callerToCallee(call, caller, mInterAlias, mParamBind);
}
  
//! Propagate a data-flow set from callee to caller
OA_ptr<DataFlow::DataFlowSet> 
ManagerICFGActive::calleeToCaller(ProcHandle callee,
    OA_ptr<DataFlow::DataFlowSet> dfset, ExprHandle call, ProcHandle caller)
{
    OA_ptr<DataFlow::LocDFSet> inRecast = dfset.convert<DataFlow::LocDFSet>();
    return inRecast->calleeToCaller(call, caller, mInterAlias, mParamBind);
}



  } // end of namespace Activity
} // end of namespace OA
