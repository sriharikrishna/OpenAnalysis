/*! \file
  
  \brief Abstract IR interface for Alias analysis.  
  
  \authors Michelle Strout
  \version $Id: AliasIRInterface.hpp,v 1.15 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  The source IR will be responsible for iterating over all the statements,
  iterating over top memory references for a stmt, and categorizing statements
  specifically for alias analysis.

*/

#ifndef AliasIRInterface_h
#define AliasIRInterface_h

#include <iostream>
#include <string>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/Location/Location.hpp>
#include <OpenAnalysis/MemRefExpr/MemRefExpr.hpp>

namespace OA {

  //! Namespace for all Alias Analysis interfaces and implementations.
  namespace Alias {

// Statements are classified into one of the following types:
enum IRStmtType {
  ANY_STMT,           // Anything not covered below.
  PTR_ASSIGN_STMT,    // assigning one pointer to another or to unnamed dyn alloc
  NONE
};

class PtrAssignPairStmtIterator {
  public:
    PtrAssignPairStmtIterator() {}
    virtual ~PtrAssignPairStmtIterator() {}

    //! right hand side
    virtual MemRefHandle currentSource() const = 0;
    //! left hand side
    virtual MemRefHandle currentTarget() const = 0;

    virtual bool isValid() const = 0;
                    
    virtual void operator++() = 0;
    void operator++(int) { ++*this; }
};


//! The AliasIRInterface abstract base class gives a set of methods
//! for querying the source IR for information relevant to alias analysis.
//! This is the primary interface for Alias to the underlying
//! intermediate representation.
class AliasIRInterface : public virtual IRHandlesIRInterface {
 public:
  AliasIRInterface() { }
  virtual ~AliasIRInterface() { }
 
  //! Given a subprogram return an IRStmtIterator for the entire
  //! subprogram
  virtual OA_ptr<IRStmtIterator> getStmtIterator(ProcHandle h) = 0; 
  
  //! Return an iterator over all the memory reference handles that appear
  //! in the given statement.  Order that memory references are iterated
  //! over can be arbitrary.
  virtual OA_ptr<MemRefHandleIterator> getAllMemRefs(StmtHandle stmt) = 0;
  
  //! Given a statement, return its Alias::IRStmtType
  virtual IRStmtType getAliasStmtType(StmtHandle h) = 0; 

  //! If this is a PTR_ASSIGN_STMT then return an iterator over MemRefHandle
  //! pairs where there is a source and target such that target
  virtual OA_ptr<PtrAssignPairStmtIterator>
      getPtrAssignStmtPairIterator(StmtHandle stmt) = 0;

  //! Given a procedure return associated SymHandle
  virtual SymHandle getSymHandle(ProcHandle h) = 0;
  
  //! Given a subprogram return an IRSymIterator for all
  //! symbols that are visible in the subprogram
  //virtual OA_ptr<IRSymIterator> getVisibleSymIterator(ProcHandle h) = 0; 

  //! Given a subprogram return an IRStmtIterator for all
  //! statements that involve an unnamed dynamic memory allocation
  //! in the subprogram
  //virtual OA_ptr<IRStmtIterator> getUnnamedDynAllocStmtIterator(ProcHandle h) = 0; 

  //! For the given symbol create a Location that indicates statically
  //! overlapping locations and information about whether the location
  //! is local or not for the given procedure, local means only visible
  //! in this procedure
  virtual OA_ptr<Location> getLocation(ProcHandle p, SymHandle s) = 0;

  //! Given a MemRefHandle return an iterator over
  //! MemRefExprs that describe this memory reference
  virtual OA_ptr<MemRefExprIterator> 
      getMemRefExprIterator(MemRefHandle h) = 0;

  //! returns true if given symbol is a pass by reference parameter 
  // FIXME: is this needed or can we take it out because 
  // interprocedural analyses should use information in ParamBindings?
  virtual bool isRefParam(SymHandle) = 0;
               
  //! Given an Alias::IRStmtType, generate a string 
  virtual std::string toString(IRStmtType x) = 0;

  // Since I have the toString for IRStmtType I have to duplicate
  // all of these guys here to do a full override of methods 
  // in IRHandlesIRInterface
  virtual std::string toString(const ProcHandle h) = 0;
  virtual std::string toString(const StmtHandle h) = 0;
  virtual std::string toString(const ExprHandle h) = 0;
  virtual std::string toString(const OpHandle h) = 0;
  virtual std::string toString(const MemRefHandle h) = 0;
  virtual std::string toString(const SymHandle h) = 0;
  virtual std::string toString(const ConstSymHandle h) = 0;
  virtual std::string toString(const ConstValHandle h) = 0;

};  

  } // end of namespace Alias
} // end of namespace OA

#endif 
