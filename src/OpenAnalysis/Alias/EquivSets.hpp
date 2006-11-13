/*! \file
  
  \brief Maps memory references into non-overlapping equivalence classes.

  \authors Michelle Strout
  \version $Id: EquivSets.hpp,v 1.18 2005/03/17 21:47:45 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef AliasEquivSets_H
#define AliasEquivSets_H

#include <cassert>
#include <iostream>
#include <set>
#include <vector>
#include <queue>
#include <map>

#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/Utils/OutputBuilder.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/Alias/Interface.hpp>
#include <OpenAnalysis/MemRefExpr/MemRefExpr.hpp>
#include <OpenAnalysis/Location/Location.hpp>

namespace OA {
  namespace Alias {

typedef std::set<MemRefHandle> MemRefSet;
typedef std::set<OA_ptr<Location> > LocSet;


//! general iterator for MemRefHandles
class EquivSetsMemRefIter : public MemRefIterator {
public:
  EquivSetsMemRefIter(OA_ptr<MemRefSet> pSet) : mSet(pSet) 
    { mIter = mSet->begin(); }
  ~EquivSetsMemRefIter() { }

  MemRefHandle current() const { return *mIter; }
  bool isValid()  const { return (mIter != mSet->end()); }
        
  void operator++() { if (mIter!=mSet->end()) ++mIter; }

  void reset() { mIter = mSet->begin(); }

private:
  OA_ptr<MemRefSet>  mSet;
  MemRefSet::iterator mIter;
};

/*
//! general iterator for Locations
class EquivSetsLocIter : public LocIterator {
public:
  EquivSetsLocIter(OA_ptr<LocSet> pSet) : mSet(pSet) 
    { mIter = mSet->begin(); }
  ~EquivSetsLocIter() { }

  OA_ptr<Location> current() { return *mIter; }
  bool isValid() { return (mIter != mSet->end()); }
        
  void operator++() { if (mIter!=mSet->end()) ++mIter; }

  void reset() { mIter = mSet->begin(); }

private:
  OA_ptr<LocSet> mSet;
  LocSet::iterator mIter;
};
*/


class EquivSets : public Alias::Interface {
  public:
    EquivSets(ProcHandle p) : mProcHandle(p), mNumSets(0), mSetEnd(0),
                              mRefSets(10), mSetLocation(10), mSetStatus(10) 
        { mKnownMemRefs = new MemRefSet; }
    ~EquivSets() {}

    static const int SET_ID_NONE = -1; 

    //! indicate the aliasing relationship between two memory references
    AliasResultType alias(MemRefHandle ref1, 
                          MemRefHandle ref2);

    //! iterator over locations that a memory reference may reference
    OA_ptr<LocIterator> getMayLocs(MemRefHandle ref);

    //! iterator over locations that a memory reference must reference
    //! these locations will all have full static overlap
    OA_ptr<LocIterator> getMustLocs(MemRefHandle ref);

    //! get iterator over all must aliases for a specific mem ref
    OA_ptr<MemRefIterator> getMustAliases(MemRefHandle ref);

    //! get iterator over all may aliases for a specific mem ref
    //! User must delete the iterator
    OA_ptr<MemRefIterator> getMayAliases(MemRefHandle ref);

    //! get iterator over all must aliases for a specific location
    OA_ptr<MemRefIterator> getMustAliases(OA_ptr<Location> loc);

    //! get iterator over all may aliases for a specific location
    OA_ptr<MemRefIterator> getMayAliases(OA_ptr<Location> loc);

    //! get iterator over all memory references that information is
    //! available for
    OA_ptr<MemRefIterator> getMemRefIter();

    //*****************************************************************
    // Info methods unique to Alias::EquivSets
    //*****************************************************************

    //! get unique id for the equivalence set for this memory reference
    //! SET_ID_NONE indicates that this memory reference doesn't map to any of
    //! the existing EquivSets
    int getEquivSetId(MemRefHandle ref); 
     
    //! get unique id for the equivalence set for this memory reference
    //! expression or an equivalent memory reference expression
    //! SET_ID_NONE indicates that this memory reference expression
    //! doesn't map to any of the existing EquivSets
    int getEquivSetId(OA_ptr<MemRefExpr> mre); 
    
    //! get unique id for the equivalence set that contains
    //! the given location, SET_ID_NONE indicates that the given
    //! location isn't in any equiv set
    int getEquivSetId(OA_ptr<OA::Location> pLoc); 
    
    //! look up location associated with a symbol
    //! returns NULL if one isn't found
    OA_ptr<Location> getLocForSym(SymHandle sym);

    //! get iterator over all mem refs in a particular set
    OA_ptr<MemRefIterator> getMemRefIter(int equivSetId); 

    //! is the equivalence set a must alias equivalence set
    bool isMust(int equivSetId); 

    //*****************************************************************
    // Construction methods 
    //*****************************************************************

    //! create an empty equiv set and return the id
    int makeEmptySet();
 
    //! associate the given location with the given equivSet
    void addLocation(OA_ptr<Location> pLoc, int equivSetId);
    
    //! associate given mre with MemRefHandle 
    void mapMemRefToMemRefExpr(MemRefHandle ref, 
                               OA_ptr<MemRefExpr> mre);

    //! associate a MemRefHandle with the given equivSet,
    //! means that the MemRefHandle can access all of the locations
    //! in the equivSet, if only one full location then is a must access
    //! if memref has already been mapped to another set then will
    //! merge the current set for memref into given set 
    void mapMemRefToEquivSet(MemRefHandle ref, int equivSetId);

    //! associate a MemRefExpr with the given equivSet,
    //! means that the MemRefExpr can access all of the locations
    //! in the equivSet, if only one full location then is a must access
    void mapMemRefToEquivSet(OA_ptr<MemRefExpr> ref, int equivSetId);

    //! associate a symbol with a location
    void mapSymToLoc(SymHandle sym, OA_ptr<Location> loc);

    //! do a trivial map of mre to loc if possible (no addressTaken
    //! and no dereferencing), otherwise return NULL
    OA_ptr<Location> trivialMREToLoc(OA_ptr<MemRefExpr> mre);

    //! associate the given location with the given equivSet
    //! and specify that all memory references that map to this
    //! location set must access this location
    //! If the equivSet has been set to may already the equiv set
    //! will stay at may
    //void addLocationMust(Location* pLoc, int equivSetId);

    //! insert the given mem ref into the equiv set
    //! and keep the equiv set as a must set if it already is one
    //void insertIntoMust(MemRefHandle ref, int equivSetId);

    //! insert the given mem ref into the equiv set
    //! and make the equiv set a may set 
    //void insertIntoMay(MemRefHandle ref, int equivSetId);

    //! merge the first set into the second set, after this operation
    //! the first set's Id will no longer be valid
    void mergeInto(int firstSetId, int secondSetId);

    //*****************************************************************
    // Output
    //*****************************************************************

    //! set a different OutputBuilder, encapsulates stream and 
    //! format (txt, xml, etc)
    //void configOutput(OutputBuilder& ob) { mOB = ob; }
    //! will use OutputBuilder to generate output with persistent handles
    //void output(MemRefExprIRShortCircuit& pIR);
    //! will use OutputBuilder to generate output with string reps of handles
    //void outputPretty(MemRefExprIRShortCircuit& pIR);

    //! incomplete output of info for debugging, just lists
    //! sets and text representation of mem refs
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);

  private:
    // data members
    ProcHandle mProcHandle; // procedure these sets are associated with

    // using a vector to keep track of sets
    int mNumSets;   // number of valid sets
    int mSetEnd;    // how far we are in mSets, index just past last
                    // set we have worked with at any point

    // parallel arrays
    std::vector< OA_ptr<MemRefSet> > mRefSets;  // for each equivSet, what MemRefHandle's map to it
    std::vector< OA_ptr<LocSet> > mSetLocation; // locations in each equivSet
    std::vector< AliasResultType > mSetStatus;  // whether a MAYALIAS or MUSTALIAS set

    //LocSet mEmptyLocSet;
    //MemRefSet mEmptyRefSet;
    OA_ptr<MemRefSet> mKnownMemRefs;  // set of mem refs we know about

    // the location for a particular symbol
    std::map<SymHandle,OA_ptr<Location> > mSymToLocMap; 

    // the equivSet a MemRefHandle maps to
    std::map<MemRefHandle,int> mMemRefToIdMap;  

    // set of memrefexpr for a memref
    std::map<MemRefHandle,std::set<OA_ptr<MemRefExpr> > > 
        mMemRefToMRESetMap;

    // the equivSet a MemRefExpr maps to
    std::map<OA_ptr<MemRefExpr>,int> mMREToIdMap;  

    // the equivSet a Location maps to
    std::map<OA_ptr<Location>,int> mLocToIdMap;    

    std::queue<int> mEmptySetIds;


    // methods and members for implementing outputs
    //OutputBuilder& mOB;
    //bool mPretty;  // whether or not we are doing pretty output
    //! both outputs have same calls to output builder
    //void outputBoth(MemRefExprIRShortCircuit& pIR);
    //! they only differ by how they create a string for Handles
    //std::string genIRHandleString(MemRefHandle h, 
    //                              MemRefExprIRShortCircuit& pIR);
};

  } // end of Alias namespace
} // end of OA namespace

#endif

