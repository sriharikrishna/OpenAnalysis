/*! \file
  
  \brief Implementation of Activity::InActivePerStmt

  \author Michelle Strout
  \version $Id: InActivePerStmt.cpp,v 1.1 2005/07/01 02:49:56 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "InActivePerStmt.hpp"

namespace OA {
  namespace Activity {

InActivePerStmt::InActivePerStmt() 
{ 
}


OA_ptr<DataFlow::LocDFSet> InActivePerStmt::getInActiveSet(StmtHandle stmt)
{
    if (mInActive.find(stmt)!=mInActive.end()) {
        return mInActive[stmt];
    } else {
        OA_ptr<DataFlow::LocDFSet> emptySet; 
        emptySet = new DataFlow::LocDFSet;
        return emptySet;
    }
}

//*****************************************************************
// Construction methods 
//*****************************************************************

void InActivePerStmt::copyIntoInActive(StmtHandle s, 
                                   OA_ptr<DataFlow::LocDFSet> dfset)
{
    OA_ptr<DataFlow::DataFlowSet> cloneDFset = dfset->clone();
    mInActive[s] = cloneDFset.convert<DataFlow::LocDFSet>();
}

   
//*****************************************************************
// Output
//*****************************************************************

//! incomplete output of info for debugging
void InActivePerStmt::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    os << "============================ InActivePerStmt" << std::endl;

    // iterate over all stmts we know about
    std::map<StmtHandle, 
             OA_ptr<DataFlow::LocDFSet> >::iterator mapIter;
    for (mapIter = mInActive.begin(); mapIter != mInActive.end(); mapIter++) {
        StmtHandle s = mapIter->first;

        os << "StmtHandle(" << s.hval() << ") " << ir->toString(s) 
           << std::endl << "\tInActive: " << std::endl;
        mapIter->second->dump(os,ir);
    }
}



  } // end of Activity namespace
} // end of OA namespace
