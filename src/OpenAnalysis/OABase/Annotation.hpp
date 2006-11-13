/*! \file
  
  \brief All analysis results have this as their base class.

  \authors Michelle Strout
  \version $Id: Annotation.hpp,v 1.1 2005/01/18 21:13:16 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef Annotation_H
#define Annotation_H

#include <iostream>
#include <string>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/Utils/OutputBuilderText.hpp>

namespace OA {

class Annotation {
  protected:
    // output builder for all annotations
    static OA_ptr<OutputBuilder> sOB;

  public:
    Annotation() 
      { if (sOB.ptrEqual(0)) sOB = new OutputBuilderText; }
      // FIXME: this could lead to performance problems but it
      // seems the only other option is to have some kind of
      // initialization method called before calling any output
      // methods or hardcode to a specific OutputBuilder subclass

    virtual ~Annotation() {}

    static void configOutput(OA_ptr<OutputBuilder> ob) { sOB = ob; }

    // methods all annotations must provide
    virtual void output(OA_ptr<IRHandlesIRInterface> ir) = 0;
};

} // end of OA namespace

#endif

