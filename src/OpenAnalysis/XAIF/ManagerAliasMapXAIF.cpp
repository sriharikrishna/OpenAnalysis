/*! \file
  
  \brief The AnnotationManager that filters UDDUChains for XAIF.

  \authors Michelle Strout
  \version $Id: ManagerAliasMapXAIF.cpp,v 1.18 2005/03/17 21:47:47 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerAliasMapXAIF.hpp"

namespace OA {
  namespace XAIF {

bool debug = false;

/*!
*/
ManagerAliasMapXAIF::ManagerAliasMapXAIF(OA_ptr<XAIFIRInterface> _ir) : mIR(_ir)
{
}


/*!
*/
OA_ptr<XAIF::AliasMapXAIF> ManagerAliasMapXAIF::performAnalysis(ProcHandle proc, 
    OA_ptr<Alias::Interface> alias)
{
  OA_ptr<AliasMapXAIF> retAliasMap; retAliasMap = new AliasMapXAIF(proc);
  
  //---------------------------------
  // First iterate over all locations and collect the locations
  // into groups based on their base symbol or stmt.
  // Therefore all locations that are statically equivalenced will result
  // in only one entry and all locations that are partial or full subsets
  // of a datastructure like an array will be in the same set.
  // The sets will be indexed by SymHandle or StmtHandle

  // named locations indexed by symbol and invisible locations indexed 
  // by base location symbol
  std::map<SymHandle,LocSet> symToLocSetMap;
  // unnamed locations indexed by StmtHandle
  std::map<StmtHandle,LocSet> stmtToLocSetMap;
  
  // for each stmt
  OA_ptr<OA::IRStmtIterator> sItPtr = mIR->getStmtIterator(proc);
  for ( ; sItPtr->isValid(); (*sItPtr)++) {
    OA::StmtHandle stmt = sItPtr->current();

    // get all memory references
    OA_ptr<MemRefHandleIterator> mrItPtr = mIR->getAllMemRefs(stmt);
    for ( ; mrItPtr->isValid(); (*mrItPtr)++) {
        MemRefHandle memref = mrItPtr->current();

        // get locations and insert into appropriate set
        OA_ptr<LocIterator> mayIterPtr = alias->getMayLocs(memref);
        for (; mayIterPtr->isValid(); (*mayIterPtr)++) {
          OA_ptr<Location> loc = mayIterPtr->current();
          
          // NamedLoc
          if (loc->isaNamed()) {
            OA_ptr<NamedLoc> namedLoc 
                = loc.convert<NamedLoc>();
            SymHandle sym = namedLoc->getSymHandle();
            symToLocSetMap[sym].insert(namedLoc.convert<Location>());
          
          // UnnamedLoc
          } else if (loc->isaUnnamed()) {
            OA_ptr<UnnamedLoc> unnamedLoc 
                = loc.convert<UnnamedLoc>();
            StmtHandle stmt = unnamedLoc->getStmtHandle();
            stmtToLocSetMap[stmt].insert(unnamedLoc.convert<Location>());
          
          // Invisible Loc
          } else if (loc->isaInvisible()) {
            OA_ptr<InvisibleLoc> invLoc 
                = loc.convert<InvisibleLoc>();
            OA_ptr<MemRefExpr> mre = invLoc->getMemRefExpr();
            assert(mre->isaRefOp());
            OA_ptr<RefOp> refOp = mre.convert<RefOp>();
            symToLocSetMap[refOp->getBaseSym()].insert(loc);

          // LocSubSets
          } else if (loc->isaSubSet()) {
              // get base location from locsubset
              OA_ptr<Location> base = loc->getBaseLoc();

              // NamedLoc
              // FIXME: refactor with above
              if (base->isaNamed()) {
                OA_ptr<NamedLoc> namedLoc
                    = base.convert<NamedLoc>();
                SymHandle sym = namedLoc->getSymHandle();
                symToLocSetMap[sym].insert(loc);
                  
              // UnnamedLoc
              } else if (base->isaUnnamed()) {
                OA_ptr<UnnamedLoc> unnamedLoc
                    = base.convert<UnnamedLoc>();
                StmtHandle stmt = unnamedLoc->getStmtHandle();
                stmtToLocSetMap[stmt].insert(loc);
              } 

          // UnknownLoc
          } else if (loc->isaUnknown()) {
              // do nothing
          }
        }
    }
  }
              
  //std::map<SymHandle,std::set<OA_ptr<Location> > symToLocSetMap;
  //std::map<StmtHandle,std::set<OA_ptr<Location> > stmtToLocSetMap;
 
  //---------------------------------
  // Create a location tuple for all locations based on the set they
  // are a part of and by maintaining a unique id.
  int uniqueId = 1;

  std::map<OA_ptr<Location>,LocTuple> locToLocTuple;

  std::map<SymHandle,LocSet>::iterator symMapIter;
  for (symMapIter=symToLocSetMap.begin();
       symMapIter!=symToLocSetMap.end(); symMapIter++ )
  {
      // within each location set count the number of LocSubSets
      // that are partial but have full accuracy, that is the 
      // number of virtual location ids that will be needed
      // if have any partial accuracy sub set locations then
      // will add in two virtual location ids
      int count = 0;
      bool partialFlag = false;
      LocSet::iterator locIter;
      LocSet locSet;
      locSet= symMapIter->second;
      for (locIter=locSet.begin(); locIter!=locSet.end(); locIter++) {
          OA_ptr<Location> loc = *locIter;
          if (debug) { std::cout << "loc = "; loc->dump(std::cout,mIR); }
          if (loc->isaSubSet()) {
              OA_ptr<LocSubSet> subLoc 
                  = loc.convert<LocSubSet>();
              // if had two instances of A[5] for example would only
              // be one in location set because they would be equiv
              if (!subLoc->isFull() && subLoc->hasFullAccuracy()) {
                if (debug) { std::cout << "then" << std::endl; }
                count++;

              // if is an inaccurate partial then set partialFlag
              } else {
                if (debug) { std::cout << "else" << std::endl; }
                if (partialFlag==false) { // first time we see this
                  count += 2;  // 2 spaces for all partial references to a loc
                }
                partialFlag = true;
              }
          
          }
      }
      if (count == 0) { count = 1; }
      if (debug) { std::cout << "count = " << count << std::endl; }
      
      // if there were partials then start currentOffset 2 in
      int currentSubOffset = 0;
      if (partialFlag==true) {
          currentSubOffset = 2;
       } 

      // now loop over all locations in this list and map to a LocTuple
      for (locIter=locSet.begin(); locIter!=locSet.end(); locIter++) {
          OA_ptr<Location> loc = *locIter;
          if (loc->isaSubSet()) {
            OA_ptr<LocSubSet> subLoc 
                = loc.convert<LocSubSet>();

            if (subLoc->hasFullAccuracy() && subLoc->isFull()) {
              locToLocTuple[loc] = LocTuple(uniqueId,uniqueId+count-1,true);
            } else if (subLoc->hasFullAccuracy() && !subLoc->isFull()) {
              locToLocTuple[loc] = LocTuple(uniqueId+currentSubOffset,
                                             uniqueId+currentSubOffset,true);
              currentSubOffset++;
            } else {
              locToLocTuple[loc] = LocTuple(uniqueId,uniqueId+count-1,false);
            }

          // location will get whole range and full or partial based on accuracy
          } else {
              if (loc->hasFullAccuracy() ) {
                locToLocTuple[loc] = LocTuple(uniqueId,uniqueId+count-1,true);
              } else {
                locToLocTuple[loc] = LocTuple(uniqueId,uniqueId+count-1,false);
              }
          }
      }
     
      // must increase uniqueId by at least one
      //if (count > 0) {
        uniqueId += count;
      //} else {
      //  uniqueId++;
     // }

  }


  //---------------------------------
  // Then iterate over the aliasing information again using the
  // newly determined LocTuples 
  // for each stmt
  sItPtr = mIR->getStmtIterator(proc);
  for ( ; sItPtr->isValid(); (*sItPtr)++) {
    OA::StmtHandle stmt = sItPtr->current();

    // get all memory references
    OA_ptr<MemRefHandleIterator> mrItPtr = mIR->getAllMemRefs(stmt);
    for ( ; mrItPtr->isValid(); (*mrItPtr)++) {
        MemRefHandle memref = mrItPtr->current();

        // trying to determine what set we wil map this memref to
        int setId = Alias::AliasMap::SET_ID_NONE;

        OA_ptr<LocIterator> mayIterPtr = alias->getMayLocs(memref);
        OA_ptr<std::set<LocTuple> > maySet;
	maySet = new std::set<LocTuple>;
        bool foundUnknown = false;
        for (; mayIterPtr->isValid(); (*mayIterPtr)++) {
            OA_ptr<Location> loc = mayIterPtr->current();
            if (loc->isaUnknown()) {
                foundUnknown = true;
            } else {
                maySet->insert(locToLocTuple[loc]);
            }
        } 

        // if contains the UnknownLoc just map to special 0 alias map set
        if (foundUnknown) {
            setId = 0;
        
        // otherwise need to figure out different setId
        } else {
        
            // attempt to find equivalent set in already existing alias map sets
            // if can then map this memory reference to the specificed alias map set
            setId = retAliasMap->getMapSetId(maySet);
        
            // if can't then create a new one and map it to a new one
            if (setId==Alias::AliasMap::SET_ID_NONE) {
                setId = retAliasMap->makeEmptySet();

                // insert all locations into this new set
                retAliasMap->mapLocTupleSet(maySet,setId);
            }

        }
        // map top memrefhandle to determined alias map set
        retAliasMap->mapMemRefToMapSet(memref,setId);
    }

  }
  return retAliasMap;
}

  } // end of namespace XAIF
} // end of namespace OA
