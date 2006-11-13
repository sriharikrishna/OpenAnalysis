/*! \file
  
  \brief Abstract IR interface for differentiable location visitor, which 
         is ultimately used for activity analysis.
  
  \authors Michelle Strout
  \version $Id: ActivityIRInterface.hpp,v 1.4 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef ActivityIRInterface_h
#define ActivityIRInterface_h

#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/Location/Location.hpp>
#include <OpenAnalysis/ExprTree/ExprTree.hpp>

#include "ExprStmtPairIterator.hpp"

namespace OA {

typedef IRCallsiteParamIterator ExprHandleIterator;

  namespace Activity {

// Statements are classified into one of the following types:
enum IRStmtType {
  ANY_STMT,           // Anything not covered below.
  EXPR_STMT,          // A statement involving only target = expression
                      // pairs
  NONE
};


//! The ActivityIRInterface abstract base class gives a 
//! set of methods for querying the source IR for information relevant 
//! to the Dep phase of activity analysis.
class ActivityIRInterface : public virtual IRHandlesIRInterface {
 public:
  ActivityIRInterface() { }
  virtual ~ActivityIRInterface() { }

  //! Return and iterator over all independent locations for given proc
  //! if not known then must return iterator over set with UnknownLoc
  virtual OA_ptr<LocIterator> getIndepLocIter(ProcHandle h) = 0;
  
  //! Return and iterator over all dependent locations for given proc
  //! if not known then must return iterator over set with UnknownLoc
  virtual OA_ptr<LocIterator> getDepLocIter(ProcHandle h) = 0;
  
  //! Given a subprogram return an IRStmtIterator for the entire
  //! subprogram
  virtual OA_ptr<IRStmtIterator> getStmtIterator(ProcHandle h) = 0; 

  //! Return an iterator over all of the callsites in a given stmt
  virtual OA_ptr<IRCallsiteIterator> getCallsites(StmtHandle h) = 0;

  //! Get IRCallsiteParamIterator for a callsite.
  //! Iterator visits actual parameters in called order.
  virtual OA::OA_ptr<OA::IRCallsiteParamIterator> 
    getCallsiteParams(OA::ExprHandle h) = 0;

  //! returns true if given symbol is a pass by reference parameter 
  virtual bool isRefParam(SymHandle) = 0;
               
  //! Given an ExprHandle, return an ExprTree 
  virtual OA_ptr<ExprTree> getExprTree(ExprHandle h) = 0;
  
  //! Return a list of all the target memory reference handles that appear
  //! in the given statement.
  virtual OA_ptr<MemRefHandleIterator> getDefMemRefs(StmtHandle stmt) = 0;
  
  //! Return a list of all the source memory reference handles that appear
  //! in the given statement.
  virtual OA_ptr<MemRefHandleIterator> getUseMemRefs(StmtHandle stmt) = 0;
 
  //! Given a statement, return its Activity::IRStmtType
  virtual IRStmtType getActivityStmtType(StmtHandle h) = 0; 
 
  //! Given a statement return a list to the pairs of 
  //! target MemRefHandle, ExprHandle where
  //! target = expr
  virtual OA_ptr<ExprStmtPairIterator> 
      getExprStmtPairIterator(StmtHandle h) = 0; 

  //! given a symbol return the size in bytes of that symbol
  virtual int getSizeInBytes(SymHandle h) = 0;
  
  //! Given a function call return the callee symbol handle
  virtual SymHandle getSymHandle(ExprHandle expr) = 0;
  
};  

  } // end of namespace Activity
} // end of namespace OA

#endif 
