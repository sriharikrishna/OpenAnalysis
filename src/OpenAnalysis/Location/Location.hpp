/*! \file
  
  \brief Declarations for the Location abstraction.

  \authors Michelle Strout
  \version $Id: Location.hpp,v 1.26 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef Location_H
#define Location_H

#include <iostream>
#include <cassert>
#include <list>
#include <set>
#include <algorithm>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/OABase/Annotation.hpp>
#include <OpenAnalysis/MemRefExpr/MemRefExpr.hpp>

namespace OA {

class Location;

// to avoid circular reference in header files
class LocationVisitor;


void dumpLocSet(std::set<OA_ptr<Location> >& set, std::ostream& os, 
                OA_ptr<IRHandlesIRInterface> pIR);

/*!
   Function that intersects sets of Location
*/

OA_ptr<std::set<OA_ptr<Location> > > 
intersectLocSets(std::set<OA_ptr<Location> >& set1,
                 std::set<OA_ptr<Location> >& set2);
                 
/*!
   Function the unions sets of Location
*/

OA_ptr<std::set<OA_ptr<Location> > > 
unionLocSets(std::set<OA_ptr<Location> >& set1, std::set<OA_ptr<Location> >& set2);

/*!
   Returns true if any location in set1 mayOverlap with any location
   in set2.  Otherwise returns false
*/
bool mayOverlapLocSets(std::set<OA_ptr<Location> >& set1,
                       std::set<OA_ptr<Location> >& set2);

/*!
   Function that determines if set1 of Locations* is a subset of set2
*/
bool subSetOf(std::set<OA_ptr<Location> >& set1, 
              std::set<OA_ptr<Location> >& set2);

/*! abstract base class that has default implementations for the
    methods that all Locations must provide
*/
class Location : public Annotation {
  public:
    Location() {}
    Location(Location &loc) {}
    virtual ~Location() {}

    virtual void acceptVisitor(LocationVisitor& pVisitor) = 0;

    //*****************************************************************
    // Subclass type methods 
    //*****************************************************************
    virtual bool isaNamed() { return false; }
    virtual bool isaUnnamed() { return false; }
    virtual bool isaUnknown() { return false; }
    virtual bool isaInvisible() { return false; }
    virtual bool isaSubSet() { return false; }

    //*****************************************************************
    // Info methods 
    //*****************************************************************
    
    //! return base location for this location
    virtual OA_ptr<Location> getBaseLoc() = 0;
    
    //! true if the location block represents a local variable that is 
    //! only visible within the current procedure or an unnamed variable 
    //! that is allocated in the current procedure. If the current procedure
    //! is nested and has access to local variables of a parent procedure, 
    //! those variables are not considered local for the current procedure.
    virtual bool isLocal() { return false; }

    //! be because there could be multiple allocations at the same 
    //! statement and they maybe lumped together depending on how much
    //! context is maintained. A LocBlock for an unnamed allocated memory 
    //! block could be Unique if there is a different LocBlock for each 
    //! possible context of the alloc.
    //! A local variable is always unique, allocated memory blocks may
    virtual bool isUnique() { return false; }
    
    //! does the Location abstraction accurately represent a 
    //! specific memory location, for example, if a range in an
    //! array then false, if one index of an array then true
    virtual bool hasFullAccuracy() { return true; }


    //*****************************************************************
    // Relationship methods 
    //*****************************************************************

    //! an ordering for locations, needed for use within STL containers
    virtual bool operator<(Location & other) = 0;

    //! indicate whether this location is semantically equivalent
    virtual bool operator==(Location & other) = 0;

    //! indicate whether this location is semantically equivalent
    virtual bool operator!=(Location & other) { return ! operator==(other); }

    //! Indicate whether this location may overlap with the given
    //! location either fully or partially.
    virtual bool mayOverlap(Location & other) { return true; }

    //! Indicate whether this location must overlap fully with the given location.
    virtual bool mustOverlap(Location & other) { return false; }

    //! Indicate whether this location is a subset of the given location
    virtual bool subSetOf(Location & other) { return true; }

    //*****************************************************************
    // Debugging
    //*****************************************************************
    virtual void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR) = 0;
    virtual void dump(std::ostream& os) = 0;

    virtual std::string toString(OA_ptr<IRHandlesIRInterface> pIR) = 0;
};

/*!
   A named location has a SymHandle associated with it. 
   Examples include local and global variables.
*/
class NamedLoc: public Location {
  public:
    NamedLoc(SymHandle sh, bool isLocal) :
        mSymHandle(sh), mLocal(isLocal) { }

    //! copy constructor
    NamedLoc(NamedLoc &other); 

    ~NamedLoc() { }

    void addFullOverlap(SymHandle s) { mFullOverlap.insert(s); }
    void addPartOverlap(SymHandle s) { mPartOverlap.insert(s); }

    void acceptVisitor(LocationVisitor& pVisitor);

    //*****************************************************************
    // Subclass type methods 
    //*****************************************************************
    bool isaNamed() { return true; }

    //*****************************************************************
    // Info methods 
    //*****************************************************************
    OA_ptr<Location> getBaseLoc(); 
    bool isLocal() { return mLocal; }
    bool isUnique() { return true; }
    SymHandle getSymHandle() { return mSymHandle; }
    bool staticFullOverlap(SymHandle);
    bool staticPartOverlap(SymHandle);
    OA_ptr<SymHandleIterator> getPartOverlapIter();
    OA_ptr<SymHandleIterator> getFullOverlapIter();

    //*****************************************************************
    // Relationship methods 
    //*****************************************************************
    bool operator<(Location & other);
    bool operator==(Location& other);

    bool mayOverlap(Location& other);

    bool mustOverlap(Location& other);

    bool subSetOf(Location& other);

    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir); 

    //*****************************************************************
    // Debugging
    //*****************************************************************
    void dump(std::ostream& os);
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);

    std::string toString(OA_ptr<IRHandlesIRInterface> pIR);


  private:
    SymHandle mSymHandle;

    bool mLocal;
    
    // need sets for these so can use comparison of sets
    std::set<SymHandle>  mFullOverlap;
    std::set<SymHandle>  mPartOverlap;
};

/*!
   An unnamed location has a StmtHandle associated with it. 
   Examples include statements that call malloc or new to allocate
   blocks of memory.  
*/
class UnnamedLoc: public Location {
  public:
    UnnamedLoc(StmtHandle h, bool isLocal) : mStmtHandle(h), mLocal(isLocal)
	    { }

    //! copy constructor
    UnnamedLoc(UnnamedLoc &other);

    ~UnnamedLoc() { }

    void acceptVisitor(LocationVisitor& pVisitor);

    //*****************************************************************
    // Subclass type methods 
    //*****************************************************************
    bool isaUnnamed() { return true; }

    //*****************************************************************
    // Info methods 
    //*****************************************************************
    OA_ptr<Location> getBaseLoc(); 
    bool isLocal() { return mLocal; }
    bool isUnique() { return false; }
    bool hasFullAccuracy() { return false; }
    StmtHandle getStmtHandle() { return mStmtHandle; }

    //*****************************************************************
    // Relationship methods 
    //*****************************************************************
    bool operator<(Location & other);
    bool operator==(Location& other);

    bool mayOverlap(Location& other);

    bool mustOverlap(Location& other);

    bool subSetOf(Location& other);

    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir);

    //*****************************************************************
    // Debugging
    //*****************************************************************
    void dump(std::ostream& os);
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);

    std::string toString(OA_ptr<IRHandlesIRInterface> pIR);


  private:
    StmtHandle mStmtHandle;
    bool mLocal;
};

/*!
   An invisible location involves a memory reference whose base is
   a param or non-local symbol.  Invisible locations are used to summarize
   information within a procedure so that callers of that procedure can
   use the summarized information.
*/
class InvisibleLoc: public Location {
  public:
    InvisibleLoc(OA_ptr<MemRefExpr> mre) : mMRE(mre) { }

    //! copy constructor
    InvisibleLoc(InvisibleLoc &other);

    ~InvisibleLoc() { }

    void acceptVisitor(LocationVisitor& pVisitor);

    //*****************************************************************
    // Subclass type methods 
    //*****************************************************************
    bool isaInvisible() { return true; }

    //*****************************************************************
    // Info methods 
    //*****************************************************************
    OA_ptr<Location> getBaseLoc(); 
    bool isLocal() { return false; }
    bool isUnique() { return false; } // actually unknown
    bool hasFullAccuracy() { return mMRE->hasFullAccuracy(); }
    OA_ptr<MemRefExpr> getMemRefExpr() { return mMRE; }

    //*****************************************************************
    // Relationship methods 
    //*****************************************************************
    bool operator<(Location & other);
    bool operator==(Location& other);

    bool mayOverlap(Location& other);

    bool mustOverlap(Location& other);

    bool subSetOf(Location& other);

    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir);

    //*****************************************************************
    // Debugging
    //*****************************************************************
    void dump(std::ostream& os);
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);

    std::string toString(OA_ptr<IRHandlesIRInterface> pIR);


  private:
    OA_ptr<MemRefExpr> mMRE;
};

/*!
   An unknown location may overlap with any location in the whole program.
*/
class UnknownLoc: public Location {
  public:
    UnknownLoc() { }

    void acceptVisitor(LocationVisitor& pVisitor);

    //! copy constructor
    UnknownLoc(UnknownLoc &other) {}

    ~UnknownLoc() { }

    //*****************************************************************
    // Subclass type methods 
    //*****************************************************************
    bool isaUnknown() { return true; }

    //*****************************************************************
    // Info methods 
    //*****************************************************************
    OA_ptr<Location> getBaseLoc(); 
    bool isLocal() { return false; } // actually unknown, conservative
    bool isUnique() { return false; } // actually unknown, conservative
    bool hasFullAccuracy() { return false; }

    //*****************************************************************
    // Relationship methods 
    //*****************************************************************
    bool operator<(Location & other);
    bool operator==(Location& other);

    bool mayOverlap(Location& other) { return true; }

    bool mustOverlap(Location& other) { return false; }

    bool subSetOf(Location& other) { return true; }

    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir);

    //*****************************************************************
    // Debugging
    //*****************************************************************
    void dump(std::ostream& os);
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);

    std::string toString(OA_ptr<IRHandlesIRInterface> pIR);

};

/*!
   The LocSubSet implements a decorator pattern for locations.
   If we decorate a location with a LocSubSet that represents some
   subset of the location depending on what subset abstraction is being used.
   Many subset abstractions are possible.
*/
class LocSubSet: public Location {
  public:
    LocSubSet(OA_ptr<Location> baseLoc);
    LocSubSet(OA_ptr<Location> baseLoc, bool full);

    //! copy constructor
    LocSubSet(LocSubSet &other) : mLoc(other.mLoc), mFull(other.mFull) {}

    ~LocSubSet() { }

    virtual void acceptVisitor(LocationVisitor& pVisitor);

    //*****************************************************************
    // Location subclass type methods 
    //*****************************************************************
    bool isaSubSet() { return true; }

    //*****************************************************************
    // LocSubSet subclass type methods 
    //*****************************************************************
    //! This needs to be logical OR of all isa methods below it
    //! If a direct instance of this class then will return false
    bool isSubClassOfLocSubSet() { return isaIdxSubSet(); }

    virtual bool isaIdxSubSet() { return false; }

    //*****************************************************************
    // Info methods 
    //*****************************************************************
    OA_ptr<Location> getBaseLoc() { return mLoc; }
    OA_ptr<Location> getLoc() { return mLoc; }

    //! something useful to have for all LocSubSet's
    virtual bool isFull() { return mFull; }
    virtual bool hasFullAccuracy() { return isFull(); }
    
    bool isLocal() { assert(!mLoc.ptrEqual(NULL));  return mLoc->isLocal(); }

    bool isUnique() { assert(!mLoc.ptrEqual(NULL));  return mLoc->isUnique(); }
    // FIXME: does this make sense for a subset?

    //*****************************************************************
    // Relationship methods
    //*****************************************************************
    virtual bool operator<(Location & other);
    virtual bool operator==(Location& other);
    virtual bool mayOverlap(Location& other);

    virtual bool mustOverlap(Location& other);

    virtual bool subSetOf(Location& other);

    // *****************************************************************
    // Debugging
    // *****************************************************************
    void dump(std::ostream& os);
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);

    std::string toString(OA_ptr<IRHandlesIRInterface> pIR);

    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir);

private:
    OA_ptr<Location> mLoc;
    bool mFull;

    //! helper function for constructor
    void composeWithBaseLoc(OA_ptr<Location> baseLoc);
};

/*!
   The LocBasicSubSet describes a subset that either covers the whole location
   or some part of that location.
*/
/*
class LocBasicSubSet: public LocSubSet {
  public:
    LocBasicSubSet(OA_ptr<Location> loc) : LocSubSet(loc), mFull(true) { }
    LocBasicSubSet(OA_ptr<Location> loc, bool isFull) : LocSubSet(loc), 
                                                  mFull(isFull) { }

    //! copy constructor
    LocBasicSubSet(LocBasicSubSet &other);

    ~LocBasicSubSet() { }

    // *****************************************************************
    // LocSubSet subclass type methods 
    // *****************************************************************
    bool isaBasicSubSet() { return true; }
    
    // *****************************************************************
    // Info methods 
    // *****************************************************************
    bool isFull() { return mFull;}
    bool hasFullAccuracy() { return isFull(); }

    // *****************************************************************
    // Relationship methods, will be defined in subclasses 
    // *****************************************************************
    bool operator<(Location & other);
    bool operator==(Location& other);
    
    bool mayOverlap(Location& other);

    bool mustOverlap(Location& other);

    bool subSetOf(Location& other);

    // *****************************************************************
    // Annotation Interface
    // *****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir);

    // *****************************************************************
    // Debugging
    // *****************************************************************
    void dump(std::ostream& os);
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);

    std::string toString(OA_ptr<IRHandlesIRInterface> pIR);

private:
    bool mFull;
};
*/

/*!
   The LocIdxSubSet describes a subset for an array that includes one
   constant index.  Way of representing an individual element of an array.
*/
class LocIdxSubSet: public LocSubSet {
  public:
    LocIdxSubSet(OA_ptr<Location> loc, int idx) : LocSubSet(loc), mIdx(idx) { }

    //! copy constructor
    LocIdxSubSet(LocIdxSubSet &other) : LocSubSet(other), mIdx(other.mIdx) {}

    ~LocIdxSubSet() { }

    void acceptVisitor(LocationVisitor& pVisitor);

    //*****************************************************************
    // LocSubSet subclass type methods 
    //*****************************************************************
    bool isaIdxSubSet() { return true; }
    
    //*****************************************************************
    // Info methods 
    //*****************************************************************
    bool isFull() { return false; }
    bool hasFullAccuracy() { return true; }
    int getIdx() { return mIdx;}

    //*****************************************************************
    // Relationship methods, will be defined in subclasses 
    //*****************************************************************
    bool operator<(Location & other);
    bool operator==(Location& other);
    
    bool mayOverlap(Location& other);

    bool mustOverlap(Location& other);

    bool subSetOf(Location& other);

    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir);

    //*****************************************************************
    // Debugging
    //*****************************************************************
    void dump(std::ostream& os);
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);

    std::string toString(OA_ptr<IRHandlesIRInterface> pIR);

private:
    int mIdx;
};

class LocIterator {
public:
  LocIterator() { }
  virtual ~LocIterator() { }

  virtual OA_ptr<Location> current() = 0;  // Returns the current item.
  virtual bool isValid() = 0;        // False when all items are exhausted.
        
  virtual void operator++() = 0;
  void operator++(int) { ++*this; } ;

  virtual void reset() = 0;
};

typedef std::set<OA_ptr<Location> > LocSet;

//! general iterator for Locations
class LocSetIterator : public LocIterator {
public:
  LocSetIterator(OA_ptr<LocSet> pSet) : mSet(pSet) 
    { mIter = mSet->begin(); }
  ~LocSetIterator() { }

  OA_ptr<Location> current() { if (isValid()) return *mIter; 
                               else { OA_ptr<Location> retval; return retval;
                                    }
                             }
  bool isValid() { return (mIter != mSet->end()); }
        
  void operator++() { if (mIter!=mSet->end()) ++mIter; }

  void reset() { mIter = mSet->begin(); }

private:
  OA_ptr<LocSet> mSet;
  LocSet::const_iterator mIter;
};

//! An iterator over Symbols
class OverlapSymIterator : public virtual OA::SymHandleIterator,
                           public OA::IRHandleSetIterator<OA::SymHandle>
{
public:
  OverlapSymIterator(OA::OA_ptr<std::set<OA::SymHandle> > pSet) 
    : OA::IRHandleSetIterator<OA::SymHandle>(pSet) {}
  ~OverlapSymIterator() {}

  void operator++() { OA::IRHandleSetIterator<OA::SymHandle>::operator++(); }
  bool isValid() const
    { return OA::IRHandleSetIterator<OA::SymHandle>::isValid(); }
  OA::SymHandle current() const
    { return OA::IRHandleSetIterator<OA::SymHandle>::current(); }
  void reset() { OA::IRHandleSetIterator<OA::SymHandle>::current(); }
};



} // end of OA namespace

#endif

