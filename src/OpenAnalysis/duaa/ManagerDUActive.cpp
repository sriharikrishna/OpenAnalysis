/*! \file
  
  \brief The AnnotationManager that generates InterActive 
         INTERprocedurally.

  \authors Jaewook Shin
  \version $Id: ManagerDUActive.cpp,v 1.0 2005/04/10

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include <iostream>
#include "ManagerDUActive.hpp"



namespace OA {
  namespace Activity {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerDUActive)
    static bool debug = true;
#else
    static bool debug = false;
#endif

ManagerDUActive::ManagerDUActive(
  OA_ptr<Activity::ActivityIRInterface> _ir,
  OA_ptr<OA::DUG::DUGStandard> _dug)
    : mIR(_ir), mDUG(_dug)
{    
}

/*!
*/
void
ManagerDUActive::markVaried()
{
  std::list<ExprHandle> callStack;
  std::set<OA_ptr<DUG::DUGStandard::Edge> > visited;
  std::set<unsigned> onPath;

  // iterate over independent variable locations
  std::set<SymHandle>& indepSyms = mDUG->getIndepSyms();
  std::set<SymHandle>::iterator indepIter = indepSyms.begin();
  for (; indepIter != indepSyms.end(); indepIter++) {
    callStack.empty();
    SymHandle sym = *indepIter;
    if (mDUG->isNode(sym)){
      OA_ptr<DUG::DUGStandard::Node> node = mDUG->getNode(sym, OA_ptr<Location>(), 0);
      onPath.insert(node->getId());
      node->markVaried(callStack, mIR, visited, onPath, node->getProc());
      onPath.erase(node->getId());
    }
  }
}

/*!
*/
void
ManagerDUActive::markUseful()
{
  std::list<ExprHandle> callStack;
  std::set<OA_ptr<DUG::DUGStandard::Edge> > visited;
  std::set<unsigned> onPath;

  // iterate over dependent variable locations
  std::set<SymHandle>& depSyms = mDUG->getDepSyms();
  std::set<SymHandle>::iterator depIter = depSyms.begin();
  for (; depIter != depSyms.end(); depIter++) {
    callStack.empty();
    SymHandle sym = *depIter;
    if (mDUG->isNode(sym)){
      OA_ptr<DUG::DUGStandard::Node> node = mDUG->getNode(sym, OA_ptr<Location>(), 0);
      onPath.insert(node->getId());
      node->markUseful(callStack, mIR, visited, onPath, node->getProc());
      onPath.erase(node->getId());
    }
  }
}

/*!
*/
OA_ptr<InterActive> 
ManagerDUActive::performAnalysis(
  OA_ptr<ICFG::ICFGStandard>                   icfg,
  OA_ptr<DataFlow::ParamBindings>              paramBind,
  OA_ptr<Alias::InterAliasInterface>           interAlias)
{
  OA_ptr<InterActive> retval;
  retval = new InterActive;

  // Def-Use Activity Analysis
  markVaried();
  markUseful();
  
  //-------------------------------------------------------------
  // copy active SymHandles from mDUG to 'retval'
  OA_ptr<std::set<SymHandle> > activeSymSet = mDUG->getActiveSymSet();
  std::set<SymHandle>::iterator ssIter = activeSymSet->begin();
  for (; ssIter != activeSymSet->end(); ssIter++){
    retval->setActive(*ssIter);
  }
  
  // store results that will be needed in callbacks
  mICFG       = icfg;
  mParamBind  = paramBind;
  mInterAlias = interAlias;

  // create an empty InterActive
  mInterActive = new InterActive();

  // then iterate over each statement to find active use memrefs

  // assign activity results for each procedure into InterActive
  std::map<ProcHandle,OA_ptr<ActiveStandard> >::iterator mapIter;
  for (mapIter=mActiveMap.begin(); mapIter!=mActiveMap.end(); mapIter++) {
    retval->mapProcToActive(mapIter->first,mapIter->second);
  }

  // iterate over all symbols to determine the size
  OA_ptr<SymHandleIterator> symIter = retval->getActiveSymIterator();
  int bytes = 0;
  for ( ; symIter->isValid(); (*symIter)++ ) {
      SymHandle sym = symIter->current();

      bytes += mIR->getSizeInBytes(sym);

      if (debug) {
          std::cout << "ManagerICFGActive: sym = " << mIR->toString(sym)
                    << ", size = " << mIR->getSizeInBytes(sym) << ", bytes = "
                    << bytes << std::endl;
      }
  }
  retval->setActiveSizeInBytes(bytes);

  return retval;
}

  } // end of namespace Activity
} // end of namespace OA
