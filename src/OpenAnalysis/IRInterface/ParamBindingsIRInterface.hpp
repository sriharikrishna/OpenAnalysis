/*! \file
  
  \brief Abstract IR interface for determining parameter bindings.
  
  \authors Michelle Strout
  \version $Id: ParamBindingsIRInterface.hpp,v 1.3 2005/08/15 19:29:55 utke Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef ParamBindingsIRInterface_h
#define ParamBindingsIRInterface_h

#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/Location/Location.hpp>
#include <OpenAnalysis/ExprTree/ExprTree.hpp>

namespace OA {

typedef IRCallsiteParamIterator ExprHandleIterator;

  namespace DataFlow {


//! This is the primary interface for determining parameter bindings to
//! underlying intermediate representation.
class ParamBindingsIRInterface : public virtual IRHandlesIRInterface {
 public:
  ParamBindingsIRInterface() { }
  virtual ~ParamBindingsIRInterface() { }

  //! Get IRCallsiteParamIterator for a callsite.
  //! Iterator visits actual parameters in called order.
  virtual OA::OA_ptr<OA::IRCallsiteParamIterator> 
    getCallsiteParams(OA::ExprHandle h) = 0;

  //! Given a subprogram return an IRSymIterator for all
  //! symbols that are referenced within the subprogram
  virtual OA_ptr<IRSymIterator> getRefSymIterator(ProcHandle h) = 0; 

  //! returns true if given symbol is a parameter 
  virtual bool isParam(SymHandle) = 0;

  //! returns true if given symbol is a pass by reference parameter 
  virtual bool isRefParam(SymHandle) = 0;
               
  //! return the formal parameter that an actual parameter is associated with 
  virtual SymHandle getFormalForActual(ProcHandle caller, ExprHandle call, 
                                       ProcHandle callee, ExprHandle param) = 0; 
 
  //! Given an ExprHandle, return an ExprTree 
  virtual OA_ptr<ExprTree> getExprTree(ExprHandle h) = 0;
  
  // indicates to source IR the current procedure that we will be making
  // queries about
  // FIXME: should probably put ProcHandle as param to other interface methods
  // this is a temporary bandaid or expect source IR to know context
  virtual void currentProc(ProcHandle p) = 0;
};  

  } // end of namespace DataFlow
} // end of namespace OA

#endif 
