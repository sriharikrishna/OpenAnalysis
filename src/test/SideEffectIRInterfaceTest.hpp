/*! \file
    
    \brief Test IR interface for SideEffect analysis.  
        
    \authors Michelle Strout
    \version $Id: SideEffectIRInterfaceTest.hpp,v 1.1 2004/11/28 21:33:29 mstrout Exp $
 
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

#include <OpenAnalysis/IRInterface/SideEffectIRInterface.hpp>
#include "IRHandlesIRInterfaceTest.hpp"

#ifndef SideEffectIRInterfaceTest_HPP
#define SideEffectIRInterfaceTest_HPP



/*! methods needed to build the TestIR and implement
    required IRInterfaces 
*/
class SideEffectIRInterfaceTest 
    : public virtual OA::SideEffect::SideEffectIRInterface,
      public IRHandlesIRInterfaceTest
{ 
  private:
    // for getStmtIterator
    std::map<OA::ProcHandle,std::list<OA::StmtHandle> > mProcBody;

    // for getDefMemRefs
    std::map<OA::StmtHandle,std::list<OA::MemRefHandle> > mStmtToDefMemRefs;
    
    // for getUseMemRefs
    std::map<OA::StmtHandle,std::list<OA::MemRefHandle> > mStmtToUseMemRefs;

    // for Callsites
    std::map<OA::StmtHandle,std::list<OA::ExprHandle> > mStmtToCallsites;

  public:
    SideEffectIRInterfaceTest() {}
    ~SideEffectIRInterfaceTest() {}

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
    // Implementation of SideEffectIRInterface
    //*****************************************************************
    
    //! Given a subprogram return an IRStmtIterator for the entire
    //! subprogram
    OA::OA_ptr<OA::IRStmtIterator> getStmtIterator(OA::ProcHandle h); 

    //! Return a list of all the target memory reference handles that appear
    //! in the given statement.
    OA::OA_ptr<OA::MemRefHandleIterator> getDefMemRefs(OA::StmtHandle stmt);
  
    //! Return a list of all the source memory reference handles that appear
    //! in the given statement.
    OA::OA_ptr<OA::MemRefHandleIterator> getUseMemRefs(OA::StmtHandle stmt);
  
    //! Return an iterator over all of the callsites in a given stmt
    OA::OA_ptr<OA::IRCallsiteIterator> getCallsites(OA::StmtHandle h);


    //*****************************************************************
    // Construction methods 
    //*****************************************************************
    void addStmt(OA::ProcHandle,OA::StmtHandle);

    void addUseMemRef(OA::StmtHandle stmt, OA::MemRefHandle memref);

    void addDefMemRef(OA::StmtHandle stmt, OA::MemRefHandle memref);

    void addCallsite(OA::StmtHandle stmt, OA::ExprHandle expr);


    void dump(std::ostream& os);

};

#endif
