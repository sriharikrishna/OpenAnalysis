/*! \file
    
    \brief Test IR interface for AccessLoc analysis.  
        
    \authors Michelle Strout
    \version $Id: AccessLocIRInterfaceTest.hpp,v 1.2 2004/11/19 19:21:53 mstrout Exp $
 
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
#include <OpenAnalysis/MemRefExpr/MemRefExpr.hpp>

#include <OpenAnalysis/IRInterface/AccessLocIRInterface.hpp>
#include "IRHandlesIRInterfaceTest.hpp"

#ifndef AccessLocIRInterfaceTest_HPP
#define AccessLocIRInterfaceTest_HPP



/*! methods needed to build the TestIR and implement
    required IRInterfaces 
*/
class AccessLocIRInterfaceTest 
    : public virtual OA::AccessLoc::AccessLocIRInterface,
      public IRHandlesIRInterfaceTest
{ 
  private:
    std::map<OA::ProcHandle,std::list<OA::StmtHandle> > mUnnamedDynAllocStmts;
    std::map<OA::ProcHandle,OA::SymHandle> mProcToSym;

    std::map<OA::SymHandle,
             std::list<OA::OA_ptr<OA::MemRefExpr::MemRefExpr> > > mSymToMRE;

    std::map<OA::SymHandle, std::list<OA::SymHandle> > mProcSymToFormalParams;

    std::map<OA::ProcHandle,
             std::map<OA::SymHandle, 
                      OA::OA_ptr<OA::Location::Location> > > mAccessibleSymToLoc;

  public:
    AccessLocIRInterfaceTest() {}
    ~AccessLocIRInterfaceTest() {}

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
    // Implementation of AccessLocIRInterface
    //*****************************************************************
    
    //! Given a subprogram return an IRSymIterator for all
    //! symbols that are accessible in the subprogram
    //! To properly handle pointer arithmetic, this should include
    //! all globals and all possible stack locations or locals.
    OA::OA_ptr<OA::IRSymIterator> getAccessibleSymIterator(OA::ProcHandle h);
  
    //! Given a subprogram return an IRStmtIterator for all
    //! statements that involve an unnamed dynamic memory allocation
    //! in the subprogram.  
    //! To properly handle pointer arithmetic, this should include
    //! all statements in the whole program that can do dynamic allocation.
    OA::OA_ptr<OA::IRStmtIterator> 
        getUnnamedDynAllocStmtIterator(OA::ProcHandle h); 

    //! Given a procedure return associated SymHandle
    OA::SymHandle getSymHandle(OA::ProcHandle h)
    { return mProcToSym[h]; }
  
    //! For the given symbol create a Location that indicates statically
    //! overlapping locations and information about whether the location
    //! is local or not
    OA::OA_ptr<OA::Location::Location> getLocation(OA::ProcHandle proc,
                                                   OA::SymHandle s)
    { return mAccessibleSymToLoc[proc][s]; }
  
    //! Given a SymHandle return an iterator over
    //! MemRefExprs that describe how this symbol can be referenced.
    //! For example, if the symbol declaration is "int** p", then it can
    //! be referenced as p, *p, or **p.
    OA::OA_ptr<OA::MemRefExpr::MemRefExprIterator> 
        getPossMemRefExprIterator(OA::SymHandle h);

    //! get iterator over formal parameters in a function declaration
    OA::OA_ptr<OA::IRFormalParamIterator> 
        getFormalParamIterator(OA::SymHandle h);

    //*****************************************************************
    // Construction methods 
    //*****************************************************************
    void mapProcToSym(OA::ProcHandle, OA::SymHandle);
    void addSym(OA::ProcHandle, OA::SymHandle,
                OA::OA_ptr<OA::Location::Location>);
    void addFormalParam(OA::SymHandle proc,OA::SymHandle param);
    void addUnnamedDynAllocStmt(OA::ProcHandle, OA::StmtHandle);

    void addMemRefExpr(OA::SymHandle,
                       OA::OA_ptr<OA::MemRefExpr::MemRefExpr>);

    void dump(std::ostream& os);

};

#endif
