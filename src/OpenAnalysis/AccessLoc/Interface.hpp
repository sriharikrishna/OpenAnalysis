/*! \file
  
  \brief Declarations for the abstract AccessLoc interface.

  \authors Michelle Strout
  \version $Id: Interface.hpp,v 1.3 2005/03/17 21:47:44 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef AccessLocInterface_H
#define AccessLocInterface_H

#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/Location/Location.hpp>

namespace OA {
  namespace AccessLoc {

class Interface {
  public:
    Interface() {}
    virtual ~Interface() {};

    //! Return an iterator over all accessible locations for procedure
    virtual OA_ptr<LocIterator> getAccessibleLocIterator() = 0;
};

  } // end of AccessLoc namespace
} // end of OA namespace

#endif

