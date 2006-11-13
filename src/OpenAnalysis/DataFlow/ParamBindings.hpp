/*! \file
  
  \brief One-to-one mapping between formal parameter symbols in callee and
         memory reference handles in caller.

  \authors Michelle Strout 
  \version $Id: ParamBindings.hpp,v 1.4 2005/08/15 19:29:55 utke Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef ParamBindings_h
#define ParamBindings_h

#include <iostream>
#include <map>

#include <OpenAnalysis/IRInterface/IRHandles.hpp>

namespace OA {
  namespace DataFlow {

class ParamBindings {
public:
  ParamBindings() { }
  ~ParamBindings() {}

  //! get caller mem ref associated with callee formal
  MemRefHandle getCallerMemRef(ExprHandle call, SymHandle calleeFormal);

  //! returns an iterator over the memory reference parameters for a call
  OA_ptr<MemRefHandleIterator> getActualIterator(ExprHandle call);

  //! get callee formal associated with caller mem ref
  SymHandle getCalleeFormal(ExprHandle call, MemRefHandle callerRef);

  //! returns an iterator over the formal symbols for a procedure
  OA_ptr<SymHandleIterator> getFormalIterator(ProcHandle proc);

  //! returns true if given formal is a reference parameter
  bool isRefParam(SymHandle formal);

  //*****************************************************************
  // Output
  //*****************************************************************
  //! debugging output
  void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);
  
  //*****************************************************************
  // Construction methods 
  //*****************************************************************
  void mapMemRefToFormal(ExprHandle call, MemRefHandle ref, SymHandle sym);
  void setRefParam(SymHandle sym);
  void mapFormalToProc(SymHandle formal, ProcHandle proc);
  void mapActualToCall(MemRefHandle actual, ExprHandle call);

private:
  std::map<ExprHandle,std::map<MemRefHandle,SymHandle> > mCallerToCalleeMap;
  std::map<ExprHandle,std::map<SymHandle,MemRefHandle> > mCalleeToCallerMap;
  std::map<ProcHandle,OA_ptr<std::set<SymHandle> > > mProcToFormalSetMap;
  std::map<ExprHandle,OA_ptr<std::set<MemRefHandle> > > mCallToActualSetMap;
  std::map<SymHandle,bool> mIsRefParam;


  //! An iterator over Symbols
  class SymIterator : public virtual OA::SymHandleIterator,
                      public OA::IRHandleSetIterator<OA::SymHandle>
  {
  public:
    SymIterator(OA::OA_ptr<std::set<OA::SymHandle> > pSet) 
      : OA::IRHandleSetIterator<OA::SymHandle>(pSet) {}
    ~SymIterator() {}

    void operator++() { OA::IRHandleSetIterator<OA::SymHandle>::operator++(); }
    bool isValid() const
      { return OA::IRHandleSetIterator<OA::SymHandle>::isValid(); }
    OA::SymHandle current() const
      { return OA::IRHandleSetIterator<OA::SymHandle>::current(); }
    void reset() { OA::IRHandleSetIterator<OA::SymHandle>::reset(); }
  };
  
  //! An iterator over MemRefHandle's
  class MemRefIterator : public virtual OA::MemRefHandleIterator,
                         public OA::IRHandleSetIterator<OA::MemRefHandle>
  {
  public:
    MemRefIterator(OA::OA_ptr<std::set<OA::MemRefHandle> > pList) 
      : OA::IRHandleSetIterator<OA::MemRefHandle>(pList) {}
    ~MemRefIterator() {}

    void operator++() { OA::IRHandleSetIterator<OA::MemRefHandle>::operator++(); }
    bool isValid() const
      { return OA::IRHandleSetIterator<OA::MemRefHandle>::isValid(); }
    OA::MemRefHandle current() const
      { return OA::IRHandleSetIterator<OA::MemRefHandle>::current(); }
    void reset() { OA::IRHandleSetIterator<OA::MemRefHandle>::reset(); }
  };


};

  } // end of DataFlow namespace
} // end of OA namespace

#endif
