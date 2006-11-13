/*! \file
  
  \brief Implementation of ReachDefs::ReachDefsStandard

  \author Michelle Strout
  \version $Id: ReachDefsStandard.cpp,v 1.8 2004/12/21 21:15:19 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ReachDefsStandard.hpp"

namespace OA {
  namespace ReachDefs {

//! Return an iterator that will iterate over the set of reaching
//! defs for the stmt
OA_ptr<Interface::ReachDefsIterator> 
ReachDefsStandard::getReachDefsIterator(StmtHandle s) 
{ 
    OA_ptr<ReachDefsStandardReachDefsIterator> retval;

    if (mReachDefs[s].ptrEqual(0)) {
        mReachDefs[s] = new std::set<StmtHandle>;
    }
    retval = new ReachDefsStandardReachDefsIterator(mReachDefs[s]);
    return retval;
}

//! Return an iterator over reaching definitions that reach the exit node
//! in the procedure
OA_ptr<Interface::ReachDefsIterator> 
ReachDefsStandard::getExitReachDefsIterator() 
{ 
    OA_ptr<ReachDefsStandardReachDefsIterator> retval;
    retval = new ReachDefsStandardReachDefsIterator(mExitReachDefs);
    return retval;
}


//! incomplete output of info for debugging, just lists stmts
//! and associated set of stmts that are reaching defs for the given
void ReachDefsStandard::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    std::map<StmtHandle,OA_ptr<std::set<StmtHandle> > >::iterator mapIter;
    for (mapIter = mReachDefs.begin(); mapIter != mReachDefs.end(); mapIter++) {
        StmtHandle s = mapIter->first;

        os << "StmtHandle(" << s.hval() << ") " << ir->toString(s) 
           << std::endl << "\treachdefs: " << std::endl;
        // iterate over reaching defs for statement s
        OA_ptr<Interface::ReachDefsIterator> setIterPtr
            = getReachDefsIterator(s);
        for (; setIterPtr->isValid(); (*setIterPtr)++) {
            os << "\t" << ir->toString(setIterPtr->current()) << std::endl;
        }

    }
}



  } // end of ReachDefs namespace
} // end of OA namespace
