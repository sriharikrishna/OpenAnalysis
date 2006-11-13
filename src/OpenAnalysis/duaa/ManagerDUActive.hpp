/*! \file
  
  \brief Declarations of the AnnotationManager that generates Activity
         analysis results for each procedure using interprocedural 
         Def-Use graph.

  \authors Jaewook Shin
  \version $Id: ManagerDUActive.hpp,v 1.0 2006/04/10

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef ManagerDUActive_h
#define ManagerDUActive_h


// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/DUG/DUGStandard.hpp>
#include <OpenAnalysis/Activity/InterActive.hpp>
#include <OpenAnalysis/ICFG/ICFGStandard.hpp>
#include <OpenAnalysis/IRInterface/ActivityIRInterface.hpp>



//--------------------------------------------------------------------
// OpenAnalysis headers

namespace OA {
  namespace Activity {


/*! 
  Generates InterActive.
*/
class ManagerDUActive  {
public:
  ManagerDUActive(OA_ptr<Activity::ActivityIRInterface>,
                  OA_ptr<OA::DUG::DUGStandard>);
  ~ManagerDUActive () {}

  OA_ptr<InterActive> performAnalysis(
    OA_ptr<ICFG::ICFGStandard> icfg,
    OA_ptr<DataFlow::ParamBindings> paramBind,
    OA_ptr<Alias::InterAliasInterface> interAlias);

  void markVaried();
  void markUseful();

  //! Indicate whether the given symbol is active or not within any procedure
  bool isActive(SymHandle sym){ return mDUG->isActive(sym); }

  //! Indicate whether the given stmt is active or not within given proc
  bool isActive(ProcHandle proc, StmtHandle stmt)
    {
      return mDUG->isActive(stmt);
    }

  //! Indicate whether the given memref is active or not within given proc
  bool isActive(ProcHandle proc, MemRefHandle memref)
    {
      return mDUG->isActive(memref);
    }
 
private:
  OA_ptr<Activity::ActivityIRInterface>        mIR;
  std::map<ProcHandle,OA_ptr<ActiveStandard> > mActiveMap;
  OA_ptr<InterActive>                          mInterActive;

  OA_ptr<DataFlow::ParamBindings>              mParamBind;
  OA_ptr<Alias::InterAliasInterface>           mInterAlias;
  OA_ptr<ICFG::ICFGStandard>                   mICFG;

  OA_ptr<OA::DUG::DUGStandard>                 mDUG;
};

  } // end of Activity namespace
} // end of OA namespace

#endif
