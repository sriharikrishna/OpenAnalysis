#include "CallGraphIRInterfaceTest.hpp"

//*****************************************************************
// Implementation of CallGraphIRInterface
//*****************************************************************
    
//! Given a subprogram return an IRStmtIterator for the entire
//! subprogram
OA::OA_ptr<OA::IRStmtIterator> 
CallGraphIRInterfaceTest::getStmtIterator(OA::ProcHandle h) 
{
  OA::OA_ptr<TestIRStmtIterator> retval;
  OA::OA_ptr<std::list<OA::StmtHandle> > listptr;
  listptr = new std::list<OA::StmtHandle>(mProcBody[h]); 
  retval = new TestIRStmtIterator( listptr ); 
  return retval;
}

//! Return an iterator over all of the callsites in a given stmt
OA::OA_ptr<OA::IRCallsiteIterator> 
CallGraphIRInterfaceTest::getCallsites(OA::StmtHandle h)
{
  OA::OA_ptr<TestIRExprIterator> retval;
  OA::OA_ptr<std::list<OA::ExprHandle> > listptr;
  listptr = new std::list<OA::ExprHandle>(mStmtToCallsites[h]); 
  retval = new TestIRExprIterator( listptr ); 
  return retval;
}



//*****************************************************************
// Construction methods 
//*****************************************************************

void CallGraphIRInterfaceTest::mapProcToSym(OA::ProcHandle p, OA::SymHandle s)
{
    mProcToSym[p]  = s;
}

void CallGraphIRInterfaceTest::addStmtCallsite(OA::ProcHandle p, 
        OA::StmtHandle s, OA::ExprHandle call)
{
    mProcBody[p].push_back(s);
    mStmtToCallsites[s].push_back(call);
}

void CallGraphIRInterfaceTest::addCallSym(OA::ExprHandle call, OA::SymHandle s)
{
    mCallsiteSym[call] = s;
}

void CallGraphIRInterfaceTest::dump(std::ostream& os)
{
    std::cout << "CallGraphIRInterfaceTest::dump not implemented yet!";
    std::cout << std::endl;
}


