/*! \file
  
  \brief The AnnotationManager that generates InterAliasMaps

  \authors Michelle Strout
  \version $Id: ManagerInterAliasMapBasic.cpp,v 1.1 2004/11/30 13:51:33 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerInterAliasMapBasic.hpp"


namespace OA {
  namespace Alias {

static bool debug = false;

OA_ptr<Alias::InterAliasMap> ManagerInterAliasMapBasic::performAnalysis() 
{
  OA_ptr<ManagerAliasMapBasic> aliasmapman;
  aliasmapman = new ManagerAliasMapBasic(mIR);
  OA_ptr<InterAliasMap> retval;
  retval = new InterAliasMap(aliasmapman);
  return retval;
}

  } // end of namespace Alias
} // end of namespace OA
