/*! \file
  
  \brief The AnnotationManager that filters UDDUChains for XAIF.

  \authors Michelle Strout
  \version $Id: ManagerAliasMapXAIF.cpp,v 1.18 2005/03/17 21:47:47 mstrout Exp $

  Copyright (c) 2002-2005, Rice University <br>
  Copyright (c) 2004-2005, University of Chicago <br>
  Copyright (c) 2006, Contributors <br>
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "ManagerAliasMapXAIF.hpp"
#include <Utils/Util.hpp>

namespace OA {
  namespace XAIF {

bool debug = false;

/*!
*/
ManagerAliasMapXAIF::ManagerAliasMapXAIF(OA_ptr<XAIFIRInterface> _ir) : mIR(_ir)
{
    OA_DEBUG_CTRL_MACRO("DEBUG_ManagerAliasMapXAIF:ALL", debug);
}


/*!
*/
OA_ptr<XAIF::AliasMapXAIF> ManagerAliasMapXAIF::performAnalysis(ProcHandle proc, 
    OA_ptr<Alias::Interface> alias)
{
  OA_ptr<AliasMapXAIF> retAliasMap; retAliasMap = new AliasMapXAIF(proc);

  if(debug) {
     std::cout << "The Procedure is" << mIR->toString(proc) << std::endl;
  }

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
  std::map<ExprHandle,LocSet> exprToLocSetMap;
  
  // for each stmt
  OA_ptr<OA::IRStmtIterator> sItPtr = mIR->getStmtIterator(proc);
  for ( ; sItPtr->isValid(); (*sItPtr)++) {
    OA::StmtHandle stmt = sItPtr->current();
    if (debug) {
        std::cout << "===========================" << std::endl << "stmt = ";
        mIR->dump(stmt,std::cout);
        std::cout << std::endl;
    }

    // get all memory references
    OA_ptr<MemRefHandleIterator> mrItPtr = mIR->getAllMemRefs(stmt);
    for ( ; mrItPtr->isValid(); (*mrItPtr)++) {
        MemRefHandle memref = mrItPtr->current();
        if (debug) {
            std::cout << "memref = " << mIR->toString(memref) << std::endl;
        }

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
            ExprHandle expr = unnamedLoc->getExprHandle();
            exprToLocSetMap[expr].insert(unnamedLoc.convert<Location>());
          
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
                ExprHandle expr = unnamedLoc->getExprHandle();
                exprToLocSetMap[expr].insert(loc);
                
              } else if (base->isaInvisible()) {
                OA_ptr<InvisibleLoc> invLoc
                  = base.convert<InvisibleLoc>();
                OA_ptr<MemRefExpr> mre = invLoc->getMemRefExpr();
                assert(mre->isaRefOp());
                OA_ptr<RefOp> refOp = mre.convert<RefOp>();
                symToLocSetMap[refOp->getBaseSym()].insert(loc);
              }

          // UnknownLoc
          } else if (loc->isaUnknown()) {
              // do nothing
          }
        }
    }
  }
              
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
                if (partialFlag==false) { // first time we see this
                  count += 2;  // 2 spaces for all partial references to a loc
                }
                partialFlag = true;
          } else if(loc->isaNamed()) {
                count++;
          } else if(loc->isaUnnamed()) { 
                count++;
          } else if(loc->isaUnknown()) { 
                count++;
          } else if(loc->isaInvisible()) { 
              OA_ptr<InvisibleLoc> invisibleLoc
                  = loc.convert<InvisibleLoc>();
              OA_ptr<MemRefExpr> mre = invisibleLoc->getMemRefExpr();
              assert(mre->isaRefOp());
              OA_ptr<RefOp> refOp = mre.convert<RefOp>();
              if(refOp->isaSubSetRef()) {
                 count += 2;
              } else {
                 count++;
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

            locToLocTuple[loc] = LocTuple(uniqueId,uniqueId+count-1,false);
          } else {
            locToLocTuple[loc] = LocTuple(uniqueId,uniqueId+count-1,true);
          }
      }
     
      // must increase uniqueId by at least one
      uniqueId += count;

  }


  std::map<ExprHandle,LocSet>::iterator exprMapIter;
  for (exprMapIter=exprToLocSetMap.begin();
       exprMapIter!=exprToLocSetMap.end(); exprMapIter++ )
  {
      // within each location set count the number of LocSubSets
      // that are partial but have full accuracy, that is the
      // number of virtual location ids that will be needed
      // if have any partial accuracy sub set locations then
      // will add in two virtual location ids
      int count = 0;
      LocSet::iterator locIter;
      LocSet locSet;
      locSet= exprMapIter->second;

      // now loop over all locations in this list and map to a LocTuple
      for (locIter=locSet.begin(); locIter!=locSet.end(); locIter++) {
          OA_ptr<Location> loc = *locIter;
          if (loc->isaUnnamed()) {
              count+=2;
              locToLocTuple[loc] = LocTuple(uniqueId,uniqueId+count-1,false);
          } else {
              assert(0);
          }
      }

      uniqueId += count;
  }

  //---------------------------------
  // Then iterate over the aliasing information again using the
  // newly determined LocTuples 
  
  // get all memory references that alias analysis has info for
  if (debug) {
      std::cout << "==== MemRefHandles that alias analysis has info for" 
                << std::endl;
  }
  OA_ptr<MemRefHandleIterator> mrItPtr = alias->getMemRefIter();
  for ( ; mrItPtr->isValid(); (*mrItPtr)++) {
      MemRefHandle memref = mrItPtr->current();
        if (debug) {
            std::cout << "memref = " << mIR->toString(memref) << std::endl;
        }

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
       
      // if the set is empty then do not want to make an alias map
      // because this is an addressOf MemRef and does not access a location
      } else if (maySet->empty()) {
          continue;

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

  return retAliasMap;
}

  } // end of namespace XAIF
} // end of namespace OA
