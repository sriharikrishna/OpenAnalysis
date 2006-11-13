/*! \file
  
  \brief Implementation of UDDUChains::UDDUChainsStandard

  \author Michelle Strout
  \version $Id: UDDUChainsStandard.cpp,v 1.10 2004/12/23 07:37:53 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "UDDUChainsStandard.hpp"

namespace OA {
  namespace UDDUChains {

//! Return an iterator over all statements in this procedure
//! that may define a location used in the given statement
OA_ptr<Interface::ChainStmtIterator> 
UDDUChainsStandard::getUDChainStmtIterator(StmtHandle s)
{ 
  OA_ptr<ChainStmtIterator> retval;

  // need to create an empty StmtSet if we haven't seen this stmt before
  if ((*mUDChainForStmt)[s].ptrEqual(0)) {
    (*mUDChainForStmt)[s] = new StmtSet;
  }
  retval = new ChainStmtIterator((*mUDChainForStmt)[s]);
  return retval;
}

//! Return an iterator over all statements for which UDChain info
//! is available
OA_ptr<Interface::StmtsWithUDChainIterator> 
UDDUChainsStandard::getStmtsWithUDChainIterator()
{
    OA_ptr<StmtsWithUDChainIterator> retval;
    retval = new StmtsWithUDChainIterator(mUDChainForStmt);
    return retval;
}

//! Return an iterator over all statements in this procedure
//! that may use a location defined in the given statement
OA_ptr<Interface::ChainStmtIterator> 
UDDUChainsStandard::getDUChainStmtIterator(StmtHandle s)
{ 
  OA_ptr<ChainStmtIterator> retval;

  // need to create an empty StmtSet if we haven't seen this stmt before
  if ((*mDUChainForStmt)[s].ptrEqual(0)) {
    (*mDUChainForStmt)[s] = new StmtSet;
  }
  retval = new ChainStmtIterator((*mDUChainForStmt)[s]);
  return retval;
}

//! Return an iterator over all statements for which UDChain info
//! is available
OA_ptr<Interface::StmtsWithDUChainIterator> 
UDDUChainsStandard::getStmtsWithDUChainIterator()
{
    OA_ptr<StmtsWithDUChainIterator> retval;
    retval = new StmtsWithDUChainIterator(mDUChainForStmt);
    return retval;
}


//! Return an iterator over all statements in this procedure
//! that may define a location used by the given memory reference
OA_ptr<Interface::ChainStmtIterator> 
UDDUChainsStandard::getUDChainStmtIterator(MemRefHandle h)
{ 
  OA_ptr<ChainStmtIterator> retval;

  // need to create an empty StmtSet if we haven't seen this memref before
  if ((*mUDChainForMemRef)[h].ptrEqual(0)) {
    (*mUDChainForMemRef)[h] = new StmtSet;
  }
  retval = new ChainStmtIterator((*mUDChainForMemRef)[h]);
  return retval;
}

//! Return an iterator over all memory references for which UDChain info
//! is available
OA_ptr<Interface::MemRefsWithUDChainIterator> 
UDDUChainsStandard::getMemRefsWithUDChainIterator()
{
    OA_ptr<MemRefsWithUDChainIterator> retval;
    retval = new MemRefsWithUDChainIterator(mUDChainForMemRef);
    return retval;
}

//! Return an iterator over all statements in this procedure
//! that may use a location defined by the given memory reference
OA_ptr<Interface::ChainStmtIterator> 
UDDUChainsStandard::getDUChainStmtIterator(MemRefHandle h)
{ 
  OA_ptr<ChainStmtIterator> retval;

  // need to create an empty StmtSet if we haven't seen this memref before
  if ((*mDUChainForMemRef)[h].ptrEqual(0)) {
    (*mDUChainForMemRef)[h] = new StmtSet;
  }
  retval = new ChainStmtIterator((*mDUChainForMemRef)[h]);
  return retval;
}

//! Return an iterator over all memrefs for which UDChain info
//! is available
OA_ptr<Interface::MemRefsWithDUChainIterator> 
UDDUChainsStandard::getMemRefsWithDUChainIterator()
{
    OA_ptr<MemRefsWithDUChainIterator> retval;
    retval = new MemRefsWithDUChainIterator(mDUChainForMemRef);
    return retval;
}

//*****************************************************************
// Construction methods 
//*****************************************************************

//! insert a define use relationship
void UDDUChainsStandard::insertDefUse(StmtHandle def, StmtHandle use)
{ 
    if ((*mDUChainForStmt)[def].ptrEqual(0)) {
      (*mDUChainForStmt)[def] = new StmtSet;
    }
    (*mDUChainForStmt)[def]->insert(use); 

    if ((*mUDChainForStmt)[use].ptrEqual(0)) {
      (*mUDChainForStmt)[use] = new StmtSet;
    }
    (*mUDChainForStmt)[use]->insert(def); 
}

//! insert a memref, the memref may end up having an empty
//! chains so must make sure chain is initialized
void UDDUChainsStandard::insertMemRefDef(MemRefHandle ref)
{ 
    if ((*mDUChainForMemRef)[ref].ptrEqual(0)) {
      (*mDUChainForMemRef)[ref] = new StmtSet;
    }
}
void UDDUChainsStandard::insertMemRefUse(MemRefHandle ref)
{ 
    if ((*mUDChainForMemRef)[ref].ptrEqual(0)) {
      (*mUDChainForMemRef)[ref] = new StmtSet;
    }
}


//! insert a define use relationship with the specific def
//! memory reference
void UDDUChainsStandard::insertMemRefDefStmtUse(MemRefHandle def, StmtHandle use)
{ 
    if ((*mDUChainForMemRef)[def].ptrEqual(0)) {
      (*mDUChainForMemRef)[def] = new StmtSet;
    }
    (*mDUChainForMemRef)[def]->insert(use); 
}

//! insert a define use relationship with the specific use
//! memory reference
void UDDUChainsStandard::insertStmtDefMemRefUse(StmtHandle def, MemRefHandle use)
{ 
    if ((*mUDChainForMemRef)[use].ptrEqual(0)) {
      (*mUDChainForMemRef)[use] = new StmtSet;
    }
    (*mUDChainForMemRef)[use]->insert(def); 
}

//*****************************************************************
// Output
//*****************************************************************

//! incomplete output of info for debugging
void UDDUChainsStandard::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    os << "============= UDDUChains ============" << std::endl;
    // first iterate over all statements that have a UDChain associated
    // with them
    OA_ptr<Interface::StmtsWithUDChainIterator> useStmtIterPtr
        = getStmtsWithUDChainIterator(); 
    for ( ; useStmtIterPtr->isValid(); (*useStmtIterPtr)++ )
    {
        StmtHandle useStmt = useStmtIterPtr->current();

        os << "Use StmtHandle(" << useStmt.hval() << ") " << ir->toString(useStmt) 
           << std::endl << "\tDefine Statements: " << std::endl;
        // iterate over reaching defs for statement useStmt
        OA_ptr<Interface::ChainStmtIterator> udChainPtr
            = getUDChainStmtIterator(useStmt);
        for ( ; udChainPtr->isValid(); (*udChainPtr)++) {
            StmtHandle s = udChainPtr->current();
            if (s.hval()==0) {
              os << "\tStmtHandle(0)" << std::endl;
            } else {
              os << "\t" << ir->toString(s) << std::endl;
            }
        }

    }

    // now iterate over all statements that have a DUChain associated
    // with them
    OA_ptr<Interface::StmtsWithDUChainIterator> defStmtIterPtr
        = getStmtsWithDUChainIterator(); 
    for ( ; defStmtIterPtr->isValid(); (*defStmtIterPtr)++ )
    {
        StmtHandle defStmt = defStmtIterPtr->current();

        os << "Def StmtHandle(" << defStmt.hval() << ") " << ir->toString(defStmt) 
           << std::endl << "\tUse Statements: " << std::endl;
        // iterate over uses for statement defStmt
        OA_ptr<Interface::ChainStmtIterator> duChainPtr
            = getDUChainStmtIterator(defStmt);
        for ( ; duChainPtr->isValid(); (*duChainPtr)++) {
            os << "\t" << ir->toString(duChainPtr->current()) << std::endl;
        }

    }

    // use memrefs that have UDChains
    OA_ptr<Interface::MemRefsWithUDChainIterator> useIterPtr
        = getMemRefsWithUDChainIterator();
    for ( ; useIterPtr->isValid(); (*useIterPtr)++ ) {
        MemRefHandle use = useIterPtr->current();

        os << "Use MemRefHandle(" << use.hval() << ") " << ir->toString(use) 
           << std::endl << "\tDefine Statements: " << std::endl;
        // iterate over defs for memref use
        OA_ptr<Interface::ChainStmtIterator> udChainPtr
            = getUDChainStmtIterator(use);
        for ( ; udChainPtr->isValid(); (*udChainPtr)++) {
            StmtHandle s = udChainPtr->current();
            if (s.hval()==0) {
              os << "\tStmtHandle(0)" << std::endl;
            } else {
              os << "\t" << ir->toString(s) << std::endl;
            }
        }

    }

    // def memrefs that have DUChains
    OA_ptr<Interface::MemRefsWithUDChainIterator> defIterPtr
        = getMemRefsWithDUChainIterator();
    for ( ; defIterPtr->isValid(); (*defIterPtr)++ ) {
        MemRefHandle def = defIterPtr->current();

        os << "Def MemRefHandle(" << def.hval() << ") " << ir->toString(def) 
           << std::endl << "\tUse Statements: " << std::endl;
        // iterate over uses for memref def
        OA_ptr<Interface::ChainStmtIterator> duChainPtr
            = getDUChainStmtIterator(def);
        for ( ; duChainPtr->isValid(); (*duChainPtr)++) {
            os << "\t" << ir->toString(duChainPtr->current()) << std::endl;
        }

    }


}



  } // end of UDDUChains namespace
} // end of OA namespace
