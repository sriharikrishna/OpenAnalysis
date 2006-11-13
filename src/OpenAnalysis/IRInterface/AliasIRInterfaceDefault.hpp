/*! \file
  
  \brief Default implementation for some of the methods in the
         abstract IR interface for Alias analysis.  
  
  \authors Michelle Strout
  \version $Id: AliasIRInterfaceDefault.hpp,v 1.10 2005/06/14 03:52:34 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef AliasIRInterfaceDefault_h
#define AliasIRInterfaceDefault_h

#include <iostream>
#include <list>
#include "AliasIRInterface.hpp"

namespace OA {
  namespace Alias {

/*! Enumerates no statements for default implementation of dynamically
    allocated unnamed locations.  Optimistic assumption that might be
    true for F90 but isn't for C or C++.
 */
class EmptyIRStmtIterator  : public IRStmtIterator {
public:
  EmptyIRStmtIterator() { }
  ~EmptyIRStmtIterator() { };

  StmtHandle current() const { return StmtHandle(0); }
  bool isValid()  const { return false; }
        
  void operator++() {}

  void reset() {}
};



class AliasIRInterfaceDefault : public virtual AliasIRInterface {
 public:
  AliasIRInterfaceDefault() { }
  virtual ~AliasIRInterfaceDefault() { }
 
  //! Given a subprogram return an IRStmtIterator* for all
  //! statements that involve an unnamed dynamic memory allocation
  //! in the subprogram
  //! The user must free the iterator's memory via delete.
  OA_ptr<IRStmtIterator> getUnnamedDynAllocStmtIterator(ProcHandle h)
      { OA_ptr<IRStmtIterator> retval;
        retval = new EmptyIRStmtIterator; 
        return retval;
      }

  //--------------------------------------------------------
  // Debugging: would be very helpful
  //--------------------------------------------------------  

  //! Given an Alias::IRStmtType, generate a string 
  std::string toString(IRStmtType x) 
  {
    switch (x) {
      // case NULL_Accuracy: return "NULL";
      case NONE:                return "Alias::NONE"; // FIXME: standardize with NULL
      case ANY_STMT:            return "Alias::ANY_STMT";
      case PTR_ASSIGN_STMT:     return "Alias::PTR_ASSIGN";
      default:                  return "<INVALID Alias::IRStmtType>";
    }

  }
  // need all for full override
  virtual std::string toString(const ProcHandle h) = 0;
  virtual std::string toString(const StmtHandle h) = 0;
  virtual std::string toString(const ExprHandle h) = 0;
  virtual std::string toString(const OpHandle h) = 0;
  virtual std::string toString(const MemRefHandle h) = 0;
  virtual std::string toString(const SymHandle h) = 0;
  virtual std::string toString(const ConstSymHandle h) = 0;
  virtual std::string toString(const ConstValHandle h) = 0;


  //! Given a statement, pretty-print it to the output stream os.
  //! The default implementation does nothing.
  virtual void dump(StmtHandle stmt, std::ostream& os) { }

  //! Given a memory reference, pretty-print it to the output stream os.
  //! The default implementation does nothing.
  virtual void dump(MemRefHandle stmt, std::ostream& os) { }

};  

  } // end of namespace Alias
} // end of namespace OA

#endif 
