/*! \file
  
  \brief Maps StmtHandles to set of locations that are InActive.

  \authors Michelle Strout
  \version $Id: InActivePerStmt.hpp,v 1.1 2005/07/01 02:49:56 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef InActivePerStmt_hpp
#define InActivePerStmt_hpp

#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/DataFlow/LocDFSet.hpp>

namespace OA {
  namespace Activity {



class InActivePerStmt {
  public:
    InActivePerStmt();
    ~InActivePerStmt() {}

    //! returns a location data flow set of locations that 
    //! are active coming into the statement 
    OA_ptr<DataFlow::LocDFSet> getInActiveSet(StmtHandle s);

    //*****************************************************************
    // Construction methods 
    //*****************************************************************

    //! set the InActive set for the statement
    void copyIntoInActive(StmtHandle s, OA_ptr<DataFlow::LocDFSet> dfset);
    
    //! set number of iterations over all nodes
    void setNumIter(int n) { mNumIter = n; }

    //*****************************************************************
    // Output
    //*****************************************************************

    //! incomplete output of info for debugging
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);

  private:
    // data members
    std::map<StmtHandle,OA_ptr<DataFlow::LocDFSet> > mInActive;
    int mNumIter;
};

  } // end of Activity namespace
} // end of OA namespace

#endif

