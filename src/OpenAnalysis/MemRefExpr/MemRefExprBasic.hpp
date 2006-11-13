/*! \file
  
  \brief Keeps track of location block being accessed and how many times
         that location block is being dereferenced.

  \authors Michelle Strout
  \version $Id: MemRefExprBasic.hpp,v 1.16 2005/03/17 21:47:46 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  Deprecated
*/

#ifndef MemRefExprBasic_H
#define MemRefExprBasic_H

#include <iostream>
#include <string>
//#include <OpenAnalysis/MemRefExpr/Interface.hpp>
#include <OpenAnalysis/Utils/OutputBuilder.hpp>
//#include "../config.hpp"

#include <OpenAnalysis/MemRefExpr/LocBlock.hpp>

namespace OA {

typedef enum {FULL, PARTIAL} AccuracyType;
typedef enum {
    SOURCE,     // mem ref specifies where to find source location for use
    TARGET,     // mem ref specifies where to put result of assign
    SUB         // mem ref is a sub expression of another mem ref
} MemRefHandleType;

class MemRefExprBasic {
  public:
    // Memrefs involving named locations
    MemRefExprBasic(MemRefHandle h, MemRefHandleType hType, SymHandle sh,
                    int numDerefs, bool addressOf, AccuracyType accuracy) :
        mMemRefHandle(h), mMemRefHandleType(hType), mNamed(true), 
        mSymHandle(sh), mNumDerefs(numDerefs), mAddressOf(addressOf),
        mAccuracy(accuracy)//, mOB(defaultOutputBuilder)
        { }

    // Memrefs involving unnamed locations
    MemRefExprBasic(MemRefHandle h, MemRefHandleType hType, std::string filename,
                    StmtHandle sh,
                    int numDerefs, bool addressOf, AccuracyType accuracy) :
        mMemRefHandle(h), mMemRefHandleType(hType), mNamed(false), 
        mFilename(filename), mStmtHandle(sh),
        mNumDerefs(numDerefs), mAddressOf(addressOf),
        mAccuracy(accuracy)//, mOB(defaultOutputBuilder)
        { }


    virtual ~MemRefExprBasic() {}

    //! does the mem ref expr involve a local variable
    //bool isLocal() { return mLocBlock->isLocal(); }

    //! does the memref expr involve a named variable
    bool isNamed() { return mNamed; }

    //! return the MemRefHandle
    MemRefHandle getMemRefHandle() { return mMemRefHandle; }

    //! return the SymHandle
    SymHandle getSymHandle() { return mSymHandle; }

    //! is this a target mem ref
    bool isTarget() { return mMemRefHandleType==TARGET; }

    //! is this a source mem ref
    bool isSource() { return mMemRefHandleType==SOURCE; }

    //! does the mem ref expr have its address taken
    bool hasAddressTaken() { return mAddressOf; }

    //! how many derefs occur in the memory reference
    int getNumDerefs() { return mNumDerefs; }

    //! whether the memory reference expression full describes the ref
    int hasFullAccuracy() { return (mAccuracy == FULL); }

    //! get the location block
    //LocBlock* getLocBlock() { return mLocBlock; }

    //! return a new MemRefExpr that has the same loc block and
    //! no addressOf, etc
    //MemRefExprBasic* baseLocBlock() { return new MemRefExprBasic(mMemRefHandle,
    //    mMemRefHandleType, mLocBlock, 0, false, mAccuracy); } 
    
    //! determine if the given MRE other has the same location
    //! block as this, but has no derefs and no address taken
    //bool baseLocRef(MemRefExprBasic* other);

    //! set a different OutputBuilder, encapsulates stream and 
    //! format (txt, xml, etc)
    //void configOutput(OutputBuilder& ob) { mOB = ob; }
    //! will use OutputBuilder to generate output with persistent handles
    void output(OA_ptr<IRHandlesIRInterface> pIR);
    //! will use OutputBuilder to generate output with string reps of handles
    void outputPretty(OA_ptr<IRHandlesIRInterface> pIR);

    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);
    void dump(std::ostream& os, IRHandlesIRInterface& pIR);

    MemRefExprBasic& operator=(MemRefExprBasic& other) 
    {
        mMemRefHandle = other.mMemRefHandle;
        mMemRefHandleType = other.mMemRefHandleType;
        mNamed = other.mNamed;
        mSymHandle = other.mSymHandle;
        mFilename = other.mFilename;
        mStmtHandle = other.mStmtHandle;
        mNumDerefs = other.mNumDerefs;
        mAddressOf = other.mAddressOf;
        mAccuracy = other.mAccuracy;

        return *this;
    }

  private:
    // data members
    MemRefHandle mMemRefHandle; // memory reference this expr is associated with
    MemRefHandleType mMemRefHandleType;
    bool mNamed;
    SymHandle mSymHandle;
    std::string mFilename;
    StmtHandle mStmtHandle;
    int mNumDerefs;
    bool mAddressOf;
    AccuracyType mAccuracy;

    // methods and members for implementing outputs
    //OutputBuilder& mOB;
    bool mPretty;  // whether or not we are doing pretty output
    //! both outputs have same calls to output builder
    void outputBoth(OA_ptr<IRHandlesIRInterface> pIR);
    //! they only differ by how they create a string for Handles
    std::string genIRHandleString(MemRefHandle h, 
                                  OA_ptr<IRHandlesIRInterface> pIR);
};


} // end of OA namespace

#endif

