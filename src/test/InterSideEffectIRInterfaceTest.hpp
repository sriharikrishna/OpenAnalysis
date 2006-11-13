/*! \file
    
    \brief Test IR interface for interprocedural side-effect analysis
        
    \authors Michelle Strout
    \version $Id: InterSideEffectIRInterfaceTest.hpp,v 1.1 2004/11/30 13:51:33 mstrout Exp $
 
     Copyright (c) 2002-2004, Rice University
     Copyright (c) 2004, University of Chicago  
     All rights reserved.
     See ../../../Copyright.txt for details.
 
*/

#include <OpenAnalysis/IRInterface/InterSideEffectIRInterfaceDefault.hpp>
#include "IRHandlesIRInterfaceTest.hpp"
#include "CallGraphDFProblemIRInterfaceTest.hpp"
#include "SideEffectIRInterfaceTest.hpp"

#ifndef InterSideEffectIRInterfaceTest_HPP
#define InterSideEffectIRInterfaceTest_HPP



/*! methods needed to build the TestIR and implement
    required IRInterfaces 
*/
class InterSideEffectIRInterfaceTest 
    : public OA::SideEffect::InterSideEffectIRInterfaceDefault
{ 
  private:
    OA::OA_ptr<SideEffectIRInterfaceTest> mSideEffectIR;
    OA::OA_ptr<CallGraphDFProblemIRInterfaceTest> mCallGraphDFProblemIR;

  public:
    InterSideEffectIRInterfaceTest(OA::OA_ptr<SideEffectIRInterfaceTest> seIR,
            OA::OA_ptr<CallGraphDFProblemIRInterfaceTest> cgdfpIR)
        : mSideEffectIR(seIR), mCallGraphDFProblemIR(cgdfpIR) {}
    ~InterSideEffectIRInterfaceTest() {}

    //*****************************************************************
    // Implementation of IRHandlesIRInterface
    //*****************************************************************
    virtual std::string toString(OA::ProcHandle h) 
    { return mSideEffectIR->toString(h); }

    virtual std::string toString(OA::StmtHandle h) 
    { return mSideEffectIR->toString(h); }

    virtual std::string toString(OA::MemRefHandle h) 
    { return mSideEffectIR->toString(h); }

    virtual std::string toString(OA::SymHandle h) 
    { return mSideEffectIR->toString(h); }

    virtual std::string toString(OA::ExprHandle h) 
    { return mSideEffectIR->toString(h); }

    virtual std::string toString(OA::ConstSymHandle h) 
    { return mSideEffectIR->toString(h); }

    virtual std::string toString(OA::ConstValHandle h) 
    { return mSideEffectIR->toString(h); }

    virtual std::string toString(OA::OpHandle h) 
    { return mSideEffectIR->toString(h); }

    virtual void dump(OA::MemRefHandle h, std::ostream& os)
    { mSideEffectIR->dump(h,os); }

    virtual void dump(OA::StmtHandle h, std::ostream& os)
    { mSideEffectIR->dump(h,os); }

    //*****************************************************************
    // Implementation of CallGraphDFProblemIRInterface
    //*****************************************************************
    //! Get IRCallsiteParamIterator for a callsite.
    //! Iterator visits actual parameters in called order.
    OA::OA_ptr<OA::IRCallsiteParamIterator> getCallsiteParams(OA::ExprHandle h)
    { return mCallGraphDFProblemIR->getCallsiteParams(h); }
    
    //! returns true if given symbol is a pass by reference parameter 
    bool isRefParam(OA::SymHandle sym)
    { return mCallGraphDFProblemIR->isRefParam(sym); }
               
    //! return the formal parameter that an actual parameter is associated with 
    OA::SymHandle getFormalForActual(OA::ExprHandle call, OA::ExprHandle param)
    { return mCallGraphDFProblemIR->getFormalForActual(call,param); }
 
    //! For the given symbol create a Location that indicates statically
    //! overlapping locations and information about whether the location
    //! is local or not
    OA::OA_ptr<OA::Location::Location> getLocation(OA::ProcHandle proc,
                                                   OA::SymHandle s)
    { return mCallGraphDFProblemIR->getLocation(proc,s); }

    //! Given an ExprHandle, return an ExprTree 
    OA::OA_ptr<OA::ExprTree::ExprTree> getExprTree(OA::ExprHandle h)
    { return mCallGraphDFProblemIR->getExprTree(h); }
    
    //*****************************************************************
    // Implementation of SideEffectIRInterface
    //*****************************************************************
    
    //! Given a subprogram return an IRStmtIterator for the entire
    //! subprogram
    OA::OA_ptr<OA::IRStmtIterator> getStmtIterator(OA::ProcHandle h)
    { return mSideEffectIR->getStmtIterator(h); }

    //! Return a list of all the target memory reference handles that appear
    //! in the given statement.
    OA::OA_ptr<OA::MemRefHandleIterator> getDefMemRefs(OA::StmtHandle stmt)
    { return mSideEffectIR->getDefMemRefs(stmt); }
  
    //! Return a list of all the source memory reference handles that appear
    //! in the given statement.
    OA::OA_ptr<OA::MemRefHandleIterator> getUseMemRefs(OA::StmtHandle stmt)
    { return mSideEffectIR->getUseMemRefs(stmt); }
  
    //! Return an iterator over all of the callsites in a given stmt
    OA::OA_ptr<OA::IRCallsiteIterator> getCallsites(OA::StmtHandle h)
    { return mSideEffectIR->getCallsites(h); }


    void dump(std::ostream& os);

};

#endif
