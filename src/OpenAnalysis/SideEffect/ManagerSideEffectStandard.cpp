/*! \file
  
  \brief The AnnotationManager that generates SideEffectStandard
         which maps each procedure to the sets of locations belonging to
         LMOD, MOD, LUSE, USE, LDEF, DEF, LREF, and REF.  

  \authors Michelle Strout
  \version $Id: ManagerSideEffectStandard.cpp,v 1.14 2005/08/08 20:03:52 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "ManagerSideEffectStandard.hpp"


namespace OA {
  namespace SideEffect {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerSideEffectStandard)
static bool debug = true;
#else
static bool debug = false;
#endif

/*! concrete LocationVisitor that determines that determines whether
    the visited location involves a symbol that is referenced within
    the procedure or not.
    If the symbol is referenced then mRef will be true.
    If the location does not involve a symbol (ie. UnnamedLoc)
    then mRef will be true as well.
    A set of referenced symbols must be passed in to the constructor.
*/
class RefVisitor : public virtual LocationVisitor {
  private:
    std::set<SymHandle>& mRefSymSet;
  public:
    bool mRef;

    RefVisitor(std::set<SymHandle> &pRefSymSet)
        : mRefSymSet(pRefSymSet), mRef(true) {}
    ~RefVisitor() {}
    void visitNamedLoc(NamedLoc& loc) 
      { 
        if (mRefSymSet.find(loc.getSymHandle()) != mRefSymSet.end())
        { mRef = true; } 
        else { mRef = false; }
      }
    void visitUnnamedLoc(UnnamedLoc& loc) { mRef = true; }
    void visitInvisibleLoc(InvisibleLoc& loc) { mRef = true; }
    void visitUnknownLoc(UnknownLoc& loc) { mRef = true; }
    void visitLocSubSet(LocSubSet& loc) { 
        // have this visitor visit the base location
        OA_ptr<Location> baseLoc = loc.getLoc();
        baseLoc->acceptVisitor(*this);
    }
};


/*!
    \brief    Used to perform analysis when not using AQM

    Currently just does an intraprocedural side-effect analysis.
    We conservatively assume that function calls define and use the
    UnknownLoc, which overlaps with all possible locations.

*/
OA_ptr<SideEffectStandard> 
ManagerStandard::performAnalysis(ProcHandle proc, 
                                 OA_ptr<Alias::Interface> alias,
                                 OA_ptr<InterSideEffectInterface> inter)
{
  mProc = proc;
  if (debug) {
      std::cout << "ManagerSideEffectStandard: proc = " << mIR->toString(proc)
                << std::endl;
  }

  // create a new (waiting to be filled) SideEffectStandard as member
  OA_ptr<SideEffectStandard> retSideEffect;
  retSideEffect= new SideEffectStandard();

  // empty out all the sets
  retSideEffect->emptyLMOD();
  retSideEffect->emptyMOD();
  retSideEffect->emptyLDEF();
  retSideEffect->emptyDEF();
  retSideEffect->emptyLUSE();
  retSideEffect->emptyUSE();
  retSideEffect->emptyLREF();
  retSideEffect->emptyREF();
  
  // get a set of symbols that have been referenced within the proc
  OA_ptr<IRSymIterator> symIter = mIR->getRefSymIterator(proc);
  std::set<SymHandle> refSet;
  for ( ; symIter->isValid(); (*symIter)++ ) {
      SymHandle sym = symIter->current();
      refSet.insert(sym);
      if (debug) {
          std::cout << "inserting into refSet " << mIR->toString(sym) 
                    << "(" << sym.hval() << ")" << std::endl;
      }

  }

  // declare a visitor that knows about the refSet
  RefVisitor refVisitor(refSet);

  // loop through statements
  OA_ptr<OA::IRStmtIterator> sIt = mIR->getStmtIterator(proc);
  for ( ; sIt->isValid(); (*sIt)++) {
    OA::StmtHandle stmt = sIt->current();
    if (debug) {
        std::cout << "\tstmt = ";
        mIR->dump(stmt,std::cout);
    }

    // for each use memory reference in the stmt
    OA_ptr<MemRefHandleIterator> mrIterPtr = mIR->getUseMemRefs(stmt);
    for (; mrIterPtr->isValid(); (*mrIterPtr)++ )
    {
      MemRefHandle memref = mrIterPtr->current();
      if (debug) {
        std::cout << "\tmemref = ";
        mIR->dump(memref,std::cout);
      }

      // each location the memory reference may reference
      OA_ptr<LocIterator> locIterPtr = alias->getMayLocs(memref);
      for (; locIterPtr->isValid(); ++(*locIterPtr)) {

        OA_ptr<Location> useLoc = locIterPtr->current();
        if (debug) {
            std::cout << "useLoc = ";
            useLoc->dump(std::cout, mIR);
        }

        // will only put into these sets if the location is referenced
        // in the procedure
        useLoc->acceptVisitor(refVisitor);
        if  (refVisitor.mRef == true) {

            // put each location used through a memory reference in LREF and REF
            retSideEffect->insertLREF( useLoc);
            retSideEffect->insertREF( useLoc);
    
            // put each location used through a memory reference in LUSE 
            // and USE if the location is not already in DEF
            // FIXME?: assuming everything is used in a statement before 
            // anything
            // is defined, is this not the case in things like short circuits?
            if (!retSideEffect->inDEF(useLoc)) {
            
                retSideEffect->insertLUSE(useLoc);
                retSideEffect->insertUSE(useLoc);
            }
        }
      }
    }

    // for each def memory reference in the stmt
    mrIterPtr = mIR->getDefMemRefs(stmt);
    for (; mrIterPtr->isValid(); (*mrIterPtr)++ )
    {
      MemRefHandle memref = mrIterPtr->current();
      if (debug) {
        std::cout << "\tmemref = ";
        mIR->dump(memref,std::cout);
      }
      
      // put each location may defined into LMOD and MOD
      OA_ptr<LocIterator> locIterPtr = alias->getMayLocs(memref);
      for (; locIterPtr->isValid(); ++(*locIterPtr)) {
        OA_ptr<Location> mayDef = locIterPtr->current();
        if (debug) {
            std::cout << "mayDef = ";
            mayDef->dump(std::cout, mIR);
        }

        // will only put into these sets if the location is referenced
        // in the procedure
        mayDef->acceptVisitor(refVisitor);
        if  (refVisitor.mRef == true) {
          retSideEffect->insertLMOD(mayDef);
          retSideEffect->insertMOD(mayDef);
        }
      } 

      // put each location must defined into LDEF and DEF
      locIterPtr = alias->getMustLocs(memref);
      for (; locIterPtr->isValid(); ++(*locIterPtr)) {
        OA_ptr<Location> mustDef = locIterPtr->current();

        // will only put into these sets if the location is referenced
        // in the procedure
        mustDef->acceptVisitor(refVisitor);
        if (refVisitor.mRef == true) {
          retSideEffect->insertLDEF(mustDef);
          retSideEffect->insertDEF(mustDef);
        }
      } 
    }
    
    // Iterate over procedure calls of a statement
    // to determine their effect
    OA_ptr<IRCallsiteIterator> callsiteItPtr = mIR->getCallsites(stmt);
    for ( ; callsiteItPtr->isValid(); ++(*callsiteItPtr)) {
      ExprHandle expr = callsiteItPtr->current();
      if (debug) {
        std::cout << "\texpr = " << mIR->toString(expr) << std::endl;
      }

      OA_ptr<LocIterator> locIterPtr;
      
      // MOD
      locIterPtr = inter->getMODIterator(expr);
      for ( ; locIterPtr->isValid(); (*locIterPtr)++) {
          retSideEffect->insertMOD(locIterPtr->current());
          if (debug) {
            std::cout << "\tinserting loc = ";
            locIterPtr->current()->dump(std::cout,mIR);
            std::cout << "\tinto MOD" << std::endl;
          }
      } 
      // DEF
      locIterPtr = inter->getDEFIterator(expr);
      for ( ; locIterPtr->isValid(); (*locIterPtr)++) {
          retSideEffect->insertDEF(locIterPtr->current());
      } 
      // USE
      locIterPtr = inter->getUSEIterator(expr);
      for ( ; locIterPtr->isValid(); (*locIterPtr)++) {
          //if (!retSideEffect->inDEF(locIterPtr->current())) {
          //FIXME: don't know order of use vs def in func call
          retSideEffect->insertUSE(locIterPtr->current());
          //}
      } 
      // REF
      locIterPtr = inter->getREFIterator(expr);
      for ( ; locIterPtr->isValid(); (*locIterPtr)++) {
          retSideEffect->insertREF(locIterPtr->current());
      } 
    }
	
  } // end of loop over statements

  return retSideEffect;
}
  } // end of namespace SideEffect
} // end of namespace OA
