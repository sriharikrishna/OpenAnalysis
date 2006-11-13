/*! \file
    
    \brief Test IR implementation for  IRHandlesIRInterface
        
    \authors Michelle Strout
    \version $Id: IRHandlesIRInterfaceTest.cpp,v 1.4 2004/12/03 14:01:34 mstrout Exp $
 
     Copyright (c) 2002-2004, Rice University
     Copyright (c) 2004, University of Chicago  
     All rights reserved.
     See ../../../Copyright.txt for details.
 
*/

#include "IRHandlesIRInterfaceTest.hpp"

//*****************************************************************
// Implementation of IRHandlesIRInterface
//*****************************************************************
std::string IRHandlesIRInterfaceTest::toString(OA::ProcHandle h) 
{ return mProcString[h]; }

std::string IRHandlesIRInterfaceTest::toString(OA::StmtHandle h) 
{ return mStmtString[h]; }

std::string IRHandlesIRInterfaceTest::toString(OA::MemRefHandle h) 
{ return mMemRefString[h]; }

std::string IRHandlesIRInterfaceTest::toString(OA::SymHandle h) 
{ return mSymString[h]; }

std::string IRHandlesIRInterfaceTest::toString(OA::ExprHandle h) 
{ return mExprString[h]; }

std::string IRHandlesIRInterfaceTest::toString(OA::ConstSymHandle h) 
{ return mConstSymString[h]; }

std::string IRHandlesIRInterfaceTest::toString(OA::ConstValHandle h) 
{ return mConstValString[h]; }

std::string IRHandlesIRInterfaceTest::toString(OA::OpHandle h) 
{ return mOpString[h]; }

void IRHandlesIRInterfaceTest::dump(OA::MemRefHandle h, std::ostream& os)
{ os << "MemRefHandle(hval=" << h.hval() << ", " << toString(h) 
     << ")" << std::endl;
}

void IRHandlesIRInterfaceTest::dump(OA::StmtHandle h, std::ostream& os)
{ os << "StmtHandle(hval=" << h.hval() << ", " << toString(h) 
     << ")" << std::endl;
}

//*****************************************************************
// Extra methods for testing framework
//*****************************************************************
OA::OA_ptr<OA::IRProcIterator> IRHandlesIRInterfaceTest::getProcIterator()
{
  OA::OA_ptr<TestIRProcIterator> retval;
  OA::OA_ptr<std::list<OA::ProcHandle> > listptr;
  listptr = new std::list<OA::ProcHandle>; 
  
  // loop through all procedures we know about
  std::map<OA::ProcHandle,std::string>::iterator mapIter;
  for (mapIter=mProcString.begin();
       mapIter!=mProcString.end(); mapIter++)
  {
      listptr->push_back(mapIter->first);
  }
  retval = new TestIRProcIterator( listptr );
  return retval;
}

//*****************************************************************
// Construction methods 
//*****************************************************************
void IRHandlesIRInterfaceTest::addProcString(OA::ProcHandle proc,std::string str)
  {
    mProcString[proc] = str;
  }

void IRHandlesIRInterfaceTest::addStmtString(OA::StmtHandle stmt, std::string str)
  {
    mStmtString[stmt] = str;
  }

void IRHandlesIRInterfaceTest::addMemRefString(OA::MemRefHandle memref,std::string str)
  {
    mMemRefString[memref] = str;
  }

void IRHandlesIRInterfaceTest::addSymString(OA::SymHandle sym,std::string str)
  {
    mSymString[sym] = str;
  }

void IRHandlesIRInterfaceTest::addExprString(OA::ExprHandle expr,std::string str)
  {
    mExprString[expr] = str;
  }

void IRHandlesIRInterfaceTest::addOpString(OA::OpHandle h,std::string str)
  {
    mOpString[h] = str;
  }

void IRHandlesIRInterfaceTest::addConstValString(OA::ConstValHandle h,
                                                 std::string str)
  {
    mConstValString[h] = str;
  }

void IRHandlesIRInterfaceTest::addConstSymString(OA::ConstSymHandle h,
                                                 std::string str)
  {
    mConstSymString[h] = str;
  }

