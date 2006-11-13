/*! \file
  
  \brief DataflowSet that consists of a count.

  \authors Michelle Strout 
  \version $Id: CountDFSet.cpp,v 1.2 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "CountDFSet.hpp"

namespace OA {
  namespace DataFlow {

#if defined(DEBUG_ALL) || defined(DEBUG_CountDFSet)
static bool debug = true;
#else
static bool debug = false;
#endif


CountDFSet::CountDFSet() : mCount(0)
{ 
}

CountDFSet::CountDFSet(int count) : mCount(count)
{ 
}

CountDFSet::CountDFSet(const CountDFSet &other) : mCount(other.mCount)
{ 
}


OA_ptr<DataFlowSet> CountDFSet::clone()
{ 
  OA_ptr<CountDFSet> retval;
  retval = new CountDFSet(*this); 
  return retval; 
}
  
// param for these can't be const because will have to 
// dynamic cast to specific subclass
bool CountDFSet::operator ==(DataFlowSet &other) const
{ 
    CountDFSet& recastOther 
        = dynamic_cast<CountDFSet&>(other);
    return mCount == recastOther.mCount; 
}

bool CountDFSet::operator !=(DataFlowSet &other) const
{ 
    CountDFSet& recastOther 
        = dynamic_cast<CountDFSet&>(other);
    return mCount != recastOther.mCount; 
}


void CountDFSet::dump(std::ostream &os)
{
    os << "CountDFSet: mCount = " << mCount << std::endl;
}


void CountDFSet::dump(std::ostream &os, OA_ptr<IRHandlesIRInterface> ir)
{
    os << "CountDFSet: mCount = " << mCount << std::endl;
}

  } // end of DataFlow namespace
} // end of OA namespace

