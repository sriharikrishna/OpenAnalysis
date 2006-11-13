/*! \file
  
  \brief Implementation of InterActive

  \author Michelle Strout
  \version $Id: InterActive.cpp,v 1.6 2005/06/10 02:32:02 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "InterActive.hpp"

namespace OA {
  namespace Activity {

InterActive::InterActive()
{
    mUnknownLocActive = false;
    mNumIterUseful = 0;
    mNumIterVary = 0;
    mSizeInBytes = 0;
}

//! Return an iterator over all procedures for which activity
//! analysis is available
OA_ptr<ProcHandleIterator> InterActive::getKnownProcIterator()
{         
    // create a procSet
    OA_ptr<std::set<ProcHandle> > procSet;
    procSet = new std::set<ProcHandle>;
    
    // loop through map to add to list of ProcHandle's
    std::map<ProcHandle,OA_ptr<Activity::ActiveStandard> >::iterator mIter;
    for (mIter=mProcToActiveMap.begin(); mIter != mProcToActiveMap.end(); mIter++)
    {
        procSet->insert(mIter->first);
    }

    OA_ptr<ActivityProcIter> retval;
    retval = new ActivityProcIter(procSet);
    return retval;
}

//! Indicate whether the given symbol is active or not within any procedure
bool InterActive::isActive(SymHandle sym)
{
  // an unknown location is active, therefore all symbols are active
  if (mUnknownLocActive) {
    return true;
  } else if (mActiveSymSet.find(sym) != mActiveSymSet.end()) {
    return true;
  } else {
    return false;
  }  
}

//! Indicate whether the given stmt is active or not within given proc
bool InterActive::isActive(ProcHandle proc, StmtHandle stmt)
{
    if (mProcToActiveMap[proc].ptrEqual(0)) {
      assert(0);  // we don't know about this proc
    } else {
      return mProcToActiveMap[proc]->isActive(stmt);
    }
    return false; // default return to make compilers happy
}

//! Indicate whether the given memref is active or not within given proc
bool InterActive::isActive(ProcHandle proc, MemRefHandle memref)
{
    if (mProcToActiveMap[proc].ptrEqual(0)) {
      assert(0);  // we don't know about this proc
    } else {
      return mProcToActiveMap[proc]->isActive(memref);
    }
    return false; // default return to make compilers happy
}


OA_ptr<LocIterator> 
InterActive::getActiveLocsIterator(ProcHandle proc)
{
    OA_ptr<LocIterator> retval;

    if (mProcToActiveMap[proc].ptrEqual(0)) {
      assert(0);  // we don't know about this proc
    } else {
      retval = mProcToActiveMap[proc]->getActiveLocsIterator();
    }

    return retval;
}

OA_ptr<StmtHandleIterator> 
InterActive::getActiveStmtIterator(ProcHandle proc)
{
    OA_ptr<StmtHandleIterator> retval;

    if (mProcToActiveMap[proc].ptrEqual(0)) {
      assert(0);  // we don't know about this proc
    } else {
      retval = mProcToActiveMap[proc]->getActiveStmtIterator();
    }

    return retval;
}

OA_ptr<MemRefHandleIterator> 
InterActive::getActiveMemRefIterator(ProcHandle proc) 
{
    OA_ptr<MemRefHandleIterator> retval;

    if (mProcToActiveMap[proc].ptrEqual(0)) {
      assert(0);  // we don't know about this proc
    } else {
      retval = mProcToActiveMap[proc]->getActiveMemRefIterator();
    }

    return retval;
}

//! Associate the given procedure with the given Activity info
void InterActive::mapProcToActive(ProcHandle proc, 
    OA_ptr<Activity::ActiveStandard> active)
{
    mProcToActiveMap[proc] = active;

    // get all known active symbols from the procedure so we know all active
    // symbols in program
    OA_ptr<SymHandleIterator> symIter = active->getActiveSymIterator();
    for ( ; symIter->isValid(); (*symIter)++) {
        mActiveSymSet.insert(symIter->current());
    }
 
    // if the procedure has an UnknownLoc that is active then must indicate
    // that for whole program
    if (active->getUnknownLocActive()==true) {
        mUnknownLocActive = true;
    }
}

//! Return an iterator for set of active symbols
OA_ptr<SymHandleIterator> InterActive::getActiveSymIterator() 
{
  OA_ptr<SymHandleIterator> retval;
  OA_ptr<std::set<SymHandle> > retSet;
  retSet = new std::set<SymHandle>;
  std::set<SymHandle>::iterator symIter;
  for (symIter = mActiveSymSet.begin(); symIter!=mActiveSymSet.end();
       symIter++) 
  {
    retSet->insert(*symIter);
  }
  retval = new ActiveSymIterator(retSet);
  return retval;
}


//! incomplete output of info for debugging
void InterActive::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    std::cout << "====================== InterActive" << std::endl;

    // Loop through all known procedures
    OA_ptr<ProcHandleIterator> procIterPtr = getKnownProcIterator();
    for (; procIterPtr->isValid(); (*procIterPtr)++) {
        ProcHandle proc = procIterPtr->current();
        ir->currentProc(proc);

        // print all sets for each procedure
        os << "Procedure( " << ir->toString(proc) << " )" << std::endl;

        OA_ptr<LocIterator> locIterPtr;

        os << "\tActiveLocSet = ";
        locIterPtr = getActiveLocsIterator(proc);
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
          locIterPtr->current()->dump(std::cout,ir);
        }
        os << std::endl;

        os << "\tActiveStmtList = " << std::endl;
        OA_ptr<StmtHandleIterator> stmtIterPtr = getActiveStmtIterator(proc);
        for ( ; stmtIterPtr->isValid(); (*stmtIterPtr)++ ) {
            os << ir->toString(stmtIterPtr->current()) << std::endl;
        }
        os << std::endl;

        os << "\tActiveMemRefList = " << std::endl;
        OA_ptr<MemRefHandleIterator> memrefIterPtr = getActiveMemRefIterator(proc);
        for ( ; memrefIterPtr->isValid(); (*memrefIterPtr)++ ) {
            os << ir->toString(memrefIterPtr->current()) << std::endl;
        }
        os << std::endl;
    }

    os << "\tActiveSymList" << std::endl;
    std::set<SymHandle>::iterator symIter;
    for (symIter = mActiveSymSet.begin(); symIter!=mActiveSymSet.end();
         symIter++) 
    {
        os << "\t\t" << ir->toString(*symIter) << " hval = " 
           << (*symIter).hval() 
           << ", active = " << isActive(*symIter) << std::endl;
    }

    os << "\tgetActiveSizeInBytes() = " << getActiveSizeInBytes() << std::endl;

}


  } // end of Activity namespace
} // end of OA namespace
