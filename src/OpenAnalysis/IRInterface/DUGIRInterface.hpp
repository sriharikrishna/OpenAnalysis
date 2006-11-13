/*! \file
  
  \brief Abstract interface that the DUG AnnotationManagers require.

  \authors Michelle Strout
  \version $Id: DUGIRInterface.hpp,v 1.2 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  An DUG AnnotationManager generates an DUG  representation of the
  program.  The analysis engine within the DUG AnnotationManager requires
  that the DUGIRInterface be implemented so that queries can be made to
  the Source IR relevant to constructing call graphs.  

*/

#ifndef DUGIRInterface_h
#define DUGIRInterface_h

#include <iostream>

#include <OpenAnalysis/Location/Location.hpp>
#include <OpenAnalysis/ExprTree/ExprTree.hpp>
#include "IRHandles.hpp"
#include "ExprStmtPairIterator.hpp"

namespace OA {

  namespace DUG {

/*! 
 * The DUGIRInterface abstract base class gives a set of methods for 
 * manipulating a program.  This is the primary interface to the underlying 
 * intermediate representation.
 */
class DUGIRInterface : public virtual IRHandlesIRInterface {
 public:
  DUGIRInterface() { }
  virtual ~DUGIRInterface() {} 
  
  //! Return and iterator over all independent locations for given proc
  //! if not known then must return iterator over set with UnknownLoc
  virtual OA_ptr<LocIterator> getIndepLocIter(ProcHandle h) = 0;
  
  //! Return and iterator over all dependent locations for given proc
  //! if not known then must return iterator over set with UnknownLoc
  virtual OA_ptr<LocIterator> getDepLocIter(ProcHandle h) = 0;
  
  //! Given a subprogram return an IRStmtIterator* for the entire
  //! subprogram
  //! The user must free the iterator's memory via delete.
  virtual OA_ptr<IRStmtIterator> getStmtIterator(ProcHandle h) = 0; 

  //! Return an iterator over all of the callsites in a given stmt
  virtual OA_ptr<IRCallsiteIterator> getCallsites(StmtHandle h) = 0;

  //! Get IRCallsiteParamIterator for a callsite.
  //! Iterator visits actual parameters in called order.
  virtual OA::OA_ptr<OA::IRCallsiteParamIterator> 
    getCallsiteParams(OA::ExprHandle h) = 0;

  //! Given an ExprHandle, return an ExprTree 
  virtual OA_ptr<ExprTree> getExprTree(ExprHandle h) = 0;
  
  //! Given a function call return the callee symbol handle
  virtual SymHandle getSymHandle(ExprHandle expr) = 0;
  
  //! Given the callee symbol returns the callee proc handle
  virtual ProcHandle getProcHandle(SymHandle sym) = 0;

  //! For the given symbol create a Location that indicates statically
  //! overlapping locations and information about whether the location
  //! is local or not for the given procedure, local means only visible
  //! in this procedure
  virtual OA_ptr<Location> getLocation(ProcHandle p, SymHandle s) = 0;

  //! Return a list of all the def memory reference handles that appear
  //! in the given statement.
  //! User is responsible for deleting the list
  virtual OA_ptr<MemRefHandleIterator> getDefMemRefs(StmtHandle stmt) = 0;

  //! Return a list of all the source and sub memory reference handles 
  //! that appear in the given statement.
  //! User is responsible for deleting the list
  virtual OA_ptr<MemRefHandleIterator> getUseMemRefs(StmtHandle stmt) = 0;

  //! Given a MemRefHandle return an iterator over
  //! MemRefExprs that describe this memory reference
  virtual OA_ptr<MemRefExprIterator> 
      getMemRefExprIterator(MemRefHandle h) = 0;

  //! Return an iterator over all the memory reference handles that appear
  //! in the given statement.  Order that memory references are iterated
  //! over can be arbitrary.
  virtual OA_ptr<MemRefHandleIterator> getAllMemRefs(StmtHandle stmt) = 0;
};


  } // end of namespace DUG
} // end of namespace OA

#endif
