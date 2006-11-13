#include "AccessLocIRInterfaceTest.hpp"

//*****************************************************************
// Implementation of AccessLocIRInterface
//*****************************************************************
OA::OA_ptr<OA::IRSymIterator> 
AccessLocIRInterfaceTest::getAccessibleSymIterator(OA::ProcHandle h)
{
  OA::OA_ptr<TestIRSymIterator> retval;
  OA::OA_ptr<std::list<OA::SymHandle> > listptr;
  listptr = new std::list<OA::SymHandle>; 
  
  // loop through all accessible syms
  std::map<OA::SymHandle,
           OA::OA_ptr<OA::Location::Location> >::iterator mapIter;
  for (mapIter=mAccessibleSymToLoc[h].begin();
       mapIter!=mAccessibleSymToLoc[h].end(); mapIter++)
  {
      listptr->push_back(mapIter->first);
  }
  retval = new TestIRSymIterator( listptr );
  return retval;
}

OA::OA_ptr<OA::IRStmtIterator> 
AccessLocIRInterfaceTest::getUnnamedDynAllocStmtIterator(OA::ProcHandle h)
{
    OA::OA_ptr<TestIRStmtIterator> retval;
    OA::OA_ptr<std::list<OA::StmtHandle> > listptr;
    listptr = new std::list<OA::StmtHandle>(mUnnamedDynAllocStmts[h]) ; 
    retval = new TestIRStmtIterator( listptr );
    return retval;
}

OA::OA_ptr<OA::MemRefExpr::MemRefExprIterator> 
AccessLocIRInterfaceTest::getPossMemRefExprIterator(OA::SymHandle h)
{
    OA::OA_ptr<TestIRMemRefExprIterator> retval;
    OA::OA_ptr<std::list<OA::OA_ptr<OA::MemRefExpr::MemRefExpr> > > listptr;
    listptr 
      = new std::list<OA::OA_ptr<OA::MemRefExpr::MemRefExpr> >(mSymToMRE[h]);
     retval = new TestIRMemRefExprIterator( listptr );
     return retval;
}

OA::OA_ptr<OA::IRFormalParamIterator> 
AccessLocIRInterfaceTest::getFormalParamIterator(OA::SymHandle h)
{
  OA::OA_ptr<TestIRSymIterator> retval;
  OA::OA_ptr<std::list<OA::SymHandle> > listptr;
  listptr = new std::list<OA::SymHandle>(mProcSymToFormalParams[h]); 
  retval = new TestIRSymIterator( listptr );
  return retval;
}

//*****************************************************************
// Construction methods 
//*****************************************************************

void AccessLocIRInterfaceTest::mapProcToSym(OA::ProcHandle p, OA::SymHandle s)
{
    mProcToSym[p] = s;
}

void AccessLocIRInterfaceTest::addSym(OA::ProcHandle proc, OA::SymHandle sym, 
        OA::OA_ptr<OA::Location::Location> loc)
{
    mAccessibleSymToLoc[proc][sym] = loc;
}

void AccessLocIRInterfaceTest::addFormalParam(OA::SymHandle proc,
                                              OA::SymHandle param)
{
    mProcSymToFormalParams[proc].push_back(param);
}

void AccessLocIRInterfaceTest::addUnnamedDynAllocStmt(OA::ProcHandle p, 
                                                      OA::StmtHandle s)
{
    mUnnamedDynAllocStmts[p].push_back(s);
}

void AccessLocIRInterfaceTest::addMemRefExpr(OA::SymHandle s, 
        OA::OA_ptr<OA::MemRefExpr::MemRefExpr> mre)
{
    mSymToMRE[s].push_back(mre);
}


void AccessLocIRInterfaceTest::dump(std::ostream& os)
{
    os << "AccessLocIRInterfaceTest::dump(std::ostream&) is NOT implemented yet";
    os << std::endl;
}


