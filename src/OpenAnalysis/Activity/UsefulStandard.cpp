/*! \file
  
  \brief Implementation of Activity::UsefulStandard

  \author Michelle Strout
  \version $Id: UsefulStandard.cpp,v 1.6 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "UsefulStandard.hpp"

namespace OA {
  namespace Activity {

UsefulStandard::UsefulStandard(ProcHandle p)
{ 
    mDepLocSet = new DataFlow::LocDFSet; 
    mFinalInUseful = new DataFlow::LocDFSet;
    //mNumIter = 0;
}

OA_ptr<LocIterator> UsefulStandard::getDepSetIterator() 
{ 
    OA_ptr<LocIterator> retval;
    retval = new DataFlow::LocDFSetIterator(*mDepLocSet);
    return retval;
}

OA_ptr<LocIterator> UsefulStandard::getInUsefulIterator(StmtHandle s)
{
    OA_ptr<LocIterator> retval;
    if (mInUseful[s].ptrEqual(0)) {
        mInUseful[s] = new DataFlow::LocDFSet;
    }
    retval = new DataFlow::LocDFSetIterator(*mInUseful[s]);
    return retval;
}

OA_ptr<DataFlow::LocDFSet> UsefulStandard::getFinalUseful()
{
    return mFinalInUseful;
}

OA_ptr<DataFlow::LocDFSet> UsefulStandard::getOutUsefulSet(ExprHandle call)
{
    if (mOutUseful.find(call)!=mOutUseful.end()) {
        return mOutUseful[call];
    } else {
        OA_ptr<DataFlow::LocDFSet> emptySet; 
        emptySet = new DataFlow::LocDFSet;
        return emptySet;
    }
}
//*****************************************************************
// Construction methods 
//*****************************************************************

void UsefulStandard::mapFinalUseful(OA_ptr<DataFlow::LocDFSet> final)
{
    mFinalInUseful = final;
}

//! incomplete output of info for debugging
void UsefulStandard::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    os << "=================================== UsefulStandard" << std::endl;
    os << "DepSet = " << std::endl;
    mDepLocSet->dump(os,ir);

    // iterate over all stmts we know about
    std::map<StmtHandle, 
             OA_ptr<DataFlow::LocDFSet> >::iterator mapIter;
    for (mapIter = mInUseful.begin(); mapIter != mInUseful.end(); mapIter++) {
        StmtHandle s = mapIter->first;

        os << "StmtHandle(" << s.hval() << ") " << ir->toString(s) 
           << std::endl << "\tInUseful: " << std::endl;
        mapIter->second->dump(os,ir);
    }
}



  } // end of Activity namespace
} // end of OA namespace
