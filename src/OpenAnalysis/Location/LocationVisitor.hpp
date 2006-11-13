/*! \file
  
  \brief Abstract visitor for Location's
  
  \authors Michelle Strout
  \version $Id: LocationVisitor.hpp,v 1.2 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef LocationVisitor_H
#define LocationVisitor_H

#include <OpenAnalysis/Location/Location.hpp>

namespace OA {

//--------------------------------------------------------------------
/*! LocationVisitor is an abstract base class used to implement the 
    Visitor pattern on Locations and possible instances of the
    LocSubSet decorator subclass.

    The only place in the Location hierarchy where changes can
    occur are new LocSubSet subclasses.  Each time a new subclass
    is created a new method must be added to this visitor.  As long
    as existing concrete visitors have a default implementation for 
    the LocSubSet class they will still work properly.

    Memory Management:  It is assumed that the concrete visitors
    will NOT keep references to Location and that
    the Location will not be deallocated while an LocationVisitor
    is visiting a Location.
    Concrete visitors should also never take the address of a Location
    and pass it to a method that requires an OA_ptr<Location>.
*/
class LocationVisitor {
protected:
  LocationVisitor() {}
public:
  virtual ~LocationVisitor() {}

  //---------------------------------------
  // methods for each Location subclasses
  //---------------------------------------
  
  virtual void visitNamedLoc(NamedLoc& loc) = 0;
  virtual void visitUnnamedLoc(UnnamedLoc& loc) = 0;
  virtual void visitInvisibleLoc(InvisibleLoc& loc) = 0;
  virtual void visitUnknownLoc(UnknownLoc& loc) = 0;

  // default LocSubSet base class so that visitors can handle 
  // LocSubSet sub-classes added later in a generic fasion
  // and direct instances of LocSubSet
  virtual void visitLocSubSet(LocSubSet& loc) = 0;

  // each subclass of LocSubSet should call visitLocSubSet
  // as default
  virtual void visitLocIdxSubSet(LocIdxSubSet& loc) 
    { visitLocSubSet(loc); }

};


} // end of OA namespace

#endif
