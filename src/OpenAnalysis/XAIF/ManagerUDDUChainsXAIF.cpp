/*! \file
  
  \brief The AnnotationManager that filters UDDUChains for XAIF.

  \authors Michelle Strout
  \version $Id: ManagerUDDUChainsXAIF.cpp,v 1.23 2005/01/18 21:13:16 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerUDDUChainsXAIF.hpp"


namespace OA {
  namespace XAIF {
    
    static bool debug = false;
    
    /**
     *
     */
    ManagerStandard::ManagerStandard(OA_ptr<XAIFIRInterface> _ir) : mIR(_ir) {
    }

    /**
     *  \brief    Used to perform analysis when not using AQM
     */
    OA_ptr<UDDUChainsXAIF> ManagerStandard::performAnalysis(ProcHandle proc, 
							    OA_ptr<CFG::Interface> cfg, 
							    OA_ptr<UDDUChains::Interface> udChains,
							    bool donotfilterBB) {
      OA_ptr<UDDUChainsXAIF> aUDDUChainsXAIF;
      aUDDUChainsXAIF = new UDDUChainsXAIF(proc);

      // making a mapping of Stmt's to basic block id's
      // FIXME: add functionality to CFG?
      // also mapping memory references to stmts
      // first assign StmtHandle(0) to entry node, which should have no
      // other statements in it
      mStmtToBBMap[StmtHandle(0)] = cfg->getEntry();
      if (debug) { std::cout << "mStmtToBBMap[StmtHandle(0)] = "
			     << mStmtToBBMap[StmtHandle(0)] << std::endl; }
      //std::set<MemRefHandle> topMemRefs;
  
      OA_ptr<CFG::Interface::NodesIterator> nodeIterPtr;
      nodeIterPtr = cfg->getNodesIterator();
      // looping over basic blocks
      for ( ;nodeIterPtr->isValid(); ++(*nodeIterPtr) ) {
	OA_ptr<CFG::Interface::Node> node = nodeIterPtr->current();
	OA_ptr<CFG::Interface::NodeStatementsIterator> stmtIterPtr 
	  = node->getNodeStatementsIterator();
	// looping over statements in basic blocks
	for (; stmtIterPtr->isValid(); ++(*stmtIterPtr)) {
	  OA::StmtHandle stmt = stmtIterPtr->current();
	  // mapping of statement to CFG nodes
	  mStmtToBBMap[stmt] = node;
	  if (debug) { std::cout << "mStmtToBBMap[stmt] = "
				 << mStmtToBBMap[stmt] << std::endl; }

	  // for all memory references in the statement
	  OA_ptr<MemRefHandleIterator> mrItPtr = mIR->getAllMemRefs(stmt);
	  for ( ; mrItPtr->isValid(); (*mrItPtr)++) {
	    if (debug) {
	      std::cout << "memref (" << mrItPtr->current().hval() 
			<< ") = " << mIR->toString(mrItPtr->current()) << std::endl;
	    }
	    mMemRefToStmt[mrItPtr->current()] = stmt;
	  }
	}
      }
      // for each UDChain starting from a use memory reference
      OA_ptr<UDDUChains::Interface::MemRefsWithUDChainIterator> useIterPtr; 
      useIterPtr = udChains->getMemRefsWithUDChainIterator();
      for (; useIterPtr->isValid(); ++(*useIterPtr)) {
	bool insertedOutsideDefPlaceHolder=false; 
	MemRefHandle use = useIterPtr->current();

	if (debug) {
	  std::cout << "use memref (" << use.hval() << ") = " << mIR->toString(use);
	  std::cout << std::endl;
	}
	// BB for use
	OA_ptr<CFG::Interface::Node> useBB = mStmtToBBMap[mMemRefToStmt[use]];
	// determine the subset of the def statements that are
	// within the same basic block, include the StmtHandle(0) if
	// a def statement is not within the same basic block
	// or comes after the use statement
	StmtSet subSet;
	OA_ptr<UDDUChains::Interface::ChainStmtIterator> defStmtIterPtr;
	defStmtIterPtr = udChains->getUDChainStmtIterator(use);
	for (; defStmtIterPtr->isValid(); (*defStmtIterPtr)++) {
	  StmtHandle defStmt = defStmtIterPtr->current();
	  if (debug) {
	    std::cout << "def stmt (" << defStmt.hval() << ") = " 
		      << mIR->toString(defStmt) << std::endl;
	    std::cout << "mStmtToBBMap[defStmt] = " << mStmtToBBMap[defStmt] 
		      << std::endl;
	    std::cout << "useBB = " << useBB << std::endl;
	  }
	  if (donotfilterBB) {
	    if (debug) { std::cout << "Stmts are not filtered" << std::endl; }
	    // iterate over all statements in this procedure, if hit def
	    // first then insert it, if hit use first then insert StmtHandle(0)
	    OA_ptr<OA::IRStmtIterator> stmtIterPtr = mIR->getStmtIterator(proc);
	    for ( ; stmtIterPtr->isValid(); (*stmtIterPtr)++) {
	      OA::StmtHandle stmt = stmtIterPtr->current();
	      if (!insertedOutsideDefPlaceHolder && stmt == mMemRefToStmt[use]) {
		if (debug) { std::cout << "Inserting StmtHandle(0)" << std::endl; }
		subSet.insert(StmtHandle(0));
		insertedOutsideDefPlaceHolder=true;
		break;
	      } 
	      if (stmt == defStmt) {
		if (debug) { std::cout << "Inserting def stmt" << std::endl; }
		subSet.insert(defStmt);
		break;
	      } 
	    }
	  }
	  else { // do filter 
	    // is in same basic block
	    if (mStmtToBBMap[defStmt]==useBB) {
	      if (debug) { std::cout << "Stmts are in same basic block" << std::endl; }
	      // iterate over statements in this basic block, if hit def
	      // first then insert it, if hit use first then insert StmtHandle(0)
	      OA_ptr<CFG::Interface::NodeStatementsIterator> stmtIterPtr 
		= useBB->getNodeStatementsIterator();
	      // looping over statements in basic blocks
	      for (; stmtIterPtr->isValid(); ++(*stmtIterPtr)) {
		OA::StmtHandle stmt = stmtIterPtr->current();
		if (!insertedOutsideDefPlaceHolder && stmt == mMemRefToStmt[use]) {
		  if (debug) { std::cout << "Inserting StmtHandle(0)" << std::endl; }
		  subSet.insert(StmtHandle(0));
		  break;
		} 
		if (stmt == defStmt) {
		  if (debug) { std::cout << "Inserting def stmt" << std::endl; }
		  subSet.insert(defStmt);
		  insertedOutsideDefPlaceHolder=true;
		  break;
		} 
	      }
	    }
	    else { // filtered and not in same basic block
	      if (debug) { std::cout << "Inserting StmtHandle(0)" << std::endl; }
	      subSet.insert(StmtHandle(0));
	    }
	  }
	} // end for def stmts
	// see if the subset is already in a UDDUChainsXAIF data structure
	int setId = aUDDUChainsXAIF->findSet(subSet);
	if (setId != UDDUChainsXAIF::SET_ID_NONE) {
	  // if it is then insert the use memory reference into that set
	  aUDDUChainsXAIF->insertInto(use, setId);
	} else {
	  // if it isn't then make a new set in the UDDUChainsXAIF 
	  // and add the subset of statements into that set
	  int newSetId = aUDDUChainsXAIF->makeEmptySet();
	  aUDDUChainsXAIF->insertInto(use,newSetId);
	  aUDDUChainsXAIF->addStmtSet(subSet,newSetId);
	}
      } // end for use stmts
      // for each DUChain starting from a def memory reference
      OA_ptr<UDDUChains::Interface::MemRefsWithDUChainIterator> defIterPtr; 
      defIterPtr = udChains->getMemRefsWithDUChainIterator();
      for (; defIterPtr->isValid(); ++(*defIterPtr)) {
	bool insertedOutsideUsePlaceHolder=false; 
	MemRefHandle def = defIterPtr->current();
	// BB for def
	OA_ptr<CFG::Interface::Node> defBB = mStmtToBBMap[mMemRefToStmt[def]];
	if (debug) {
	  std::cout << "defBB = " << defBB << std::endl;
	}
	// determine the subset of the use statements that are
	// within the same basic block, include the StmtHandle(0) if
	// a statement is not within the same basic block
	StmtSet subSet;
	OA_ptr<UDDUChains::Interface::ChainStmtIterator> useStmtIterPtr;
	useStmtIterPtr = udChains->getDUChainStmtIterator(def);
	for (; useStmtIterPtr->isValid(); (*useStmtIterPtr)++) {
	  StmtHandle useStmt = useStmtIterPtr->current();
	  if (debug) {
	    std::cout << "\tuseStmt (" << useStmt.hval() << ") = " 
		      << mIR->toString(useStmt) << ", BB = " 
		      << mStmtToBBMap[useStmt];
	    std::cout << std::endl;
	  }
	  if (donotfilterBB) { 
	    OA_ptr<OA::IRStmtIterator> stmtIterPtr = mIR->getStmtIterator(proc);
	    for ( ; stmtIterPtr->isValid(); (*stmtIterPtr)++) {
	      OA::StmtHandle stmt = stmtIterPtr->current();
	      if (!insertedOutsideUsePlaceHolder && stmt == useStmt) {
		subSet.insert(StmtHandle(0));
		insertedOutsideUsePlaceHolder=true; 
		break;
	      } 
	      if (stmt == mMemRefToStmt[def]) {
		subSet.insert(useStmt);
		break;
	      } 
	    }
	  } 
	  else { // filtered
	    if (mStmtToBBMap[useStmt]==defBB) { // in same basic block
	      // iterate over statements in this basic block, if hit def
	      // first then insert use, if hit use first then insert StmtHandle(0)
	      OA_ptr<CFG::Interface::NodeStatementsIterator> stmtIterPtr 
		= defBB->getNodeStatementsIterator();
	      // looping over statements in basic blocks
	      for (; stmtIterPtr->isValid(); ++(*stmtIterPtr)) {
		OA::StmtHandle stmt = stmtIterPtr->current();
		if (!insertedOutsideUsePlaceHolder && stmt == useStmt) {
		  subSet.insert(StmtHandle(0));
		  insertedOutsideUsePlaceHolder=true;
		  break;
		} 
		if (stmt == mMemRefToStmt[def]) {
		  subSet.insert(useStmt);
		  break;
		} 
	      }
	    } 
	    else { //  not in same basic block
	      subSet.insert(StmtHandle(0));
	    }
	  }
	} // use stmts
	// see if the subset is already in a UDDUChainsXAIF data structure
	int setId = aUDDUChainsXAIF->findSet(subSet);
	if (setId != UDDUChainsXAIF::SET_ID_NONE) {
	  // if it is then insert the use memory reference into that set
	  aUDDUChainsXAIF->insertInto(def, setId);
	} 
	else {
	  // if it isn't then make a new set in the UDDUChainsXAIF 
	  // and add the subset of statements into that set
	  int newSetId = aUDDUChainsXAIF->makeEmptySet();
	  aUDDUChainsXAIF->insertInto(def,newSetId);
	  aUDDUChainsXAIF->addStmtSet(subSet,newSetId);
	}
      } // def stmts
  
      // insert an empty set into set 0 because that is the default set in XAIF
      // don't insert it until the end because while building don't want to find
      // empty set at 0, want to find it at 1
      StmtSet emptySet;
      aUDDUChainsXAIF->addStmtSet(emptySet,0);
  
  
      return aUDDUChainsXAIF;
    }
    
  } // end of namespace XAIF
} // end of namespace OA
