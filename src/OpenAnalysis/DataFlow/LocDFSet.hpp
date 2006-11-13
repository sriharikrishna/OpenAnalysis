/*! \file
  
  \brief DataflowSet that consists of a set of Locations.

  \authors Michelle Strout 
  \version $Id: LocDFSet.hpp,v 1.2 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef LocDFSet_H
#define LocDFSet_H

#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>

// abstract interface that this class implements
#include <OpenAnalysis/DataFlow/DataFlowSet.hpp>

#include <OpenAnalysis/Location/Location.hpp>
#include <OpenAnalysis/Location/LocationVisitor.hpp>
#include <OpenAnalysis/DataFlow/CalleeToCallerVisitor.hpp>

namespace OA {
  namespace DataFlow {

class LocDFSet : public virtual DataFlowSet {
public:
  LocDFSet();
  LocDFSet(const LocDFSet &other);
  ~LocDFSet() {}


  OA_ptr<DataFlowSet> clone();
  
  // param for these can't be const because will have to 
  // dynamic cast to specific subclass
  bool operator ==(DataFlowSet &other) const;

  bool operator !=(DataFlowSet &other) const;

  LocDFSet& setUnion(DataFlowSet &other);

  LocDFSet& setIntersect(LocDFSet &other);

  LocDFSet& setDifference(LocDFSet &other);

  void dump(std::ostream &os);

  void dump(std::ostream &os, OA_ptr<IRHandlesIRInterface> ir);
  
  //========================================================
  // methods specific to LocDFSet
  //========================================================
  
  void insert(OA_ptr<Location> loc);
  void remove(OA_ptr<Location> loc);

  //! whether or not the set is empty
  bool empty();
  
  //! whether or not the set contains the given location
  bool hasLoc(OA_ptr<Location> loc);

  //! whether or not the set contains a location that overlaps with
  //! the given location
  bool hasOverlapLoc(OA_ptr<Location> loc);

  //! iterator over locations
  OA_ptr<LocIterator> getLocIterator();

  //! iterator over locations in set that overlap with given location
  OA_ptr<LocIterator> getOverlapLocIterator(OA_ptr<Location> loc);

  //! converts this set of locations to equivalent set in callee
  //! and returns a newly created set
  OA_ptr<LocDFSet> callerToCallee(ExprHandle call, ProcHandle caller,
          OA_ptr<Alias::InterAliasInterface> interAlias,
          OA_ptr<ParamBindings> paramBind);

  //! converts this set of locations to equivalent set in caller
  //! and returns a newly created set
  OA_ptr<LocDFSet> calleeToCaller(ExprHandle call, ProcHandle caller,
          OA_ptr<Alias::InterAliasInterface> interAlias,
          OA_ptr<ParamBindings> paramBind);

protected:
  friend class LocDFSetIterator;

  // maintaining the locations in a set and also in sets based on their
  // base location, will always keep set up-to-date,
  // will memoize mapping of base locations to sets of locs when it
  // is needed for routines like mayOverlap
  OA_ptr<LocSet> mSetPtr;

  //-------------------------------------------------------
  // Data structures and code for making this faster
  //-------------------------------------------------------
private:
  // all part of map to make using LocDFSet quicker, especially
  // hasOverlapLoc and getOverlapLocIterator
  bool mBaseLocToSetMapValid;
  bool mHasUnknownLoc;
  OA_ptr<LocSet> mInvLocs;    // separate set of invisible locations
  // mapping for base named and unnamed locs to set of locations that
  // may overlap with each, doing mapping based on IRHandle for speed
  std::map<IRHandle,OA_ptr<LocSet> > mBaseLocToSetMap;

  void updateMap();
  void removeFromMap(OA_ptr<Location> loc);
  void addToMap(OA_ptr<Location> loc);
  void associateWithBaseHandle(IRHandle baseHandle, OA_ptr<Location> loc);
  void disassociateWithBaseHandle(IRHandle baseHandle, OA_ptr<Location> loc);
  bool setHasOverlapLoc(OA_ptr<Location> loc, OA_ptr<LocSet> aSet);
  OA_ptr<LocDFSet>  overlapLocSet(OA_ptr<Location> loc, OA_ptr<LocSet> aSet);

};

//! general iterator for Locations
class LocDFSetIterator : public LocSetIterator {
public:
  LocDFSetIterator(LocDFSet& pDFSet) 
      : LocSetIterator(pDFSet.mSetPtr) {}
  ~LocDFSetIterator() { }
};



  } // end of DataFlow namespace
} // end of OA namespace

#endif
