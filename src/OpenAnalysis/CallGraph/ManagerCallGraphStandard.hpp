/*! \file
  
  \brief Declarations of the AnnotationManager that generates a CallGraphStandard.

  \authors Arun Chauhan (2001 as part of Mint), Nathan Tallent, Michelle Strout
  \version $Id: ManagerCallGraphStandard.hpp,v 1.6 2004/11/19 19:21:50 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef CallGraphMANAGERSTANDARD_H
#define CallGraphMANAGERSTANDARD_H

//--------------------------------------------------------------------
// standard headers
#ifdef NO_STD_CHEADERS
# include <string.h>
#else
# include <cstring>
#endif

// STL headers
#include <list>
#include <set>
#include <map>

// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/CallGraph/CallGraphStandard.hpp>
#include <OpenAnalysis/IRInterface/CallGraphIRInterface.hpp>

namespace OA {
  namespace CallGraph {



//--------------------------------------------------------------------
/*! 
   The AnnotationManager for a CallGraphStandard Annotation.  Knows how to
   build a CallGraphStandard, read one in from a file, and write one out to a file.
*/
class ManagerStandard { //??? eventually public OA::AnnotationManager
public:
  ManagerStandard(OA_ptr<CallGraphIRInterface> _ir) : mIR(_ir) {}
  virtual ~ManagerStandard () {}

  //??? don't think this guy need AQM, but will eventually have
  //to have one so is standard with other AnnotationManagers
  //what type of handle are we going to attach the CallGraph to?
  virtual OA_ptr<CallGraphStandard> 
        performAnalysis(OA_ptr<IRProcIterator> procIter);

  //-------------------------------------
  // information access
  //-------------------------------------
  OA_ptr<CallGraphIRInterface> getIRInterface() { return mIR; }

  //------------------------------------------------------------------
  // Exceptions
  //------------------------------------------------------------------
  class CallGraphException : public Exception {
    public:
      void report (std::ostream& os) const { os << "E! Unexpected." << std::endl; }
  };

private:
 
  //------------------------------------------------------------------
  // Methods that build CallGraphStandard
  //------------------------------------------------------------------
  void build_graph(OA_ptr<IRProcIterator> funcIter);

private:
  OA_ptr<CallGraphIRInterface> mIR;
  OA_ptr<CallGraphStandard> mCallGraph;

};
//--------------------------------------------------------------------

  } // end of CallGraph namespace
} // end of OA namespace

#endif
