/*! \file
  
  \brief Abstract IR interface for Accessible Location analysis.  
  
  \authors Michelle Strout
  \version $Id: AccessLocIRInterface.hpp,v 1.3 2005/03/17 21:47:45 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef AccessLocIRInterface_h
#define AccessLocIRInterface_h

#include <iostream>
#include <string>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/Location/Location.hpp>

namespace OA {

  //! Namespace for all Alias Analysis interfaces and implementations.
  namespace AccessLoc {

//! The AccessLocIRInterface abstract base class gives a set of methods
//! for querying the source IR for information relevant to determining
//! all of the accessible locations for a procedure.
//! This is the primary interface for AccessLoc to the underlying
//! intermediate representation.
class AccessLocIRInterface : public virtual IRHandlesIRInterface {
 public:
  AccessLocIRInterface() { }
  virtual ~AccessLocIRInterface() { }
 
  //! Given a subprogram return an IRSymIterator for all
  //! symbols that are accessible in the subprogram
  //! To properly handle pointer arithmetic, this should include
  //! all globals and all possible stack locations or locals.
  virtual OA_ptr<IRSymIterator> getAccessibleSymIterator(ProcHandle h) = 0; 

  //! Given a subprogram return an IRStmtIterator for all
  //! statements that involve an unnamed dynamic memory allocation
  //! in the subprogram.  
  //! To properly handle pointer arithmetic, this should include
  //! all statements in the whole program that can do dynamic allocation.
  virtual OA_ptr<IRStmtIterator> getUnnamedDynAllocStmtIterator(ProcHandle h) = 0; 

  //! For the given symbol create a Location that indicates statically
  //! overlapping locations and information about whether the location
  //! is local or not for the given procedure, local means only visible
  //! in this procedure
  virtual OA_ptr<Location> getLocation(ProcHandle p, SymHandle s) = 0;

  //! Given a SymHandle return an iterator over
  //! MemRefExprs that describe how this symbol can be referenced.
  //! For example, if the symbol declaration is "int** p", then it can
  //! be referenced as p, *p, or **p.
  virtual OA_ptr<MemRefExprIterator> 
      getPossMemRefExprIterator(SymHandle h) = 0;

  //! Given a procedure return associated SymHandle
  virtual SymHandle getSymHandle(ProcHandle h) = 0;
  
  //! get iterator over formal parameters in a function declaration
  virtual OA_ptr<IRFormalParamIterator> getFormalParamIterator(SymHandle h) = 0;


};  

  } // end of namespace Alias
} // end of namespace OA

#endif 
