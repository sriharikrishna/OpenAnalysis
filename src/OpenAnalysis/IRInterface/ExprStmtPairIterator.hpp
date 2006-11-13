/*! \file
  
  \brief Abstract iterator for pairs of MemRefHandle and ExprHandles
         where MemRefHandle = ExprHandle.
  
  \authors Michelle Strout
  \version $Id: ExprStmtPairIterator.hpp,v 1.1 2004/12/03 14:01:33 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef ExprStmtPairIterator_h
#define ExprStmtPairIterator_h

#include <OpenAnalysis/IRInterface/IRHandles.hpp>

namespace OA {

class ExprStmtPairIterator {
  public:
    ExprStmtPairIterator() {}
    virtual ~ExprStmtPairIterator() {}

    //! right hand side
    virtual ExprHandle currentSource() const = 0;
    //! left hand side
    virtual MemRefHandle currentTarget() const = 0;

    virtual bool isValid() const = 0;
                    
    virtual void operator++() = 0;
    void operator++(int) { ++*this; }
};


} // end of namespace OA

#endif 
