/*! \file
    
    \brief Test IR interface for CallGraph
        
    \authors Michelle Strout
    \version $Id: CallGraphIRInterfaceTest.hpp,v 1.1 2004/11/29 20:59:00 mstrout Exp $
 
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
#include <OpenAnalysis/Location/Location.hpp>
#include <OpenAnalysis/ExprTree/ExprTree.hpp>

//#include <OpenAnalysis/CallGraph/CallGraphStandard.hpp>
#include <OpenAnalysis/IRInterface/CallGraphIRInterface.hpp>
#include "IRHandlesIRInterfaceTest.hpp"

#ifndef CallGraphIRInterfaceTest_HPP
#define CallGraphIRInterfaceTest_HPP



/*! methods needed to build the TestIR and implement
    required IRInterfaces 
*/
class CallGraphIRInterfaceTest 
    : public virtual OA::CallGraph::CallGraphIRInterface,
      public IRHandlesIRInterfaceTest
{ 
  private:
    std::map<OA::ProcHandle,OA::SymHandle> mProcToSym;
    std::map<OA::ExprHandle,OA::SymHandle> mCallsiteSym;

    // for getStmtIterator
    std::map<OA::ProcHandle,std::list<OA::StmtHandle> > mProcBody;

    // for Callsites
    std::map<OA::StmtHandle,std::list<OA::ExprHandle> > mStmtToCallsites;

  public:
    CallGraphIRInterfaceTest() {}
    ~CallGraphIRInterfaceTest() {}

    //*****************************************************************
    // Implementation of IRHandlesIRInterface
    //*****************************************************************
    virtual std::string toString(OA::ProcHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::StmtHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::MemRefHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::SymHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::ExprHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::ConstSymHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::ConstValHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::OpHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual void dump(OA::MemRefHandle h, std::ostream& os)
    { IRHandlesIRInterfaceTest::dump(h,os); }

    virtual void dump(OA::StmtHandle h, std::ostream& os)
    { IRHandlesIRInterfaceTest::dump(h,os); }

    //*****************************************************************
    // Implementation of CallGraphIRInterface
    //*****************************************************************
    
    //! Given a procedure return associated SymHandle
    OA::SymHandle getProcSymHandle(OA::ProcHandle h)
    { return mProcToSym[h]; }
  
    //! Given a subprogram return an IRStmtIterator* for the entire
    //! subprogram
    //! The user must free the iterator's memory via delete.
    OA::OA_ptr<OA::IRStmtIterator> getStmtIterator(OA::ProcHandle h);
  
    //! Return an iterator over all of the callsites in a given stmt
    OA::OA_ptr<OA::IRCallsiteIterator> getCallsites(OA::StmtHandle h);

    //! Return SymHandle for function call
    OA::SymHandle getSymHandle(OA::ExprHandle expr)
      { return mCallsiteSym[expr]; }
    
    //*****************************************************************
    // Construction methods 
    //*****************************************************************
    void mapProcToSym(OA::ProcHandle, OA::SymHandle);
    void addStmtCallsite(OA::ProcHandle, OA::StmtHandle, OA::ExprHandle call);
    void addCallSym(OA::ExprHandle call, OA::SymHandle);

    void dump(std::ostream& os);

};

#endif
