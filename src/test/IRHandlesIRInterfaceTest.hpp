/*! \file
    
    \brief Test IR interface for  IRHandlesIRInterface

    Also includes some iterators used by many of the IRInterfaceTest
    implementations.
        
    \authors Michelle Strout
    \version $Id: IRHandlesIRInterfaceTest.hpp,v 1.5 2004/12/03 14:01:34 mstrout Exp $
 
     Copyright (c) 2002-2004, Rice University
     Copyright (c) 2004, University of Chicago  
     All rights reserved.
     See ../../../Copyright.txt for details.
 
*/

#include <string>
#include <list>
#include <map>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/MemRefExpr/MemRefExpr.hpp>

#ifndef IRHANDLESIRINTERFACETEST_H
#define IRHANDLESIRINTERFACETEST_H

//! An iterator over symbols 
class TestIRSymIterator : public virtual OA::IRSymIterator,
                          public OA::IRHandleListIterator<OA::SymHandle>
{
public:
  TestIRSymIterator(OA::OA_ptr<std::list<OA::SymHandle> > pList) 
    : OA::IRHandleListIterator<OA::SymHandle>(pList) {}
  ~TestIRSymIterator() {}

  void operator++() { OA::IRHandleListIterator<OA::SymHandle>::operator++(); }
  bool isValid() const
    { return OA::IRHandleListIterator<OA::SymHandle>::isValid(); }
  OA::SymHandle current() const
    { return OA::IRHandleListIterator<OA::SymHandle>::current(); }
  void reset() { OA::IRHandleListIterator<OA::SymHandle>::current(); }
};

//! An iterator over memory references 
class TestIRMemRefIterator : public virtual OA::MemRefHandleIterator,
                             public OA::IRHandleListIterator<OA::MemRefHandle>
{
public:
  TestIRMemRefIterator(OA::OA_ptr<std::list<OA::MemRefHandle> > pList) 
    : OA::IRHandleListIterator<OA::MemRefHandle>(pList) {}
  ~TestIRMemRefIterator() {}

  void operator++() { OA::IRHandleListIterator<OA::MemRefHandle>::operator++(); }
  bool isValid() const
    { return OA::IRHandleListIterator<OA::MemRefHandle>::isValid(); }
  OA::MemRefHandle current() const
    { return OA::IRHandleListIterator<OA::MemRefHandle>::current(); }
  void reset() { OA::IRHandleListIterator<OA::MemRefHandle>::current(); }
};


//! An iterator over stmts 
class TestIRStmtIterator : public virtual OA::IRStmtIterator,
                           public OA::IRHandleListIterator<OA::StmtHandle>
{
public:
  TestIRStmtIterator(OA::OA_ptr<std::list<OA::StmtHandle> > pList) 
    : OA::IRHandleListIterator<OA::StmtHandle>(pList) {}
  ~TestIRStmtIterator() {}

  void operator++() { OA::IRHandleListIterator<OA::StmtHandle>::operator++(); }
  bool isValid() const
    { return OA::IRHandleListIterator<OA::StmtHandle>::isValid(); }
  OA::StmtHandle current() const
    { return OA::IRHandleListIterator<OA::StmtHandle>::current(); }
  void reset() { OA::IRHandleListIterator<OA::StmtHandle>::current(); }
};

//! An iterator over expression 
class TestIRExprIterator : public virtual OA::ExprHandleIterator,
                           public OA::IRHandleListIterator<OA::ExprHandle>
{
public:
  TestIRExprIterator(OA::OA_ptr<std::list<OA::ExprHandle> > pList) 
    : OA::IRHandleListIterator<OA::ExprHandle>(pList) {}
  ~TestIRExprIterator() {}

  void operator++() { OA::IRHandleListIterator<OA::ExprHandle>::operator++(); }
  bool isValid() const
    { return OA::IRHandleListIterator<OA::ExprHandle>::isValid(); }
  OA::ExprHandle current() const
    { return OA::IRHandleListIterator<OA::ExprHandle>::current(); }
  void reset() { OA::IRHandleListIterator<OA::ExprHandle>::current(); }
};


//! An iterator over memory references 
class TestIRMemRefExprIterator : public OA::MemRefExpr::MemRefExprIterator {
public:
  TestIRMemRefExprIterator(OA::OA_ptr<std::list<OA::OA_ptr<OA::MemRefExpr::MemRefExpr> > > pList)  : mList(pList) { mIter = mList->begin(); }
  ~TestIRMemRefExprIterator() { }

  OA::OA_ptr<OA::MemRefExpr::MemRefExpr> current() const 
    { return *mIter; }
  bool isValid() const { return mIter != mList->end(); }
        
  void operator++() { mIter++; }

  virtual void reset() { mIter = mList->begin(); }
private:
  OA::OA_ptr<std::list<OA::OA_ptr<OA::MemRefExpr::MemRefExpr> > > mList;
  std::list<OA::OA_ptr<OA::MemRefExpr::MemRefExpr> >:: iterator mIter;

};

//! An iterator over procs 
class TestIRProcIterator : public virtual OA::IRProcIterator,
                           public OA::IRHandleListIterator<OA::ProcHandle>
{
public:
  TestIRProcIterator(OA::OA_ptr<std::list<OA::ProcHandle> > pList) 
    : OA::IRHandleListIterator<OA::ProcHandle>(pList) {}
  ~TestIRProcIterator() {}

  void operator++() { OA::IRHandleListIterator<OA::ProcHandle>::operator++(); }
  bool isValid() const
    { return OA::IRHandleListIterator<OA::ProcHandle>::isValid(); }
  OA::ProcHandle current() const
    { return OA::IRHandleListIterator<OA::ProcHandle>::current(); }
  void reset() { OA::IRHandleListIterator<OA::ProcHandle>::current(); }
};


/*! methods needed to build the TestIR and implement
    the IRHandlesIRInterface
*/
class IRHandlesIRInterfaceTest 
    : public virtual OA::IRHandlesIRInterface
{ 
  protected:
    std::map<OA::ProcHandle,std::string> mProcString;
    std::map<OA::StmtHandle,std::string > mStmtString;
    std::map<OA::MemRefHandle,std::string > mMemRefString;
    std::map<OA::SymHandle,std::string > mSymString;
    std::map<OA::ExprHandle,std::string > mExprString;
    std::map<OA::OpHandle,std::string > mOpString;
    std::map<OA::ConstValHandle,std::string > mConstValString;
    std::map<OA::ConstSymHandle,std::string > mConstSymString;

  public:
    IRHandlesIRInterfaceTest() {}
    virtual ~IRHandlesIRInterfaceTest() {}

    //*****************************************************************
    // Implementation of IRHandlesIRInterface
    //*****************************************************************
    virtual std::string toString(OA::ProcHandle h);

    virtual std::string toString(OA::StmtHandle h);

    virtual std::string toString(OA::MemRefHandle h);

    virtual std::string toString(OA::SymHandle h);

    virtual std::string toString(OA::ExprHandle h);

    virtual std::string toString(OA::ConstSymHandle h);

    virtual std::string toString(OA::ConstValHandle h); 

    virtual std::string toString(OA::OpHandle h);

    virtual void dump(OA::MemRefHandle h, std::ostream& os);

    virtual void dump(OA::StmtHandle h, std::ostream& os);

    //*****************************************************************
    // Extra methods for testing framework
    //*****************************************************************
    virtual OA::OA_ptr<OA::IRProcIterator> getProcIterator();

    //*****************************************************************
    // Construction methods 
    //*****************************************************************
    virtual void addProcString(OA::ProcHandle proc,std::string str);

    virtual void addStmtString(OA::StmtHandle stmt, std::string str);

    virtual void addMemRefString(OA::MemRefHandle memref,std::string str);

    virtual void addSymString(OA::SymHandle sym,std::string str);

    virtual void addExprString(OA::ExprHandle expr,std::string str);

    virtual void addOpString(OA::OpHandle op,std::string str);

    virtual void addConstSymString(OA::ConstSymHandle op,std::string str);

    virtual void addConstValString(OA::ConstValHandle op,std::string str);
};

#endif
