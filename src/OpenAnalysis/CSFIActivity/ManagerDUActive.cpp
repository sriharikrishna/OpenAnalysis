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

// #define DEBUG_DUAA_LAST


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
  std::list<CallHandle> callStack;
  std::set<OA_ptr<DUG::EdgeInterface> > visited;
  std::set<unsigned> onPath;

  // iterate over independent variables
  std::list<std::pair<SymHandle, ProcHandle> >& indepSyms = mDUG->getIndepSyms();
  std::list<std::pair<SymHandle, ProcHandle> >::iterator indepIter = indepSyms.begin();
  for (; indepIter != indepSyms.end(); indepIter++) {
    callStack.empty();
    std::pair<SymHandle, ProcHandle> indepPair = *indepIter;
    SymHandle sym = indepPair.first;
    ProcHandle proc = indepPair.second;
    
    if (mDUG->isNode(sym, proc)){
#ifdef DEBUG_DUAA_LAST
      std::cout << "markVaried: Independent(" << mIR->toString(sym)
                << ")" << std::endl;
#endif
      OA_ptr<DUG::NodeInterface> node = mDUG->getNode(sym, proc);
      onPath.insert(node->getId());
      node->markVaried(callStack, mIR, visited, onPath, node->getProc(), 
                       (unsigned)-1, OA_ptr<DUG::EdgeInterface>());
      onPath.erase(node->getId());
    }
  }
  
}

/*!
*/
void
ManagerDUActive::markUseful()
{
  std::list<CallHandle> callStack;
  std::set<OA_ptr<DUG::EdgeInterface> > visited;
  std::set<unsigned> onPath;


  // iterate over dependent variables
  std::list<std::pair<SymHandle, ProcHandle> >& depSyms = mDUG->getDepSyms();
  std::list<std::pair<SymHandle, ProcHandle> >::iterator depIter = depSyms.begin();
  for (; depIter != depSyms.end(); depIter++) {
    callStack.empty();
    std::pair<SymHandle, ProcHandle> depPair = *depIter;
    SymHandle sym = depPair.first;
    ProcHandle proc = depPair.second;

    if (mDUG->isNode(sym, proc)){
#ifdef DEBUG_DUAA_LAST
      std::cout << "markUseful: Dependent(" << mIR->toString(sym)
                << ")" << std::endl;
#endif
      OA_ptr<DUG::NodeInterface> node = mDUG->getNode(sym, proc);
      onPath.insert(node->getId());


      
      node->markUseful(callStack, mIR, visited, onPath, node->getProc(), 
                       (unsigned)-1, OA_ptr<DUG::EdgeInterface>());
      onPath.erase(node->getId());
    }
  }
}

/*!
*/
OA_ptr<InterActiveFortran> 
ManagerDUActive::performAnalysis(
  OA_ptr<DataFlow::ParamBindings>              paramBind)
{
  OA_ptr<InterActiveFortran> retval;
  retval = new InterActiveFortran;

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
  mParamBind  = paramBind;

  // create an empty InterActive
  mInterActiveFortran = new InterActiveFortran();

  mActiveMap = mDUG->getActiveMap();

  // then iterate over each statement to find active use memrefs

  // assign activity results for each procedure into InterActiveFortran
  std::map<ProcHandle,OA_ptr<ActiveStandard> >::iterator mapIter;
  for (mapIter=mActiveMap.begin(); mapIter!=mActiveMap.end(); mapIter++) {
    retval->mapProcToActive(mapIter->first,mapIter->second);
  }

  // iterate over all symbols to determine the size
  OA_ptr<SymHandleIterator> symIter = retval->getActiveSymIterator();
  int bytes = 0;
  int numActiveSyms = 0;
  for ( ; symIter->isValid(); (*symIter)++ ) {
      SymHandle sym = symIter->current();

      bytes += mIR->getSizeInBytes(sym);
      numActiveSyms++;

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