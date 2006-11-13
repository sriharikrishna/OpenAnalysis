/*! \file
  
  \brief Definitions for MemRefBasic.

  \authors Michelle Strout
  \version $Id: MemRefExprBasic.cpp,v 1.9 2005/03/17 21:47:46 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  Deprecated
*/

#include "MemRefExprBasic.hpp"

namespace OA {

const std::string mMemRefHandleTypeString[] = {"SOURCE", "TARGET", "SUB"};
const std::string mAccuracyTypeString[] = {"FULL", "PARTIAL"};

/*
bool MemRefExprBasic::baseLocRef(MemRefExprBasic* other) 
{
    if (!other->hasAddressTaken() && other->getNumDerefs()==0 
        && *mLocBlock == *(other->mLocBlock)) 
    {
        return true;
    } else {
        return false;
    }
}
*/

void MemRefExprBasic::output(OA_ptr<IRHandlesIRInterface> pIR) 
{
    mPretty = false;
    outputBoth(pIR);
}

void MemRefExprBasic::outputPretty(OA_ptr<IRHandlesIRInterface> pIR) 
{
    mPretty = true;
    outputBoth(pIR);
}

void MemRefExprBasic::dump(std::ostream& os, IRHandlesIRInterface& pIR)
{
    //os << pIR->toString(mMemRefHandle);
    pIR.dump(mMemRefHandle,os);
    os << " < " << mMemRefHandleTypeString[mMemRefHandleType] << ", ";
    os << "< ";
    //mLocBlock->dump(os,pIR);
    os << ">, ";
    os << mNumDerefs << ", ";
    os << mAddressOf << ", ";
    os << mAccuracyTypeString[mAccuracy] << " >";
}

void MemRefExprBasic::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
    dump(os,*pIR);
}

std::string 
MemRefExprBasic::genIRHandleString(MemRefHandle h, 
                                   OA_ptr<IRHandlesIRInterface> pIR) 
{
    std::ostringstream oss;

    if (mPretty) {
        //return pIR->toString(h);
        pIR->dump(h,oss);
    } else {
        // ir.getPersistantString(h, oss);
    }
    //return "";
    return oss.str();
}


void MemRefExprBasic::outputBoth(OA_ptr<IRHandlesIRInterface>  pIR) 
{
/*
    mOB.objStart("MemRefExprBasic");    
    
    // mMemRefHandle field
    mOB.fieldName("mMemRefHandle");
    //mOB.fieldValue( (std::string)mMemRefHandle );
    mOB.fieldValue( genIRHandleString(mMemRefHandle,pIR) );
    mOB.fieldDelimit();

    // mMemRefHandleType field
    mOB.fieldName("mMemRefHandleType");
    switch (mMemRefHandleType)
    {
        case SOURCE: 
            mOB.fieldValue("SOURCE");
            break;
        case TARGET: 
            mOB.fieldValue("TARGET");
            break;
        case SUB: 
            mOB.fieldValue("SUB");
            break;
    }
    mOB.fieldDelimit();
 */  
    /*
    // mLocBlock field
    mOB.fieldName("mLocBlock");
    if (mLocBlock == NULL ) {
        mOB.fieldValue("UNKNOWN");
    } else {
        mOB.fieldValueBegin();
        mLocBlock->output(pIR);
        mOB.fieldValueEnd();
    }
    */
/*
    mOB.fieldDelimit();

    // mNumDerefs field
    mOB.fieldName("mNumDerefs");
    mOB.fieldValue( int2string(mNumDerefs) );

    mOB.fieldDelimit();


    // mAddressOf field
    mOB.fieldName("mAddressOf");
    mOB.fieldValue( bool2string(mAddressOf) );

    mOB.fieldDelimit();

    // mAccuracy field
    mOB.fieldName("mAccuracy");
    switch (mAccuracy)
    {
        case FULL: 
            mOB.fieldValue("FULL");
            break;
        case PARTIAL: 
            mOB.fieldValue("PARTIAL");
            break;
    }

    mOB.objEnd();
*/
}


} // end of OA namespace


