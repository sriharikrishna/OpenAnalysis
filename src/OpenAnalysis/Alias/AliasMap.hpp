/*! \file
  
  \brief Maps MemRefHandle's to a particular alias map set and satisfies 
         DataFlowSet interface.

  \authors Michelle Strout
  \version $Id: AliasMap.hpp,v 1.23 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  Each map set has an id and a set of virtual storage locations 
  that memory references mapped to the alias set might reference.
*/

#ifndef AliasMap_H
#define AliasMap_H

#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/Alias/Interface.hpp>
#include <OpenAnalysis/MemRefExpr/MemRefExpr.hpp>
#include <OpenAnalysis/Location/Location.hpp>

#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/OABase/Annotation.hpp>

namespace OA {
  namespace Alias {

typedef std::set<MemRefHandle> MemRefSet;
typedef std::set<OA_ptr<MemRefExpr> > MemRefExprSet;
typedef std::set<OA_ptr<Location> > LocSet;

//! general iterator for MemRefExpr that are in the AliasMap
class AliasMapMemRefIter : public virtual MemRefIterator,
                           public IRHandleSetIterator<MemRefHandle>
{
  public:
    AliasMapMemRefIter(OA_ptr<std::set<MemRefHandle> > pSet) 
        : IRHandleSetIterator<MemRefHandle>(pSet) {}
    ~AliasMapMemRefIter() {}

    void operator++() { IRHandleSetIterator<MemRefHandle>::operator++(); }
    bool isValid() const 
      { return IRHandleSetIterator<MemRefHandle>::isValid(); }
    MemRefHandle current() const 
      { return IRHandleSetIterator<MemRefHandle>::current(); }
    void reset() { IRHandleSetIterator<MemRefHandle>::current(); }
};

//! iterator over alias map set ids
class IdIterator {
  public:
    IdIterator(std::map<int, OA_ptr<LocSet> >& aMap)
      {
        // iterate through ids and put them in a set
        std::map<int,OA_ptr<LocSet> >::iterator mapIter;
        for (mapIter=aMap.begin(); mapIter!=aMap.end(); mapIter++) {
            mSet.insert(mapIter->first);
        }
        reset();
      }
    ~IdIterator() {}

    void operator++() { if (isValid()) mIter++; }
    bool isValid() const  { return mIter!=mSet.end(); }
    int current() const  { return *mIter; }
    void reset() { mIter = mSet.begin(); }
  private:
    std::set<int> mSet;
    std::set<int>::iterator mIter;
};

/*! Satisfies the Alias::Interface and Annotation interface.
*/
class AliasMap : public virtual Alias::Interface,
                 public virtual Annotation 
{
  public:
    AliasMap(ProcHandle p);
    AliasMap(AliasMap& other);
    ~AliasMap() {}

    static const int SET_ID_NONE = -1; 

    //*****************************************************************
    // Alias::Interface 
    //*****************************************************************
    
    //! indicate the aliasing relationship between two memory references
    AliasResultType alias(MemRefHandle ref1, MemRefHandle ref2);
    
    //! iterator over locations that a memory reference may reference
    //! the user must delete the iterator
    OA_ptr<LocIterator> getMayLocs(MemRefHandle ref);

    //! iterator over locations that a memory reference must reference
    //! these locations will all have full static overlap
    //! the user must delete the iterator
    OA_ptr<LocIterator> getMustLocs(MemRefHandle ref);


    //! get iterator over all must aliases for a specific mem ref
    //! User must delete the iterator
    OA_ptr<MemRefIterator> getMustAliases(MemRefHandle ref);

    //! get iterator over all may aliases for a specific mem ref
    //! User must delete the iterator
    OA_ptr<MemRefIterator> getMayAliases(MemRefHandle ref);

    //! get iterator over all must aliases for a specific location
    //! User must delete the iterator
    OA_ptr<MemRefIterator> getMustAliases(OA_ptr<Location> loc);

    //! get iterator over all may aliases for a specific location
    //! User must delete the iterator
    OA_ptr<MemRefIterator> getMayAliases(OA_ptr<Location> loc);

    //! get iterator over all memory references that information is
    //! available for
    //! User must delete the iterator
    OA_ptr<MemRefIterator> getMemRefIter();

    //*****************************************************************
    // Info methods unique to Alias::AliasMap
    //*****************************************************************

    //! get unique id for the alias map set for this memory reference
    //! SET_ID_NONE indicates that this memory reference doesn't map to any of
    //! the existing AliasMap sets
    int getMapSetId(MemRefHandle ref); 
     
    //! get unique id for the alias map set for this memory reference
    //! expression or an equivalent memory reference expression
    //! SET_ID_NONE indicates that this memory reference expression
    //! doesn't map to any of the existing AliasMap sets
    int getMapSetId(OA_ptr<MemRefExpr> mre); 
    
    //! get id for an alias map set that contains
    //! the given location, SET_ID_NONE indicates that the given
    //! location isn't in any AliasMap sets 
    int getMapSetId(OA_ptr<OA::Location> pLoc); 
     
    //! get id for an alias map set that contains
    //! an equivalent set of locations, SET_ID_NONE indicates 
    //! that no equivalent location set was found in any AliasMap sets 
    int getMapSetId(LocSet pLocSet); 

    //! get iterator over ids for this alias map
    OA_ptr<IdIterator> getIdIterator();
 
    //! look up location associated with a symbol
    //! returns NULL if one isn't found
    //OA_ptr<Location> getLocForSym(SymHandle sym);

    //! get iterator over all locations in a particular set
    OA_ptr<LocIterator> getLocIterator(int setId); 

    //! is the equivalence set a must alias equivalence set
    bool isMust(int setId)
    { return mIdToSetStatusMap[setId]; }

    //*****************************************************************
    // Construction methods 
    //*****************************************************************
    
    //! set the starting id for the sets
    //void setStartId(int s) { mStartId = s; }

    //! get the next id that this guy will use if it needs to make
    //! another map set
    int getNextId() { return mStartId + mNumSets; }

    //! create a new alias map set and return the id
    int makeEmptySet();
 
    //! associate a MemRefHandle with the given mapSet,
    //! means that the MemRefHandle can access all of the locations
    //! in the mapSet
    void mapMemRefToMapSet(MemRefHandle ref, int setId);

    //! associate a MemRefExpr with the given mapSet,
    //! means that the MemRefExpr can access all of the locations
    //! in the mapSet, if only one full location then is a must access
    void mapMemRefToMapSet(OA_ptr<MemRefExpr> ref, int setId);

    //! add the given location to the set of possible locations for the
    //! given alias map set
    void addLocation(OA_ptr<Location> loc, int setId);

    //! merge all alias map sets that involve the given locations
    //! and add a new set if necessary containing these two locations
    void aliasLocs(OA_ptr<Location> loc1, OA_ptr<Location> loc2);

    //! remap all memrefhandles and memrefexprs to the new set
    //! and remove the oldSet
    void remapMemRefs(int oldSetId, int newSetId);

    //*****************************************************************
    // Output
    //*****************************************************************

    //! will use OutputBuilder to generate output 
    void output(OA_ptr<IRHandlesIRInterface> pIR);

    //! incomplete output of info for debugging, just lists map 
    //! set Ids and associated set of locations and mapping of 
    //! memrefs to the map set Ids
    void dump(std::ostream& os, OA_ptr<OA::IRHandlesIRInterface> ir);

  private:
    // data members
    ProcHandle mProcHandle; // procedure these sets are associated with

    // keep track of id mapping to location sets and status with a map
    int mNumSets; 
    int mStartId;
    std::map<int,OA_ptr<LocSet> > mIdToLocSetMap; 
    std::map<int,AliasResultType> mIdToSetStatusMap;

    // what memory references map to this alias map set
    std::map<int,MemRefSet> mIdToMemRefSetMap;

    // what memory reference expressions map to this alias map set
    std::map<int,MemRefExprSet> mIdToMRESetMap;

    // the location set a MemRefHandle maps to
    std::map<MemRefHandle,int> mMemRefToIdMap;  

    // set of memrefexpr for a memref
    //std::map<MemRefHandle,std::set<OA_ptr<MemRefExpr> > > 
    //    mMemRefToMRESetMap;

    // the location set a MemRefExpr maps to
    std::map<OA_ptr<MemRefExpr>,int> mMREToIdMap;  

    // the location set a Location belongs to
    std::map<OA_ptr<Location>,int> mLocToIdMap;    

};


  } // end of Alias namespace
} // end of OA namespace

#endif

