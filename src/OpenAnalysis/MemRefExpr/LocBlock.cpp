/*! \file
  
  \brief Definition for LocBlock class.  

  \authors Michelle Strout
  \version $Id: LocBlock.cpp,v 1.9 2005/03/17 21:47:46 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  Deprecated
*/

#include "LocBlock.hpp"

namespace OA {

void LocBlock::output(IRHandlesIRInterface & pIR) 
{
    mPretty = false;
    outputBoth(pIR);
}

void LocBlock::outputPretty(IRHandlesIRInterface & pIR) 
{
    mPretty = true;
    outputBoth(pIR);
}

std::string 
LocBlock::genIRHandleString(SymHandle h, IRHandlesIRInterface &pIR) 
{
    if (mPretty) {
        return pIR.toString(h);
    } else {
        // ir.getPersistantString(h, oss);
	return "";
    }
}
void LocBlock::outputBoth(IRHandlesIRInterface &pIR)
{
/*
    mOB.objStart("LocBlock");    
    
    // determine whether or not named
    if (mNamed) {
        mOB.fieldName("mSymHandle");
        mOB.fieldValue( genIRHandleString(mSymHandle,pIR) );
    } else {
        mOB.fieldName("mFilename");
        mOB.fieldValue( mFilename );
        mOB.fieldDelimit();
    }

    mOB.fieldDelimit();

    // mLocal field
    mOB.fieldName("mLocal");
    mOB.fieldValue( bool2string(mLocal) );

    mOB.fieldDelimit();


    // mNamed field
    mOB.fieldName("mNamed");
    mOB.fieldValue( bool2string(mNamed) );

    mOB.fieldDelimit();
    
    
    // mUnique field
    mOB.fieldName("mUnique");
    mOB.fieldValue( bool2string(mUnique) );
    mOB.fieldDelimit();
*/
    /*
    // mFullOverlap list
    mOB.fieldName("mFullOverlap");
    mOB.listBegin();
    std::list<LocBlock*>::iterator iter; 
    for (iter=mFullOverlap.begin(); iter != mFullOverlap.end(); ++iter) {
        mOB.listItemBegin(); 
        (*iter)->output(pIR);
        mOB.listItemEnd();
        mOB.listItemDelimit();
    }
    mOB.listEnd();
    mOB.fieldDelimit();

    // mPartOverlap list
    mOB.fieldName("mPartOverlap");
    mOB.listBegin();
    for (iter=mPartOverlap.begin(); iter != mPartOverlap.end(); ++iter) {
        mOB.listItemBegin(); 
        (*iter)->output(pIR);
        mOB.listItemEnd();
        mOB.listItemDelimit();
    }
    mOB.listEnd();
    */

 //   mOB.objEnd();

}

void LocBlock::dump(std::ostream& os, IRHandlesIRInterface &pIR)
{
    if (mNamed) {
        // string for mSymHandle
        os << pIR.toString(mSymHandle);

    // for now assuming malloced
    } else {
        //os << "heap_{" << mLineNum << "}";
    }
    os << ", ";

    os << mLocal << ", " << mNamed << ", " << mUnique;
}

} // end of OA namespace

