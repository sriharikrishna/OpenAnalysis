/*! \file
  
  \brief Implementation of Activity::ActiveStandard

  \author Michelle Strout
  \version $Id: ActiveStandard.cpp,v 1.8 2005/06/10 02:32:01 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ActiveStandard.hpp"

namespace OA {
  namespace Activity {

ActiveStandard::ActiveStandard(ProcHandle p) 
{ mActiveStmtSet = new std::set<StmtHandle>;
  mActiveMemRefSet = new std::set<MemRefHandle>;
  mActiveLocSet = new DataFlow::LocDFSet;
  mActiveSymSet = new std::set<SymHandle>;
  mUnknownLocActive = false;
}

//! Return an iterator for set of active locations
OA_ptr<LocIterator> ActiveStandard::getActiveLocsIterator() 
{ OA_ptr<LocIterator> retval;
  retval = new DataFlow::LocDFSetIterator(*mActiveLocSet);
  return retval;
}

//! Return an iterator for set of active stmts
OA_ptr<StmtHandleIterator> ActiveStandard::getActiveStmtIterator() 
{
  OA_ptr<StmtHandleIterator> retval;
  retval = new ActiveStmtIterator(mActiveStmtSet);
  return retval;
}

//! Return an iterator for set of active memory references
OA_ptr<MemRefHandleIterator> ActiveStandard::getActiveMemRefIterator() 
{
  OA_ptr<MemRefHandleIterator> retval;
  retval = new ActiveMemRefIterator(mActiveMemRefSet);
  return retval;
}


//! Return an iterator for set of active symbols
OA_ptr<SymHandleIterator> ActiveStandard::getActiveSymIterator() 
{
  OA_ptr<SymHandleIterator> retval;
  retval = new ActiveSymIterator(mActiveSymSet);
  return retval;
}



//! Indicate whether the given sym is active or not
bool ActiveStandard::isActive(SymHandle sym)
{
  // an unknown location is active, therefore all symbols are active
  if (mUnknownLocActive) {
    return true;
  } else if (mActiveSymSet->find(sym) != mActiveSymSet->end()) {
    return true;
  } else {
    return false;
  }  
}

//! Indicate whether the given stmt is active or not
bool ActiveStandard::isActive(StmtHandle stmt)
{
  if (mActiveStmtSet->find(stmt) != mActiveStmtSet->end()) {
    return true;
  } else {
    return false;
  }  
}

//! Indicate whether the given memref is active or not
bool ActiveStandard::isActive(MemRefHandle memref)
{
  if (mActiveMemRefSet->find(memref) != mActiveMemRefSet->end()) {
    return true;
  } else {
    return false;
  }  
}

//*****************************************************************
// Construction methods 
//*****************************************************************

//! insert an active location
void ActiveStandard::insertLoc(OA_ptr<Location> loc) 
{ 
    mActiveLocSet->insert(loc); 

    // translate into a Symbol, right now only handle UnknownLoc, NamedLoc
    // and InvisibleLoc for reference parameters
    // FIXME: I have got to implement a visitor pattern for this kind of thing

    // Unknown
    if (loc->isaUnknown()) {
      mUnknownLocActive = true;

    // Named
    } else if (loc->isaNamed()) {
      OA_ptr<NamedLoc> nloc = loc.convert<NamedLoc>();
      mActiveSymSet->insert( nloc->getSymHandle() );

      OA_ptr<SymHandleIterator> symIter = nloc->getFullOverlapIter();
      for ( ; symIter->isValid(); (*symIter)++) {
          mActiveSymSet->insert(symIter->current());
      }
      symIter = nloc->getPartOverlapIter();
      for ( ; symIter->isValid(); (*symIter)++) {
          mActiveSymSet->insert(symIter->current());
      }
 
    // Unnamed
    } else if (loc->isaUnnamed()) {
      assert(0);
      // not handling this yet

    // Invisible
    } else if (loc->isaInvisible()) {
      OA_ptr<InvisibleLoc> invLoc 
          = loc.convert<InvisibleLoc>();
      OA_ptr<MemRefExpr> mre = invLoc->getMemRefExpr();

      // get symbol from memory reference expression if no derefs
      // FIXME: here need another visitor for MemRefExpr, for now assuming
      // only named ones
      if (mre->isaNamed()) {
          OA_ptr<NamedRef> namedRef 
              = namedRef.convert<NamedRef>();
          mActiveSymSet->insert( namedRef->getSymHandle() );
      } else {
          assert(0);
      }

    // LocSubSet
    } else if (loc->isaSubSet()) {
      OA_ptr<Location> baseLoc = loc->getBaseLoc();
      // FIXME: now really want to call visitor on this guy but instead will just
      // call this function recursively
      insertLoc(baseLoc);

    } else {
      assert(0);
    }

      
}

//! insert an active Stmt
void ActiveStandard::insertStmt(StmtHandle stmt) 
{ mActiveStmtSet->insert(stmt); }

//! insert an active MemRef
void ActiveStandard::insertMemRef(MemRefHandle memref) 
{ mActiveMemRefSet->insert(memref); }



//*****************************************************************
// Output
//*****************************************************************

//! incomplete output of info for debugging
void ActiveStandard::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    os << "ActiveLocSet = " << std::endl;
    mActiveLocSet->dump(os,ir);

    os << std::endl << "ActiveSymSet = " << std::endl;
    OA_ptr<SymHandleIterator> symIterPtr;
    symIterPtr = new ActiveSymIterator(mActiveSymSet);
    for ( ; symIterPtr->isValid(); (*symIterPtr)++ ) {
        os << ir->toString(symIterPtr->current()) << std::endl;
    }
    os << "mUnknownLocActive = " << mUnknownLocActive << std::endl;

    os << std::endl << "ActiveStmtList = " << std::endl;
    OA_ptr<StmtHandleIterator> stmtIterPtr = getActiveStmtIterator();
    for ( ; stmtIterPtr->isValid(); (*stmtIterPtr)++ ) {
        os << ir->toString(stmtIterPtr->current()) << std::endl;
    }

    os << std::endl << "ActiveMemRefList = " << std::endl;
    OA_ptr<MemRefHandleIterator> memrefIterPtr = getActiveMemRefIterator();
    for ( ; memrefIterPtr->isValid(); (*memrefIterPtr)++ ) {
        os << ir->toString(memrefIterPtr->current()) << std::endl;
    }

}


  } // end of Activity namespace
} // end of OA namespace
