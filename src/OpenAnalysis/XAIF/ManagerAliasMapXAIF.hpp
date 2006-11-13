/*! \file
  
  \brief Declarations of the AnnotationManager that generates a 
         AliasMapXAIF.   These don't satisfy the AliasMap::Interface
         because they are a filtered version of AliasMap specifically
         for XAIF.

  \authors Michelle Strout
  \version $Id: ManagerAliasMapXAIF.hpp,v 1.6 2004/11/24 16:49:32 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  Special things about AliasMap:
  - Only mappings for top memory reference handles to AliasMap sets are
    provided.
  - This manager calls it maintains a counter for alias map set ids.
    Therefore all procedures that have their AliasMap generated with
    this manager will have unique ids for their alias map sets across
    all AliasMaps except for the zeroth id.

*/

#ifndef ManagerAliasMapXAIF_h
#define ManagerAliasMapXAIF_h

//--------------------------------------------------------------------
// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/UDDUChains/Interface.hpp>
#include <OpenAnalysis/CFG/Interface.hpp>
#include <OpenAnalysis/XAIF/AliasMapXAIF.hpp>
#include <OpenAnalysis/IRInterface/XAIFIRInterface.hpp>

namespace OA {
  namespace XAIF {


/*! 
   The AnnotationManager for AliasMapXAIF.
   This class can build an AliasMapXAIF, 
   (eventually) read one in from a file, and write one out to a file.
*/
class ManagerAliasMapXAIF { //??? eventually public OA::AnnotationManager

public:
  ManagerAliasMapXAIF(OA_ptr<XAIFIRInterface>);
  ~ManagerAliasMapXAIF () {}

  //! Used to perform analysis when not using AQM
  OA_ptr<XAIF::AliasMapXAIF> performAnalysis(ProcHandle, 
                                          OA_ptr<Alias::Interface> alias);

  //! this method will be used when the AQM is working because the 
  //! Alias information will be queried from AnnotationQueryManager
  //AliasMapXAIF* performAnalysis(ProcHandle);

private:
  OA_ptr<XAIFIRInterface> mIR;    
  static int mCurrentStartId;

};

  } // end of XAIF namespace
} // end of OA namespace

#endif
