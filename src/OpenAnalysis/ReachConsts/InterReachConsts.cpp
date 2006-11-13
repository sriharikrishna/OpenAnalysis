/*! \file
  
  \brief Definition for InterReachConsts

  \authors Michelle Strout, Barbara Kreaseck
  \version $Id: InterReachConsts.cpp,v 1.2 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "InterReachConsts.hpp"

namespace OA {
  namespace ReachConsts {

//! Returns ReachConsts results for the given procedure
OA_ptr<ReachConsts::Interface> InterReachConsts::getReachConsts(ProcHandle proc)
      { if (mProcToReachConstsMap[proc].ptrEqual(0)) {
          OA_ptr<ReachConsts::Interface> empty;
          empty = new ReachConsts::ReachConstsStandard(proc);
          mProcToReachConstsMap[proc] = empty;
        }
        return mProcToReachConstsMap[proc];
      }


//*****************************************************************
// Output
//*****************************************************************
//! debugging output
void InterReachConsts::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
        os << "====================== InterDep" << std::endl;
        std::map<ProcHandle,OA_ptr<ReachConsts::Interface> >::iterator mapIter;
        for (mapIter=mProcToReachConstsMap.begin();
             mapIter!=mProcToReachConstsMap.end();
             mapIter++) 
        {
            os << "----------------------- proc results ----" << std::endl;
            OA_ptr<ReachConsts::Interface> results = mapIter->second;
            results->dump(os,ir);
            os << "-------------------------------" << std::endl;
        }


}

  } // end of ReachConsts namespace
} // end of OA namespace


