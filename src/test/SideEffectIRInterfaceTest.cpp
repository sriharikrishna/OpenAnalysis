#include "SideEffectIRInterfaceTest.hpp"

//*****************************************************************
// Implementation of SideEffectIRInterface
//*****************************************************************
    
//! Given a subprogram return an IRStmtIterator for the entire
//! subprogram
OA::OA_ptr<OA::IRStmtIterator> 
SideEffectIRInterfaceTest::getStmtIterator(OA::ProcHandle h) 
{
  OA::OA_ptr<TestIRStmtIterator> retval;
  OA::OA_ptr<std::list<OA::StmtHandle> > listptr;
  listptr = new std::list<OA::StmtHandle>(mProcBody[h]); 
  retval = new TestIRStmtIterator( listptr ); 
  return retval;
}


//! Return a list of all the target memory reference handles that appear
//! in the given statement.
OA::OA_ptr<OA::MemRefHandleIterator> 
SideEffectIRInterfaceTest::getDefMemRefs(OA::StmtHandle stmt)
{ 
  OA::OA_ptr<TestIRMemRefIterator> retval;
  OA::OA_ptr<std::list<OA::MemRefHandle> > listptr;
  listptr = new std::list<OA::MemRefHandle>(mStmtToDefMemRefs[stmt]); 
  retval = new TestIRMemRefIterator( listptr ); 
  return retval;
}

  
//! Return a list of all the source memory reference handles that appear
//! in the given statement.
OA::OA_ptr<OA::MemRefHandleIterator> 
SideEffectIRInterfaceTest::getUseMemRefs(OA::StmtHandle stmt)
{ 
  OA::OA_ptr<TestIRMemRefIterator> retval;
  OA::OA_ptr<std::list<OA::MemRefHandle> > listptr;
  listptr = new std::list<OA::MemRefHandle>(mStmtToUseMemRefs[stmt]); 
  retval = new TestIRMemRefIterator( listptr ); 
  return retval;
}

//! Return an iterator over all of the callsites in a given stmt
OA::OA_ptr<OA::IRCallsiteIterator> 
SideEffectIRInterfaceTest::getCallsites(OA::StmtHandle h)
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
void SideEffectIRInterfaceTest::addStmt(OA::ProcHandle proc, 
                                        OA::StmtHandle stmt)
{
    mProcBody[proc].push_back(stmt);
}

void SideEffectIRInterfaceTest::addUseMemRef(OA::StmtHandle stmt, 
                                             OA::MemRefHandle memref)
{
    mStmtToUseMemRefs[stmt].push_back(memref);
}

void SideEffectIRInterfaceTest::addDefMemRef(OA::StmtHandle stmt, 
                                             OA::MemRefHandle memref)
{
    mStmtToDefMemRefs[stmt].push_back(memref);
}

void SideEffectIRInterfaceTest::addCallsite(OA::StmtHandle stmt, 
                                            OA::ExprHandle expr)
{
    mStmtToCallsites[stmt].push_back(expr);
}



void SideEffectIRInterfaceTest::dump(std::ostream& os)
{
    os << "SideEffectIRInterfaceTest::dump(std::ostream&) is NOT implemented yet";
    os << std::endl;
}


