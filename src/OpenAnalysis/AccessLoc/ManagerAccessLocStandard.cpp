/*! \file
  
  \brief The AnnotationManager that generates AccessLoc

  \authors Michelle Strout
  \version $Id: ManagerAccessLocStandard.cpp,v 1.5 2005/03/17 22:23:50 ntallent Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include <OpenAnalysis/AccessLoc/ManagerAccessLocStandard.hpp>


namespace OA {
  namespace AccessLoc {

static bool debug = false;

/*!
*/
OA_ptr<AccessLoc::AccessLocStandard> 
ManagerAccessLocStandard::performAnalysis(ProcHandle proc) 
{
  if (debug) {
      std::cout << "In ManagerAccessLocStandard::performAnalysis:" << std::endl;
  }

  // Initialize EquivSets by creating an empty set that will contain
  // all memory locations but the locals
  OA_ptr<AccessLocStandard> retAccessLoc; 
  retAccessLoc = new AccessLocStandard(proc);

  // get all accessible symbols and their associated NamedLoc object
  // from the source IR
  OA_ptr<IRSymIterator> symIterPtr = mIR->getAccessibleSymIterator(proc);
  for (; symIterPtr->isValid(); (*symIterPtr)++) {
      SymHandle sym = symIterPtr->current();
      OA_ptr<Location> loc = mIR->getLocation(proc,sym);

      retAccessLoc->addLocation(loc);

      // if not local create invisible locations for all possible
      // dereferences of symbol
      if (!loc->isLocal()) {
        OA_ptr<MemRefExprIterator> mreIterPtr 
          = mIR->getPossMemRefExprIterator(sym);
        // create an invisible location for each of these that is a deref
        for ( ; mreIterPtr->isValid(); (*mreIterPtr)++ ) {
          if (mreIterPtr->current()->isaRefOp()) {
	    OA_ptr<MemRefExpr> mre = mreIterPtr->current();
	    OA_ptr<RefOp> ref = mre.convert<RefOp>();
            if (ref->isaDeref()) {
              OA_ptr<InvisibleLoc> invLoc;
              invLoc = new InvisibleLoc(mreIterPtr->current());
            }
          }
        }
      }
  }

  // iterate over symbols for formal parameters
  symIterPtr = mIR->getFormalParamIterator(mIR->getSymHandle(proc));
  for ( ; symIterPtr->isValid(); (*symIterPtr)++ ) {
      SymHandle sym = symIterPtr->current();

      // create invisible locations for all possible
      // dereferences of symbol for formal parameter
      OA_ptr<MemRefExprIterator> mreIterPtr 
        = mIR->getPossMemRefExprIterator(sym);
      // create an invisible location for each of these that is a deref
      for ( ; mreIterPtr->isValid(); (*mreIterPtr)++ ) {
        if (mreIterPtr->current()->isaRefOp()) {
	  OA_ptr<MemRefExpr> mre = mreIterPtr->current();
	  OA_ptr<RefOp> ref = mre.convert<RefOp>();
          if (ref->isaDeref()) {
            OA_ptr<InvisibleLoc> invLoc;
            invLoc = new InvisibleLoc(mreIterPtr->current());
          }
        }
      }
  }

  // get all unnamed locations
  OA_ptr<IRStmtIterator> stmtIterPtr = mIR->getUnnamedDynAllocStmtIterator(proc);
  for (; stmtIterPtr->isValid(); (*stmtIterPtr)++) {
      StmtHandle stmt = stmtIterPtr->current();

      // create a location datastructure for this guy
      // FIXME: for now all of these are specified as local,
      // if returning all malloc statements in whole program to
      // handle ptrs that isn't true
      OA_ptr<Location> loc;
      loc = new UnnamedLoc(stmt,true);
      retAccessLoc->addLocation(loc);
  }


  return retAccessLoc;

}

  } // end of namespace AccessLoc
} // end of namespace OA
