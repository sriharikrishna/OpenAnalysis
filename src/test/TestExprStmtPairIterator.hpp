/*! \file
  
  \brief Iterator for pairs of MemRefHandle and ExprHandles
         where MemRefHandle = ExprHandle.
  
  \authors Michelle Strout
  \version $Id: TestExprStmtPairIterator.hpp,v 1.1 2004/12/03 14:01:34 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include <OpenAnalysis/IRInterface/ExprStmtPairIterator.hpp>

#ifndef TestExprStmtPairIterator_h
#define TestExprStmtPairIterator_h

struct TestExprStmtPair {
    TestExprStmtPair() {}
    TestExprStmtPair(OA::MemRefHandle m, OA::ExprHandle e) 
        : memref(m), expr(e) {}
    ~TestExprStmtPair() {}

    OA::MemRefHandle memref;
    OA::ExprHandle expr;
};

class TestExprStmtPairIterator : public OA::ExprStmtPairIterator 
{
  public:
    TestExprStmtPairIterator(std::list<TestExprStmtPair> pairList) 
        : mList(pairList), mIter(mList.begin()) { }
    ~TestExprStmtPairIterator() {}

    //! right hand side
    OA::ExprHandle currentSource() const 
      { if (isValid()) return (*mIter).expr;
        else return OA::ExprHandle(0);
      }
    //! left hand side
    OA::MemRefHandle currentTarget() const
      { if (isValid()) return (*mIter).memref;
        else return OA::MemRefHandle(0);
      }

    bool isValid() const { return mIter != mList.end(); }
                    
    void operator++() { if (isValid()) mIter++; }

  private:
    std::list<TestExprStmtPair> mList;
    std::list<TestExprStmtPair>::iterator mIter;
};


#endif 
