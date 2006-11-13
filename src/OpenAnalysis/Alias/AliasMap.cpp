/*! \file
  
  \brief Implementation of Alias::AliasMap

  \author Michelle Strout
  \version $Id: AliasMap.cpp,v 1.24 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "AliasMap.hpp"

namespace OA {
  namespace Alias {

#if defined(DEBUG_ALL) || defined(DEBUG_AliasMap)
static bool debug = true;
#else
static bool debug = false;
#endif

AliasMap::AliasMap(ProcHandle p) : mProcHandle(p), mNumSets(1), mStartId(0)
{
  // default set for things MemRefHandles that we don't know enough
  // information about
  mIdToSetStatusMap[0] = MAYALIAS;
  OA_ptr<Location> loc;
  loc = dynamic_cast<Location*>(new UnknownLoc());
  mIdToLocSetMap[0] = new LocSet;
  mIdToLocSetMap[0]->insert( loc );
}

/*! 
   Copy constructor
*/
AliasMap::AliasMap(AliasMap& other)
{
    mProcHandle = other.mProcHandle;
    mNumSets = other.mNumSets;
    mStartId = other.mStartId;
    mIdToLocSetMap = other.mIdToLocSetMap;
    mIdToSetStatusMap = other.mIdToSetStatusMap;
    mIdToMemRefSetMap = other.mIdToMemRefSetMap;
    mMemRefToIdMap = other.mMemRefToIdMap;
    mMREToIdMap = other.mMREToIdMap;
    mLocToIdMap = other.mLocToIdMap;
}



/*!
   Indicate the aliasing relationship between two memory references

   It is possible for memory references that map to different
   alias map sets to must or may alias each other.  They will must
   alias each other if the associated location sets contain only one
   full location and that location must overlap.  They will may
   alias each other if any of the locations in the associated sets
   may overlap.
*/
AliasResultType AliasMap::alias(MemRefHandle ref1, MemRefHandle ref2)
{ 
    unsigned int id1, id2;
    id1 = mMemRefToIdMap[ref1];
    id2 = mMemRefToIdMap[ref2];
    AliasResultType retval = MAYALIAS;

    // first check if the memory references map to the same alias map set
    if (id1==0 || id2==0) {
        retval = MAYALIAS;
    } else if (id1==id2 && mIdToSetStatusMap[id1]==MUSTALIAS) {
        retval = MUSTALIAS;
    } else if (id1==id2 && mIdToSetStatusMap[id1]==MAYALIAS) {
        retval = MAYALIAS;

    // next check the relationships between locations if there are two
    // different alias map sets, only do full relationship check if
    // this information hasn't been cached
    } else if (mayOverlapLocSets(*mIdToLocSetMap[id1],
                                           *mIdToLocSetMap[id2]) )
    {
        retval = MAYALIAS;
        // if is a MAYALIAS and both sets have MUSTALIAS status then
        // this is really a MUSTALIAS
        if (isMust(id1) && isMust(id2)) {
            retval = MUSTALIAS;
        }

    // otherwise these two references do not alias
    } else {
        retval = NOALIAS; 
    }

    return retval; 
} 

//! get iterator over all locations in a particular set
OA_ptr<LocIterator> AliasMap::getLocIterator(int setId) 
{ OA_ptr<LocSetIterator> retval;
  if (mIdToLocSetMap[setId].ptrEqual(0)) {
    OA_ptr<LocSet> emptySet; emptySet = new LocSet;
    retval = new LocSetIterator(emptySet);  // empty loc set
  } else {
    retval = new LocSetIterator(mIdToLocSetMap[setId]); 
  }
  return retval;
}

//! iterator over locations that a memory reference may reference
OA_ptr<LocIterator> AliasMap::getMayLocs(MemRefHandle ref)
{
    // trivial with alias maps, just return iterator to location
    // set we map to 
    int setId = mMemRefToIdMap[ref];
    OA_ptr<LocSetIterator> retval;
    if (mIdToLocSetMap[setId].ptrEqual(0)) {
        OA_ptr<LocSet> emptySet; emptySet = new LocSet;
        retval = new LocSetIterator(emptySet);  // empty loc set
    } else {
        retval = new LocSetIterator(mIdToLocSetMap[setId]); 
    }
    return retval;
}

//! iterator over locations that a memory reference must reference
//! these locations will all have full static overlap
OA_ptr<LocIterator> AliasMap::getMustLocs(MemRefHandle ref)
{
    OA_ptr<LocIterator> retIter;

    // trivial with alias maps, just return iterator to location
    // set we map to if that location map has MUSTALIAS status
    int setId = mMemRefToIdMap[ref];
    if (mIdToSetStatusMap[setId]==MUSTALIAS 
        && !mIdToLocSetMap[setId].ptrEqual(0)) 
    {
        retIter = new LocSetIterator(mIdToLocSetMap[setId]);
    } else {
        OA_ptr<LocSet> emptySet; emptySet = new LocSet;
        retIter = new LocSetIterator(emptySet);  // empty loc set
    }

    return retIter;
}

//! get iterator over all must aliases for a specific mem ref
OA_ptr<MemRefIterator> AliasMap::getMustAliases(MemRefHandle ref)
{ 
    // create an empty MemRefSet that will be given to iterator
    OA_ptr<MemRefSet> mustSet; mustSet = new MemRefSet;
    OA_ptr<MemRefIterator> retIter; retIter = new AliasMapMemRefIter(mustSet);

    // determine what set the reference maps to
    int id = mMemRefToIdMap[ref];
    if (debug) {
      std::cout << "AliasMap::getMustAliases: id = " << id << std::endl;
    }

    // find all the map sets that contain locations which may overlap with
    // this map set (it should find itself) and both alias map sets have
    // MUST status
    std::set<int> mustOverlapMapSetIds;
    std::map<int,OA_ptr<LocSet> >::iterator mapIter;
    for (mapIter = mIdToLocSetMap.begin(); mapIter != mIdToLocSetMap.end();
         mapIter++) 
    {
        int otherId = mapIter->first;
        if (mIdToSetStatusMap[otherId]==MUSTALIAS 
            && mIdToSetStatusMap[id]==MUSTALIAS
            && mayOverlapLocSets(*(mapIter->second),*mIdToLocSetMap[id])) 
        {
            mustOverlapMapSetIds.insert(otherId);
        }
    }

    // find all the memory references that map to those loc sets
    // and put them in the set of must aliases
    std::set<int>::iterator setIter;
    for (setIter=mustOverlapMapSetIds.begin(); 
         setIter!=mustOverlapMapSetIds.end(); setIter++ )
    {
        int setId = *setIter;

        MemRefSet::iterator memrefSetIter;
        for (memrefSetIter=mIdToMemRefSetMap[setId].begin();
             memrefSetIter!=mIdToMemRefSetMap[setId].end();
             memrefSetIter++) 
        {
            mustSet->insert(*memrefSetIter);
        }
    }

    return retIter;

}

//! get iterator over all may aliases for a specific mem ref
OA_ptr<MemRefIterator> AliasMap::getMayAliases(MemRefHandle ref)
{ 
    // create an empty MemRefSet that will be given to iterator
    OA_ptr<MemRefSet> maySet; maySet = new MemRefSet;
    OA_ptr<MemRefIterator> retIter; retIter = new AliasMapMemRefIter(maySet);

    // determine what set the reference maps to
    int id = mMemRefToIdMap[ref];
    if (debug) {
      std::cout << "AliasMap::getMustAliases: id = " << id << std::endl;
    }

    // find all the map sets that contain locations which may overlap with
    // this map set (it should find itself)
    std::set<int> mayOverlapMapSetIds;
    std::map<int,OA_ptr<LocSet> >::iterator mapIter;
    for (mapIter = mIdToLocSetMap.begin(); mapIter != mIdToLocSetMap.end();
         mapIter++) 
    {
        int otherId = mapIter->first;
        if (mayOverlapLocSets(*(mapIter->second),
                                        *mIdToLocSetMap[id])) 
        {
            mayOverlapMapSetIds.insert(otherId);
        }
    }

    // find all the memory references that map to those loc sets
    // and put them in the set of may aliases
    std::set<int>::iterator setIter;
    for (setIter=mayOverlapMapSetIds.begin(); 
         setIter!=mayOverlapMapSetIds.end(); setIter++ )
    {
        int setId = *setIter;

        MemRefSet::iterator memrefSetIter;
        for (memrefSetIter=mIdToMemRefSetMap[setId].begin();
             memrefSetIter!=mIdToMemRefSetMap[setId].end();
             memrefSetIter++) 
        {
            maySet->insert(*memrefSetIter);
        }
    }

    return retIter;

}
    
//! get iterator over all must aliases for a specific location
OA_ptr<MemRefIterator> AliasMap::getMustAliases(OA_ptr<Location> loc)
{
    // create an empty MemRefSet that will be given to iterator
    OA_ptr<MemRefSet> mustSet; mustSet = new MemRefSet;
    OA_ptr<MemRefIterator> retIter; retIter = new AliasMapMemRefIter(mustSet);

    // find all the map sets that contain locations which may overlap with
    // this location and the alias map set is a MUST alias and this location
    // is fully accurate
    LocSet singleEntrySet;
    singleEntrySet.insert(loc);
    std::set<int> mustOverlapMapSetIds;
    std::map<int,OA_ptr<LocSet> >::iterator mapIter;
    for (mapIter = mIdToLocSetMap.begin(); mapIter != mIdToLocSetMap.end();
         mapIter++) 
    {
        int otherId = mapIter->first;
        if (mIdToSetStatusMap[otherId]==MUSTALIAS 
            && loc->hasFullAccuracy()
            && mayOverlapLocSets(*(mapIter->second),
                                           singleEntrySet)) 
        {
            mustOverlapMapSetIds.insert(otherId);
        }
    }

    // find all the memory references that map to those loc sets
    // and put them in the set of must aliases
    std::set<int>::iterator setIter;
    for (setIter=mustOverlapMapSetIds.begin(); 
         setIter!=mustOverlapMapSetIds.end(); setIter++ )
    {
        int setId = *setIter;

        MemRefSet::iterator memrefSetIter;
        for (memrefSetIter=mIdToMemRefSetMap[setId].begin();
             memrefSetIter!=mIdToMemRefSetMap[setId].end();
             memrefSetIter++) 
        {
            mustSet->insert(*memrefSetIter);
        }
    }

    return retIter;
}

//! get iterator over all may aliases for a specific location
OA_ptr<MemRefIterator> AliasMap::getMayAliases(OA_ptr<Location> loc)
{
    // create an empty MemRefSet that will be given to iterator
    OA_ptr<MemRefSet> maySet; maySet = new MemRefSet;
    OA_ptr<MemRefIterator> retIter; retIter = new AliasMapMemRefIter(maySet);

    // find all the map sets that contain locations which may overlap with
    // this location 
    LocSet singleEntrySet;
    singleEntrySet.insert(loc);
    std::set<int> mayOverlapMapSetIds;
    std::map<int,OA_ptr<LocSet> >::iterator mapIter;
    for (mapIter = mIdToLocSetMap.begin(); mapIter != mIdToLocSetMap.end();
         mapIter++) 
    {
        int otherId = mapIter->first;
        if (mayOverlapLocSets(*(mapIter->second),singleEntrySet)) 
        {
            mayOverlapMapSetIds.insert(otherId);
        }
    }

    // find all the memory references that map to those loc sets
    // and put them in the set of may aliases
    std::set<int>::iterator setIter;
    for (setIter=mayOverlapMapSetIds.begin(); 
         setIter!=mayOverlapMapSetIds.end(); setIter++ )
    {
        int setId = *setIter;

        MemRefSet::iterator memrefSetIter;
        for (memrefSetIter=mIdToMemRefSetMap[setId].begin();
             memrefSetIter!=mIdToMemRefSetMap[setId].end();
             memrefSetIter++) 
        {
            maySet->insert(*memrefSetIter);
        }
    }

    return retIter;
}

//! get iterator over all memory references that information is
//! available for
OA_ptr<MemRefIterator> AliasMap::getMemRefIter()
{
    // create an empty MemRefSet that will be given to iterator
    OA_ptr<MemRefSet> memrefSet; memrefSet = new MemRefSet;

    // put all memory references that we know about into the set
    std::map<MemRefHandle,int>::iterator mapIter;
    for (mapIter=mMemRefToIdMap.begin(); mapIter!=mMemRefToIdMap.end();
         mapIter++) 
    {
        memrefSet->insert(mapIter->first);
    }

    OA_ptr<MemRefIterator> retIter; retIter = new AliasMapMemRefIter(memrefSet);
    return retIter;
}

//*****************************************************************
// DataFlowSet interface
//*****************************************************************
/*
bool AliasMap::operator ==(DataFlow::DataFlowSet &other) const
{
    if (mIdToLocSetMap==other.mIdToLocSetMap
        && mIdToMemRefSetMap==other.mIdToMemRefSetMap
        && mIdToMRESetMap == other.mIdToMRESetMap)
    {
        return true;
    } else {
        return false;
    }
}

bool AliasMap::operator !=(DataFlow::DataFlowSet &other) const
{
    return ! operator==(other);
}

OA_ptr<DataFlow::DataFlowSet> AliasMap::clone()
{
    OA_ptr<AliasMap> retval;
    retval = new AliasMap(*this);
    return retval;
}
*/
//*****************************************************************
// Meet routine
//*****************************************************************

/*! Read through AliasMap sets in each, alias all locations that are within
    one alias map set in the returned AliasMap set.

    Mostly copying over mappings so start with a copy of ourselves.
    Then, add any mappings from other that aren't already in retval.
*/
/*
OA_ptr<AliasMap> AliasMap::meet(AliasMap& other)
{
    OA_ptr<AliasMap> retval;

    // first make a copy of ourselves
    retval = new AliasMap(*this); 
    
    // loop through all sets in other
    OA_ptr<IdIterator> idIterPtr = getIdIterator();
    for ( ; idIterPtr->isValid(); ++(*idIterPtr) ) {
      int i = idIterPtr->current();
      // if there is more than one location
    
}
*/ 

//*****************************************************************
// Info methods unique to Alias::AliasMap
//*****************************************************************
    
//! get unique id for the alias map set for this memory reference,
//! SET_ID_NONE indicates that this memory reference doesn't map to any of
//! the existing AliasMap sets
int AliasMap::getMapSetId(MemRefHandle ref) 
{
    std::map<MemRefHandle,int>::iterator pos;
    pos = mMemRefToIdMap.find(ref);
    if (pos != mMemRefToIdMap.end()) {
        return pos->second;
    } else {

        return AliasMap::SET_ID_NONE;
    }
}

//! get unique id for the alias map set for this memory reference
//! expression or an equivalent memory reference expression
//! will create a new map set if this memory reference involves a 
//! full named location that doesn't have its address taken.
//! SET_ID_NONE indicates that this memory reference expression
//! doesn't map to any of the existing AliasMap sets
int AliasMap::getMapSetId(OA_ptr<MemRefExpr> mre) 
{
    int retval = AliasMap::SET_ID_NONE;

    std::map<OA_ptr<MemRefExpr>,int>::iterator pos;
    pos = mMREToIdMap.find(mre);
    if (pos != mMREToIdMap.end()) {
        return pos->second;
    } else {
        return AliasMap::SET_ID_NONE;
    }
}
    

/*! get id for an alias map set that contains
    the given location, SET_ID_NONE indicates that the given
    location isn't in any AliasMap sets 

    If more than one map set contains this location, will return
    the set with the fewest number of other locations
    because that is what is maintained in the mapping of locations
    to sets
*/
int AliasMap::getMapSetId(OA_ptr<OA::Location> pLoc)
{
    int retval = SET_ID_NONE;

    // loop through mapping of locations's to alias map ids to determine if
    // the given location is equivalent
    std::map<OA_ptr<Location>,int>::iterator mapIter;
    for (mapIter=mLocToIdMap.begin(); mapIter!=mLocToIdMap.end(); mapIter++) {
        OA_ptr<Location> knownloc = mapIter->first;
        // see if the already mapped location is equivalent to the given location
        if ( knownloc == pLoc ) {
            retval = mapIter->second;
            break;
        }
    }

    return retval;
}

//! get id for an alias map set that contains
//! an equivalent set of locations, SET_ID_NONE indicates 
//! that no equivalent location set was found in any AliasMap sets 
int AliasMap::getMapSetId(LocSet pLocSet)
{
    int retval = SET_ID_NONE;

    // loop through location sets and compare them to given location set
    std::map<int,OA_ptr<LocSet> >::iterator mapIter;
    for (mapIter=mIdToLocSetMap.begin(); mapIter!=mIdToLocSetMap.end();
         mapIter++)
    {
        OA_ptr<LocSet> mapLocSet = mapIter->second;
        if (subSetOf(pLocSet,*mapLocSet) 
            && subSetOf(*mapLocSet,pLocSet) )
        {
            return mapIter->first;
        } 
    }

    return retval;
}

OA_ptr<IdIterator> AliasMap::getIdIterator()
{
    OA_ptr<IdIterator> retval;
    retval = new IdIterator(mIdToLocSetMap);
    return retval;
}
 
/*
//! look up location associated with a symbol
//! returns NULL if one isn't found
//! user should delete location returned
OA_ptr<Location> AliasMap::getLocForSym(SymHandle sym)
{
    OA_ptr<Location> retval; retval = NULL;
    std::map<SymHandle,OA_ptr<Location> >::iterator pos;
    pos = mSymToLocMap.find(sym);
    if (pos!=mSymToLocMap.end()) {
        retval = pos->second;
    }
    return retval;
}
*/

//! create an empty alias map set and return the id
int AliasMap::makeEmptySet()
{
    int newsetId = mStartId + mNumSets;
    mIdToSetStatusMap[newsetId] = MUSTALIAS;
    mIdToLocSetMap[newsetId] = new LocSet;
    mNumSets++;
    return newsetId;
}
     

/*
//! insert the given mem ref expr instance into the alias map set
void AliasMap::insertInto(MemRefHandle ref, int setId)
{
    assert(setId < mNumSets);
    assert(setId >= 0);

    mMemRef2IdMap[ref] = setId;
}
*/

//! associate the given location with the given mapSet
void AliasMap::addLocation(OA_ptr<Location> pLoc, int setId)
{
    if (debug) {
        std::cout << "AliasMap::addLocation: pLoc = ";
        pLoc->dump(std::cout);
    }
    // check that an empty location set exists for this setId
    if (mIdToLocSetMap[setId].ptrEqual(0)) {
       mIdToLocSetMap[setId] = new LocSet;
    }

    // if new location doesn't have full accuracy then need
    // to give the set MAYALIAS status
    if (mIdToSetStatusMap[setId]==MUSTALIAS) {
        if (!pLoc->hasFullAccuracy()) {
            mIdToSetStatusMap[setId] = MAYALIAS;
            if (debug) {
                std::cout << "\tpLoc does not have full accuracy" << std::endl;
            }

        } else {
            // also if there are other locations in the set already 
            // and they don't all fully overlap with this location
            LocSetIterator locIter(mIdToLocSetMap[setId]);
            for (; locIter.isValid(); ++locIter) {
                OA_ptr<Location> loc = locIter.current();
                if (!loc->mustOverlap(*pLoc)) {
                    mIdToSetStatusMap[setId] = MAYALIAS;
                }
            }
        }
    }

    // insert new location into set
    mIdToLocSetMap[setId]->insert(pLoc);

    // only map the location to this set if it is not already mapped
    // or if it is mapped to a set with less accuracy
    bool moreAccurate = false;
    std::map<OA_ptr<Location>,int>::iterator mapIter;
    for (mapIter=mLocToIdMap.begin(); mapIter!=mLocToIdMap.end(); mapIter++) {
        OA_ptr<Location> mappedLoc = mapIter->first;
        if (mappedLoc == pLoc 
            && mIdToSetStatusMap[mapIter->second]==MUSTALIAS)
        {
            moreAccurate = true;
        }
    }
    if (!moreAccurate) {
        mLocToIdMap[pLoc] = setId;
    }
}

/*! 
   - find all existing alias map sets that involve the two locations
   - create new alias map set that includes all locations from those sets
   - map all mre's and memrefhandles from old sets to new set
   - remove old alias map sets 

   FIXME?: right now this implements transitivity of aliasing for the
   aliases in any sets with loc1 and loc2
*/
void AliasMap::aliasLocs(OA_ptr<Location> loc1, OA_ptr<Location> loc2)
{
    std::set<int> affectedSets;

    // loop through all the locations set to find all map sets
    // that contain the given locations
    OA_ptr<IdIterator> idIterPtr = getIdIterator();
    for ( ; idIterPtr->isValid(); ++(*idIterPtr) ) {
      int i = idIterPtr->current();
      if (mIdToLocSetMap[i]->find(loc1)!=mIdToLocSetMap[i]->end()) {
          affectedSets.insert(i);
      }
      if (mIdToLocSetMap[i]->find(loc2)!=mIdToLocSetMap[i]->end()) {
          affectedSets.insert(i);
      }
    }

    // if only one affected set then just put both locations in there,
    // means at least one is already in there
    if (affectedSets.size()==1) {
        std::set<int>::iterator iter;
        iter = affectedSets.begin();
        addLocation(loc1, *iter);
        addLocation(loc2, *iter);
    } else {

        // create a new alias map set
        int newSetId = makeEmptySet();

        // put all the locations from the affected sets into the new set
        // remap all the memory references in the affected sets
        std::set<int>::iterator iter;
        for (iter=affectedSets.begin(); iter!=affectedSets.end(); iter++) {
            OA_ptr<LocIterator> locIterPtr = getLocIterator(*iter);
            for (; locIterPtr->isValid(); (*locIterPtr)++ ) {
                OA_ptr<Location> loc = (locIterPtr->current());
                addLocation(loc, newSetId);
            }

            remapMemRefs(*iter, newSetId);
        }

        // make sure both locations are in new set
        addLocation(loc1, newSetId);
        addLocation(loc2, newSetId);
    }

 }

/*!
    Remaps all memrefhandles and memrefexprs to the new set
    and removes the oldSet.

    Will replace removed set with last set to maintain contiguity.
 */
void AliasMap::remapMemRefs(int oldSetId, int newSetId)
{
    ////// move all the MemRefHandles and MemRefExprs to the new map set
    // first the MemRefHandles
    MemRefSet::iterator mrIter;
    for (mrIter = mIdToMemRefSetMap[oldSetId].begin();
         mrIter != mIdToMemRefSetMap[oldSetId].end(); mrIter++ )
    {
        mMemRefToIdMap[*mrIter] = newSetId;
        mIdToMemRefSetMap[newSetId].insert(*mrIter);
    }
    // then the MemRefExprs
    MemRefExprSet::iterator mreIter;
    for (mreIter = mIdToMRESetMap[oldSetId].begin();
         mreIter != mIdToMRESetMap[oldSetId].end(); mreIter++ )
    {
        mMREToIdMap[*mreIter] = newSetId;
        mIdToMRESetMap[newSetId].insert(*mreIter);
    }

    ////// move last alias map set to the old spot
    int lastSetId = mStartId+mNumSets-1;

    // move  the MemRefHandles
    mIdToMemRefSetMap[oldSetId] = mIdToMemRefSetMap[lastSetId];
    for (mrIter = mIdToMemRefSetMap[oldSetId].begin();
         mrIter != mIdToMemRefSetMap[oldSetId].end(); mrIter++ )
    {
        mMemRefToIdMap[*mrIter] = oldSetId;
    }

    // move  the MemRefExprs
    mIdToMRESetMap[oldSetId] = mIdToMRESetMap[lastSetId];
    for (mreIter = mIdToMRESetMap[oldSetId].begin();
         mreIter != mIdToMRESetMap[oldSetId].end(); mreIter++ )
    {
        mMREToIdMap[*mreIter] = oldSetId;
    }
    

    // reduce the number of sets
    mNumSets--;

}

//! associate a MemRefHandle with the given mapset,
//! means that the MemRefHandle can access all of the locations
//! in the mapset, if only one full location then is a must access
void AliasMap::mapMemRefToMapSet(MemRefHandle ref, int setId)
{
    // should not be mapping the same MemRefHandle 
    // to a different setId than previously mapped without
    // handling associated MemRefExprs as well
    int currSetId = getMapSetId(ref);
    assert(currSetId==SET_ID_NONE || currSetId==setId);

    mMemRefToIdMap[ref] = setId;
    mIdToMemRefSetMap[setId].insert(ref);
}

//! associate a MemRefExpr with the given mapSet,
//! means that the MemRefExpr may access all of the locations
//! in the mapSet
void AliasMap::mapMemRefToMapSet(OA_ptr<MemRefExpr> ref, int setId)
{
    // should not be mapping the same MemRefExpr 
    // to a different setId than previously mapped
    // without handling associated MemRefHandles as well
    int currSetId = getMapSetId(ref);
    assert(currSetId==SET_ID_NONE || currSetId==setId);

    if (!ref.ptrEqual(0)) { 
        mMREToIdMap[ref] = setId; 
        mIdToMRESetMap[setId].insert(ref);
    }
}

void AliasMap::output(OA_ptr<IRHandlesIRInterface> ir)
{
    sOB->objStart("AliasMap");

    sOB->fieldStart("mProcHandle");
    sOB->outputIRHandle(mProcHandle,ir);
    sOB->fieldEnd("mProcHandle");
    
    sOB->field("mNumSets",int2string(mNumSets));

    sOB->field("mStartId",int2string(mStartId));

    sOB->mapStart("mIdToLocSetMap", "id", "locset");
    OA_ptr<IdIterator> idIterPtr = getIdIterator();
    for ( ; idIterPtr->isValid(); ++(*idIterPtr) ) {
      sOB->mapEntryStart();
      int i = idIterPtr->current();
      sOB->mapKey(int2string(i));
      sOB->mapValueStart();
      sOB->listStart();
      OA_ptr<LocIterator> locIterPtr = getLocIterator(i);
      for (; locIterPtr->isValid(); (*locIterPtr)++ ) {
        sOB->listItemStart();
        OA_ptr<Location> loc = (locIterPtr->current());
        loc->output(ir);
        sOB->listItemEnd();
      }
      sOB->listEnd();
      sOB->mapValueEnd();
      sOB->mapEntryStart();
    }
    sOB->mapEnd("mIdToLocSetMap");

    sOB->objEnd("AliasMap");
}

//! incomplete output of info for debugging 
//! may also include non-persistent handle values
void AliasMap::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    // print locations for each set, ID : { locs }
    os << "============= AliasMap ============" << std::endl;
    OA_ptr<IdIterator> idIterPtr = getIdIterator();
    for ( ; idIterPtr->isValid(); ++(*idIterPtr) ) {
      int i = idIterPtr->current();
      os << "AliasMapSet[" << i << "] = { ";
      OA_ptr<LocIterator> locIterPtr = getLocIterator(i);
      for (; locIterPtr->isValid(); (*locIterPtr)++ ) {
        OA_ptr<Location> loc = (locIterPtr->current());
        os << ", ";
        os << "<";
        loc->dump(os,ir);
        os << "> ";
      }
      os << " }" << std::endl;
    }

    // print all memrefs and their mapping
    os << "MemRef mapping to AliasMap sets:" << std::endl;
    OA_ptr<MemRefIterator> memIterPtr = getMemRefIter();
    for (; memIterPtr->isValid(); (*memIterPtr)++) {
      MemRefHandle memref = memIterPtr->current();
      os << "(" << memref.hval() << ") " << ir->toString(memref);
      os << " --> " << getMapSetId(memref) << std::endl;
    }
 
}


  } // end of Alias namespace
} // end of OA namespace
