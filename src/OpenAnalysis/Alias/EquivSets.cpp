/*! \file
  
  \brief Implementation of Alias::EquivSets

  \author Michelle Strout
  \version $Id: EquivSets.cpp,v 1.18 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "EquivSets.hpp"

namespace OA {
  namespace Alias {

static bool debug = false;



//! indicate the aliasing relationship between two memory references
AliasResultType EquivSets::alias(MemRefHandle ref1, 
                                 MemRefHandle ref2)
{
    int ref1Id = getEquivSetId(ref1);
    int ref2Id = getEquivSetId(ref2);

    if (ref1Id!=SET_ID_NONE && ref1Id==ref2Id) {
        return mSetStatus[ref1Id];
    } else {
        return NOALIAS;
    }

}

//! iterator over locations that a memory reference may reference
OA_ptr<LocIterator> EquivSets::getMayLocs(MemRefHandle ref)
{
    OA_ptr<LocSetIterator> retval;

    // see if the given memory reference has been mapped to locations
    int setId = getEquivSetId(ref);
    if (setId != SET_ID_NONE && !mSetLocation[setId].ptrEqual(0)) {
        retval = new LocSetIterator(mSetLocation[setId]);
    } else {
        OA_ptr<LocSet> emptySet; emptySet = new LocSet;
        retval = new LocSetIterator(emptySet);
    }
    return retval;
}

//! iterator over locations that a memory reference must reference
//! these locations will all have full static overlap
OA_ptr<LocIterator> EquivSets::getMustLocs(MemRefHandle ref)
{
    OA_ptr<LocSetIterator> retval;
    // if the equivset that the ref maps to isMust then return
    // an iterator to that set, otherwise return an iterator to the
    // empty set
    int setId = getEquivSetId(ref);
    if (setId != SET_ID_NONE && isMust(setId)
        && !mSetLocation[setId].ptrEqual(0)) 
    {
        retval = new LocSetIterator(mSetLocation[setId]);
    } else {
        OA_ptr<LocSet> emptySet; emptySet = new LocSet;
        retval = new LocSetIterator(emptySet);
    }
    return retval;
}


//! get iterator over all must aliases for a specific mem ref
OA_ptr<MemRefIterator> EquivSets::getMustAliases(MemRefHandle ref)
{
    int refId = getEquivSetId(ref);
    OA_ptr<EquivSetsMemRefIter> retval;

    if (refId != SET_ID_NONE && isMust(refId)
        && !mSetLocation[refId].ptrEqual(0)) 
    {
        assert(refId >= 0 || refId < mSetEnd);
        retval = new EquivSetsMemRefIter(mRefSets[refId]);
    } else {
        OA_ptr<MemRefSet> emptySet; emptySet = new MemRefSet;
        retval = new EquivSetsMemRefIter(emptySet);
    }
    return retval;
}

//! get iterator over all may or must aliases for a specific mem ref
//! Keep in mind that a must alias is also a may alias, but not the other
//! way around.
OA_ptr<MemRefIterator> EquivSets::getMayAliases(MemRefHandle ref)
{
    int refId = getEquivSetId(ref);
    OA_ptr<EquivSetsMemRefIter> retval; retval = NULL;

    if (refId != SET_ID_NONE && !mSetLocation[refId].ptrEqual(0)) 
    {
        assert(refId >= 0 || refId < mSetEnd);
        retval = new EquivSetsMemRefIter(mRefSets[refId]);
    } else {
        OA_ptr<MemRefSet> emptySet; emptySet = new MemRefSet;
        retval = new EquivSetsMemRefIter(emptySet);
    }
    return retval;
}

//! get iterator over all must aliases for a specific location
OA_ptr<MemRefIterator> EquivSets::getMustAliases(OA_ptr<Location> loc)
{
    assert(!loc.ptrEqual(NULL));
    int refId = getEquivSetId(loc);
    OA_ptr<EquivSetsMemRefIter> retval;

    if (refId != SET_ID_NONE && isMust(refId) 
        && !mSetLocation[refId].ptrEqual(0)) 
    {
        assert(refId >= 0 || refId < mSetEnd);
        retval = new EquivSetsMemRefIter(mRefSets[refId]);
    } else {
        OA_ptr<MemRefSet> emptySet; emptySet = new MemRefSet;
        retval = new EquivSetsMemRefIter(emptySet);
    }
    return retval;
}

//! get iterator over all may or must aliases for a specific location
//! Keep in mind that a must alias is also a may alias, but not the other
//! way around.
OA_ptr<MemRefIterator> EquivSets::getMayAliases(OA_ptr<Location> loc)
{
    assert(!loc.ptrEqual(NULL));
    int refId = getEquivSetId(loc);
    OA_ptr<EquivSetsMemRefIter> retval;

    if (refId != SET_ID_NONE && !mSetLocation[refId].ptrEqual(0)) 
    {
        assert(refId >= 0 || refId < mSetEnd);
        retval = new EquivSetsMemRefIter(mRefSets[refId]);
    } else {
        OA_ptr<MemRefSet> emptySet; emptySet = new MemRefSet;
        retval = new EquivSetsMemRefIter(emptySet);
    }
    return retval;
}


//! get iterator over all memory references that information is
//! available for
OA_ptr<MemRefIterator> EquivSets::getMemRefIter()
{
    OA_ptr<EquivSetsMemRefIter> retval;

    // empty out set
    mKnownMemRefs->empty();

    std::map<MemRefHandle,int>::iterator mapIter;
    for (mapIter=mMemRefToIdMap.begin(); mapIter!=mMemRefToIdMap.end(); 
         mapIter++) 
    {
        mKnownMemRefs->insert(mapIter->first);
    }
    retval = new EquivSetsMemRefIter(mKnownMemRefs);
    return retval;
}

//! get unique id for the equivalence set for this memory reference
//! SET_ID_NONE indicates that this memory reference doesn't map to any of
//! the existing EquivSets
int EquivSets::getEquivSetId(MemRefHandle ref) 
{
    std::map<MemRefHandle,int>::iterator pos;
    pos = mMemRefToIdMap.find(ref);
    if (pos != mMemRefToIdMap.end()) {
        return pos->second;
    } else {
        return SET_ID_NONE;
    }
}
 
//! get unique id for the equivalence set for this memory reference
//! expression or an equivalent memory reference expression
//! SET_ID_NONE indicates that this memory reference expression
//! doesn't map to any of the existing EquivSets
int EquivSets::getEquivSetId(OA_ptr<MemRefExpr> mre)
{
    int retval = SET_ID_NONE;

    // loop through mapping of mre's to equivSet ids to determine if
    // the given mre is equivalent
    std::map<OA_ptr<MemRefExpr>,int>::iterator pos;
    pos = mMREToIdMap.find(mre);
    if (pos!=mMREToIdMap.end() ) {
      retval = pos->second;
    }

/*
    // if we didn't find a mapping for an equivalent MemRefExpr then
    // see if this memref expression maps trivially to a location
    Location* loc = trivialMREToLoc(mre);
    if (loc != NULL) {
        // determine which set the location the mre trivially maps to
        // is in, if any
        retval = getEquivSetId(loc);
    }
    */

    return retval;
}
 
   
//! get unique id for the equivalence set that contains
//! the given location, SET_ID_NONE indicates that the given
//! location isn't in any equiv set
int EquivSets::getEquivSetId(OA_ptr<OA::Location> pLoc) 
{
    int retval = SET_ID_NONE;

    // loop through mapping of locations's to equivSet ids to determine if
    // the given location is equivalent
    std::map<OA_ptr<Location>,int>::iterator pos;
    pos = mLocToIdMap.find(pLoc); 
    if (pos != mLocToIdMap.end()) {
      retval = pos->second;
    }
   
    return retval;
}

//! look up location associated with a symbol
//! returns NULL if one isn't found
OA_ptr<Location> EquivSets::getLocForSym(SymHandle sym)
{
    OA_ptr<Location> retval;  retval = NULL;

    std::map<SymHandle,OA_ptr<Location> >::iterator pos;
    pos = mSymToLocMap.find(sym);
    if (pos!=mSymToLocMap.end()) {
        retval = pos->second;
    }
    return retval;
}
 
OA_ptr<MemRefIterator> EquivSets::getMemRefIter(int equivSetId)
{
    assert(equivSetId < mSetEnd);
    assert(equivSetId >= 0);

    OA_ptr<EquivSetsMemRefIter> retval;
    if (!mSetLocation[equivSetId].ptrEqual(0)) 
    {
        retval = new EquivSetsMemRefIter(mRefSets[equivSetId]);
    } else {
        OA_ptr<MemRefSet> emptySet; emptySet = new MemRefSet;
        retval = new EquivSetsMemRefIter(emptySet);
    }
    return retval;
}
    
//! is the equivalence set a must alias equivalence set
bool EquivSets::isMust(int equivSetId)
{
    assert(equivSetId < mSetEnd);
    assert(equivSetId >= 0);

    return mSetStatus[equivSetId]==MUSTALIAS;
}

//*****************************************************************
// Construction methods 
//*****************************************************************
    
//! create an empty equiv set and return the id
int EquivSets::makeEmptySet()
{
    int newsetId = SET_ID_NONE;

    // first check if there are any empty sets due to merges
    if (mEmptySetIds.size() > 0) {
        newsetId = mEmptySetIds.front();
        mEmptySetIds.pop();

    // otherwise we will add a new set onto end of vector
    } else {    
        newsetId = mSetEnd;
        mSetEnd++;

        // make sure we have enough room
        if (mSetEnd >= (int)mRefSets.size()) {
            mRefSets.resize(2*mRefSets.size());
            mSetLocation.resize(2*mSetLocation.size());
            mSetStatus.resize(2*mSetStatus.size());
        }
    }

    // default status
    mSetStatus[newsetId] = MUSTALIAS;

    // keep track of number of sets
    mNumSets++;

    return newsetId;
}
     
//! merge the first set into the second set, after this operation
//! the first set's Id will no longer be valid and just have
//! an empty set of locations
void EquivSets::mergeInto(int firstSetId, int secondSetId) 
{
    assert(firstSetId >= 0 && firstSetId < mSetEnd);
    assert(secondSetId >= 0 && secondSetId < mSetEnd);

    // put all memrefs and locations from first set into secondSet
    MemRefSet::iterator siter;
    for (siter=mRefSets[firstSetId]->begin(); siter!=mRefSets[firstSetId]->end();
         siter++)
    {
        MemRefHandle ref = *siter;
        mMemRefToIdMap[ref] = secondSetId;
        mRefSets[secondSetId]->insert(ref);
        // for each MemRefExpr associated with the current ref
        // change their mapping as well
        std::set<OA_ptr<MemRefExpr> >::iterator mreSetIter;
        for (mreSetIter=mMemRefToMRESetMap[ref].begin();
             mreSetIter!=mMemRefToMRESetMap[ref].end(); mreSetIter++) 
        {
            OA_ptr<MemRefExpr> currMRE = *mreSetIter;
            // loop through mapping of mre's to equivSet ids to find
            // mres equivalent to currMRE and map them to the secondSetId
            std::map<OA_ptr<MemRefExpr>,int>::iterator mapIter;
            for (mapIter=mMREToIdMap.begin(); mapIter!=mMREToIdMap.end(); mapIter++) {
                OA_ptr<MemRefExpr> mre = mapIter->first;
                if ( mre == currMRE ) {
                    mMREToIdMap[mre] = secondSetId;
                }
            }
        }
    }

    if (mSetLocation[firstSetId].ptrEqual(0)) {
        mSetLocation[firstSetId] = new LocSet;
    }
    if (mSetLocation[secondSetId].ptrEqual(0)) {
        mSetLocation[secondSetId] = new LocSet;
    }
    LocSet::iterator lsiter;
    for (lsiter=mSetLocation[firstSetId]->begin(); 
         lsiter!=mSetLocation[firstSetId]->end(); lsiter++)
    {
        mSetLocation[secondSetId]->insert(*lsiter);
    }

    // empty the first set and update mNumSets
    mEmptySetIds.push(firstSetId);
    mRefSets[firstSetId]->erase(mRefSets[firstSetId]->begin(), mRefSets[firstSetId]->end());
    mSetLocation[firstSetId]->erase(mSetLocation[firstSetId]->begin(), 
        mSetLocation[firstSetId]->end());
    mNumSets--;
}

//! associate the given location with the given equivSet
void EquivSets::addLocation(OA_ptr<Location> pLoc, int equivSetId)
{
    if (debug) {
        std::cout << "EquivSets::addLocation: pLoc = ";
        pLoc->dump(std::cout);
        std::cout << "EquivSets::addLocation: equivSetId = " << equivSetId
                  << std::endl;
    }
        
    assert(equivSetId >= 0 && equivSetId < mSetEnd);
    if (mSetLocation[equivSetId].ptrEqual(0)) {
        mSetLocation[equivSetId] = new LocSet;
    }

    // if new location doesn't have full accuracy then need
    // to give the set MAYALIAS status
    if (mSetStatus[equivSetId]==MUSTALIAS) {
        if (!pLoc->hasFullAccuracy()) {
            mSetStatus[equivSetId] = MAYALIAS;

        } else {
            // also if there are other locations in the set already 
            // and they don't all fully overlap with this location
            LocSetIterator locIter(mSetLocation[equivSetId]);
            for (; locIter.isValid(); ++locIter) {
                OA_ptr<Location> loc = locIter.current();
                if (!loc->mustOverlap(*pLoc)) {
                    mSetStatus[equivSetId] = MAYALIAS;
                }
            }
        }
    }

    // insert new location into set
    mSetLocation[equivSetId]->insert(pLoc);
    mLocToIdMap[pLoc] = equivSetId;
}


//! associate given mre with MemRefHandle 
void EquivSets::mapMemRefToMemRefExpr(MemRefHandle ref, 
        OA_ptr<MemRefExpr> mre)
{
    mMemRefToMRESetMap[ref].insert(mre);
}

/*
//! insert the given mem ref into the equiv set
//! and keep the equiv set as a must set if it already is one
void EquivSets::insertIntoMust(MemRefHandle ref, int equivSetId)
{
    assert(equivSetId < mSetEnd);
    assert(equivSetId >= 0);

    // default status is MUSTALIAS so no need to change, because if
    // it has already been switched to MAYALIAS we can't change anyway

    mRefSets[equivSetId].insert(ref);
}

//! insert the given mem ref into the equiv set
//! and make the equiv set a may set 
void EquivSets::insertIntoMay(MemRefHandle ref, int equivSetId)
{
    assert(equivSetId < mSetEnd);
    assert(equivSetId >= 0);

    mRefSets[equivSetId].insert(ref);
    mRefSets[equivSetId] = MAYALIAS;
}
*/

   
//! associate a MemRefHandle with the given equivSet,
//! means that the MemRefHandle can access all of the locations
//! in the equivSet, if only one full location then is a must access
//! if memref has already been mapped to another set then will
//! merge the current set for memref into given set 
void EquivSets::mapMemRefToEquivSet(MemRefHandle ref, int equivSetId)
{
    assert(equivSetId >= 0 || equivSetId < mSetEnd);
    int currSetId = getEquivSetId(ref);
    if (currSetId != SET_ID_NONE) {
        mergeInto(currSetId,equivSetId);
    } else {
        mMemRefToIdMap[ref] = equivSetId;
        if (mRefSets[equivSetId].ptrEqual(0)) {
            mRefSets[equivSetId] = new MemRefSet;
        }
        mRefSets[equivSetId]->insert(ref);
    }
}

//! associate a MemRefExpr with the given equivSet,
//! means that the MemRefExpr can access all of the locations
//! in the equivSet
void EquivSets::mapMemRefToEquivSet(OA_ptr<MemRefExpr> ref, 
        int equivSetId)
{
    int currSetId = getEquivSetId(ref);
    assert(currSetId==SET_ID_NONE);

    mMREToIdMap[ref] = equivSetId;
}

//! associate a symbol with a location
void EquivSets::mapSymToLoc(SymHandle sym, OA_ptr<Location> loc)
{
    mSymToLocMap[sym] = loc;
}
    
/*! do a trivial map of mre to loc if possible (no addressTaken
   and no dereferencing), otherwise return NULL

NOTE: for now this method is duplicated in AliasMap.cpp, if make any changes
here to handle array indices, fields, etc. should reflect those changes
in AliasMap.cpp.
FIXME: refactoring needs to happen between AliasMap and EquivSets because
there is so much shared code.  The trivial mapping logic might be relevant
to all Alias::Interface instantiations.
*/
OA_ptr<Location> 
EquivSets::trivialMREToLoc(OA_ptr<MemRefExpr> mre)
{
    OA_ptr<Location> retLoc; retLoc = NULL;

    if (!mre->isaRefOp() && mre->isaNamed() && !mre->hasAddressTaken()) {
      OA_ptr<NamedRef> namedMRE 
          = mre.convert<NamedRef>();

      // get location associated with base symbol
      OA_ptr<Location> baseloc = getLocForSym(namedMRE->getSymHandle());

      // if the memory reference doesn't have full accuracy then
      // create a partial subset of the base memory location
      if (!mre->hasFullAccuracy()) {
          retLoc = new LocSubSet(baseloc,false);
      } else {
          retLoc = baseloc;
      }

    } 

    return retLoc;
}

//! incomplete output of info for debugging, just lists
//! sets and text representation of mem refs
void EquivSets::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    // print out info for each set
    for (int i=0; i<mSetEnd; i++) {
      /*
      MemRefSet aSet = mRefSets[i];
      MemRefSet::iterator siter;
      for (siter=mRefSets[i].begin(); siter!=mRefSets[i].end(); siter++) {
        os << " ";
        //(*siter)->dump(os,ir);
        os << ", " << std::endl;
      }
*/
      os << "EquivSet[" << i << "] Locations = { ";
      if (mSetLocation[i].ptrEqual(0)) {
        mSetLocation[i] = new LocSet;
      }
      LocSet::iterator locIter;
      for (locIter=mSetLocation[i]->begin(); locIter!=mSetLocation[i]->end(); 
           locIter++) 
      {
          OA_ptr<Location> loc(*locIter);
          loc->dump(os,ir);
      }
      os << " }" << std::endl;
    }

    // print mapping of memory references to equivsets
    std::map<MemRefHandle,int>:: iterator mapIter;
    for (mapIter=mMemRefToIdMap.begin(); mapIter!=mMemRefToIdMap.end();
         mapIter++) 
    {
        os << "MemRefHandle (";
        ir->dump(mapIter->first,os);
        os << ") maps to EquivSet (" << mapIter->second << ")" << std::endl;
    }
   
}
   

  
  } // end of Alias namespace
} // end of OA namespace
