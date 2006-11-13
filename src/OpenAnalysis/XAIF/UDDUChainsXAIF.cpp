/*! \file
  
  \brief Implementation of XAIF::UDDUChainsXAIF

  \author Michelle Strout
  \version $Id: UDDUChainsXAIF.cpp,v 1.12 2005/01/17 18:46:34 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include <OpenAnalysis/XAIF/UDDUChainsXAIF.hpp>

namespace OA {
  namespace XAIF {

int UDDUChainsXAIF::mCurrentStartId = 3;

UDDUChainsXAIF::UDDUChainsXAIF(ProcHandle p) 
{
    // initialize the maps
    mIdToStmtSetMap = new std::map<int,OA_ptr<StmtSet> >;
    mMemRefToIdMap = new std::map<MemRefHandle,int>;

    // have the first set be empty
    StmtSet emptySet;
    addStmtSet(emptySet,1);

    // start out with StmtHandle(0) in the second set
    addStmt(StmtHandle(0),2);
}

//! Return an iterator over all statements in this procedure
//! that may define a location used by the given memory reference
OA_ptr<UDDUChainsXAIF::ChainStmtIterator> 
UDDUChainsXAIF::getUDChainStmtIterator(MemRefHandle h)
{
    OA_ptr<ChainStmtIterator> retval;

    // if haven't seen this MemRef before then just create an empty set
    // and return an iterator over that
    std::map<MemRefHandle,int>::iterator pos = mMemRefToIdMap->find(h);
    if (pos == mMemRefToIdMap->end()) {
      OA_ptr<StmtSet> emptySet;  emptySet = new StmtSet;
      retval = new ChainStmtIterator(emptySet);
 
    // otherwise return an iterator over the set with which
    // this mem ref is associated
    } else {
      retval = new ChainStmtIterator((*mIdToStmtSetMap)[pos->second]);
    } 

    return retval; 
}

//! Return an iterator over all statements in this procedure
//! that may use a location defined by the given memory reference
//! Assumes that the given memory reference is a define
OA_ptr<UDDUChainsXAIF::ChainStmtIterator> 
UDDUChainsXAIF::getDUChainStmtIterator(MemRefHandle h)
{
  // DU and UD chains are stored in the same fashion
  return getUDChainStmtIterator(h);
}

//! get the UDDUChain Id for a particular memory reference
int UDDUChainsXAIF::getUDDUChainId(MemRefHandle ref)
{
    int retval;

    // if have seen this MemRef before then return setId
    std::map<MemRefHandle,int>::iterator pos = mMemRefToIdMap->find(ref);
    if (pos != mMemRefToIdMap->end()) {
      retval = pos->second;
    } else {
      // we don't have a chaing for this memory reference so indicate that
      // it has an empty chain by returning the zeroth chain
      retval = 0;
    }

    return retval;
}

//! for a particular UDDUChainId, get an iterator over the statements
//! involved in that chain
OA_ptr<UDDUChainsXAIF::ChainStmtIterator> 
UDDUChainsXAIF::getUDDUChainSetIterator(int id)
{
    OA_ptr<ChainStmtIterator> retval;

    // if don't have a chain for this id then just create an empty set
    // and return an iterator over that
    std::map<int,OA_ptr<StmtSet> >::iterator pos = mIdToStmtSetMap->find(id);
    if (pos == mIdToStmtSetMap->end()) {
      OA_ptr<StmtSet> emptySet;  emptySet = new StmtSet;
      retval = new ChainStmtIterator(emptySet);
  
    // else create an iterator for requested set
    } else {
      retval = new ChainStmtIterator(pos->second);
    }

    return retval;
}
 
//! Return an iterator over all the memory references for which
//! we have UD or DU Chain information
OA_ptr<UDDUChainsXAIF::MemRefsWithChainIterator> 
UDDUChainsXAIF::getMemRefsWithChainIterator()
{
    OA_ptr<MemRefsWithChainIterator> retval;
    retval = new MemRefsWithChainIterator(mMemRefToIdMap);
    return retval;
}

//! get an iterator over all chains known about
OA_ptr<UDDUChainsXAIF::ChainIterator> UDDUChainsXAIF::getChainIterator()
{
    OA_ptr<ChainIterator> retval;
    retval = new ChainIterator(mIdToStmtSetMap);
    return retval;
}
    
//! create a new UDDUChains set and return the id
int UDDUChainsXAIF::makeEmptySet()
{
    //int newsetId = mStartId + mNumSets;
    //mNumSets++;
    int newsetId = mCurrentStartId;
    mCurrentStartId++;
    return newsetId;
}
      
//! add a statement into the possible uses or defs for a UDDUChains set
void UDDUChainsXAIF::addStmt(StmtHandle stmt, int setId)
{
    if ((*mIdToStmtSetMap)[setId].ptrEqual(0)) {
      (*mIdToStmtSetMap)[setId] = new StmtSet;
    }
    (*mIdToStmtSetMap)[setId]->insert(stmt);
}

//! add the given stmt set into the UDDUChains set
void UDDUChainsXAIF::addStmtSet(StmtSet& stmtSet, int setId)
{
    if ((*mIdToStmtSetMap)[setId].ptrEqual(0)) {
      (*mIdToStmtSetMap)[setId] = new StmtSet;
    }
    // just iterate over stmtSet and insert each entry
    StmtSet::iterator setIter;
    for (setIter=stmtSet.begin(); setIter!=stmtSet.end(); setIter++) {
        (*mIdToStmtSetMap)[setId]->insert(*setIter);
    }
}

//! insert the given mem ref into the UDDUChains set
void UDDUChainsXAIF::insertInto(MemRefHandle ref, int setId)
{
    (*mMemRefToIdMap)[ref] = setId;
}

//! return UDDUChains set Id that has identical set of Stmts
//! in it, if no set has this set of stmts then return SET_ID_NONE
int UDDUChainsXAIF::findSet(StmtSet& stmtSet)
{
    int retval = SET_ID_NONE;

    OA_ptr<ChainIterator> chainIterPtr = getChainIterator();
    for ( ; chainIterPtr->isValid(); ++(*chainIterPtr)) {
        if (*((*mIdToStmtSetMap)[chainIterPtr->currentId()]) == stmtSet) {
            retval = chainIterPtr->currentId();
        }
    }

    return retval;
}


//! incomplete output of info for debugging
//! FIXME using clearer interface?
void UDDUChainsXAIF::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    // print statemetns for each set, ID : { stmts }
    os << "============= UDDUChainsXAIF ============" << std::endl;
    OA_ptr<ChainIterator> chainIterPtr = getChainIterator();
    for ( ; chainIterPtr->isValid(); ++(*chainIterPtr)) {
      os << "UDDUChainsSet[" << chainIterPtr->currentId() << "] = { ";
      OA_ptr<ChainStmtIterator> siterPtr = chainIterPtr->currentChainStmtIterator();
      for ( ; siterPtr->isValid(); (*siterPtr)++ ) {
        StmtHandle stmt = siterPtr->current();
        os << ", ";
        os << "<" << ir->toString(stmt) << "> ";
      }
      os << " }" << std::endl;
    }

    // print all memrefs and their mapping
    os << "MemRef mapping to UDDUChains sets:" << std::endl;
    OA_ptr<MemRefsWithChainIterator> memRefIterPtr
        = getMemRefsWithChainIterator();
    for ( ; memRefIterPtr->isValid(); (*memRefIterPtr)++ ) {
      MemRefHandle memref = memRefIterPtr->current();
      int setId = getUDDUChainId(memref);
      os << "(" << memref.hval() << ") " << ir->toString(memref);
      os << " --> " << setId << std::endl;
    }
 
}



  } // end of XAIF namespace
} // end of OA namespace
