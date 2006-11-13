/*! \file
  
  \brief Implementation of ParamBindings

  \authors Michelle Strout 
  \version $Id: ParamBindings.cpp,v 1.2 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ParamBindings.hpp"

namespace OA {
  namespace DataFlow {

//! get caller mem ref associated with callee formal
MemRefHandle 
ParamBindings::getCallerMemRef(ExprHandle call, SymHandle calleeFormal)
{
    return mCalleeToCallerMap[call][calleeFormal];
}

//! returns an iterator over the memory reference parameters for a call
OA_ptr<MemRefHandleIterator> ParamBindings::getActualIterator(ExprHandle call)
{
    OA_ptr<MemRefIterator> retval;
    OA_ptr<std::set<OA::MemRefHandle> > retSet;
    OA_ptr<std::set<OA::MemRefHandle> > memrefSet = mCallToActualSetMap[call];
    if (memrefSet.ptrEqual(0)) {
        retSet = new std::set<OA::MemRefHandle>;
    } else {
        retSet = mCallToActualSetMap[call];
    }
    retval = new MemRefIterator(retSet);
    return retval;
}

//! get callee formal associated with caller mem ref
SymHandle 
ParamBindings::getCalleeFormal(ExprHandle call, MemRefHandle callerRef)
{
    return mCallerToCalleeMap[call][callerRef];
}

//! returns an iterator over the formal symbols for a procedure
OA_ptr<SymHandleIterator> ParamBindings::getFormalIterator(ProcHandle proc)
{
    OA_ptr<SymIterator> retval;
    if (mProcToFormalSetMap[proc].ptrEqual(0)) {
        OA_ptr<std::set<SymHandle> > emptySet;
        emptySet = new std::set<SymHandle>;
        retval = new SymIterator(emptySet);
    } else {
        retval = new SymIterator(mProcToFormalSetMap[proc]);
    }
    return retval;
}

//! returns true if given formal is a reference parameter
bool ParamBindings::isRefParam(SymHandle formal)
{
    return mIsRefParam[formal];
}

//*****************************************************************
// Construction methods 
//*****************************************************************
void ParamBindings::mapMemRefToFormal(ExprHandle call, 
                                      MemRefHandle ref, SymHandle sym)
{
    mCallerToCalleeMap[call][ref] = sym;
    mCalleeToCallerMap[call][sym] = ref;
}

void ParamBindings::setRefParam(SymHandle sym) 
{
    mIsRefParam[sym] = true;
}

void ParamBindings::mapFormalToProc(SymHandle formal, ProcHandle proc) 
{
    if (mProcToFormalSetMap[proc].ptrEqual(0)) {
        mProcToFormalSetMap[proc] = new std::set<SymHandle>;
    }
    mProcToFormalSetMap[proc]->insert(formal);
}

void ParamBindings::mapActualToCall(MemRefHandle actual, ExprHandle call) 
{
    if (mCallToActualSetMap[call].ptrEqual(0)) {
        mCallToActualSetMap[call] = new std::set<MemRefHandle>;
    }
    mCallToActualSetMap[call]->insert(actual);
}


void ParamBindings::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    os << "====================== ParamBindings" << std::endl;

    // iterate over all the calls we have information about
    std::map<ExprHandle,std::map<MemRefHandle,SymHandle> >::iterator callIter;
    for (callIter=mCallerToCalleeMap.begin();
         callIter!=mCallerToCalleeMap.end(); callIter++) 
    {
        os << "Call = " << ir->toString(callIter->first) << std::endl;
        std::map<MemRefHandle,SymHandle>::iterator memRefIter;
        for (memRefIter=callIter->second.begin();
             memRefIter!=callIter->second.end(); memRefIter++)
        {
            MemRefHandle ref = memRefIter->first;
            SymHandle sym = memRefIter->second;
            os << "\tCaller MemRef = " << ir->toString(ref);
            os << ", Callee Formal = " << ir->toString(sym);
            os << std::endl;
            os << "\t\tFormal isRefParam = " << isRefParam(sym);
            os << std::endl;
        }
    }

}

  } // end of DataFlow namespace
} // end of OA namespace

