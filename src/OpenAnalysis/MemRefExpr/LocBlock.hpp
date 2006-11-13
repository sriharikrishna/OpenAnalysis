/*! \file
  
  \brief Declarations for LocBlock class.  
  
  \authors Michelle Strout
  \version $Id: LocBlock.hpp,v 1.11 2005/03/17 21:47:46 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  This has been deprecated.  Still being used in Open64IRInterface code, 
  for memory kludge but needs to go away.
  
  Is an example of how to use the outputBuilder.

  A LocBlock represents a block of memory. Examples of locations block types
  are local variables, global variables, and heap allocations. If a location
  block is named it will have a SymbolHandle associated with it.

*/

#ifndef LocBlock_H
#define LocBlock_H

#include <list>
#include <string>
#include <iostream>
#include <OpenAnalysis/Utils/OutputBuilder.hpp>
//#include <OpenAnalysis/config.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>

namespace OA {

class LocBlock {
  public:
    //! construct a location block for named entities
    LocBlock(SymHandle sh, bool isLocal, bool isUnique) :
        mSymHandle(sh), mFilename(""), mStmtHandle(0), 
        mLocal(isLocal), mNamed(true), mUnique(isUnique)
        //mOB(defaultOutputBuilder)
	{ }

    //! construct a location block for unnamed entities
    LocBlock(std::string filename, StmtHandle sh, bool isLocal, bool isNamed,
        bool isUnique) 
        : mSymHandle(0), mFilename(filename), mStmtHandle(sh), 
          mLocal(isLocal), mNamed(isNamed), mUnique(isUnique)
          //mOB(defaultOutputBuilder)
        { } 

    ~LocBlock() { mFullOverlap.clear(); mPartOverlap.clear(); }

    bool isLocal() { return mLocal; }
 
    bool operator==(LocBlock& other) { return (mSymHandle==other.mSymHandle
        && mFilename==other.mFilename && mStmtHandle==other.mStmtHandle 
        && mLocal==other.mLocal 
        && mNamed==other.mNamed && mUnique==other.mUnique); } 

    void addFullOverlap(SymHandle s) { mFullOverlap.push_back(s); }
    void addPartOverlap(SymHandle s) { mPartOverlap.push_back(s); }

    //! set a different OutputBuilder, encapsulates stream and 
    //! format (txt, xml, etc)
    //void configOutput(OutputBuilder& ob) { mOB = ob; }
    //! will use OutputBuilder to generate output with persistent handles
    void output(IRHandlesIRInterface& pIR);
    //! will use OutputBuilder to generate output with string reps of handles
    void outputPretty(IRHandlesIRInterface& pIR);

    void dump(std::ostream& os, IRHandlesIRInterface &pIR);

  private:
    SymHandle mSymHandle;
    std::string mFilename;   // file name of allocation/malloc if unnamed
    StmtHandle mStmtHandle;  // StmtHandle if unnamed
    bool mLocal;
    bool mNamed;
    bool mUnique;
    std::list<SymHandle> mFullOverlap;
    std::list<SymHandle> mPartOverlap;

    // methods and members for implementing outputs
    //OutputBuilder& mOB;
    bool mPretty;  // whether or not we are doing pretty output
    //! both outputs have same calls to output builder
    void outputBoth(IRHandlesIRInterface& pIR);
    //! they only differ by how they create a string for Handles
    std::string genIRHandleString(SymHandle h, IRHandlesIRInterface& pIR);

};

} // end of OA namespace

#endif

