/*! \file
  
  \brief Abstract short circuit interface for MemRefExprBasic.  
  
  \authors Michelle Strout
  \version $Id: MREBasicIRShortCircuitDefault.hpp,v 1.5 2005/03/17 21:47:46 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  The source IR will be responsible for generating MemRefExprBasic instances
  and for being able to iterate over memory references.

*/

#ifndef MREBasicIRShortCircuitDefault_h
#define MREBasicIRShortCircuitDefault_h

#include <iostream>
#include <list>
#include "IRHandles.hpp"
#include <OpenAnalysis/MemRefExpr/MemRefExprBasic.hpp>
#include <OpenAnalysis/IRInterface/MREBasicIRShortCircuit.hpp>

namespace OA {


//! The MemRefExprIRShortCircuit abstract base class gives a set of methods
//! for querying the source IR for memory reference expressions and information
//! about them.  This is the primary interface for MemRefExpr to the underlying
//! intermediate representation.
class MREBasicIRShortCircuitDefault : public virtual MREBasicIRShortCircuit {
 public:
  MREBasicIRShortCircuitDefault() { }
  virtual ~MREBasicIRShortCircuitDefault() { }
 
  //--------------------------------------------------------
  // Debugging: would be very helpful
  //--------------------------------------------------------  

  //! Given a memory reference, pretty-print it to the output stream os.
  //! The default implementation does nothing.
  virtual void dump(MemRefHandle h, std::ostream& os) 
      { os << (std::string)h; }

  //! Given a statement, pretty-print it to the output stream os.
  //! The default implementation does nothing.
  virtual void dump(StmtHandle stmt, std::ostream& os) 
      { os << (std::string)stmt; }

};  

} // end of namespace OA

#endif 
