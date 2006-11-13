/*! \file
  
  \brief Specially formatted and filtered UDChains for XAIF.
  
  
  \authors Michelle Strout
  \version $Id: UDDUChainsXAIF.hpp,v 1.14 2005/01/17 18:46:34 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  Special things about UDDUChainsXAIF:
  - If a chain element (StmtHandle) crosses basic block boundaries
    then the special StmtHandle(0) is used instead.  This includes crossing
    a BB for a loop.  For example, if a possible def for a use comes after
    the use in the BB, then that def will show up as StmtHandle(0) in
    the ud-chain.
    Also, we assume that definitions to non-local vars have an out-of-scope use.
  - The zeroth UDDUChain is not used, it is assumed to mean there is no
    information about a memory reference.
  - The first UDDUChain is an empty chain.
  - The second UDDUChains includes the StmtHandle(0) by itself.
  - All other chains are unique across procedures.


*/

#ifndef UDDUChainsXAIF_hpp
#define UDDUChainsXAIF_hpp

#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/Utils/SetIterator.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/UDDUChains/Interface.hpp>

namespace OA {

  //! Namespace for analysis results that are specific to XAIF and do 
  //! not satisfy general interfaces.
  namespace XAIF {

typedef std::set<StmtHandle> StmtSet;



/*!
  UD chains for XAIF map each MemRefHandle X to the set of 
  StmtHandle's that may include a define for X as long as the StmtHandle
  is within the same basic block, otherwise the StmtHandle(0) is specified.
  DU chains map each MemRefHandle Y to the set of StmtHandle's that
  may include a use of Y.  Again this is for statements within the same
  basic block.
*/
class UDDUChainsXAIF {
  public:
    UDDUChainsXAIF(ProcHandle p);
    ~UDDUChainsXAIF() {}

    static const int SET_ID_NONE = -1; 

    class ChainStmtIterator;
    class ChainIterator;
    class MemRefsWithChainIterator; 

    //! Return an iterator over all statements in this procedure
    //! that may define a location used by the given memory reference
    //! Assumes that the given memory reference is a use
    OA_ptr<ChainStmtIterator> getUDChainStmtIterator(MemRefHandle h);

    //! Return an iterator over all statements in this procedure
    //! that may use a location defined by the given memory reference
    //! Assumes that the given memory reference is a define
    OA_ptr<ChainStmtIterator> getDUChainStmtIterator(MemRefHandle h);

    //! Return an iterator over all the memory references for which
    //! we have UD or DU Chain information
    OA_ptr<MemRefsWithChainIterator> getMemRefsWithChainIterator();

    //! get the UDDUChain Id for a particular memory reference
    //! the zeroth chain will always be an empty chain
    //! this method will always return a valid ChainID if the given
    //! ref was analyzed
    int getUDDUChainId(MemRefHandle ref);

    //! get an iterator over all chains known about
    OA_ptr<ChainIterator> getChainIterator();
    
    //! for a particular UDDUChainId, get an iterator over the statements
    //! involved in that chain
    OA_ptr<ChainStmtIterator> getUDDUChainSetIterator(int id);

    //*****************************************************************
    // Construction methods 
    //*****************************************************************

    //! set the starting id for the sets
    //void setStartId(int s) { mStartId = s; }

    //! get the next id that this guy will use if it needs to make
    //! another chain
    //int getNextId() { return mStartId + mNumSets; }

    //! create a new UDDUChains set and return the id
    int makeEmptySet();
 
    //! add a statement into the possible uses or defs for a UDDUChains set
    void addStmt(StmtHandle stmt, int setId);

    //! add the given stmt set into the UDDUChains set
    void addStmtSet(StmtSet& stmtSet, int setId);

    //! insert the given mem ref into the UDDUChains set
    void insertInto(MemRefHandle ref, int setId);

    //! return UDDUChains set Id that has identical set of Stmts
    //! in it, if no set has this set of stmts then return SET_ID_NONE
    int findSet(StmtSet& stmtSet);

    //*****************************************************************
    // Output
    //*****************************************************************
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);

  private:
    // data members for UDDUChainsXAIF
    //int mNumSets; // number of sets other than zeroth and first set
    //int mStartId;
    static int mCurrentStartId;  // used to keep unique set ids across
                                 // all UDDUChainsXAIF instances
    OA_ptr<std::map<int,OA_ptr<StmtSet> > > mIdToStmtSetMap;
    OA_ptr<std::map<MemRefHandle,int> > mMemRefToIdMap;

  public:
    //*****************************************************************
    // Iterators
    //*****************************************************************

    /*! Inherits implementation from SetIterator<StmtHandle>
    */
    class ChainStmtIterator : public SetIterator<StmtHandle>
    {
      public:
        ChainStmtIterator(OA_ptr<std::set<StmtHandle> > pSet) 
            : SetIterator<StmtHandle>(pSet) {}
        ~ChainStmtIterator() {}
    };


    //! iterator over UDDUChains, each chain has an Id and then a Chain
    //! associated with it
    class ChainIterator {
      public:
        ChainIterator(OA_ptr<std::map<int,OA_ptr<StmtSet> > > pChainMap) 
            : mChainMap(pChainMap) { reset(); }
        ~ChainIterator() { }

        OA_ptr<ChainStmtIterator> currentChainStmtIterator()
          { OA_ptr<ChainStmtIterator> retval;
            retval = new ChainStmtIterator((*mChainMap)[mIter->first]);
            return retval;
          }

        int currentId() { return mIter->first; }
        bool isValid() { return (mIter != mChainMap->end()); }
        
        void operator++() { if (isValid()) mIter++; }
        void operator++(int) { ++*this; }

        void reset() { mIter = mChainMap->begin(); }

      private:
        OA_ptr<std::map<int,OA_ptr<StmtSet> > > mChainMap;
        std::map<int,OA_ptr<StmtSet> >::iterator mIter;
    };

    class MemRefsWithChainIterator {
      public:
        MemRefsWithChainIterator(OA_ptr<std::map<MemRefHandle,int> > pMap) 
            : mMap(pMap), mIter(pMap->begin()) { }
        ~MemRefsWithChainIterator() { }

        MemRefHandle current() { return mIter->first; }
        bool isValid() { return (mIter != mMap->end()); }
        void operator++() { if (isValid()) mIter++; }
        void operator++(int) { ++*this; }
        void reset() { mIter = mMap->begin(); }

      private:
        OA_ptr<std::map<MemRefHandle,int> > mMap;
        std::map<MemRefHandle,int>::iterator mIter;
    };



};

  } // end of XAIF namespace
} // end of OA namespace

#endif

