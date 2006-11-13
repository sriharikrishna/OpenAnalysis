/*! \file
  
  \brief Declarations of an AnnotationManager that generates a ICFGDep.

  \authors Michelle Strout
  \version $Id: ManagerProcIterDep.hpp,v 1.1 2005/06/21 15:20:54 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  NOTE: An ICFGDep doesn't need the use,def pairs for a procedure call
  because it is used to do Vary and Useful analysis on an ICFG.
*/

#ifndef ManagerProcIterDep_H
#define ManagerProcIterDep_H

//--------------------------------------------------------------------
// OpenAnalysis headers
#include "ICFGDep.hpp"
#include "DepDFSet.hpp"
#include <OpenAnalysis/IRInterface/ActivityIRInterface.hpp>
#include <OpenAnalysis/ExprTree/DifferentiableLocsVisitor.hpp>
#include <OpenAnalysis/ExprTree/EvalToMemRefVisitor.hpp>

#include <OpenAnalysis/DataFlow/LocDFSet.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>


namespace OA {
  namespace Activity {

/*! 
   An AnnotationManager for an ICFGDep.
   ICFGDep results are only 
   valid for ManagerICFGVary and ManagerICFGUseful because they propagate
   over the ICFG.
   FIXME: could be useful to other representations that handle
   propagation to function calls as well and doesn't need to have the
   stmt with a function call include the Dep relations for the call
*/
class ManagerProcIterDep {
public:
  ManagerProcIterDep(OA_ptr<ActivityIRInterface> _ir);
  ~ManagerProcIterDep () {}

  OA_ptr<ICFGDep> 
  performAnalysis(OA_ptr<IRProcIterator> procIter,
                  OA_ptr<DataFlow::ParamBindings> paramBind,
                  OA_ptr<Alias::InterAliasInterface> interAlias);

private: // member variables

  OA_ptr<ActivityIRInterface> mIR;
  OA_ptr<ICFGDep> mDep;
  OA_ptr<Alias::InterAliasInterface> mInterAlias;
  OA_ptr<DataFlow::ParamBindings> mParamBind;

};

  } // end of Activity namespace
} // end of OA namespace

#endif
