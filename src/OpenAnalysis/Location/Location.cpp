/*! \file
  
  \brief Definition for Location class hierarchy.  

  \authors Michelle Strout
  \version $Id: Location.cpp,v 1.25 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "Location.hpp"

// included here instead of in hpp file to remove circular reference
#include <OpenAnalysis/Location/LocationVisitor.hpp>

namespace OA {

#if defined(DEBUG_ALL) || defined(DEBUG_Location)
static bool debug = true;
#else
static bool debug = false;
#endif

void dumpLocSet(std::set<OA_ptr<Location> >& set, std::ostream& os, 
                OA_ptr<IRHandlesIRInterface> pIR)
{
    os << "LocSet = { ";
    std::set<OA_ptr<Location> >::iterator setIter;
    for (setIter=set.begin(); setIter!=set.end(); setIter++) {
        OA_ptr<Location> loc = *setIter;
        loc->dump(os,pIR);
    }
    os << " }" << std::endl;   
}


OA_ptr<std::set<OA_ptr<Location> > > 
intersectLocSets(std::set<OA_ptr<Location> >& set1,
                 std::set<OA_ptr<Location> >& set2)
{
    OA_ptr<LocSet> temp; temp = new LocSet;
    std::set_intersection(set1.begin(), set1.end(), 
                          set2.begin(), set2.end(),
                          std::inserter(*temp,temp->end()));
    return temp;
}


OA_ptr<std::set<OA_ptr<Location> > > 
unionLocSets(std::set<OA_ptr<Location> >& set1, std::set<OA_ptr<Location> >& set2)
{
  OA_ptr<LocSet> temp; temp = new LocSet;
  std::set_union(set1.begin(), set1.end(), 
                 set2.begin(), set2.end(),
                 std::inserter(*temp,temp->end()));
  return temp;
}


/*!
   Returns true if any location in set1 mayOverlap with any location
   in set2.  Otherwise returns false.
*/
bool mayOverlapLocSets(std::set<OA_ptr<Location> >& set1,
                       std::set<OA_ptr<Location> >& set2)
{
    if (debug) {
        std::cout << "mayOverlapLocSets: " << std::endl;
    }
    std::set<OA_ptr<Location> >::iterator setIter1;
    std::set<OA_ptr<Location> >::iterator setIter2;
    for (setIter1=set1.begin(); setIter1!=set1.end(); setIter1++) {
        for (setIter2=set2.begin(); setIter2!=set2.end(); setIter2++) {
            OA_ptr<Location> loc1 = *setIter1, loc2 = *setIter2;
            loc1 = *setIter1;
            loc2 = *setIter2;
            if (debug) {
                std::cout << "loc1 = ";
                loc1->dump(std::cout);
                std::cout << "loc2 = ";
                loc2->dump(std::cout);
            }
            if (loc1->mayOverlap(*loc2)) {
                return true;
            }
        }
    }

    return false;
}

/*
  is set1 a subset of set2?
*/
bool subSetOf(std::set<OA_ptr<Location> >& set1, std::set<OA_ptr<Location> >& set2)
{
    bool retval = true;

    std::set<OA_ptr<Location> >::iterator setIter1;
    std::set<OA_ptr<Location> >::iterator setIter2;
    for (setIter1=set1.begin(); setIter1!=set1.end(); setIter1++) {
        bool found = false;
        for (setIter2=set2.begin(); setIter2!=set2.end(); setIter2++) {
            OA_ptr<Location> loc1, loc2;
            loc1 = *setIter1;
            loc2 = *setIter2;
            if (loc1 == loc2) {
                found = true;  
                break;
            }
        }
        if (!found) {
            retval = false;
            break;
        }
    }

    return retval;

}


//*****************************************************************
// NamedLoc methods
//*****************************************************************
NamedLoc::NamedLoc(NamedLoc & other) : mSymHandle(other.mSymHandle),
                                       mLocal(other.mLocal)
{
    //std::copy(other.mFullOverlap->begin(), other.mFullOverlap->end(),
    //          mFullOverlap->begin());
    //std::copy(other.mPartOverlap.begin(), other.mPartOverlap.end(),
    //          mPartOverlap.begin());
    mFullOverlap = other.mFullOverlap;
    mPartOverlap = other.mPartOverlap;
}

void NamedLoc::acceptVisitor(LocationVisitor& pVisitor)
{
    pVisitor.visitNamedLoc(*this);
}

OA_ptr<Location> NamedLoc::getBaseLoc() 
{ 
    OA_ptr<Location> retval; 
    retval = new NamedLoc(*this);
    return retval;
}

OA_ptr<SymHandleIterator> NamedLoc::getPartOverlapIter()
{
    OA_ptr<SymHandleIterator> retval;
    OA_ptr<std::set<SymHandle> > setCopy;
    setCopy = new std::set<SymHandle>(mPartOverlap);
    retval = new OverlapSymIterator(setCopy);
    return retval;
}

OA_ptr<SymHandleIterator> NamedLoc::getFullOverlapIter()
{
    OA_ptr<SymHandleIterator> retval;
    OA_ptr<std::set<SymHandle> > setCopy;
    setCopy = new std::set<SymHandle>(mFullOverlap);
    retval = new OverlapSymIterator(setCopy);
    return retval;
}


/*! concrete LocationVisitor to handle all possible Locations
    subsets that other could be. mLessThan holds answer to
    whether *this is less than other
*/
class NamedLocLessThanVisitor : public virtual LocationVisitor {
  private:
    NamedLoc& mThisLoc;
  public:
    bool mLessThan;

    NamedLocLessThanVisitor(NamedLoc& thisLoc) 
        : mThisLoc(thisLoc), mLessThan(false) {}
    ~NamedLocLessThanVisitor() {}
    void visitNamedLoc(NamedLoc& loc) 
      { if (mThisLoc.getSymHandle() < loc.getSymHandle())
        { mLessThan = true; } else { mLessThan = false; }
      }
    void visitUnnamedLoc(UnnamedLoc& loc) { mLessThan = true; }
    void visitInvisibleLoc(InvisibleLoc& loc) { mLessThan = true; }
    void visitUnknownLoc(UnknownLoc& loc) { mLessThan = true; }
    void visitLocSubSet(LocSubSet& loc) { mLessThan = true; }
};


/*!
   Locations are ordered first between Location subclasses in the
   following order: NamedLoc < UnnamedLoc < InvisibleLoc <
   LocIdxSubSet < LocFieldSubSet < ... < LocSubSet < UnknownLoc.
   Then there is an ordering within each subclass.

   For NamedLoc the ordering is based on the SymHandle.

   Note: this could cause a problem if consider two NamedLoc's equiv
   if they must overlap.
*/
bool NamedLoc::operator<(Location& other)
{
    // apply the visitor to the other location and return result
    NamedLocLessThanVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);

    if (debug) { 
        std::cout << "NamedLoc::operator<, *this = ";
        this->dump(std::cout);
        std::cout << ", other = ";
        other.dump(std::cout);
        std::cout << ", *this < other ==> " << ltVisitor.mLessThan << std::endl;
    }
    return ltVisitor.mLessThan;
}  

class NamedLocEqualVisitor : public virtual LocationVisitor {
  private:
    NamedLoc& mThisLoc;
  public:
    bool mEqual;

    NamedLocEqualVisitor(NamedLoc& thisLoc) 
        : mThisLoc(thisLoc), mEqual(false) {}
    ~NamedLocEqualVisitor() {}
    void visitNamedLoc(NamedLoc& loc) 
      { if (mThisLoc.getSymHandle() == loc.getSymHandle()
            && loc.isLocal() == mThisLoc.isLocal() )
        { mEqual = true; } else { mEqual = false; }
      }
    void visitUnnamedLoc(UnnamedLoc& loc) { mEqual = false; }
    void visitInvisibleLoc(InvisibleLoc& loc) { mEqual = false; }
    void visitUnknownLoc(UnknownLoc& loc) { mEqual = false; }
    void visitLocSubSet(LocSubSet& loc) { mEqual = false; }
};

//! only semantically equivalent to another NamedLoc
bool NamedLoc::operator==(Location& other)
{
    // apply the visitor to the other location and return result
    NamedLocEqualVisitor eqVisitor(*this);
    other.acceptVisitor(eqVisitor);

    // FIXME: what about may and must overlap lists
    if (debug) { 
        std::cout << "NamedLoc::operator==, *this = ";
        this->dump(std::cout);
        std::cout << ", other = ";
        other.dump(std::cout);
        std::cout << ", *this == other ==> " << eqVisitor.mEqual << std::endl;
    }
 
    return eqVisitor.mEqual;
}   

bool NamedLoc::staticFullOverlap(SymHandle sym)
{
  //return std::find(mFullOverlap->begin(),mFullOverlap->end(),sym) 
  //       != mFullOverlap->end();
  //return std::find(mFullOverlap.begin(),mFullOverlap.end(),sym) 
  //       != mFullOverlap.end();
  return mFullOverlap.find(sym) != mFullOverlap.end();
}

bool NamedLoc::staticPartOverlap(SymHandle sym)
{
  return mPartOverlap.find(sym) != mPartOverlap.end();
}

class NamedLocMayOverlapVisitor : public virtual LocationVisitor {
  private:
    NamedLoc& mThisLoc;
  public:
    bool mMayOverlap;

    NamedLocMayOverlapVisitor(NamedLoc& thisLoc) 
        : mThisLoc(thisLoc), mMayOverlap(true) {}
    ~NamedLocMayOverlapVisitor() {}
    
    // we don't overlap other named locations if we have a different
    // symbol and have not been declared to statically overlap
    void visitNamedLoc(NamedLoc& otherLoc) 
      { if (mThisLoc.getSymHandle() != otherLoc.getSymHandle()
            && !mThisLoc.staticFullOverlap(otherLoc.getSymHandle())
            && !mThisLoc.staticPartOverlap(otherLoc.getSymHandle()) )
        { mMayOverlap = false; } else { mMayOverlap = true; }
      }

    void visitUnnamedLoc(UnnamedLoc& loc) { mMayOverlap = false; }

    // don't overlap with Invisibles because anything that does should
    // be mapped to same Invisible by alias analysis
    void visitInvisibleLoc(InvisibleLoc& loc) { mMayOverlap = false; }

    // all locs overlap with the UnknownLoc
    void visitUnknownLoc(UnknownLoc& loc) { mMayOverlap = true; }

    // if the other location is a subset and we are not then
    // make the other location do the work
    void visitLocSubSet(LocSubSet& loc) 
      { mMayOverlap = loc.mayOverlap(mThisLoc); }
};

bool NamedLoc::mayOverlap(Location& other)
{
    // apply the visitor to the other location and return result
    NamedLocMayOverlapVisitor mayOverlapVisitor(*this);
    other.acceptVisitor(mayOverlapVisitor);
    return mayOverlapVisitor.mMayOverlap;
}

class NamedLocMustOverlapVisitor : public virtual LocationVisitor {
  private:
    NamedLoc& mThisLoc;

  public:
    bool mMustOverlap;

    NamedLocMustOverlapVisitor(NamedLoc& thisLoc) 
        : mThisLoc(thisLoc), mMustOverlap(false) {}
    ~NamedLocMustOverlapVisitor() {}
    
    // we don't overlap other named locations if we have a different
    // symbol and have not been declared to statically overlap
    void visitNamedLoc(NamedLoc& loc) 
      { if (mThisLoc.getSymHandle() == loc.getSymHandle()
            || mThisLoc.staticFullOverlap(loc.getSymHandle()) )
        { mMustOverlap = true; } else { mMustOverlap = false; }
      }

    void visitUnnamedLoc(UnnamedLoc& loc) { mMustOverlap = false; }

    // don't overlap with Invisibles because anything that does should
    // be mapped to same Invisible by alias analysis
    void visitInvisibleLoc(InvisibleLoc& loc) { mMustOverlap = false; }

    // no locs must overlap with the UnknownLoc
    void visitUnknownLoc(UnknownLoc& loc) { mMustOverlap = false; }

    // if the other location is a subset and we are not then
    // make the other location do the work
    void visitLocSubSet(LocSubSet& loc) 
      { mMustOverlap = loc.mustOverlap(mThisLoc); }
};


bool NamedLoc::mustOverlap(Location& other)
{
    // apply the visitor to the other location and return result
    NamedLocMustOverlapVisitor mustOverlapVisitor(*this);
    other.acceptVisitor(mustOverlapVisitor);
    return mustOverlapVisitor.mMustOverlap;
}


//! conservatively answers this question, if we may overlap with
//! the other location but don't must overlap then we may or may
//! not be a subset so we just return false
//! FIXME: some analyses may want to use this conservatively the
//! other way, in other words, checking if it is not a subset?
bool NamedLoc::subSetOf(Location & other)
{
    return mustOverlap(other);
}

void NamedLoc::output(OA_ptr<IRHandlesIRInterface> pIR)
{
    sOB->objStart("NamedLoc");

    sOB->fieldStart("mSymHandle");
    sOB->outputIRHandle(mSymHandle,pIR);
    sOB->fieldEnd("mSymHandle");

    sOB->field("mLocal", bool2string(mLocal));
   
    std::set<SymHandle>::iterator listIter;
    sOB->fieldStart("mFullOverlap");
    for (listIter=mFullOverlap.begin(); listIter!=mFullOverlap.end(); listIter++)   {
      sOB->outputIRHandle(*listIter,pIR);
    }
    sOB->fieldEnd("mFullOverlap");

    sOB->fieldStart("mPartOverlap");
    for (listIter=mPartOverlap.begin(); listIter!=mPartOverlap.end(); listIter++)   {
      sOB->outputIRHandle(*listIter,pIR);
    }
    sOB->fieldEnd("mPartOverlap");

    sOB->objEnd("NamedLoc");
}

void NamedLoc::dump(std::ostream& os)
{
    os << "NamedLoc(this=" << this << ", mSymHandle.hval()="
       << mSymHandle.hval() << ", mLocal=" << mLocal << ")";
    os << std::endl;
}

// void NamedLoc::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
// {
//     // string for mSymHandle
//     os << "NamedLoc(this=" << this << ", mSymHandle.hval()=" 
//        << mSymHandle.hval() << ", mLocal=" << mLocal << "):";
//     os << pIR->toString(mSymHandle);
//     os << std::endl;
// }

void NamedLoc::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
  // string for mSymHandle
//   os << "(" << this << ", ";
  os << pIR->toString(mSymHandle);
}

// void NamedLoc::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
// {
//   // string for mSymHandle
//   os << "(" << this << ", " << mSymHandle.hval() << ", ";
//   os << pIR->toString(mSymHandle) << ")";
// }

std::string NamedLoc::toString(OA_ptr<IRHandlesIRInterface> pIR)
{
    std::ostringstream oss;
    // string for mSymHandle
    oss << pIR->toString(mSymHandle);
    return oss.str();
}


// std::string NamedLoc::toString(OA_ptr<IRHandlesIRInterface> pIR)
// {
//     std::ostringstream oss;
//     // string for mSymHandle
//     oss << "NamedLoc(" << mSymHandle.hval() << "):" 
//         << pIR->toString(mSymHandle);
//     return oss.str();
// }


//*****************************************************************
// UnnamedLoc methods
//*****************************************************************
UnnamedLoc::UnnamedLoc(UnnamedLoc &other) : mStmtHandle(other.mStmtHandle),
                                            mLocal(other.mLocal)
{
}

void UnnamedLoc::acceptVisitor(LocationVisitor& pVisitor)
{
    pVisitor.visitUnnamedLoc(*this);
}

OA_ptr<Location> UnnamedLoc::getBaseLoc() 
{ 
    OA_ptr<Location> retval; 
    retval = new UnnamedLoc(*this);
    return retval;
}

class UnnamedLocLessThanVisitor : public virtual LocationVisitor {
  private:
    UnnamedLoc& mThisLoc;

  public:
    bool mLessThan;

    UnnamedLocLessThanVisitor(UnnamedLoc& thisLoc) 
        : mThisLoc(thisLoc), mLessThan(false) {}
    ~UnnamedLocLessThanVisitor() {}
    void visitNamedLoc(NamedLoc& loc) { mLessThan = false; }
    void visitUnnamedLoc(UnnamedLoc& loc) 
      { if (mThisLoc.getStmtHandle() < loc.getStmtHandle()) 
        { mLessThan = true; } else { mLessThan = false; }
      }
    void visitInvisibleLoc(InvisibleLoc& loc) { mLessThan = true; }
    void visitUnknownLoc(UnknownLoc& loc) { mLessThan = true; }
    void visitLocSubSet(LocSubSet& loc) { mLessThan = true; }
};

/*!
   Locations are ordered first between Location subclasses in the
   following order: NamedLoc < UnnamedLoc < InvisibleLoc <
   LocIdxSubSet < LocFieldSubSet < ... < LocSubSet < UnknownLoc.
   Then there is an ordering within each subclass.

   For UnnamedLoc the ordering is based on the StmtHandle.

*/
bool UnnamedLoc::operator<(Location& other)
{
    // apply the visitor to the other location and return result
    UnnamedLocLessThanVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);
    return ltVisitor.mLessThan;
}  


class UnnamedLocEqualVisitor : public virtual LocationVisitor {
  private:
    UnnamedLoc& mThisLoc;

  public:
    bool mEqual;

    UnnamedLocEqualVisitor(UnnamedLoc& thisLoc) 
        : mThisLoc(thisLoc), mEqual(false) {}
    ~UnnamedLocEqualVisitor() {}
    void visitNamedLoc(NamedLoc& loc) { mEqual = false; }
    void visitUnnamedLoc(UnnamedLoc& loc) 
      { if (mThisLoc.getStmtHandle() == loc.getStmtHandle()
            && loc.isLocal() == mThisLoc.isLocal() ) 
        { mEqual = true; } else { mEqual = false; }
      }
    void visitInvisibleLoc(InvisibleLoc& loc) { mEqual = false; }
    void visitUnknownLoc(UnknownLoc& loc) { mEqual = false; }
    void visitLocSubSet(LocSubSet& loc) { mEqual = false; }
};

bool UnnamedLoc::operator==(Location& other)
{
    // apply the visitor to the other location and return result
    UnnamedLocEqualVisitor eqVisitor(*this);
    other.acceptVisitor(eqVisitor);
    return eqVisitor.mEqual;
}   

class UnnamedLocMayOverlapVisitor : public virtual LocationVisitor {
  private:
    UnnamedLoc& mThisLoc;

  public:
    bool mMayOverlap;

    UnnamedLocMayOverlapVisitor(UnnamedLoc& thisLoc) 
        : mThisLoc(thisLoc), mMayOverlap(true) {}
    ~UnnamedLocMayOverlapVisitor() {}
    
    // we don't overlap other named locations
    void visitNamedLoc(NamedLoc& loc) { mMayOverlap = false; }

    // we don't overlap other unnamed locations allocated at different stmts
    void visitUnnamedLoc(UnnamedLoc& loc)
      { if (mThisLoc.getStmtHandle() != loc.getStmtHandle())
        { mMayOverlap = false; } else { mMayOverlap = true; }
      }

    // don't overlap with Invisibles because anything that does should
    // be mapped to same Invisible by alias analysis
    void visitInvisibleLoc(InvisibleLoc& loc) { mMayOverlap = false; }

    // all locs overlap with the UnknownLoc
    void visitUnknownLoc(UnknownLoc& loc) { mMayOverlap = true; }

    // if the other location is a subset and we are not then
    // make the other location do the work
    void visitLocSubSet(LocSubSet& loc) 
      { mMayOverlap = loc.mayOverlap(mThisLoc); }

};

bool UnnamedLoc::mayOverlap(Location& other)
{
    // apply the visitor to the other location and return result
    UnnamedLocMayOverlapVisitor mayOverlapVisitor(*this);
    other.acceptVisitor(mayOverlapVisitor);
    return mayOverlapVisitor.mMayOverlap;
}

bool UnnamedLoc::mustOverlap(Location& other)
{
    return false;
}

//! conservatively answers this question, if we may overlap with
//! the other location but don't must overlap then we may or may
//! not be a subset so we just return false
bool UnnamedLoc::subSetOf(Location & other)
{
    return false;
}

void UnnamedLoc::output(OA_ptr<IRHandlesIRInterface> pIR)
{
    sOB->objStart("UnnamedLoc");

    sOB->fieldStart("mStmtHandle");
    sOB->outputIRHandle(mStmtHandle,pIR);
    sOB->fieldEnd("mStmtHandle");

    sOB->field("mLocal", bool2string(mLocal));
   
    sOB->objEnd("UnnamedLoc");
}


void UnnamedLoc::dump(std::ostream& os)
{
    os << "UnnamedLoc(this=" << this << ", mStmtHandle.hval()="
       << mStmtHandle.hval() << ")";
    os << std::endl;
}

void UnnamedLoc::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
    os << "UnnamedLoc(this=" << this << ", mStmtHandle.hval()="
       << mStmtHandle.hval() << "): "
       << pIR->toString(mStmtHandle);
    os << std::endl;
}

std::string UnnamedLoc::toString(OA_ptr<IRHandlesIRInterface> pIR)
{
    std::ostringstream oss;
    oss << "UnnamedLoc(" << mStmtHandle << "):" << pIR->toString(mStmtHandle);
    return oss.str();
}


//*****************************************************************
// InvisibleLoc methods
//*****************************************************************
InvisibleLoc::InvisibleLoc(InvisibleLoc &other)
{
    mMRE = other.mMRE;
}

void InvisibleLoc::acceptVisitor(LocationVisitor& pVisitor)
{
    pVisitor.visitInvisibleLoc(*this);
}

/*!
 Implementation note: don't know whether base symbol of MemRefExpr is
 local or not so indicating it is not local (IOW visible in other procedures
 to be conservative.
 */
OA_ptr<Location> InvisibleLoc::getBaseLoc() 
{ 
    OA_ptr<Location> retval; 
    if (mMRE->isaNamed()) {
        OA_ptr<MemRefExpr> m = mMRE; // Stupid Sun CC 5.4
	OA_ptr<NamedRef> namedRef = m.convert<NamedRef>();
        retval = new NamedLoc(namedRef->getSymHandle(),false);
    } else if (mMRE->isaRefOp()) {
        OA_ptr<MemRefExpr> m = mMRE; // Stupid Sun CC 5.4
        OA_ptr<RefOp> refOp = m.convert<RefOp>();
        retval = new NamedLoc(refOp->getBaseSym(),false);
    } else {
        assert(0);  // shouldn't have an invisible loc without a named base loc
    }
    return retval;
}


class InvisibleLocLessThanVisitor : public virtual LocationVisitor {
  private:
    InvisibleLoc& mThisLoc;
  public:
    bool mLessThan;

    InvisibleLocLessThanVisitor(InvisibleLoc& thisLoc) 
        : mThisLoc(thisLoc), mLessThan(false) {}
    ~InvisibleLocLessThanVisitor() {}
    void visitNamedLoc(NamedLoc& loc) { mLessThan = false; }
    void visitUnnamedLoc(UnnamedLoc& loc) { mLessThan = false; }
    void visitInvisibleLoc(InvisibleLoc& loc) 
      { if (mThisLoc.getMemRefExpr() < loc.getMemRefExpr() )
        { mLessThan = true; } else { mLessThan = false; }
      }
    void visitUnknownLoc(UnknownLoc& loc) { mLessThan = true; }
    void visitLocSubSet(LocSubSet& loc) { mLessThan = true; }
};

/*!
   Locations are ordered first between Location subclasses in the
   following order: NamedLoc < UnnamedLoc < InvisibleLoc <
   LocIdxSubSet < LocFieldSubSet < ... < LocSubSet < UnknownLoc.
   Then there is an ordering within each subclass.

   For InvisibleLoc the ordering is based on the MRE.

*/
bool InvisibleLoc::operator<(Location& other)
{
    // apply the visitor to the other location and return result
    InvisibleLocLessThanVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);
    return ltVisitor.mLessThan;
}


class InvisibleLocEqualVisitor : public virtual LocationVisitor {
  private:
    InvisibleLoc& mThisLoc;
  public:
    bool mEqual;

    InvisibleLocEqualVisitor(InvisibleLoc& thisLoc) 
        : mThisLoc(thisLoc), mEqual(false) {}
    ~InvisibleLocEqualVisitor() {}
    void visitNamedLoc(NamedLoc& loc) { mEqual = false; }
    void visitUnnamedLoc(UnnamedLoc& loc) { mEqual = false; } 
    void visitInvisibleLoc(InvisibleLoc& loc) 
      { if ( mThisLoc.getMemRefExpr() == loc.getMemRefExpr() ) 
        { mEqual = true; } else { mEqual = false; }
      }
    void visitUnknownLoc(UnknownLoc& loc) { mEqual = false; }
    void visitLocSubSet(LocSubSet& loc) { mEqual = false; }
};
 
bool InvisibleLoc::operator==(Location& other)
{
    // apply the visitor to the other location and return result
    InvisibleLocEqualVisitor eqVisitor(*this);
    other.acceptVisitor(eqVisitor);
    return eqVisitor.mEqual;
}   

class InvisibleLocMayOverlapVisitor : public virtual LocationVisitor {
  private:
    InvisibleLoc& mThisLoc;
  public:
    bool mMayOverlap;

    InvisibleLocMayOverlapVisitor(InvisibleLoc& thisLoc) 
        : mThisLoc(thisLoc), mMayOverlap(true) {}
    ~InvisibleLocMayOverlapVisitor() {}
    
    // we don't overlap named locations because if we did then those
    // named memory references should have been mapped to same invisible
    // instead of being mapped to a named location
    void visitNamedLoc(NamedLoc& loc) { mMayOverlap = false; }

    // we don't overlap other unnamed locations for a similar reason
    void visitUnnamedLoc(UnnamedLoc& loc) { mMayOverlap = false; }

    // only overlap with other Invisibles that have an equivalent MemRefExpr
    void visitInvisibleLoc(InvisibleLoc& loc) 
      { if (mThisLoc.getMemRefExpr() == loc.getMemRefExpr())
        { mMayOverlap = true; } else { mMayOverlap = false; }
      }

    // all locs overlap with the UnknownLoc
    void visitUnknownLoc(UnknownLoc& loc) { mMayOverlap = true; }

    // if the other location is a subset and we are not then
    // make the other location do the work
    void visitLocSubSet(LocSubSet& loc) 
      { mMayOverlap = loc.mayOverlap(mThisLoc); }
};

bool InvisibleLoc::mayOverlap(Location& other)
{
    // apply the visitor to the other location and return result
    InvisibleLocMayOverlapVisitor mayOverlapVisitor(*this);
    other.acceptVisitor(mayOverlapVisitor);
    return mayOverlapVisitor.mMayOverlap;
}

class InvisibleLocMustOverlapVisitor : public virtual LocationVisitor {
  private:
    InvisibleLoc& mThisLoc;
  public:
    bool mMustOverlap;

    InvisibleLocMustOverlapVisitor(InvisibleLoc& thisLoc) 
        : mThisLoc(thisLoc), mMustOverlap(false) {}
    ~InvisibleLocMustOverlapVisitor() {}
    
    // we don't overlap other named locations 
    void visitNamedLoc(NamedLoc& loc) { mMustOverlap = false; }

    void visitUnnamedLoc(UnnamedLoc& loc) { mMustOverlap = false; }

    // if the memory reference expression is equivalent
    // then we must overlap with another invisible location
    void visitInvisibleLoc(InvisibleLoc& loc) 
      { if ( mThisLoc.getMemRefExpr() == loc.getMemRefExpr() ) 
        { mMustOverlap = true; } else { mMustOverlap = false; }
      }

    // no locs must overlap with the UnknownLoc
    void visitUnknownLoc(UnknownLoc& loc) { mMustOverlap = false; }

    // if the other location is a subset and we are not then
    // make the other location do the work
    void visitLocSubSet(LocSubSet& loc) 
      { mMustOverlap = loc.mustOverlap(mThisLoc); }
};

bool InvisibleLoc::mustOverlap(Location& other)
{
    // apply the visitor to the other location and return result
    InvisibleLocMustOverlapVisitor mustOverlapVisitor(*this);
    other.acceptVisitor(mustOverlapVisitor);
    return mustOverlapVisitor.mMustOverlap;
}

//! conservatively answers this question, if we may overlap with
//! the other location but don't must overlap then we may or may
//! not be a subset so we just return false
//! FIXME
bool InvisibleLoc::subSetOf(Location & other)
{
    return mustOverlap(other);
}

void InvisibleLoc::output(OA_ptr<IRHandlesIRInterface> pIR)
{
    sOB->objStart("InvisibleLoc");

    sOB->fieldStart("mMRE");
    mMRE->output(pIR);
    sOB->fieldEnd("mMRE");

    sOB->objEnd("InvisibleLoc");
}

void InvisibleLoc::dump(std::ostream& os)
{
    // string for mSymHandle
    os << "InvisibleLoc(this=" << this << ", mMRE=";
    mMRE->dump(os);
    os << " )" << std::endl;
}


void InvisibleLoc::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
    // string for mSymHandle
    os << "InvisibleLoc(this=" << this << ", mMRE=";
    mMRE->dump(os,pIR);
    os << " )" << std::endl;
}

std::string InvisibleLoc::toString(OA_ptr<IRHandlesIRInterface> pIR)
{
    std::ostringstream oss;
    // string for mSymHandle
    oss << "InvisibleLoc: ";
    mMRE->dump(oss,pIR);
    return oss.str();
}

//*****************************************************************
// UnknownLoc methods
//*****************************************************************
void UnknownLoc::acceptVisitor(LocationVisitor& pVisitor)
{
    pVisitor.visitUnknownLoc(*this);
}

OA_ptr<Location> UnknownLoc::getBaseLoc() 
{ 
    OA_ptr<Location> retval; 
    retval = new UnknownLoc();
    return retval;
}


/*!
   Locations are ordered first between Location subclasses in the
   following order: NamedLoc < UnnamedLoc < InvisibleLoc <
   LocIdxSubSet < LocFieldSubSet < ... < LocSubSet < UnknownLoc.
   Then there is an ordering within each subclass.

   The UnknownLoc is greater than everyone else.

*/
bool UnknownLoc::operator<(Location& other)
{
    return false;
}  


bool UnknownLoc::operator==(Location& other) 
{
    if (other.isaUnknown()) {
        return true;
    } else {
        return false;
    }
}

void UnknownLoc::output(OA_ptr<IRHandlesIRInterface> pIR)
{
    sOB->objStart("UnknownLoc");
    sOB->objEnd("UnknownLoc");
}


void UnknownLoc::dump(std::ostream& os)
{
    os << "UnknownLoc" << std::endl;
}


void UnknownLoc::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
    dump(os);
}

std::string UnknownLoc::toString(OA_ptr<IRHandlesIRInterface> pIR)
{
  std::ostringstream oss;
  oss << "UnknownLoc";
  return oss.str();
}

//*****************************************************************
// LocSubSet methods
//*****************************************************************

LocSubSet::LocSubSet(OA_ptr<Location> baseLoc) : mFull(false) 
{ 
    // will compose this loc sub set with baseLoc and set mLoc
    // accordingly
    composeWithBaseLoc(baseLoc);
}

LocSubSet::LocSubSet(OA_ptr<Location> baseLoc, bool full) : mFull(full) 
{ 
    // will compose this loc sub set with baseLoc and set mLoc
    // accordingly
    composeWithBaseLoc(baseLoc);
}

/*!
    If the given location is a subset then we will be subset of 
    the baseLoc's base location.  This method also has logic
    for determining what our accuracy should be based on our initially
    specified accuracy and the accuracy of baseLoc
 */
void LocSubSet::composeWithBaseLoc(OA_ptr<Location> baseLoc)
{    
    if (baseLoc->isaSubSet()) {
        mFull = mFull && baseLoc->hasFullAccuracy();
        mLoc = baseLoc->getBaseLoc();

    } else {
        mLoc = baseLoc;
    }
}

void LocSubSet::acceptVisitor(LocationVisitor& pVisitor)
{
    pVisitor.visitLocSubSet(*this);
}

void LocSubSet::output(OA_ptr<IRHandlesIRInterface> pIR)
{
    sOB->fieldStart("mLoc");
    mLoc->output(pIR);
    sOB->fieldEnd("mLoc");
}

class LocSubSetLessThanVisitor : public virtual LocationVisitor {
  private:
    LocSubSet& mThisLoc;
  public:
    bool mLessThan;

    LocSubSetLessThanVisitor(LocSubSet& thisLoc) 
        : mThisLoc(thisLoc), mLessThan(false) {}
    ~LocSubSetLessThanVisitor() {}
    void visitNamedLoc(NamedLoc& loc) { mLessThan = false; }
    void visitUnnamedLoc(UnnamedLoc& loc) { mLessThan = false; }
    void visitInvisibleLoc(InvisibleLoc& loc) { mLessThan = false; }
    void visitUnknownLoc(UnknownLoc& loc) { mLessThan = true; }
    void visitLocSubSet(LocSubSet& loc) 
      { // if passed in loc is really a subclass of LocSubSet that
        // we don't handle in this visitor directly then it will
        // be less than us and so mLessThan is false
        if (loc.isSubClassOfLocSubSet()) { mLessThan = false; }
        else if (mThisLoc.getLoc() < loc.getLoc())  {
          mLessThan = true;
        } else if (mThisLoc.getLoc() == loc.getLoc() &&
                   mThisLoc.isFull() && !loc.isFull()) {
          mLessThan = true;
        } else { mLessThan = false; }
    }
};

/*!
   Locations are ordered first between Location subclasses in the
   following order: NamedLoc < UnnamedLoc < InvisibleLoc <
   LocIdxSubSet < LocFieldSubSet < ... < LocSubSet < UnknownLoc.
   Then there is an ordering within each subclass.

   When comparing between direct instances of LocSubSet's will first 
   compare base locations and will then let FULL < PARTIAL.

*/
bool LocSubSet::operator<(Location& other)
{
    // apply the visitor to the other location and return result
    LocSubSetLessThanVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);
    return ltVisitor.mLessThan;
}          

class LocSubSetEqualVisitor : public virtual LocationVisitor {
  private:
    LocSubSet& mThisLoc;
  public:
    bool mEqual;

    LocSubSetEqualVisitor(LocSubSet& thisLoc) 
        : mThisLoc(thisLoc), mEqual(false) {}
    ~LocSubSetEqualVisitor() {}
    void visitNamedLoc(NamedLoc& loc) { mEqual = false; }
    void visitUnnamedLoc(UnnamedLoc& loc) { mEqual = false; } 
    void visitInvisibleLoc(InvisibleLoc& loc) { mEqual = false; } 
    void visitUnknownLoc(UnknownLoc& loc) { mEqual = false; }
    void visitLocSubSet(LocSubSet& loc) 
      // must also be a direct instance of LocSubSet, same loc, and 
      // same isFull
      { if (! loc.isSubClassOfLocSubSet() && 
              mThisLoc.getLoc() == loc.getLoc() &&
              mThisLoc.isFull() == loc.isFull() )  
        { mEqual = true; } else { mEqual = false; }
      }
};

bool LocSubSet::operator==(Location& other)
{
    // apply the visitor to the other location and return result
    LocSubSetEqualVisitor eqVisitor(*this);
    other.acceptVisitor(eqVisitor);
    return eqVisitor.mEqual;
}  

bool LocSubSet::mayOverlap(Location& other)
{
    // may overlap if underlying locations overlap
    assert(!getLoc().ptrEqual(NULL));
    return getLoc()->mayOverlap(other);
}

class LocSubSetMustOverlapVisitor : public virtual LocationVisitor {
  private:
    LocSubSet& mThisLoc;
  public:
    bool mMustOverlap;

    LocSubSetMustOverlapVisitor(LocSubSet& thisLoc) 
        : mThisLoc(thisLoc), mMustOverlap(false) {}
    ~LocSubSetMustOverlapVisitor() {}

    // helper method that determines if mThisLoc is a full
    // subset and if the underlying location for it must
    // overlap with the given location
    void fullAndLocMustOverlap(Location &loc)
      { if ( loc.mustOverlap(*(mThisLoc.getLoc()))
             && mThisLoc.isFull() )
        { mMustOverlap = true; } else { mMustOverlap = false; }
      }
    
    // we overlap with named locations if we are a full subset
    // of that NamedLoc
    void visitNamedLoc(NamedLoc& loc) { fullAndLocMustOverlap(loc); }

    // we overlap with unnamed locations if we are a full subset
    // of that NamedLoc
    void visitUnnamedLoc(UnnamedLoc& loc) { fullAndLocMustOverlap(loc); }

    // we overlap with invisible locations if we are a full subset
    // of that NamedLoc
    void visitInvisibleLoc(InvisibleLoc& loc) { fullAndLocMustOverlap(loc);}
    // no locs must overlap with the UnknownLoc
    void visitUnknownLoc(UnknownLoc& loc) { mMustOverlap = false; }

    // if the other location is a subset then we must
    // overlap with them if we are a full subset
    // and they must overlap with our underlying location
    void visitLocSubSet(LocSubSet& loc) { fullAndLocMustOverlap(loc); }

};

bool LocSubSet::mustOverlap(Location& other)
{
    // apply the visitor to the other location and return result
    LocSubSetMustOverlapVisitor mustOverlapVisitor(*this);
    other.acceptVisitor(mustOverlapVisitor);
    return mustOverlapVisitor.mMustOverlap;
}

// FIXME
bool LocSubSet::subSetOf(Location & other)
{
    // if our location must overlap the other location then
    // we are a subset of the other location
    assert(!getLoc().ptrEqual(NULL));
    if (getLoc()->mustOverlap(other)) {
        return true;
    } else {
        return false;
    }
}


void LocSubSet::dump(std::ostream& os)
{
    os << "LocSubSet(this=" << this << " ,mLoc=";
    getLoc()->dump(os);
    os << "\tisFull=" << isFull() << " )"; 
    os << std::endl;
}

void LocSubSet::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
//     os << "LocSubSet(" << this << ", ";
//     os << this << "^";
    getLoc()->dump(os,pIR);
//     os << ")"; 
}

// void LocSubSet::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
// {
//     os << "LocSubSet(this=" << this << " ,mLoc=";
//     getLoc()->dump(os,pIR);
//     os << "\tisFull=" << isFull() << " )"; 
//     os << std::endl;
// }

std::string LocSubSet::toString(OA_ptr<IRHandlesIRInterface> pIR)
{
  std::ostringstream oss;
    assert(!getLoc().ptrEqual(NULL));
    oss << getLoc()->toString(pIR);
    return oss.str();
}


// std::string LocSubSet::toString(OA_ptr<IRHandlesIRInterface> pIR)
// {
//   std::ostringstream oss;
//     oss << "LocSubSet( mLoc = ";
//     assert(!getLoc().ptrEqual(NULL));
//     oss << getLoc()->toString(pIR);
//     oss << "\tisFull = " << isFull() << " )"; 
//     return oss.str();
// }


//*****************************************************************
// LocIdxSubSet methods
//*****************************************************************

void LocIdxSubSet::acceptVisitor(LocationVisitor& pVisitor)
{
    pVisitor.visitLocIdxSubSet(*this);
}


class LocIdxSubSetLessThanVisitor : public virtual LocationVisitor {
  private:
    LocIdxSubSet& mThisLoc;
  public:
    bool mLessThan;

    LocIdxSubSetLessThanVisitor(LocIdxSubSet& thisLoc) 
        : mThisLoc(thisLoc), mLessThan(false) {}
    ~LocIdxSubSetLessThanVisitor() {}
    void visitNamedLoc(NamedLoc& loc) { mLessThan = false; }
    void visitUnnamedLoc(UnnamedLoc& loc) { mLessThan = false; }
    void visitInvisibleLoc(InvisibleLoc& loc) { mLessThan = false; }
    void visitUnknownLoc(UnknownLoc& loc) { mLessThan = true; }
    void visitLocSubSet(LocSubSet& loc) { mLessThan = true; }
    void visitLocIdxSubSet(LocIdxSubSet& loc)
    { if (mThisLoc.getLoc() < loc.getLoc()
          || ( (mThisLoc.getLoc() == loc.getLoc()) &&
               (mThisLoc.getIdx() < loc.getIdx()) ) ) 
      { mLessThan = true; } else { mLessThan = false; }
    }
};

/*!
   Locations are ordered first between Location subclasses in the
   following order: NamedLoc < UnnamedLoc < InvisibleLoc <
   LocIdxSubSet < LocFieldSubSet < ... < LocSubSet < UnknownLoc.
   Then there is an ordering within each subclass.

   When comparing between LocIdxSubSet's will first compare base
   locations and will then will compare indices.

*/
bool LocIdxSubSet::operator<(Location& other)
{
    // apply the visitor to the other location and return result
    LocIdxSubSetLessThanVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);

    return ltVisitor.mLessThan;
}   


class LocIdxSubSetEqualVisitor : public virtual LocationVisitor {
  private:
    LocIdxSubSet& mThisLoc;
  public:
    bool mEqual;

    LocIdxSubSetEqualVisitor(LocIdxSubSet& thisLoc) 
        : mThisLoc(thisLoc), mEqual(false) {}
    ~LocIdxSubSetEqualVisitor() {}
    void visitNamedLoc(NamedLoc& loc) { mEqual = false; }
    void visitUnnamedLoc(UnnamedLoc& loc) { mEqual = false; } 
    void visitInvisibleLoc(InvisibleLoc& loc) { mEqual = false; } 
    void visitUnknownLoc(UnknownLoc& loc) { mEqual = false; }
    void visitLocSubSet(LocSubSet& loc) { mEqual = false; }
    void visitLocIdxSubSet(LocIdxSubSet& loc) 
      // must also be a direct instance of LocIdxSubSet, 
      // same underlying loc, and same index
      { if ( mThisLoc.getLoc() == loc.getLoc() &&
             mThisLoc.getIdx() == loc.getIdx() )  
        { mEqual = true; } else { mEqual = false; }
      }
};

bool LocIdxSubSet::operator==(Location& other)
{
    // apply the visitor to the other location and return result
    LocIdxSubSetEqualVisitor eqVisitor(*this);
    other.acceptVisitor(eqVisitor);
    return eqVisitor.mEqual;
}

bool LocIdxSubSet::mayOverlap(Location& other)
{
    // may overlap if underlying locations overlap
    // don't know how the overlap occurs so can't assume
    // indices would have to line up
    assert(!getLoc().ptrEqual(NULL));
    return getLoc()->mayOverlap(other);
}


class LocIdxSubSetMustOverlapVisitor : public virtual LocationVisitor {
  private:
    LocIdxSubSet& mThisLoc;
  public:
    bool mMustOverlap;

    LocIdxSubSetMustOverlapVisitor(LocIdxSubSet& thisLoc) 
        : mThisLoc(thisLoc), mMustOverlap(false) {}
    ~LocIdxSubSetMustOverlapVisitor() {}

    void visitNamedLoc(NamedLoc& loc) { mMustOverlap = false; }
    void visitUnnamedLoc(UnnamedLoc& loc) { mMustOverlap = false; } 
    void visitInvisibleLoc(InvisibleLoc& loc) { mMustOverlap = false; } 
    void visitUnknownLoc(UnknownLoc& loc) { mMustOverlap = false; }
    void visitLocSubSet(LocSubSet& loc) { mMustOverlap = false; }
    void visitLocIdxSubSet(LocIdxSubSet& loc) 
      // if other is a LocIdxSubSet then the underlying
      // locations must overlap and they must have same index
      { if ( loc.getLoc()->mustOverlap(*(mThisLoc.getLoc())) 
             && mThisLoc.getIdx() == loc.getIdx() )  
        { mMustOverlap = true; } else { mMustOverlap = false; }
      }
};

bool LocIdxSubSet::mustOverlap(Location& other)
{
    // apply the visitor to the other location and return result
    LocIdxSubSetMustOverlapVisitor mustOverlapVisitor(*this);
    other.acceptVisitor(mustOverlapVisitor);
    return mustOverlapVisitor.mMustOverlap;
}   

// FIXME
bool LocIdxSubSet::subSetOf(Location & other)
{
    // if our location must overlap the other location then
    // we are a subset of the other location
    if (getLoc()->mustOverlap(other)) {
        return true;
    } else {
        return false;
    }
}

void LocIdxSubSet::output(OA_ptr<IRHandlesIRInterface> pIR)
{
    sOB->objStart("LocIdxSubSet");

    LocSubSet::output(pIR);
    sOB->field("mIdx",int2string(mIdx));

    sOB->objEnd("LocIdxSubSet");
}


void LocIdxSubSet::dump(std::ostream& os)
{
    os << "LocIdxSubSet(this=" << this << ", mLoc=";
    getLoc()->dump(os);
    os << "\tmIdx=" << getIdx() << " )"; 
    os << std::endl;
}

void LocIdxSubSet::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
    os << "LocIdxSubSet(this=" << this << ", mLoc=";
    getLoc()->dump(os,pIR);
    os << "\tmIdx=" << getIdx() << " )"; 
    os << std::endl;
}

std::string LocIdxSubSet::toString(OA_ptr<IRHandlesIRInterface> pIR)
{
  std::ostringstream oss;
    oss << "LocIdxSubSet( mLoc = ";
    assert(!getLoc().ptrEqual(NULL));
    oss << getLoc()->toString(pIR);
    oss << "\tmIdx = " << getIdx() << " )"; 
    return oss.str();
}




} // end of OA namespace

