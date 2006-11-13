/*! \file
  
  \brief Abstract short circuit interface for MemRefExprBasic.  
  
  \authors Michelle Strout
  \version $Id: MREBasicIRShortCircuit.hpp,v 1.8 2005/03/17 21:47:46 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  The source IR will be responsible for generating MemRefExprBasic instances
  and for being able to iterate over memory references.

*/

#ifndef MREBasicIRShortCircuit_h
#define MREBasicIRShortCircuit_h

#include <iostream>
#include <list>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/MemRefExpr/MemRefExprBasic.hpp>
//#include <OpenAnalysis/MemRefExpr/LocBlock.hpp>

namespace OA {

class LocBlock;

//! The MemRefExprIRShortCircuit abstract base class gives a set of methods
//! for querying the source IR for memory reference expressions and information
//! about them.  This is the primary interface for MemRefExpr to the underlying
//! intermediate representation.
class MREBasicIRShortCircuit : public virtual IRHandlesIRInterface {
 public:
  MREBasicIRShortCircuit() { }
  virtual ~MREBasicIRShortCircuit() { }
 
  //! Given a subprogram return an IRStmtIterator* for the entire
  //! subprogram
  //! The user must free the iterator's memory via delete.
  virtual OA_ptr<IRStmtIterator> getStmtIterator(ProcHandle h) = 0; 
  
  //! Given a statement return an IRTopMemRefIterator* 
  //! this is an iterator over all the top memory references in a statement
  //! for example, in *p = q[i][j], *p and q[i][j] are the top memory 
  //! references which contain the sub memory references p and q,i,j 
  //! respectively
  //! The user must free the iterator's memory via delete.
  virtual OA_ptr<IRTopMemRefIterator> getTopMemRefIterator(StmtHandle h) = 0; 
 
  //! Given a MemRefHandle for a top mem ref
  //! return a list of all MemRefExprBasics
  virtual OA_ptr<std::list<MemRefExprBasic> > 
      getMemRefExprBasicList(MemRefHandle h) = 0;
    
// For now the following methods are staying in this file, but
// at some point might want to break the LocBlock and LocSubSet
// stuff into its own interface

  //! Given a subprogram return an IRSymIterator* for all
  //! symbols that are visible in the subprogram
  //! The user must free the iterator's memory via delete.
  virtual OA_ptr<IRSymIterator> getVisibleSymIterator(ProcHandle h) = 0; 

  //! For the given symbol create a LocBlock that indicates statically
  //! overlapping locations
  virtual OA_ptr<LocBlock> getLocBlock(SymHandle s) = 0;
 
};  

} // end of namespace OA

#endif 
