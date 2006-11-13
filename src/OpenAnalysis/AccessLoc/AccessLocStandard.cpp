/*! \file
  
  \brief Implementation of AccessLoc::AccessLocStandard

  \author Michelle Strout
  \version $Id: AccessLocStandard.cpp,v 1.3 2005/03/17 21:47:44 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include <OpenAnalysis/AccessLoc/AccessLocStandard.hpp>

namespace OA {
  namespace AccessLoc {
static bool debug = false;

AccessLocStandard::AccessLocStandard(ProcHandle p) : mProcHandle(p)
{
    mLocSet = new LocSet;
}

OA_ptr<LocIterator> AccessLocStandard::getAccessibleLocIterator()
{
    OA_ptr<LocIterator> retval;
    retval = new LocSetIterator(mLocSet);
    return retval;
}

void AccessLocStandard::addLocation(OA_ptr<Location> loc)
{
    mLocSet->insert(loc);
}

//! incomplete output of info for debugging 
void AccessLocStandard::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    // print locations 
    os << "============= AccessLocStandard ============" << std::endl;
    OA_ptr<LocIterator> locIterPtr = getAccessibleLocIterator();
    os << "Locations:" << std::endl;
    for (; locIterPtr->isValid(); (*locIterPtr)++ ) {
        OA_ptr<Location> loc = (locIterPtr->current());
        os << "\t";
        loc->dump(os,ir);
        os << std::endl;
    }
    os << std::endl;
}


  } // end of AccessLoc namespace
} // end of OA namespace
