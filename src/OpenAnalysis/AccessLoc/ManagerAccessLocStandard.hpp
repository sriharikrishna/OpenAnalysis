/*! \file
  
  \brief Declarations of the AnnotationManager that generates 
         an AccessLocStandard

  \authors Michelle Strout
  \version $Id: ManagerAccessLocStandard.hpp,v 1.2 2004/11/19 19:21:49 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef ManagerAccessLocStandard_H
#define ManagerAccessLocStandard_H

//--------------------------------------------------------------------
#include <map>

// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/MemRefExpr/MemRefExpr.hpp>
#include <OpenAnalysis/Location/Location.hpp>
#include <OpenAnalysis/IRInterface/AccessLocIRInterface.hpp>
#include <OpenAnalysis/AccessLoc/AccessLocStandard.hpp>

namespace OA {
  namespace AccessLoc {

/*! 
   The AnnotationManager for an AccessLoc.  Determines all accessible
   locations, including invisible locations, for a particular procedure.
*/
class ManagerAccessLocStandard { //??? eventually public OA::AnnotationManager
public:
  ManagerAccessLocStandard(OA_ptr<AccessLocIRInterface> _ir) : mIR(_ir) {}
  virtual ~ManagerAccessLocStandard () {}

  //! Used to perform analysis when not using AQM
  virtual OA_ptr<AccessLoc::AccessLocStandard> performAnalysis(ProcHandle); 

private:
  OA_ptr<AccessLocIRInterface> mIR;

};

  } // end of AccessLoc namespace
} // end of OA namespace

#endif
