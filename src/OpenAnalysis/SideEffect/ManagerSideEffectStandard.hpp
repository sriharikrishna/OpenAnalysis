/*! \file
  
  \brief Declarations of the AnnotationManager that generates a SideEffectStandard.

  \authors Michelle Strout
  \version $Id: ManagerSideEffectStandard.hpp,v 1.5 2005/08/08 20:03:52 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef ManagerSideEffectStandard_h
#define ManagerSideEffectStandard_h

//--------------------------------------------------------------------
// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/SideEffectIRInterface.hpp>
#include <OpenAnalysis/SideEffect/InterSideEffectInterface.hpp>
#include <OpenAnalysis/SideEffect/SideEffectStandard.hpp>
#include <OpenAnalysis/Alias/Interface.hpp>
#include <OpenAnalysis/Location/LocationVisitor.hpp>


namespace OA {
  namespace SideEffect {


/*! 
   The AnnotationManager for SideEffectStandard.
   This class can build a SideEffectStandard. 
*/
class ManagerStandard {
      //??? eventually public OA::AnnotationManager
public:
  ManagerStandard(OA_ptr<SideEffectIRInterface> _ir) : mIR(_ir) {}
  ~ManagerStandard () {}

  //! Used to perform analysis when not using AQM
  OA_ptr<SideEffectStandard> performAnalysis(ProcHandle, 
                                     OA_ptr<Alias::Interface> alias,
                                     OA_ptr<InterSideEffectInterface> inter);

private: // member variables

  OA_ptr<SideEffectIRInterface> mIR;
  ProcHandle mProc;
  //OA_ptr<Alias::Interface> mAlias;
};

  } // end of SideEffect namespace
} // end of OA namespace

#endif
