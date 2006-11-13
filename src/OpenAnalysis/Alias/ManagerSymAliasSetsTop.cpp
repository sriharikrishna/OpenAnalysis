/*! \file
  
  \brief The AnnotationManager that generates SymAliasSets top

  \authors Michelle Strout
  \version $Id: ManagerSymAliasSetsTop.cpp,v 1.2 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "ManagerSymAliasSetsTop.hpp"


namespace OA {
  namespace Alias {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerSymAliasSetsTop)
static bool debug = true;
#else
static bool debug = false;
#endif

/*!
   Just creates default because default maps each symbol to self.
   In other words, it is assumed that no globals or reference parameters
   may alias each other.
*/
OA_ptr<Alias::SymAliasSets> 
ManagerSymAliasSetsTop::performAnalysis(ProcHandle proc) 
{
  OA_ptr<SymAliasSets> retMap; retMap = new Alias::SymAliasSets();
  return retMap;
}

  } // end of namespace Alias
} // end of namespace OA
