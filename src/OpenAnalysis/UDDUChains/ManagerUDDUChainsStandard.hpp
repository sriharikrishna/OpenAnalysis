/*! \file
  
  \brief Declarations of the AnnotationManager that generates a 
         UDDUChainsStandard. 

  \authors Michelle Strout
  \version $Id: ManagerUDDUChainsStandard.hpp,v 1.6 2005/03/18 18:14:16 ntallent Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef ManagerUDDUChainsStandard_h
#define ManagerUDDUChainsStandard_h

//--------------------------------------------------------------------
// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/UDDUChainsIRInterface.hpp>
#include <OpenAnalysis/ReachDefs/Interface.hpp>
#include <OpenAnalysis/Alias/Interface.hpp>
#include <OpenAnalysis/UDDUChains/UDDUChainsStandard.hpp>
#include <OpenAnalysis/SideEffect/InterSideEffectInterface.hpp>

namespace OA {
  namespace UDDUChains {


/*! 
   The AnnotationManager for UDDUChainsStandard.
   This class can build an UDDUChainsStandard, 
   (eventually) read one in from a file, and write one out to a file.
*/
class ManagerStandard {
      //??? eventually public OA::AnnotationManager
public:
  ManagerStandard(OA_ptr<UDDUChainsIRInterface> _ir);
  ~ManagerStandard () {}

  OA_ptr<UDDUChainsStandard> performAnalysis(ProcHandle, 
        OA_ptr<Alias::Interface> alias, OA_ptr<ReachDefs::Interface> reachDefs,
        OA_ptr<SideEffect::InterSideEffectInterface> interSE);

private:
  // helper functions
  bool locMayAliasMemRef(OA_ptr<Alias::Interface> alias,
                         OA_ptr<Location> loc,
                         MemRefHandle memref);

  bool locMustAliasMemRef(OA_ptr<Alias::Interface> alias,
                          OA_ptr<Location> loc,
                          MemRefHandle memref);
private:
  OA_ptr<UDDUChainsIRInterface> mIR;

};

  } // end of UDDUChains namespace
} // end of OA namespace

#endif
