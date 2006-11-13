/*! \file
    
    \brief Test IR interface for CallGraphDFProblem
        
    \authors Michelle Strout
    \version $Id: CallGraphDFProblemIRInterfaceTest.hpp,v 1.1 2004/11/29 05:11:27 mstrout Exp $
 
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

#include <OpenAnalysis/DataFlow/CallGraphDFProblem.hpp>
#include <OpenAnalysis/IRInterface/CallGraphDFProblemIRInterface.hpp>
#include "IRHandlesIRInterfaceTest.hpp"

#ifndef CallGraphDFProblemIRInterfaceTest_HPP
#define CallGraphDFProblemIRInterfaceTest_HPP



/*! methods needed to build the TestIR and implement
    required IRInterfaces 
*/
class CallGraphDFProblemIRInterfaceTest 
    : public virtual OA::DataFlow::CallGraphDFProblemIRInterface,
      public IRHandlesIRInterfaceTest
{ 
  private:
    std::map<OA::ProcHandle,
             std::map<OA::SymHandle, 
                      OA::OA_ptr<OA::Location::Location> > > mSymToLoc;

    std::map<OA::SymHandle,bool> mFormalParamIsRef;

    std::map<OA::ExprHandle,std::list<OA::ExprHandle> > mCallsiteParams;

    std::map<OA::ExprHandle,OA::SymHandle> mCallsiteParamFormal;

    std::map<OA::ExprHandle,OA::OA_ptr<OA::ExprTree::ExprTree> > 
        mCallsiteParamExprTree;

  public:
    CallGraphDFProblemIRInterfaceTest() {}
    ~CallGraphDFProblemIRInterfaceTest() {}

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
    // Implementation of CallGraphDFProblemIRInterface
    //*****************************************************************
    //! Get IRCallsiteParamIterator for a callsite.
    //! Iterator visits actual parameters in called order.
    OA::OA_ptr<OA::IRCallsiteParamIterator> getCallsiteParams(OA::ExprHandle h)
      { OA::OA_ptr<TestIRExprIterator> retval;
        OA::OA_ptr<std::list<OA::ExprHandle> > exprList;
        exprList = new std::list<OA::ExprHandle>(mCallsiteParams[h]);
        retval = new TestIRExprIterator(exprList);
        return retval;
      }

    //! returns true if given symbol is a pass by reference parameter 
    bool isRefParam(OA::SymHandle sym)
      { return mFormalParamIsRef[sym]; }
               
    //! return the formal parameter that an actual parameter is associated with 
    OA::SymHandle getFormalForActual(OA::ExprHandle call, OA::ExprHandle param)
      { return mCallsiteParamFormal[param]; }
 
    //! For the given symbol create a Location that indicates statically
    //! overlapping locations and information about whether the location
    //! is local or not
    OA::OA_ptr<OA::Location::Location> getLocation(OA::ProcHandle proc,
                                                   OA::SymHandle s)
      { return mSymToLoc[proc][s]; }

    //! Given an ExprHandle, return an ExprTree 
    OA::OA_ptr<OA::ExprTree::ExprTree> getExprTree(OA::ExprHandle h)
      { return mCallsiteParamExprTree[h]; }

    
    //*****************************************************************
    // Construction methods 
    //*****************************************************************
    void addFormalParam(OA::SymHandle formal, bool isRef);
    void addCallsiteParam(OA::ExprHandle call, OA::ExprHandle param);
    void addCallsiteParamInfo(OA::ExprHandle param, OA::SymHandle formal, 
                              OA::OA_ptr<OA::ExprTree::ExprTree> etree);
    void addSym(OA::ProcHandle, OA::SymHandle,OA::OA_ptr<OA::Location::Location>);


    void dump(std::ostream& os);

};

#endif
