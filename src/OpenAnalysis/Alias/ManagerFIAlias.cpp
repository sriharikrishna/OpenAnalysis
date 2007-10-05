/*! \file
  
  \brief The AnnotationManager that generates union-find sets of MREs, where
         each MRE in the set may alias other MREs in the set.

  \authors Michelle Strout, Brian White
  \version $Id: ManagerFIAlias.cpp,v 1.1.2.20 2006/01/18 15:38:41 mstrout Exp $

  Copyright (c) 2002-2005, Rice University <br>
  Copyright (c) 2004-2005, University of Chicago <br>
  Copyright (c) 2006, Contributors <br>
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "ManagerFIAlias.hpp"
#include <Utils/Util.hpp>

namespace OA {
  namespace Alias {

static bool debug = false;

ProcHandle AnalyzedProcIterator::current() const { return *mIt; }
bool AnalyzedProcIterator::isValid() const { return ( mIt != mAnalyzedProcs.end());  }
void AnalyzedProcIterator::operator++() { mIt++; }
void AnalyzedProcIterator::reset() { mIt = mAnalyzedProcs.begin(); }

OA_ptr<IRProcIterator> ManagerFIAlias::getAnalyzedProcIter()
{
  OA_ptr<AnalyzedProcIterator> iterator;
  iterator = new AnalyzedProcIterator(mAnalyzedProcs);
  return iterator;
}

OA_ptr<LocSetIterator> FixedLocationVisitor::getDirectRefLocIterator()
{
  OA_ptr<LocSetIterator> locSetIter;
  locSetIter = new LocSetIterator(mDirectRefLocations);
  return locSetIter;
}

void FixedLocationVisitor::visitUnknownRef(UnknownRef& ref) 
{ 
  notDirect(); 
}
    
void FixedLocationVisitor::visitDeref(Deref& ref) 
{ 
  notDirect(); 
}

void FixedLocationVisitor::notDirect() 
{
  mDirectRef = false;
  mDirectRefLocations->clear();
}


void FixedLocationVisitor::visitAddressOf(AddressOf& ref)
{
   notDirect();
}


void FixedLocationVisitor::visitNamedRef(NamedRef& ref) 
{
    mLoc = mIR->getLocation(mProc,ref.getSymHandle());
    if (mLoc.ptrEqual(0)) {
      if (debug) {
        std::cout << "FixedLocationVisitor::visitNamedRef, mLoc is null" 
                  << std::endl;
        std::cout << "\tmProc = " << mIR->toString(mProc) << std::endl;
      }
      notDirect();
    } else {
      if (debug) {
        std::cout << "FixedLocationVisitor::visitNamedRef, mLoc is direct" 
                  << std::endl;
        std::cout << "\tmProc = " << mIR->toString(mProc) << std::endl;
      }
      mDirectRef = true;

      mDirectRefLocations->insert(mLoc);
    }

}

void FixedLocationVisitor::visitUnnamedRef(UnnamedRef& ref) 
{ 
    mDirectRef = true;
    // UnnamedLocs are not local
    mLoc = new UnnamedLoc(ref.getExprHandle(),ref.isLocal());

    if (mDirectRef) {
      mDirectRefLocations->insert(mLoc);
    }
}

void FixedLocationVisitor::visitSubSetRef(SubSetRef& ref) 
{
  if (debug) { 
      std::cout << "In FixedLocationVisitor::visitSubSetRef" << std::endl;
  }
  // must also visit child memory reference
  OA_ptr<MemRefExpr> mre = ref.getMemRefExpr();
  if (!mre.ptrEqual(0)) {
       mre->acceptVisitor(*this);
  }     
  if(mDirectRef) {
       mLoc = new LocSubSet(mLoc,false);
       mDirectRefLocations->insert(mLoc);
  } 
    
}


void FixedLocationVisitor::visitFieldAccess(FieldAccess& ref) 
{
  if (debug) { 
      std::cout << "In FixedLocationVisitor::visitFieldAccess" << std::endl;
  }
  // must also visit child memory reference
  OA_ptr<MemRefExpr> mre = ref.getMemRefExpr();
  if (!mre.ptrEqual(0)) {
       mre->acceptVisitor(*this);
  }     
  if(mDirectRef) {
        OA_ptr<LocFieldSubSet> loc;
        loc = new LocFieldSubSet(mLoc, ref.getFieldName());
        mLoc = loc;
        mDirectRefLocations->insert(mLoc);
  }
}

/*!
*/
ManagerFIAlias::ManagerFIAlias( OA_ptr<AliasIRInterface> _ir) : mIR(_ir),
    mCount(1)
{
    OA_DEBUG_CTRL_MACRO("DEBUG_ManagerFIAlias:ALL", debug);
}

//!  Only incremental versions of FIAlias (e.g., FIAliasReachable)
//!  use a worklist.  Therefore, this is a noop by default.
void ManagerFIAlias::addProcToWorkList(ProcHandle proc)
{
  if ( mImplement == REACHABLE_PROCS ) {
      if ( mAnalyzedProcs.find(proc) != 
           mAnalyzedProcs.end() ) {
          return;
      }

      if ( mWorklist.find(proc) != 
           mWorklist.end() ) {
          return;
      }

      mWorklist.insert(proc);
  }
}

/*!
   Visitor over memory reference expressions that determines if there
   is some kind of RefOp performed and if so creates a version of the
   outermost RefOp that does not point to the underlying MemRefExpr.
   For example, if mre is Deref(NamedRef(a)) then getOuterRefOp will
   return a Deref().
 */
class OuterRefOpVisitor : public virtual MemRefExprVisitor {
  public:
    OuterRefOpVisitor() {}
    ~OuterRefOpVisitor() {}

    OA_ptr<RefOp> getOuterRefOp() {
        return mOuterRefOp;
    }
    void visitNamedRef(NamedRef& ref) { 
    }
    void visitUnnamedRef(UnnamedRef& ref) { 
    }
    void visitUnknownRef(UnknownRef& ref) { 
    }
    void visitAddressOf(AddressOf& ref) {
        // this visitor shouldn't be called on an MRE
        // that has its address taken
        assert(0);
    }
    void visitDeref(Deref& ref) { 
        assert(ref.getNumDerefs()==1);
        OA_ptr<MemRefExpr> nullmre;
        mOuterRefOp = new Deref(MemRefExpr::USE, nullmre, 1);
    }
    // default handling of more specific SubSet specificiations
    void visitSubSetRef(SubSetRef& ref) { 
        OA_ptr<MemRefExpr> nullmre;
        mOuterRefOp = new SubSetRef(MemRefExpr::USE, nullmre);
    }
    void visitIdxAccess(IdxAccess& ref) { 
        OA_ptr<MemRefExpr> nullmre;
        OA_ptr<MemRefExpr> childMRE = ref.getMemRefExpr();
        assert(!childMRE.ptrEqual(0));
        mOuterRefOp = new IdxAccess(MemRefExpr::USE,
                                    nullmre, ref.getIdx());

    }

    void visitFieldAccess(FieldAccess& ref) { 
        OA_ptr<MemRefExpr> nullmre;
        OA_ptr<MemRefExpr> childMRE = ref.getMemRefExpr();
        assert(!childMRE.ptrEqual(0));
        mOuterRefOp = new FieldAccess(MemRefExpr::USE,
                                      nullmre, ref.getFieldName() );
    }
  
  private:
    OA_ptr<RefOp> mOuterRefOp;
};


bool InvisibleLocationVisitor::isInvisibleRef() 
{
  return mInvisibleRef;
}

OA_ptr<Location> InvisibleLocationVisitor::getInvisibleRefLoc() 
{
  return mLoc;
}

void InvisibleLocationVisitor::visitUnnamedRef(UnnamedRef& ref) 
{ 
  notInvisible(); 
}

void InvisibleLocationVisitor::visitUnknownRef(UnknownRef& ref) 
{ 
  notInvisible(); 
}

void InvisibleLocationVisitor::notInvisible() 
{
  mInvisibleRef = false;
  OA_ptr<Location> nullLoc;
  mLoc = nullLoc;
}

void InvisibleLocationVisitor::visitAddressOf(AddressOf& ref)
{
  notInvisible(); 
}


void InvisibleLocationVisitor::visitNamedRef(NamedRef& ref) 
{
    // initialize result
    mInvisibleRef = false;
    mBaseIsNotLocal = false;
    mBaseIsFormal = false;
  
    mLoc = mIR->getLocation(mProc,ref.getSymHandle());
    // symbol isn't visible
    if (mLoc.ptrEqual(0)) {
      notInvisible();
    } else if (!mLoc->isLocal()) {
      mBaseIsNotLocal = true;
    } else if (mProcFormalSet.find(ref.getSymHandle()) 
               != mProcFormalSet.end() ) 
    {
      mBaseIsFormal = true;
    }
    // the location will be created on the outermost deref
}

void InvisibleLocationVisitor::visitDeref(Deref& ref) 
{ 
    // first call recursively on what we are a dereference for
    OA_ptr<MemRefExpr> mre = ref.getMemRefExpr();
    if (!mre.ptrEqual(0)) { mre->acceptVisitor(*this); }
    if (mBaseIsNotLocal || mBaseIsFormal) {
      mInvisibleRef = true;
      // we may not be outermost, but if we are then we need
      // to create invisible location.  If we aren't then 
      // another RefOp will replace mLoc
      
      mLoc = new InvisibleLoc(ref.clone());
    }
}   

void InvisibleLocationVisitor::visitSubSetRef(SubSetRef& ref) 
{
  OA_ptr<MemRefExpr> mre = ref.getMemRefExpr();
  if (!mre.ptrEqual(0)) { mre->acceptVisitor(*this); }
   
  if (mInvisibleRef) {
    //mLoc = new InvisibleLoc(ref.clone());
    OA_ptr<LocSubSet> sloc;
    mLoc = new LocSubSet(mLoc, false);
  }
}

void InvisibleLocationVisitor::visitFieldAccess(FieldAccess& ref)
{
  OA_ptr<MemRefExpr> mre = ref.getMemRefExpr();
  if (!mre.ptrEqual(0)) { mre->acceptVisitor(*this); }

  if (mInvisibleRef) {
    mLoc = new LocFieldSubSet(mLoc,ref.getFieldName());
  }
}

void InvisibleLocationVisitor::visitIdxAccess(IdxAccess& ref)
{
  OA_ptr<MemRefExpr> mre = ref.getMemRefExpr();
  if (!mre.ptrEqual(0)) { mre->acceptVisitor(*this); }

  if (mInvisibleRef) {
    mLoc = new LocIdxSubSet(mLoc,ref.getIdx());
  }
}

void InvisibleLocationVisitor::visitIdxExprAccess(IdxExprAccess& ref)
{
  OA_ptr<MemRefExpr> mre = ref.getMemRefExpr();
  if (!mre.ptrEqual(0)) { mre->acceptVisitor(*this); }

  if (mInvisibleRef) {
    mLoc = new LocSubSet(mLoc, false);
  }
}

//-------------------------------------------------------
void VisibleBaseVisitor::visitUnknownRef(UnknownRef& ref) 
{ 
  mBaseVisible = true;
}
    
void VisibleBaseVisitor::visitDeref(Deref& ref) 
{ 
  // call recursively on what we are a dereference for
  OA_ptr<MemRefExpr> mre = ref.getMemRefExpr();
  if (!mre.ptrEqual(0)) { mre->acceptVisitor(*this); }
}

void VisibleBaseVisitor::visitAddressOf(AddressOf& ref)
{
   assert(0);
}

void VisibleBaseVisitor::visitNamedRef(NamedRef& ref) 
{
    OA_ptr<Location> loc = mIR->getLocation(mProc,ref.getSymHandle());
    if (loc.ptrEqual(0)) {
      mBaseVisible = false;
    } else {
      mBaseVisible = true;
    }
}

void VisibleBaseVisitor::visitUnnamedRef(UnnamedRef& ref) 
{
    mBaseVisible = false;
    if(ref.isLocal() == true) {
       if(ref.getProcHandle() == mProc) {
          mBaseVisible = true;
       } 
    } else {
       mBaseVisible = true; 
    }
}

void VisibleBaseVisitor::visitSubSetRef(SubSetRef& ref) 
{
  mBaseVisible = true;  
  // call recursively on MemRefExpr encapsulated 
  // inside SubSetRef
  OA_ptr<MemRefExpr> mre = ref.getMemRefExpr();
  if (!mre.ptrEqual(0)) { mre->acceptVisitor(*this); }
}

/* MMS 3/25/07, we will need this routine if we ever decide to
 * resusitate ManagerFIAliasEquivSets, but it is not needed here
 *
void ManagerFIAlias::mergeSubSetRefs(OA_ptr<UnionFindUniverse> ufset) {

     std::map<OA_ptr<MemRefExpr>,int>::iterator mapIter;
     for (mapIter=mMREToID.begin(); mapIter!=mMREToID.end(); mapIter++) {
          OA_ptr<MemRefExpr> mymre = mapIter->first;
              
          if(mymre->isaRefOp()) {  
            OA_ptr<RefOp> submre;
            submre = mymre.convert<RefOp>();
            if(submre->isaSubSetRef()) {
               OA_ptr<SubSetRef> recast = submre.convert<SubSetRef>();
               OA_ptr<MemRefExpr> child_mre;
               child_mre = recast->getMemRefExpr();
               ufset->Union( ufset->Find(mMREToID[mymre]),
                          ufset->Find(mMREToID[child_mre]),
                          ufset->Find(mMREToID[child_mre]) );
            } 
          }
     }
}
*/

void
ManagerFIAlias::doPhase1Iteration(StmtHandle stmt, ProcHandle currProc, 
                                  OA_ptr<UnionFindUniverse> ufset)
{
    // Perform Ryder Phase 1 on stmt.

    // for each ptrassignpair
    OA_ptr<PtrAssignPairStmtIterator> pairIter
        = mIR->getPtrAssignStmtPairIterator(stmt);
    for ( ; pairIter->isValid(); (*pairIter)++ ) {

        OA_ptr<MemRefExpr> target = pairIter->currentTarget();        
        OA_ptr<MemRefExpr> source = pairIter->currentSource();        

        // map *target and *source to an id if they don't already have one
        OA_ptr<MemRefExpr> sourceDeref, targetDeref;

        // make a deref version of target mre
        targetDeref = createDeref( target );
        recordMRE(targetDeref, currProc );
            
        // make a deref version of source mre
        sourceDeref = createDeref( source );
        recordMRE(sourceDeref, currProc );
             
        if (debug) {
            std::cout << "\tsourceDeref = ";
            sourceDeref->output(*mIR);
            std::cout << "\ttargetDeref = ";
            targetDeref->output(*mIR);
            std::cout << "\t===> Union ( "
                      << ufset->Find(mMREToID[targetDeref]) << ", "
                      << ufset->Find(mMREToID[sourceDeref]) << " )"
                      << std::endl;
        }
                
        // then union the sets with *target and *source 
        ufset->Union( ufset->Find(mMREToID[targetDeref]),
                      ufset->Find(mMREToID[sourceDeref]),
                      ufset->Find(mMREToID[sourceDeref]) );

    } // iteration over ptr assign pairs

}

bool
ManagerFIAlias::doPhase2Iteration(OA_ptr<UnionFindUniverse> ufset,
                                  std::map<int,std::map<OA_ptr<MemRefExpr>,int> > & map) 
{
    bool changed = false;

    // for each memref
    std::map<OA_ptr<MemRefExpr>,int>::iterator mreMapIter;
    for (mreMapIter=mMREToID.begin(); 
         mreMapIter!=mMREToID.end(); mreMapIter++ ) 
    {
        OA_ptr<MemRefExpr> mre = mreMapIter->first;
        if (debug) {
            std::cout << std::endl << "\tmre = ";
            mre->output(*mIR);
            std::cout << "\t\tFind(mMREToID[mre]) = " 
                      << ufset->Find(mMREToID[mre]) << std::endl;
        }

        // only do the following for real MREs, if the MRE is part of
        // an addressOf operation then the outermost MRE doesn't involve
        // any memory being accessed
        if(mre->isaRefOp()) {
            OA_ptr<RefOp> refop = mre.convert<RefOp>();
            if(!refop->isaAddressOf()) {
                OuterRefOpVisitor outerRefVisitor;
                mre->acceptVisitor(outerRefVisitor);
                OA_ptr<RefOp> justRefop = outerRefVisitor.getOuterRefOp();
                OA_ptr<MemRefExpr> innerMRE = refop->getMemRefExpr();

                if (debug) {
                    std::cout << "\t\tinnerMRE = ";
                    if (! innerMRE.ptrEqual(0) ) {
                        innerMRE->output(*mIR);
                    } else { 
                        std::cout << "<null MRE>" << std::endl;
                    }
                    std::cout << "\t\tFind( innerMRE ) = "
                              << ufset->Find(mMREToID[innerMRE])
                              << std::endl;
                    std::cout << "\t\tjustRefop = ";
                    if (! justRefop.ptrEqual(0) ) {
                        justRefop->output(*mIR);
                    } else { 
                        std::cout << "<null MREF>" << std::endl;
                    }
                }

                // if find(memref)!=find( map[find(innerMRE)][justRefop] )
                int setID = ufset->Find( 
                    map[ufset->Find(mMREToID[innerMRE])][justRefop] );
                if ( ufset->Find(mMREToID[mre]) != setID ) {
                    changed = true;
                    // if map[find(memref->getMemRef)][refop] is not in a part yet
                    if (setID == 0) {
                        // then put it in the one for memref
                        map[ufset->Find(mMREToID[innerMRE])][justRefop] 
                            = ufset->Find(mMREToID[mre]);
                    // else
                    } else {
                        merge(ufset->Find(mMREToID[mre]), setID, ufset, map);
                    }
                }
            }// does not have address taken
        } // is a refop
    } // over mre
    return changed;
}

void 
ManagerFIAlias::doPhase3Iteration(CallHandle call, ProcHandle currProc,
                                  OA_ptr<UnionFindUniverse> ufset,
                                  std::map<int,std::map<OA_ptr<MemRefExpr>,int> > & map)
{
    // get the mre for the function call (eg. NamedRef('foo'))
    OA_ptr<MemRefExpr> callMRE = mIR->getCallMemRefExpr(call);

    // set of all MREs that are aliased with this callMRE
    std::set<OA_ptr<MemRefExpr> > aliasedMREs;
    aliasedMREs.insert(callMRE);

    // if it is a NamedRef then it can only be referring
    // to one procedure (assumption one-to-one mapping
    // between symbols for procedures and procedure handles
    // for procedures
    // If it isn't a nameRef then we must have a function ptr
    if (! callMRE->isaNamed()) {

        // gather all the MREs that are in the same set as the callMRE
        // NOTICE: only do this if is not a Named, NamedRefs for calls
        // aren't actually given an MREToID
        aliasedMREs  = allMemRefExprsInSameSet( callMRE, ufset );
    }

    // Previously, we iterated over params and then over
    // aliases to procedures potentially invoked at the call site.
    // I am inverting this.  We visit potential aliases to
    // discover new static resolution of indirect calls.  For
    // each of these, newly discovered, we need to add it to 
    // the worklist.  Doing this in the opposite order would
    // not allow us to visit functions without params or without
    // pointer params that are only indirectly invoked.  BSW 10/27/06

    // Note that for non-incremental version of ManagerFIAlias
    // (i.e., those other than ManagerFIAliasReachable)
    // there is no concept of a worklist.  Hence addProcToWorkList
    // is a no-op.  Nevertheless, the above-mentioned loop
    // rearrangement does not cause any inefficiencies.

    // for all mres in the same set
    std::set<OA_ptr<MemRefExpr> >::iterator mreIter; 
    for (mreIter=aliasedMREs.begin(); mreIter!=aliasedMREs.end();
         mreIter++ )
    {
        OA_ptr<MemRefExpr> possCallMRE = *mreIter;
        OA_ptr<NamedRef> namedProc;

        if (debug) {
            std::cout << "Aliased call mre: ";
            possCallMRE->output(*mIR);
        }

        // if the mre is a NamedRef then
        if (possCallMRE->isaNamed()) {
            namedProc = possCallMRE.convert<NamedRef>();
            if (debug) {
                std::cout << " (Named)" << std::endl;
            }
        } else {
            if (debug) {
                std::cout << " (Unnamed)" << std::endl;
            }
            continue;
        }
                  
        // get the symbol for the function
        SymHandle sym = namedProc->getSymHandle();
                
        // get the procedure handle for that symbol
        ProcHandle proc = mIR->getProcHandle(sym);

        // check that the symbol actually has an associated procedure
        if (proc==ProcHandle(0)) { continue; }

        // We have a statically resolvable function that may
        // be invoked at this callsite.  Add it to the
        // worklist to be analyzed, if it isn't already pending
        // and hasn't already been analyzed.
        addProcToWorkList(proc);

        // for each implicit ptrassignpair due to formal/actual binding
        OA_ptr<ParamBindPtrAssignIterator> pairIter
            = mIR->getParamBindPtrAssignIterator(call);
        for ( ; pairIter->isValid(); (*pairIter)++ ) {
 	
            int formalId = pairIter->currentFormalId();
            OA_ptr<MemRefExpr> actualMRE = pairIter->currentActual();        
            
            if (debug) {
                std::cout << "Call mre: ";
                callMRE->output(*mIR);
                std::cout << " formal id: " << formalId << std::endl;
            }

            // get the formal symbol for the param binding
            // we are handling
            SymHandle formalSym = mIR->getFormalSym(proc,formalId);
            // if the signature doesn't match then don't process
            if (formalSym==SymHandle(0)) { continue; }

            // make mre for the formal
            // FIXME: since already do this, could save time
            // by memoizing in a map[proc][formalID]
            // Create a namedRef for the formal.
            bool addressTaken = false;
            bool fullAccuracy = true;
            // DEF since formal = actual.
            MemRefExpr::MemRefType mrType = MemRefExpr::DEF;
            OA_ptr<MemRefExpr> formalmre; 
            formalmre = new NamedRef(mrType, formalSym);

            // create a dereference for the formalref and actualref
            // record both the dereferences
            // union the sets they are in
            OA_ptr<MemRefExpr> targetDeref = createDeref( formalmre );
            recordMRE(targetDeref, proc );
            OA_ptr<MemRefExpr> sourceDeref = createDeref( actualMRE );
            recordMRE(sourceDeref, currProc);
                 
            if (debug) {
                std::cout << "ParamBindPtrAssign: ";
                std::cout << "\tsourceDeref = ";
                sourceDeref->output(*mIR);
                std::cout << "\ttargetDeref = ";
                targetDeref->output(*mIR);
                std::cout << "\t===> merge ( "
                          << ufset->Find(mMREToID[targetDeref]) << ", "
                          << ufset->Find(mMREToID[sourceDeref]) << " )"
                          << std::endl;
            }
                
            // then merge the sets for the target and source deref
            merge( ufset->Find(mMREToID[targetDeref]),
                   ufset->Find(mMREToID[sourceDeref]),
                   ufset, map);

        } // over possible procedures for function call

    } // paramBindPtrAssigns
}

OA_ptr<UnionFindUniverse>
ManagerFIAlias::performFIAlias( OA_ptr<IRProcIterator> procIter,
                                FIAliasImplement implement )
{
    OA_ptr<UnionFindUniverse> ufset;

    mImplement = implement;
    if ( mImplement == ALL_PROCS ) {
        ufset = performFIAliasAllProcs(procIter);
    } else { 
        ufset = performFIAliasReachableProcs(procIter);
    }

    if ( debug ) {
        OA_ptr<IRProcIterator> analyzedProcIter = getAnalyzedProcIter();
        int numAnalyzedProcs = 0;
        for(analyzedProcIter->reset(); analyzedProcIter->isValid(); ++(*analyzedProcIter)) {
            ++numAnalyzedProcs;
        }

        std::cout << "ManagerFIAlias::performFIAlias analyzed " << numAnalyzedProcs << " procs." << std::endl;
    }

    return ufset;
}

OA_ptr<UnionFindUniverse>
ManagerFIAlias::performFIAliasAllProcs( OA_ptr<IRProcIterator> procIter)
{
    if ( debug ) {
        std::cout << "performFIAliasAllProcs" << std::endl;
    }

    // ptr to union find datastructure
    OA_ptr<UnionFindUniverse> ufset;

    // map each MemRefExpr to a unique id and count them all
    initMemRefExprs(procIter);

    // declare the union-find datastructure
    // 1 for each memrefExpr we have seen so far and each one
    // could get a deref added in the worse case, the extra
    // 1 is for zero
    ufset = new UnionFindUniverse(2*mCount+1);

//   mergeSubSetRefs(ufset);

    // for all the stmts in the whole program
    // Iterate over all the procedures in the program
    for (procIter->reset() ; procIter->isValid(); ++(*procIter)) { 
      ProcHandle currProc = procIter->current();

      // Add this procedure to the set of those that are reachable 
      // and have already been analyzed (or are about to be analyzed,
      // as with currProc).
      // FIAlias visits all procs; this makes more sense for 
      // FIAliasReachable.
      mAnalyzedProcs.insert(currProc);

      // Iterate over the statements of this procedure
      OA_ptr<IRStmtIterator> stmtIterPtr = mIR->getStmtIterator(currProc);
      // Iterate over the statements of this block adding procedure references
      for ( ; stmtIterPtr->isValid(); ++(*stmtIterPtr)) {

        StmtHandle stmt = stmtIterPtr->current();

        if (debug) {
          std::cout << "===========================" << std::endl << "stmt = ";
          mIR->dump(stmt,std::cout);
          std::cout << std::endl;
        }

        doPhase1Iteration(stmt, currProc, ufset);

      } // over stmts
    } // over procedures

    // this will be the default value for this
    // for each partition
    //    map[part][memref] = 0; 
    std::map<int,std::map<OA_ptr<MemRefExpr>,int> > map;

    bool changed = true;

    while (changed) {

        //// Phase 2 in FIAlias algorithm plus some additions for SubSetRefs
        // do this until there are no more changes
      
        // Please note that 'phases' are a bit of a misnomer since we
        // iterate over phases 2 and 3 together-- as opposed to 
        // separately iterating over phase 2, then phase 3.  Doing them
        // separately appears to be a bug in Fig 5 of the Ryder 2001 paper.

        if (debug) { 
            std::cout << std::endl 
                      << "======== Beginning of Phase 2 while loop =====" 
                      << std::endl; 
        }

        changed = doPhase2Iteration(ufset, map);
        
        //// Phase 3 in FIAlias: accounting for calls through function ptrs
        ////                     and non-function ptr calls
      
        if (debug) { 
            std::cout << std::endl 
                      << "======== Beginning of Phase 3 while loop =====" 
                      << std::endl; 
        }

        // for all the stmts in the whole program
        // Iterate over all the procedures in the program
        for (procIter->reset() ; procIter->isValid(); ++(*procIter)) { 
            ProcHandle currProc = procIter->current();

            // Iterate over the statements of this procedure
            OA_ptr<IRStmtIterator> stmtIterPtr = mIR->getStmtIterator(currProc);
            // Iterate over the statements of this block adding procedure references
            for ( ; stmtIterPtr->isValid(); ++(*stmtIterPtr)) {
                StmtHandle stmt = stmtIterPtr->current();
                // for all the callsites in the stmt
                OA_ptr<IRCallsiteIterator> callIter = mIR->getCallsites(stmt);
                for ( ; callIter->isValid(); (*callIter)++ ) {
                    CallHandle call = callIter->current();

                    if (debug) {
                        std::cout << "Call site: " << mIR->toString(call) << std::endl;
                    }

                    doPhase3Iteration(call, currProc, ufset, map);
            
                } // call sites
            } // over stmts
        } // over procedures
    } // iterating to fixed point 

    return ufset;
}

OA_ptr<UnionFindUniverse>
ManagerFIAlias::performFIAliasReachableProcs( OA_ptr<IRProcIterator> procIter)
{
    if ( debug ) {
        std::cout << "performFIAliasReachableProcs" << std::endl;
    }

    // Maintain a list of procedures known to be reachable, but
    // which have not yet been processed.  Those procs in the
    // procIter are considered a seed of reachable functions passed
    // by the caller.  In the common case, we expect procIter to 
    // hold only main.
    for (procIter->reset() ; procIter->isValid(); ++(*procIter)) { 
      ProcHandle currProc = procIter->current();
      addProcToWorkList(currProc);
    }

    // Keep a list of ambiguous/indirect calls that require
    // alias analysis for resolution.  The ProcHandle is the
    // caller.
    std::set<std::pair<CallHandle,ProcHandle> > indirect_calls;

    // ptr to union find datastructure
    OA_ptr<UnionFindUniverse> ufset;

    // declare the union-find datastructure
    // FIAlias would have found all MREs in the program
    // at this point and would know how large to make the
    // UnionFindUniverse.  We analyze procedures on demand 
    // and do not have this information.  We could analyze
    // main to seed the size of the universe, but this
    // would complicate the loop below.  Instead, just
    // set it to something arbitrary and let it grow.
    int initialSize = 1024;
    ufset = new UnionFindUniverse(2*initialSize+1);

    // this will be the default value for this
    // for each partition
    //    map[part][memref] = 0; 
    std::map<int,std::map<OA_ptr<MemRefExpr>,int> > map;

    bool changed = true;

    unsigned int iter_count = 0;
    unsigned int stmts_analyzed = 0;

    while ( changed ) {

        // Iterate over the worlist
        while ( mWorklist.begin() != mWorklist.end() ) {

            ProcHandle currProc = *(mWorklist.begin());
            mWorklist.erase(currProc);

            // Add this procedure to the set of those that are reachable 
            // and have already been analyzed (or are about to be analyzed,
            // as with currProc).
            mAnalyzedProcs.insert(currProc);

            // Map each MemRefExpr in this proc to a unique id and 
            // count them all.
            initMemRefExprs(currProc);

            // Iterate over the statements of this procedure.
            OA_ptr<IRStmtIterator> stmtIterPtr = 
                mIR->getStmtIterator(currProc);
            // Iterate over the statements of this block, adding 
            // procedure references.
            for ( ; stmtIterPtr->isValid(); ++(*stmtIterPtr)) {

	        stmts_analyzed++;
                StmtHandle stmt = stmtIterPtr->current();
                if (debug) {
                    std::cout << "===========================" 
                              << std::endl 
                              << "stmt = ";
                    mIR->dump(stmt,std::cout);
                    std::cout << std::endl;
                }

                doPhase1Iteration(stmt, currProc, ufset);

                // End Ryder phase 1.

                // Visit each call in the current proc.  If the
                // target is Named (i.e., resolvable), 
                // then add it to the worklist.  [However, notice
                // that this happens in doPhase3Iteration.] 
                // If the target is not a Named mre, 
		// then it is an indirect call--
                // add it to the list of indirect_calls.
                // In any case, perform phase 2b-- i.e., param
                // bindings.

                // For all the callsites in the stmt
                OA_ptr<IRCallsiteIterator> callIter = mIR->getCallsites(stmt);
                for ( ; callIter->isValid(); (*callIter)++ ) {
                    CallHandle call = callIter->current();

                    if (debug) {
                        std::cout << "Call site: " 
                                  << mIR->toString(call) 
                                  << std::endl;
                    }

                    // Get the mre for the function call (eg. NamedRef('foo'))
                    OA_ptr<MemRefExpr> callMRE = mIR->getCallMemRefExpr(call);

                    if (!callMRE->isaNamed()) {
                        // The call is not a named MRE-- it must be an
                        // indirect call.
                        indirect_calls.insert(std::pair<CallHandle,ProcHandle>(call,currProc));
                    }

		    // Perform param binding-- i.e., phase 2b.
                    doPhase3Iteration(call, currProc, ufset, map);

                } // End iteration over call sites

            } // over stmts

        } // over worklist
        
        //// Phase 2 in FIAlias algorithm plus some additions for SubSetRefs
        // do this until there are no more changes
      
        // Please note that 'phases' are a bit of a misnomer since we
        // iterate over phases 2 and 3 together-- as opposed to 
        // separately iterating over phase 2, then phase 3.  Doing them
        // separately appears to be a bug in Fig 5 of the Ryder 2001 paper.

        if (debug) { 
            std::cout << std::endl 
		      << "======== Beginning of Phase 2 while loop =====" 
		      << std::endl; 
        }

        changed = doPhase2Iteration(ufset, map);
        
        //// Phase 3 in FIAlias: accounting for calls through function ptrs
        ////                     and non-function ptr calls
      
        if (debug) { 
            std::cout << std::endl 
                      << "======== Beginning of Phase 3 while loop =====" 
                      << std::endl; 
        }

        // Ryder phase 3/CGO phase 2b performs param binding.
        // Notice that we have already done this above.  However,
	// an indirect call site may have acquired a static
        // resolution through alias analysis.  Therefore, revisit
        // all indirect call sites and (re-)apply param binding.

        for ( std::set<std::pair<CallHandle,ProcHandle> >::iterator it = indirect_calls.begin();
              it != indirect_calls.end(); ++it) {

            CallHandle call = it->first;
            ProcHandle caller = it->second;

            doPhase3Iteration(call, caller, ufset, map);
        } // call sites
        ++iter_count;
    } // iterating to fixed point 

    return ufset;
}

/*!
   Private method for associating an mre with a particular procedure.
*/
void
ManagerFIAlias::recordMRE( OA_ptr<MemRefExpr> mre, ProcHandle proc )
{
    
    if (debug) {
        std::cout << "In recordMRE ..." << std::endl;
        std::cout << "\tmre = ";
        mre->output(*mIR);
        std::cout << "\tproc = " << mIR->toString(proc) << std::endl;
    }

    // check if it doesn't already have an id in the range 1..
    if (mMREToID[mre] == 0) {
        mMREToID[mre] = mCount++;
    }

    mMREToProcs[mre].insert(proc);
}

/*!
   Private method for associating an mre with a particular procedure and
   MemRefHandle.  Also associates MemRefHandle with given procedure.
*/
void
ManagerFIAlias::recordMRE( OA_ptr<MemRefExpr> mre, ProcHandle proc, 
                           MemRefHandle memref)
{
    if (debug) {
        std::cout << "In recordMRE ..." << std::endl;
        std::cout << "\tmre = ";
        mre->output(*mIR);
        std::cout << "\tproc = " << mIR->toString(proc) << std::endl;
        std::cout << "\tmemref = ";
        mIR->dump(memref,std::cout);
    }
    // each MemRefHandle can only show up in one procedure
    mMemRefHandleToProc[memref] = proc;

    // check if it doesn't already have an id in the range 1..
    if (mMREToID[mre] == 0) {
        mMREToID[mre] = mCount++;
    }

    // keep track of which MemRefHandles an MRE expresses and which procs
    // it is found in
    mMREToMemRefHandles[mre].insert(memref);
    mMREToProcs[mre].insert(proc);
}

//! creates a dereferenced version of the given mre
OA_ptr<MemRefExpr>
ManagerFIAlias::createDeref( OA_ptr<MemRefExpr> mre )
{

   OA_ptr<MemRefExpr> retval;

/*
    // if this is an addressOf then remove the addressOf
    if (mre->isaRefOp()) {
      OA_ptr<RefOp> refOp = mre.convert<RefOp>();
      if(refOp->isaAddressOf()) {
         retval = mre->clone();
         retval = refOp->getMemRefExpr();
         return retval;
      }
    }
*/
    int numDerefs = 1;
    OA::OA_ptr<OA::Deref> deref_mre;
    OA::OA_ptr<OA::MemRefExpr> nullMRE;

    deref_mre = new OA::Deref(
                              OA::MemRefExpr::USE,
                              nullMRE,
                              numDerefs);
    retval = deref_mre->composeWith(mre->clone());

    return retval;

}


std::set<OA_ptr<MemRefExpr> > 
ManagerFIAlias::allMemRefExprsInSameSet( OA_ptr<MemRefExpr> pMRE,
    OA_ptr<UnionFindUniverse> ufset)
{
    std::set<OA_ptr<MemRefExpr> > retval;

    int setID = ufset->Find(mMREToID[pMRE]);

    std::map<OA_ptr<MemRefExpr>,int>::iterator mapIter;
    for (mapIter=mMREToID.begin(); mapIter!=mMREToID.end(); mapIter++)
    {
        OA_ptr<MemRefExpr> mre = mapIter->first;
        if (debug) {
            std::cout << "allMemRefExprsInSameSet: mre = ";
            mre->dump(std::cout);
        }
        if ( ufset->Find(mMREToID[mre]) == setID ) {
            retval.insert(mre);
        }
    }

    return retval;
}

/*!
   Visitor over memory reference expressions that grabs all sub 
   MREs and makes sure that they are recorded.

   NOTE that we are treating the non-address of namedref as a sub
   MRE to the named ref that has its address set.
   Actually need this for all MREs so doing it outside of this visitor.
 */

class RecordMREsVisitor : public virtual MemRefExprVisitor {
  public:
    RecordMREsVisitor(ManagerFIAlias &manager, ProcHandle proc )
        : mManager(manager), mProc(proc) {}
    ~RecordMREsVisitor() {}

    void visitNamedRef(NamedRef& ref) { 
        OA_ptr<MemRefExpr> mre = ref.clone();
        mManager.recordMRE(mre, mProc); 
    }
    void visitUnnamedRef(UnnamedRef& ref) {
        OA_ptr<MemRefExpr> mre = ref.clone();
        mManager.recordMRE(mre, mProc); 
    }
    void visitUnknownRef(UnknownRef& ref) {
        OA_ptr<MemRefExpr> mre = ref.clone();
        mManager.recordMRE(mre, mProc); 
    }
    void visitAddressOf(AddressOf& ref) {
        // do not record the addressOf, but record its child
        OA_ptr<MemRefExpr> mre = ref.getMemRefExpr();
        if (!mre.ptrEqual(0)) { 
            mre->acceptVisitor(*this); 
        }
    }
    void visitDeref(Deref& ref) { 
        // record self
        OA_ptr<MemRefExpr> mref = ref.clone();
        mManager.recordMRE(mref, mProc); 
        // must also visit child memory reference
        OA_ptr<MemRefExpr> mre = ref.getMemRefExpr();
        if (!mre.ptrEqual(0)) { 
            mre->acceptVisitor(*this); 
        }
    }
    // default handling of more specific SubSet specificiations
    void visitSubSetRef(SubSetRef& ref) {
        // record self
        OA_ptr<MemRefExpr> mref = ref.clone();
        mManager.recordMRE(mref, mProc); 
        
        // must also visit child memory reference
        OA_ptr<MemRefExpr> mre = ref.getMemRefExpr();
        if (!mre.ptrEqual(0)) { 
            mre->acceptVisitor(*this); 
        }
    }
  private:
    ManagerFIAlias& mManager;
    ProcHandle mProc;

};


void ManagerFIAlias::initMemRefExprs( ProcHandle currProc )
{
    // visitor that can record all mres and sub mres within an mre
    RecordMREsVisitor visitor(*this, currProc);

    // Iterate over the statements of this procedure
    OA_ptr<IRStmtIterator> stmtIterPtr = mIR->getStmtIterator(currProc);
    // Iterate over the statements of this block adding procedure references
    for ( ; stmtIterPtr->isValid(); ++(*stmtIterPtr)) {

        StmtHandle stmt = stmtIterPtr->current();
        if (debug) {
          std::cout << std::endl << "stmt = ";
          mIR->dump(stmt,std::cout);
        }

        // map each memory reference expr to a unique number in the range 1..
        OA_ptr<MemRefHandleIterator> mrIterPtr = mIR->getAllMemRefs(stmt);
        for (; mrIterPtr->isValid(); (*mrIterPtr)++ ) {
            
            MemRefHandle memref = mrIterPtr->current();

            // get the memory reference expressions for this handle
            OA_ptr<MemRefExprIterator> mreIterPtr 
                = mIR->getMemRefExprIterator(memref);
      
            // for each mem-ref-expr associated with this memref
            for (; mreIterPtr->isValid(); (*mreIterPtr)++) {
                OA_ptr<OA::MemRefExpr> mre = mreIterPtr->current();

                // associate this mre with the particular MemRefHandle
                recordMRE(mre, currProc, memref);

                // the recordMREs visitor will get all subrefs
                // and not associate them with a MemRefHandle

                mre->acceptVisitor(visitor);   
            }

        } // loop over MemRefHandles

        // there might be some MemRefExprs in PtrAssignPairs that are
        // not associated with a MemRefHandle
        // for each ptrassignpair
        OA_ptr<PtrAssignPairStmtIterator> pairIter
          = mIR->getPtrAssignStmtPairIterator(stmt);
        for ( ; pairIter->isValid(); (*pairIter)++ ) {
            OA_ptr<MemRefExpr> target = pairIter->currentTarget();        
            target->acceptVisitor(visitor);   

            OA_ptr<MemRefExpr> source = pairIter->currentSource();        
            source->acceptVisitor(visitor);   
        }

        // there can be some implicit MemRefExprs in ParamBindPtrAssigns
        // for all the callsites in the stmt
        OA_ptr<IRCallsiteIterator> callIter = mIR->getCallsites(stmt);
        for ( ; callIter->isValid(); (*callIter)++ ) {
            CallHandle call = callIter->current();

            // get the mre for the function call (eg. NamedRef('foo'))
            OA_ptr<MemRefExpr> callMRE = mIR->getCallMemRefExpr(call);
            callMRE->acceptVisitor(visitor);   

            // for each implicit ptrassignpair due to formal/actual binding
            OA_ptr<ParamBindPtrAssignIterator> pairIter
              = mIR->getParamBindPtrAssignIterator(call);
            for ( ; pairIter->isValid(); (*pairIter)++ ) {
                OA_ptr<MemRefExpr> actualMRE = pairIter->currentActual();        

                recordMRE(actualMRE, currProc);
                actualMRE->acceptVisitor(visitor);   
            }
        }


    } // loop over stmts

    // loop over formals for this procedure and recordMREs for each formal
    // also add this formal SymHandle to the set of formals for currProc
    int formalCount = 0;
    SymHandle formalSym;
    while ( (formalSym=mIR->getFormalSym(currProc,formalCount)) 
            != SymHandle(0) ) 
    {
        // put ian set of formals for currProc
        mProcToFormalSet[currProc].insert(formalSym);

        // make mre for the formal
        // Create a namedRef for the formal.
        bool addressTaken = false;
        bool fullAccuracy = true;
        // DEF since formal = actual.
        MemRefExpr::MemRefType mrType = MemRefExpr::DEF;
        OA_ptr<MemRefExpr> formalmre; 
        formalmre = new NamedRef(mrType, formalSym);

        // recordMRE for formal
        // update mMREToMemRefHandle, mMREToProc, and mMREToID
        // and updates mCount of all memory references
        
        formalmre->acceptVisitor(visitor);   

        // increment the count used within this loop
        formalCount++;
    }
}

void ManagerFIAlias::initMemRefExprs( OA_ptr<IRProcIterator> procIter )
{
    // for all the stmts in the whole program
    // Iterate over all the procedures in the program
    for (procIter->reset() ; procIter->isValid(); ++(*procIter)) { 
      ProcHandle currProc = procIter->current();
      initMemRefExprs(currProc);
    }

}

/*! An implementation of the merge function in the Ryder2001 description
 *  of FIAlias.
 */
void ManagerFIAlias::merge(int part1, int part2, 
        OA_ptr<UnionFindUniverse> ufset, 
        std::map<int,std::map<OA_ptr<MemRefExpr>,int> > & map  ) 
{
    int part1_find = ufset->Find(part1);
    int part2_find = ufset->Find(part2);
    if (part1_find != part2_find) {
        std::map<OA_ptr<MemRefExpr>,int> &old1 = map[part1_find];
        std::map<OA_ptr<MemRefExpr>,int> &old2 = map[part2_find];
        ufset->Union(part1_find, part2_find, part2_find);

        if (debug) {
            std::cout << "\t===> Union ( "
                      << part1_find << ", " << part2_find << " )" << std::endl;
        }

        // loop over all of the mres that are now in the newly unioned set
        // I think the fastest way to do this is to look over the mres
        // in the two old sets.  Otherwise I have to loop over all the MREs
        // and figure out which ones are in the new set.
        std::map<OA_ptr<MemRefExpr>,int>::iterator mapIter;
        part1_find = ufset->Find(part1);
        for (mapIter = old1.begin(); mapIter != old1.end(); mapIter++) {
            OA_ptr<MemRefExpr> mre = mapIter->first;
            if ( old1[mre]==0 && old2[mre]==0 ) {
                map[part1_find][mre] = 0;
            } else if (old2[mre]==0) {
                map[part1_find][mre] = old1[mre];
            } else if (old1[mre]==0) {
                map[part1_find][mre] = old2[mre];
            } else {
                merge(old1[mre], old2[mre], ufset, map);
            }
        }
        for (mapIter = old2.begin(); mapIter != old2.end(); mapIter++) {
            OA_ptr<MemRefExpr> mre = mapIter->first;
            if ( old1[mre]==0 && old2[mre]==0 ) {
                map[part1_find][mre] = 0;
            } else if (old2[mre]==0) {
                map[part1_find][mre] = old1[mre];
            } else if (old1[mre]==0) {
                map[part1_find][mre] = old2[mre];
            } else {
                merge(old1[mre], old2[mre], ufset, map);
            }
        }
    }

}

void ManagerFIAlias::outputMREsInSet(int setID, 
        OA_ptr<UnionFindUniverse> ufset, 
        std::map<int,std::map<OA_ptr<MemRefExpr>,int> > & map  ) 
{
  std::cout << "All mres in setID = " << setID << std::endl;
  std::map<OA_ptr<MemRefExpr>,int>::iterator mapIter;
  for (mapIter=mMREToID.begin(); mapIter!=mMREToID.end(); mapIter++) {
    if ( ufset->Find(mMREToID[mapIter->first]) == ufset->Find(setID) ) {
      OA_ptr<MemRefExpr> mymre = mapIter->first;
      mymre->output(*mIR);
    }
  }
}

  } // end of namespace Alias
} // end of namespace OA

