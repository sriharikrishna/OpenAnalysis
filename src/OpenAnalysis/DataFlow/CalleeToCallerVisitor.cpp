/*! \file
  
  \brief Implementation of CalleeToCallerVisitor class.
  
  \authors Michelle Strout
  \version $Id: CalleeToCallerVisitor.cpp,v 1.2 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "CalleeToCallerVisitor.hpp"

namespace OA {
  namespace DataFlow {

#if defined(DEBUG_ALL) || defined(DEBUG_CalleeToCallerVisitor)
static bool debug = true;
#else
static bool debug = false;
#endif


OA_ptr<LocIterator> CalleeToCallerVisitor::getCallerLocIterator()
{
    OA_ptr<LocSetIterator> retval;
    retval = new LocSetIterator(mLocSet);
    return retval;
}
        
CalleeToCallerVisitor::CalleeToCallerVisitor(
        ExprHandle call, ProcHandle caller,
        OA_ptr<Alias::InterAliasInterface> interAlias,
        OA_ptr<ParamBindings> paramBind)
    : mCall(call), mCaller(caller), 
      mInterAlias(interAlias), mParamBind(paramBind)
{
    mCallerAlias = mInterAlias->getAliasResults(caller);
}

void CalleeToCallerVisitor::visitNamedLoc(NamedLoc& loc) 
{
    // should start newly empty each time
    mLocSet = new LocSet;

    // if the location is not local then just return it
    if (loc.isLocal()==false) {
        OA_ptr<Location> locCopy; locCopy = new NamedLoc(loc);
        mLocSet->insert(locCopy);
        if (debug) {
            std::cout << "CalleeToCallerVisitor: loc is not local, loc = ";
            loc.dump(std::cout);
            std::cout << std::endl;
        }
    
    } else {
        // assume it is a parameter and try to get corresponding memref
        SymHandle formal = loc.getSymHandle();
        if (debug) {
            std::cout << "CalleeToCallerVisitor: formal = " << formal.hval();
            std::cout << std::endl;
        }
    
        // get the memory reference in the caller
        MemRefHandle memref = mParamBind->getCallerMemRef(mCall,formal);
    
        // only do the conversion if actual parameter is a memory reference
        if (memref!=MemRefHandle(0)) {
            // get all the may locs for the mem ref in caller
            // and insert them into the caller locs for this reference
            OA_ptr<LocIterator> mayLocIter = mCallerAlias->getMayLocs(memref);
            for ( ; mayLocIter->isValid(); (*mayLocIter)++ ) {
                if (debug) {
                    std::cout << "\tmemref = " << memref.hval()
                            << " has mayLoc ";
                    mayLocIter->current()->dump(std::cout);
                    std::cout << std::endl;
                }
                mLocSet->insert(mayLocIter->current());
            }
        }
    }
}

void CalleeToCallerVisitor::visitUnnamedLoc(UnnamedLoc& loc)
{
    mLocSet = new LocSet;
    OA_ptr<Location> locptr;
    locptr = new UnnamedLoc(loc);
    mLocSet->insert(locptr);
}

void CalleeToCallerVisitor::visitInvisibleLoc(InvisibleLoc& loc)
{
    assert(0);  // not handling yet
}

void CalleeToCallerVisitor::visitUnknownLoc(UnknownLoc& loc)
{
    mLocSet = new LocSet;
    OA_ptr<Location> locptr;
    locptr = new UnknownLoc();
    mLocSet->insert(locptr);
}

void CalleeToCallerVisitor::visitLocSubSet(LocSubSet& loc)
{
    // not handling any of the more precise LocSubSet classes
    // just going to make a generic LocSubSet to wrap around
    // all the NamedLoc's the underlying location maps to
    OA_ptr<Location> baseLoc = loc.getLoc();
    if (debug) {
        std::cout << "CalleeToCallerVisitor::visitLocSubSet" << std::endl;
        std::cout << "\tbaseLoc = ";
        baseLoc->dump(std::cout);
    }
    baseLoc->acceptVisitor(*this);
    OA_ptr<LocSet> temp = mLocSet;
    mLocSet = new LocSet;
    OA_ptr<LocIterator> locIter; locIter = new LocSetIterator(temp);
    for ( ; locIter->isValid(); (*locIter)++ ) {
        OA_ptr<Location> mayLoc = locIter->current();
        if (debug) {
            std::cout << "\t\tmayLoc = ";
            mayLoc->dump(std::cout);
        }
        
        // the constructor for LocSubSet handles various mayLoc
        // subclasses
        OA_ptr<Location> subLoc;
        subLoc = new LocSubSet(mayLoc);
        mLocSet->insert(subLoc);
    }

}


  } // end of DataFlow namespace
} // end of OA namespace

