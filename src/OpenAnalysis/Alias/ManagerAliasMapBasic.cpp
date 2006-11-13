/*! \file
  
  \brief The AnnotationManager that generates AliasMaps by giving
         each local variable its own number and virtual location and 
         mapping everyone else to unknown.  

  \authors Michelle Strout
  \version $Id: ManagerAliasMapBasic.cpp,v 1.28 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerAliasMapBasic.hpp"


namespace OA {
  namespace Alias {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerAliasMapBasic)
static bool debug = true;
#else
static bool debug = false;
#endif

/*!
   Visitor over memory reference expressions that creates an
   appropriate Location data structure for any MemRefExpr.
   
   Conservatively handles addressOf (only way UnnamedRefs happen) and 
   derefs.  Therefore resulting mLoc will be UnknownLoc for those.
 */
class CreateLocationVisitor : public virtual MemRefExprVisitor {
  public:
    OA_ptr<Location> mLoc;
    CreateLocationVisitor(OA_ptr<AliasIRInterface> ir,
                          ProcHandle proc) : mIR(ir),mProc(proc) {}
    ~CreateLocationVisitor() {}
    void visitNamedRef(NamedRef& ref) 
      {
          mLoc = mIR->getLocation(mProc,ref.getSymHandle());

          // if the memory reference doesn't have full accuracy then
          // create a partial subset of the base memory location
          if (!ref.hasFullAccuracy()) {
              mLoc = new LocSubSet(mLoc,false);
          }

          // if this is an addressOf just switch it to UnknownLoc
          if (ref.hasAddressTaken()) {
              mLoc = new UnknownLoc();
          }
      }
    void visitUnnamedRef(UnnamedRef& ref) { mLoc = new UnknownLoc; }
    void visitUnknownRef(UnknownRef& ref) { mLoc = new UnknownLoc; }
    void visitDeref(Deref& ref) { mLoc = new UnknownLoc; }
    // default handling of more specific SubSet specificiations
    void visitSubSetRef(SubSetRef& ref) 
      {
          // will set mLoc to our base location
          ref.getMemRefExpr()->acceptVisitor(*this);
          if (mLoc->isaNamed()) {
            mLoc = new LocSubSet(mLoc,false);
          }
          // if this is an addressOf just switch it to UnknownLoc
          if (ref.hasAddressTaken()) {
              mLoc = new UnknownLoc();
          }
      }

  private:
    OA_ptr<AliasIRInterface> mIR;
    ProcHandle mProc;

};


/*! do a trivial map of mre to loc if possible (no addressTaken
   and no dereferencing), otherwise return NULL

NOTE: for now this method is duplicated in EquivSets.cpp, if make any changes
here to handle array indices, fields, etc. should reflect those changes
in EquivSets.cpp.
FIXME: refactoring needs to happen between AliasMap and EquivSets because
there is so much shared code.  The trivial mapping logic might be relevant
to all Alias::Interface instantiations.
*/
/*
OA_ptr<Location> 
ManagerAliasMapBasic::trivialMREToLoc(OA_ptr<MemRefExpr> mre)
{
    OA_ptr<Location> retLoc; retLoc = NULL;

    if (!mre->isaRefOp() && mre->isaNamed() && !mre->hasAddressTaken()) {
      OA_ptr<NamedRef> namedMRE 
          = mre.convert<NamedRef>();

      // if have a reference parameter then need to 
      // make an invisible location for this symbol
// FIXME: this really makes things overly conservative since isRefParam
// returns true most of the time
//      if (mIR->isRefParam(namedMRE->getSymHandle())) {
//        retLoc = new InvisibleLoc(mre);
//
//      } else {
        // get location associated with base symbol
        OA_ptr<Location> baseloc;
        baseloc = mIR->getLocation(mProc,namedMRE->getSymHandle());

        // if the memory reference doesn't have full accuracy then
        // create a partial subset of the base memory location
       if (!mre->hasFullAccuracy()) {
            retLoc = new LocSubSet(baseloc,false);
        } else {
            retLoc = baseloc;
        }
//      }

    } 

    return retLoc;
}
*/
/*!
*/
OA_ptr<Alias::AliasMap> ManagerAliasMapBasic::performAnalysis(ProcHandle proc,
        OA_ptr<AliasMap> seedAliasMap) 
{
  OA_ptr<AliasMap> retAliasMap; retAliasMap = seedAliasMap;

  mProc = proc;

  // semantics of the AliasMap are that the zeroth set specifies that
  // no alias info is known about a memory reference, it could reference
  // any location
  unsigned int unknownSetId = 0;

  // for each statement stmt
  OA_ptr<OA::IRStmtIterator> sIt = mIR->getStmtIterator(proc);
  for ( ; sIt->isValid(); (*sIt)++) {
    OA::StmtHandle stmt = sIt->current();
        
    if (debug) {
        std::cout << "\tstmt = ";
        mIR->dump(stmt,std::cout);
    }

    //   for each memory reference in the stmt
    OA_ptr<MemRefHandleIterator> mrIterPtr = mIR->getAllMemRefs(stmt);
    for (; mrIterPtr->isValid(); (*mrIterPtr)++ )
    {
      MemRefHandle memref = mrIterPtr->current();
      if (debug) {
        std::cout << "\tmemref = ";
        mIR->dump(memref,std::cout);
      }

      // get the memory reference expressions for this handle
      OA_ptr<MemRefExprIterator> mreIterPtr 
          = mIR->getMemRefExprIterator(memref);
      
      // for each mem-ref-expr associated with this memref
      for (; mreIterPtr->isValid(); (*mreIterPtr)++) {
        OA_ptr<OA::MemRefExpr> mre = mreIterPtr->current();
        if (debug) {
            std::cout << "\tmre = ";
            mre->dump(std::cout,mIR);
        }
        
        // see if we have not already mapped an equivalent memrefexpr
        int setId = retAliasMap->getMapSetId(mre);
        if (debug) {
            std::cout << "\tsetId = " << setId << std::endl;
        }

        // need to determine what locations mre maps to 
        if (setId == AliasMap::SET_ID_NONE ) {
            
            // default set id
            setId = unknownSetId;

            // use visitor to get Location this MRE maps to
            CreateLocationVisitor locVisitor(mIR,proc);
            mre->acceptVisitor(locVisitor);
            OA_ptr<Location> loc = locVisitor.mLoc;
            
            // attempt to find location in current location sets
            setId = retAliasMap->getMapSetId(loc);
            if (debug) {
                std::cout << "\tsetId = " << setId << ", loc = ";
                loc->dump(std::cout, mIR);
            }

            // if we didn't find the location in the aliasmap
            // sets then we need to put the location in a
            // new aliasmap set 
            if (setId==AliasMap::SET_ID_NONE) {
                // get a new set
                setId = retAliasMap->makeEmptySet();

                // put the loc into the new map set
                retAliasMap->addLocation(loc,setId);
            } 

            // map mre to set only if wasn't already mapped
            if (!mre.ptrEqual(0)) { retAliasMap->mapMemRefToMapSet(mre,setId); }
        }
        // map memref to set
        // FIXME: don't know if memref only has one mre
        retAliasMap->mapMemRefToMapSet(memref,setId);
        //if (setId == AliasMap::SET_ID_NONE ) {
        //  retAliasMap->mapMemRefToMapSet(memref,unknownSetId);
        //} else {
       // }

      } // loop over mre's
    } // loop over memory references
  } // loop over statements

  return retAliasMap;
}

  } // end of namespace Alias
} // end of namespace OA
