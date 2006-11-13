/*! \file
  
  \brief Stores all accessible locations for a particular procedure.

  \authors Michelle Strout
  \version $Id: AccessLocStandard.hpp,v 1.3 2005/03/17 21:47:44 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef AccessLocStandard_H
#define AccessLocStandard_H

#include <cassert>
#include <iostream>
#include <set>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/Location/Location.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/AccessLoc/Interface.hpp>

namespace OA {
  namespace AccessLoc {

//typedef std::set<OA_ptr<Location::Location> > LocSet;

class AccessLocStandard : public AccessLoc::Interface {
  public:
    AccessLocStandard(ProcHandle p);
    ~AccessLocStandard() {}

    static const int SET_ID_NONE = -1; 

    //! Return an iterator over all accessible locations for procedure
    OA_ptr<LocIterator> getAccessibleLocIterator();

    //*****************************************************************
    // Construction methods 
    //*****************************************************************
    
    //! add the given location to the set of accessible locations
    void addLocation(OA_ptr<Location> loc);

    //*****************************************************************
    // Output
    //*****************************************************************

    //! incomplete output of info for debugging, just lists locations
    void dump(std::ostream& os, OA_ptr<OA::IRHandlesIRInterface> ir);

  private:
    // data members
    ProcHandle mProcHandle; // procedure these sets are associated with

    OA_ptr<LocSet> mLocSet;

};


  } // end of AccessLoc namespace
} // end of OA namespace

#endif

