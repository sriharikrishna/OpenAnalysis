/*! \file
  
  \brief Implementation of XAIF::AliasMapXAIF

  \author Michelle Strout
  \version $Id: AliasMapXAIF.cpp,v 1.3 2005/02/16 21:48:20 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "AliasMapXAIF.hpp"

namespace OA {
  namespace XAIF {
static bool debug = false;

int AliasMapXAIF::sCurrentSetId = 1;

AliasMapXAIF::AliasMapXAIF(ProcHandle p) : mProcHandle(p)
{
}

//! get iterator over all memory references that information is
//! available for
OA_ptr<MemRefHandleIterator> AliasMapXAIF::getMemRefIter()
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

    OA_ptr<MemRefHandleIterator> retIter; 
    retIter = new Alias::AliasMapMemRefIter(memrefSet);
    return retIter;
}

//! get unique id for the alias map set for this memory reference,
//! SET_ID_NONE indicates that this memory reference doesn't map to any of
//! the existing AliasMap sets
int AliasMapXAIF::getMapSetId(MemRefHandle ref) 
{
    std::map<MemRefHandle,int>::iterator pos;
    pos = mMemRefToIdMap.find(ref);
    if (pos != mMemRefToIdMap.end()) {
        return pos->second;
    } else {
        return AliasMapXAIF::SET_ID_NONE;
    }
}


//! get id for an alias map set that contains
//! an equivalent set of locations, SET_ID_NONE indicates 
//! that no equivalent location set was found in any AliasMap sets 
int AliasMapXAIF::getMapSetId(OA_ptr<std::set<LocTuple> > pLocTupleSet)
{
    int retval = SET_ID_NONE;

    // loop through location sets and compare them to given location set
    std::map<int,OA_ptr<std::set<LocTuple> > >::iterator mapIter;
    for (mapIter=mIdToLocTupleSetMap.begin(); mapIter!=mIdToLocTupleSetMap.end();
         mapIter++)
    {
        OA_ptr<std::set<LocTuple> > mapLocTupleSet = mapIter->second;
        if (pLocTupleSet == mapLocTupleSet)
        {
            return mapIter->first;
        } 
    }

    return retval;
}
 
OA_ptr<IdIterator> AliasMapXAIF::getIdIterator()
{
    OA_ptr<IdIterator> retval;
    retval = new IdIterator(mIdToLocTupleSetMap);
    return retval;
}

//! get iterator over all locations in a particular set
OA_ptr<LocTupleIterator> AliasMapXAIF::getLocIterator(int setId) 
{ OA_ptr<LocTupleIterator> retval;
  retval = new LocTupleIterator(*mIdToLocTupleSetMap[setId]); 
  return retval;
}


//! create an empty alias map set and return the id
int AliasMapXAIF::makeEmptySet()
{
    int newsetId = sCurrentSetId;
    sCurrentSetId++;
    return newsetId;
}
     
//! associate the given location set with the given mapSet
void AliasMapXAIF::mapLocTupleSet(OA_ptr<std::set<LocTuple> > ltSet, int setId)
{
    mIdToLocTupleSetMap[setId] = ltSet;
}

//! associate a MemRefHandle with the given mapset,
//! means that the MemRefHandle can access all of the locations
//! in the mapset, if only one full location then is a must access
void AliasMapXAIF::mapMemRefToMapSet(MemRefHandle ref, int setId)
{
    // if this memref is already pointed to one mapset then need
    // to take it out of mIdToMemRefSetMap for the current set id
    int currSetId = getMapSetId(ref);
    if (currSetId != AliasMapXAIF::SET_ID_NONE) {
        mIdToMemRefSetMap[currSetId].erase(ref);
    } 
    mMemRefToIdMap[ref] = setId;
    mIdToMemRefSetMap[setId].insert(ref);
}


//! incomplete output of info for debugging 
void AliasMapXAIF::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    // print locations for each set, ID : { LocTuple }
    os << "============= AliasMapXAIF ============" << std::endl;
    OA_ptr<IdIterator> idIterPtr = getIdIterator();
    for ( ; idIterPtr->isValid(); ++(*idIterPtr) ) {
      int i = idIterPtr->current();
      os << "AliasMapSet[" << i << "] = { ";
      OA_ptr<LocTupleIterator> locIterPtr = getLocIterator(i);
      for (; locIterPtr->isValid(); ++(*locIterPtr) ) {
        LocTuple loc = locIterPtr->current();
        os << ", ";
        os << "< " << loc.getLocRange().getStart() << "..";
        os << loc.getLocRange().getEnd() << ", " << loc.isFull();
        os << " > ";
      }
      os << " }" << std::endl;
    }

    // print all memrefs and their mapping
    os << "MemRef mapping to AliasMap sets:" << std::endl;
    OA_ptr<MemRefHandleIterator> memIterPtr = getMemRefIter();
    for (; memIterPtr->isValid(); (*memIterPtr)++) {
      MemRefHandle memref = memIterPtr->current();
      os << "(" << memref << ") " << ir->toString(memref);
      os << " --> " << getMapSetId(memref) << std::endl;
    }
}


  } // end of Alias namespace
} // end of OA namespace
