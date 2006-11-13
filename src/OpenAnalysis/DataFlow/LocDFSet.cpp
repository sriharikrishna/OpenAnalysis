/*! \file
  
  \brief DataflowSet that consists of a set of Locations.

  \authors Michelle Strout 
  \version $Id: LocDFSet.cpp,v 1.4 2005/07/01 02:49:57 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "LocDFSet.hpp"

namespace OA {
  namespace DataFlow {

#if defined(DEBUG_ALL) || defined(DEBUG_LocDFSet)
static bool debug = true;
#else
static bool debug = false;
#endif


LocDFSet::LocDFSet() : mBaseLocToSetMapValid(true), mHasUnknownLoc(false)
{ 
    mSetPtr = new LocSet; 

    mInvLocs = new LocSet;
}

LocDFSet::LocDFSet(const LocDFSet &other) 
    : mBaseLocToSetMapValid(other.mBaseLocToSetMapValid), 
      mHasUnknownLoc(other.mHasUnknownLoc),
      mBaseLocToSetMap(other.mBaseLocToSetMap)
{   mSetPtr = new LocSet; 
    *mSetPtr = *(other.mSetPtr);

    /*
    LocSet::iterator locIter;
    // copy set
    for (locIter=other.mSetPtr->begin(); locIter!=other.mSetPtr->end();
         locIter++) 
    {
      mSetPtr->insert(*locIter);
    }
    */
}


OA_ptr<DataFlowSet> LocDFSet::clone()
{ 
  OA_ptr<LocDFSet> retval;
  retval = new LocDFSet(*this); 
  return retval; 
}
  
// param for these can't be const because will have to 
// dynamic cast to specific subclass
bool LocDFSet::operator ==(DataFlowSet &other) const
{ 
    LocDFSet& recastOther 
        = dynamic_cast<LocDFSet&>(other);
    return *mSetPtr == *(recastOther.mSetPtr); 
}

bool LocDFSet::operator !=(DataFlowSet &other) const
{ 
    LocDFSet& recastOther 
        = dynamic_cast<LocDFSet&>(other);
    return *mSetPtr != *(recastOther.mSetPtr); 
}

LocDFSet& LocDFSet::setUnion(DataFlowSet &other)
{ 
    LocDFSet& recastOther 
        = dynamic_cast<LocDFSet&>(other);
    OA_ptr<LocSet> temp; temp = new LocSet;
    std::set_union(mSetPtr->begin(), mSetPtr->end(), 
                   recastOther.mSetPtr->begin(), recastOther.mSetPtr->end(),
                   std::inserter(*temp,temp->end()));
    *mSetPtr = *temp;
    mBaseLocToSetMapValid = false;
    return *this;
}

LocDFSet& LocDFSet::setIntersect(LocDFSet &other)
{ 
    OA_ptr<LocSet> temp; temp = new LocSet;
    std::set_intersection(mSetPtr->begin(), mSetPtr->end(), 
                          other.mSetPtr->begin(), other.mSetPtr->end(),
                          std::inserter(*temp,temp->end()));
    *mSetPtr = *temp;
    mBaseLocToSetMapValid = false;
    return *this;
}

LocDFSet& LocDFSet::setDifference(LocDFSet &other)
{ 
    OA_ptr<LocSet> temp; temp = new LocSet;
    std::set_difference(mSetPtr->begin(), mSetPtr->end(), 
                        other.mSetPtr->begin(), other.mSetPtr->end(),
                        std::inserter(*temp,temp->end()));
    *mSetPtr = *temp;
    mBaseLocToSetMapValid = false;
    return *this;
}

//! converts this set of locations to equivalent set in callee
//! and returns a newly created set
//! Transitive in that if a parameter being passed in may access
//! a location in the set then the formal parameter is put in the returned
//! set even if it isn't a reference parameter.
OA_ptr<LocDFSet> LocDFSet::callerToCallee(ExprHandle call, 
          ProcHandle caller,
          OA_ptr<Alias::InterAliasInterface> interAlias,
          OA_ptr<ParamBindings> paramBind)
{
  OA_ptr<DataFlow::LocDFSet> retval;
  retval = new DataFlow::LocDFSet;
  if (debug) { 
      std::cout << "callerToCallee" << std::endl;
  }

  // get alias results for caller procedure
  OA_ptr<Alias::Interface> alias = interAlias->getAliasResults(caller);

  // iterate over all the parameters for the call
  OA_ptr<MemRefHandleIterator> memrefIter = paramBind->getActualIterator(call);
  for (; memrefIter->isValid(); (*memrefIter)++ ) {
      MemRefHandle memref = memrefIter->current();
      if (debug) { 
          std::cout << "\tmemref = " << memref.hval() << std::endl;
      }

      // loop over may locs for the memref
      OA_ptr<LocIterator> locIter = alias->getMayLocs(memref);
      for ( ; locIter->isValid(); (*locIter)++ ) {
        OA_ptr<Location> loc = locIter->current();
        if (debug) {
            std::cout << "\t\tmayloc for memref, loc = ";
            loc->dump(std::cout);
        }
        if (hasOverlapLoc(loc)) {
          SymHandle sym = paramBind->getCalleeFormal(call,memref);
          OA_ptr<Location> nloc;
          nloc = new NamedLoc(sym,true);
          retval->insert(nloc);
          if (debug) {
            std::cout << "\t\thasOverlapLoc so inserting sym = " 
                      << sym.hval() << std::endl;
          }
        }
      }
  }

  // loop over all locations in this set and put all non-local locations
  // into the set for the callee
  OA_ptr<LocIterator> locIter = getLocIterator();
  for ( ; locIter->isValid(); (*locIter)++ ) {
      OA_ptr<Location> loc = locIter->current();
      if (loc->isLocal()==false) {
          retval->insert(loc);
      }
  }

  return retval;
}

//! converts this set of locations to equivalent set in caller
//! and returns a newly created set
OA_ptr<LocDFSet> LocDFSet::calleeToCaller(ExprHandle call, ProcHandle caller,
          OA_ptr<Alias::InterAliasInterface> interAlias,
          OA_ptr<ParamBindings> paramBind)
{
    OA_ptr<DataFlow::LocDFSet> retval;
    retval = new DataFlow::LocDFSet;

    // have visitor convert all locations into aliased locations in
    // the caller
    OA_ptr<DataFlow::CalleeToCallerVisitor> locVisitor; 
    locVisitor = new DataFlow::CalleeToCallerVisitor(call, 
            caller, interAlias, paramBind);

    OA_ptr<LocIterator> locIter = getLocIterator();
    for ( ; locIter->isValid(); (*locIter)++ ) {
        OA_ptr<Location> loc = locIter->current();
        loc->acceptVisitor(*locVisitor);
        OA_ptr<LocIterator> callerLoc = locVisitor->getCallerLocIterator();
        for ( ; callerLoc->isValid(); (*callerLoc)++ ) {
            retval->insert(callerLoc->current());
        }
    }

    return retval;
}


void LocDFSet::dump(std::ostream &os)
{
    os << "LocDFSet: mSet = ";
    // iterate over IRHandle's and have the IR print them out
    OA_ptr<LocIterator> locIter = getLocIterator();
    for ( ; locIter->isValid(); (*locIter)++ ) {
        OA_ptr<Location> loc = locIter->current();
        loc->dump(os);
        os << ", ";
    }
    os << std::endl;
}



void LocDFSet::dump(std::ostream &os, OA_ptr<IRHandlesIRInterface> ir)
{
    os << "LocDFSet: mSet = ";
    // iterate over IRHandle's and have the IR print them out
    OA_ptr<LocIterator> locIter = getLocIterator();
    for ( ; locIter->isValid(); (*locIter)++ ) {
        OA_ptr<Location> loc = locIter->current();
        loc->dump(os,ir);
        os << ", ";
    }
    os << std::endl;
}

void 
LocDFSet::associateWithBaseHandle(IRHandle baseHandle, OA_ptr<Location> loc)
{
    if (mBaseLocToSetMap[baseHandle].ptrEqual(0)) {
        mBaseLocToSetMap[baseHandle] = new LocSet;
    }
    mBaseLocToSetMap[baseHandle]->insert(loc);
}

// NOTE: can't use a visitor because will be inserting loc into
// sets that want OA_ptrs.  More compact this way anyway.
void LocDFSet::addToMap(OA_ptr<Location> loc)
{
    // get the base location
    OA_ptr<Location> baseLoc = loc->getBaseLoc();

    // Named Location
    if (baseLoc->isaNamed()) {
        OA_ptr<NamedLoc> namedLoc = baseLoc.convert<NamedLoc>();
        associateWithBaseHandle(namedLoc->getSymHandle(), loc);

        OA_ptr<SymHandleIterator> symIter = namedLoc->getFullOverlapIter();
        for ( ; symIter->isValid(); (*symIter)++ ) {
            associateWithBaseHandle(symIter->current(), loc);
        }
        symIter = namedLoc->getPartOverlapIter();
        for ( ; symIter->isValid(); (*symIter)++ ) {
            associateWithBaseHandle(symIter->current(), loc);
        }

    // UnnamedLoc
    } else if (baseLoc->isaUnnamed()) {
        OA_ptr<UnnamedLoc> unnamedLoc = baseLoc.convert<UnnamedLoc>();
        associateWithBaseHandle(unnamedLoc->getStmtHandle(), loc);

    // InvisibleLoc
    } else if (baseLoc->isaInvisible()) {
        mInvLocs->insert(loc);

    // UnknownLoc
    } else if (baseLoc->isaUnknown()) {
        mHasUnknownLoc = true;
    }
 
}

void LocDFSet::insert(OA_ptr<Location> loc) 
{ 
    if ( ! hasLoc(loc) ) {
        mSetPtr->insert(loc); 
        addToMap(loc);
    }
}


void 
LocDFSet::disassociateWithBaseHandle(IRHandle baseHandle, OA_ptr<Location> loc)
{
    if (!mBaseLocToSetMap[baseHandle].ptrEqual(0)) {
        mBaseLocToSetMap[baseHandle]->erase(loc);
    }
}

void LocDFSet::removeFromMap(OA_ptr<Location> loc)
{
    // get the base location
    OA_ptr<Location> baseLoc = loc->getBaseLoc();

    // Named Location
    if (baseLoc->isaNamed()) {
        OA_ptr<NamedLoc> namedLoc = baseLoc.convert<NamedLoc>();
        disassociateWithBaseHandle(namedLoc->getSymHandle(), loc);

        OA_ptr<SymHandleIterator> symIter = namedLoc->getFullOverlapIter();
        for ( ; symIter->isValid(); (*symIter)++ ) {
            disassociateWithBaseHandle(symIter->current(), loc);
        }
        symIter = namedLoc->getPartOverlapIter();
        for ( ; symIter->isValid(); (*symIter)++ ) {
            disassociateWithBaseHandle(symIter->current(), loc);
        }

    // UnnamedLoc
    } else if (baseLoc->isaUnnamed()) {
        OA_ptr<UnnamedLoc> unnamedLoc = baseLoc.convert<UnnamedLoc>();
        disassociateWithBaseHandle(unnamedLoc->getStmtHandle(), loc);

    // InvisibleLoc
    } else if (baseLoc->isaInvisible()) {
        mInvLocs->erase(loc);

    // UnknownLoc
    } else if (baseLoc->isaUnknown()) {
        mHasUnknownLoc = false;
    }
}

void LocDFSet::remove(OA_ptr<Location> loc) 
{ 
    if ( hasLoc(loc) ) {
        mSetPtr->erase(loc); 
        removeFromMap(loc);
    }
}

//! whether or not the set is empty
bool LocDFSet::empty() { return mSetPtr->empty(); }

void LocDFSet::updateMap()
{
    mBaseLocToSetMap.clear();

    OA_ptr<LocIterator> locIter = getLocIterator();
    for ( ; locIter->isValid(); (*locIter)++ ) {
        addToMap(locIter->current());
    }

    mBaseLocToSetMapValid = true;
}

//! whether the give loc overlaps with anything in the given set
bool LocDFSet::setHasOverlapLoc(OA_ptr<Location> loc, OA_ptr<LocSet> aSet)
{
    LocSet::iterator locIter;
    if (! aSet.ptrEqual(0) ) {
      for (locIter=aSet->begin(); locIter!=aSet->end(); locIter++) {
        OA_ptr<Location> temp = *locIter;
        if (loc->mayOverlap(*temp)) {
            return true;
        }
      }
    }
    return false;
}


//! whether or not the set contains a location that overlaps with
//! the given location
bool LocDFSet::hasOverlapLoc(OA_ptr<Location> loc) 
{ 
    // make sure the map is up-to-date
    if (!mBaseLocToSetMapValid) {
        updateMap();
    }

    if (mHasUnknownLoc) {
        return true;
    }
       
    // get the base location
    OA_ptr<Location> baseLoc = loc->getBaseLoc();

    // Named Location
    if (baseLoc->isaNamed()) {
        OA_ptr<NamedLoc> namedLoc = baseLoc.convert<NamedLoc>();
        return setHasOverlapLoc(loc, 
                mBaseLocToSetMap[namedLoc->getSymHandle()]);

    // UnnamedLoc
    } else if (baseLoc->isaUnnamed()) {
        OA_ptr<UnnamedLoc> unnamedLoc = baseLoc.convert<UnnamedLoc>();
        return setHasOverlapLoc(loc, 
                mBaseLocToSetMap[unnamedLoc->getStmtHandle()]);

    // InvisibleLoc
    } else if (baseLoc->isaInvisible()) {
        return setHasOverlapLoc(loc, mInvLocs);

    // UnknownLoc
    } else if (baseLoc->isaUnknown()) {
        return true;
    }

}

//! whether the give loc overlaps with anything in the given set
OA_ptr<LocDFSet>  
LocDFSet::overlapLocSet(OA_ptr<Location> loc, OA_ptr<LocSet> aSet)
{
    OA_ptr<LocDFSet> retset;
    retset = new LocDFSet;

    if (! aSet.ptrEqual(0) ) {
        LocSet::iterator locIter;
        for (locIter=aSet->begin(); locIter!=aSet->end(); locIter++) {
            OA_ptr<Location> temp = *locIter;
            if (loc->mayOverlap(*temp)) {
                retset->insert(temp);
            }
        }
    }
    return retset;
}

//! iterator over locations in set that overlap with given location
OA_ptr<LocIterator> LocDFSet::getOverlapLocIterator(OA_ptr<Location> loc) 
{ 
    // make sure the map is up-to-date
    if (!mBaseLocToSetMapValid) {
        updateMap();
    }

    OA_ptr<LocDFSet> retset;
    retset = new LocDFSet;

    if (loc->isaUnknown() ) {
        // iterator over all locations in this set
        return getLocIterator();
    }

    // add the unknown location if it is in this set
    if  (mHasUnknownLoc) {
        OA_ptr<Location> unknown;
        unknown = new UnknownLoc();
        retset->insert(unknown);
    }

    // get the base location
    OA_ptr<Location> baseLoc = loc->getBaseLoc();

    // Named Location
    if (baseLoc->isaNamed()) {
        OA_ptr<NamedLoc> namedLoc = baseLoc.convert<NamedLoc>();
        OA_ptr<LocDFSet> tempSet = overlapLocSet(loc, 
                mBaseLocToSetMap[namedLoc->getSymHandle()]) ;
        retset->setUnion( *tempSet );

    // UnnamedLoc
    } else if (baseLoc->isaUnnamed()) {
        OA_ptr<UnnamedLoc> unnamedLoc = baseLoc.convert<UnnamedLoc>();
        OA_ptr<LocDFSet> tempSet = overlapLocSet(loc, 
                mBaseLocToSetMap[unnamedLoc->getStmtHandle()]);
        retset->setUnion( *tempSet );

    // InvisibleLoc
    } else if (baseLoc->isaInvisible()) {
        OA_ptr<LocDFSet> tempSet = overlapLocSet(loc, mInvLocs);
        retset->setUnion( *tempSet );
    }

    OA_ptr<LocDFSetIterator> retval;
    retval = new LocDFSetIterator(*retset);
    return retval;
}

//! whether or not the set contains the given location
bool LocDFSet::hasLoc(OA_ptr<Location> loc) 
{ 
    return mSetPtr->find(loc)!=mSetPtr->end(); 
}

//! iterator over locations
OA_ptr<LocIterator> LocDFSet::getLocIterator()
{
    OA_ptr<LocDFSetIterator> retval;
    retval = new LocDFSetIterator(*this);
    return retval;
}


  } // end of DataFlow namespace
} // end of OA namespace

