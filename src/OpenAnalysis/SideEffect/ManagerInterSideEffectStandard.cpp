/*! \file
  
  \brief The AnnotationManager that generates SideEffectStandard
         which maps each procedure  to the sets of locations belonging to
         LMOD, MOD, LUSE, USE, LDEF, DEF, LREF, and REF.  
         The SideEffect analysis results can also determine the set of locations
         in MOD, USE, DEF, and REF for a function call.

  \authors Michelle Strout
  \version $Id: ManagerInterSideEffectStandard.cpp,v 1.21 2005/08/08 20:03:52 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "ManagerInterSideEffectStandard.hpp"


namespace OA {
  namespace SideEffect {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerInterSideEffectStandard)
static bool debug = true;
#else
static bool debug = false;
#endif
/*!
*/
ManagerInterSideEffectStandard::ManagerInterSideEffectStandard(
  OA_ptr<InterSideEffectIRInterface> _ir) 
    : DataFlow::CallGraphDFProblemNew(DataFlow::CallGraphDFProblemNew::BottomUp),
      mIR(_ir)
{
}

/*! concrete LocationVisitor that determines that determines whether
    the visited location involves a visible symbol or not.
    If the symbol is visible then mVisible will be true.
    If the location does not involve a symbol then mVisible will be
    true as well.
    A set of visible symbols must be passed in to the constructor.
*/
class VisibleVisitor : public virtual LocationVisitor {
  private:
    std::set<SymHandle> mVisibleSymSet;
  public:
    bool mVisible;

    VisibleVisitor(std::set<SymHandle> pVisibleSymSet)
        : mVisibleSymSet(mVisibleSymSet), mVisible(true) {}
    ~VisibleVisitor() {}
    void visitNamedLoc(NamedLoc& loc) 
      { if (mVisibleSymSet.find(loc.getSymHandle()) != mVisibleSymSet.end())
        { mVisible = true; } else { mVisible = false; }
      }
    void visitUnnamedLoc(UnnamedLoc& loc) { mVisible = true; }
    void visitInvisibleLoc(InvisibleLoc& loc) { mVisible = true; }
    void visitUnknownLoc(UnknownLoc& loc) { mVisible = true; }
    void visitLocSubSet(LocSubSet& loc) { 
        // have this visitor visit the base location
        OA_ptr<Location> baseLoc = loc.getLoc();
        baseLoc->acceptVisitor(*this);
    }
};


/*!
   Going to use the solve on CallGraph.  Just passing around count of locations
   in the combination of all size SideEffect sets, because if doing things 
   properly would essentially end up with six
   LocDFSets.  Instead, the callbacks will just update the InterSideEffect
   datastructure.
*/
OA_ptr<InterSideEffectStandard> 
ManagerInterSideEffectStandard::performAnalysis(
        OA_ptr<CallGraph::Interface> callGraph,
        OA_ptr<DataFlow::ParamBindings> paramBind,
        OA_ptr<Alias::InterAliasInterface> interAlias,
        OA_ptr<ManagerStandard> intraMan)
{
  // create a new (waiting to be filled) InterSideEffectStandard as member
  mInterSideEffect = new InterSideEffectStandard();

  // save other things needed for analysis in member variables
  mParamBind = paramBind;
  mInterAlias = interAlias;
  mIntraMan = intraMan;

  // for all functions defined in IR create optimistic intraprocedural
  // side-effect results
  // NOTE: not using initializeNode because for this analysis we aren't
  // using the DataFlowSets being passed around
  OA_ptr<CallGraph::Interface::NodesIterator> nodeIterPtr;
  nodeIterPtr = callGraph->getNodesIterator();
  for ( ; nodeIterPtr->isValid(); ++(*nodeIterPtr) ) {
      OA_ptr<CallGraph::Interface::Node> node = nodeIterPtr->current();
      ProcHandle proc = node->getProc();
      if (node->isDefined()) {
        // use optimistic intra side effect analysis results
        OA_ptr<SideEffectStandard> procSideEffect;
        procSideEffect= new SideEffectStandard();

        // empty out all the sets
        procSideEffect->emptyLMOD();
        procSideEffect->emptyMOD();
        procSideEffect->emptyLDEF();
        procSideEffect->emptyDEF();
        procSideEffect->emptyLUSE();
        procSideEffect->emptyUSE();
        procSideEffect->emptyLREF();
        procSideEffect->emptyREF();

        mInterSideEffect->mapProcToSideEffect(proc, procSideEffect);

      } 

  } // loop over nodes in call graph

  // use the CallGraph dataflow solver to visit procedures iteratively
  // and solve for side-effect sets for each function call
  DataFlow::CallGraphDFProblemNew::solve(callGraph);


  return mInterSideEffect;
}

//========================================================
// implementation of CallGraphDFProblemNew callbacks
//========================================================
//--------------------------------------------------------
// initialization callbacks
//--------------------------------------------------------
/*!
   Data-flow set passed around on the call graph is an
   CountDFSet.  
*/
OA_ptr<DataFlow::DataFlowSet> ManagerInterSideEffectStandard::initializeTop()
{
    OA_ptr<DataFlow::CountDFSet> retval;
    retval = new DataFlow::CountDFSet;
    return retval;
}

/*! Shouldn't be called.
 */
OA_ptr<DataFlow::DataFlowSet>  
ManagerInterSideEffectStandard::initializeBottom()
{
    assert(0);
    OA_ptr<DataFlow::CountDFSet> retval;
    return retval;
}

//! Should generate an initial DataFlowSet for a procedure
OA_ptr<DataFlow::DataFlowSet> 
ManagerInterSideEffectStandard::initializeNode(ProcHandle proc)
{
    return initializeTop();
}

//! Should generate an initial DataFlowSet, use if for a call if both caller
//! and callee are defined
OA_ptr<DataFlow::DataFlowSet> 
ManagerInterSideEffectStandard::initializeEdge(ExprHandle call, 
                                                 ProcHandle caller,
                                                 ProcHandle callee)
{
    return initializeTop();
}

//! Should generate an initial DataFlowSet for a call,
//! called when callee is not defined in call graph and therefore
//! doesn't have a procedure definition handle
OA_ptr<DataFlow::DataFlowSet> 
ManagerInterSideEffectStandard::initializeEdge(ExprHandle call, 
                                                 ProcHandle caller,
                                                 SymHandle callee)
{
    return initializeTop();
}


//--------------------------------------------------------
// solver callbacks
//--------------------------------------------------------

//! OK to modify set1 and return it as result, because solver
//! only passes a tempSet in as set1
OA_ptr<DataFlow::DataFlowSet> 
ManagerInterSideEffectStandard::meet(OA_ptr<DataFlow::DataFlowSet> set1, 
                      OA_ptr<DataFlow::DataFlowSet> set2)
{
    return set1->clone();
}

//! What the analysis does for the particular procedure
OA_ptr<DataFlow::DataFlowSet>
ManagerInterSideEffectStandard::atCallGraphNode(
        OA_ptr<DataFlow::DataFlowSet> inSet, 
        OA::ProcHandle proc)
{
    OA_ptr<DataFlow::CountDFSet> retval;

    if (debug) {
        std::cout << "ManagerInterSideEffectStandard::atCallGraphNode: ";
        std::cout << "proc = " << mIR->toString(proc) << std::endl;
    }

    // do intraprocedural side-effect analysis passing in 
    // interprocedural results we have so far
    OA_ptr<Alias::Interface> alias = mInterAlias->getAliasResults(proc);
    OA_ptr<SideEffect::SideEffectStandard> sideEffect 
        = mIntraMan->performAnalysis(proc, alias, mInterSideEffect); 
    mInterSideEffect->mapProcToSideEffect(proc,sideEffect);

    if (debug) {
      std::cout << "\tIntra results ---------------" << std::endl;
      sideEffect->dump(std::cout,mIR);
    }

    // count how many locations are in all sets for this  procedure so we
    // can see if any are added
    int count = mInterSideEffect->getLocCount(proc);
    retval = new DataFlow::CountDFSet(count);

    return retval;
}

//! What the analysis does for a particular call
OA_ptr<DataFlow::DataFlowSet>
ManagerInterSideEffectStandard::atCallGraphEdge(
        OA_ptr<DataFlow::DataFlowSet> inSet, 
        ExprHandle call, ProcHandle caller, ProcHandle callee )
{
    if (debug) {
       std::cout << "atCallGraphEdge::inSet = ";
       inSet->dump(std::cout, mIR);
    }

    // initialize side-effect information for call to have all empty sets
    mInterSideEffect->initCallSideEffect(call);

    // if callee procedure is defined then map callee results to caller results
    if (callee != ProcHandle(0)) {
        
        // iterate through each set of locations for callee and get set
        // of locations for call
        OA_ptr<DataFlow::CalleeToCallerVisitor> c2cVisitor; 
        c2cVisitor = new DataFlow::CalleeToCallerVisitor(call, 
                caller, mInterAlias, mParamBind);
        OA_ptr<LocIterator> locIterPtr;
        OA_ptr<LocIterator> callLocIterPtr;

        // LMOD
        locIterPtr = mInterSideEffect->getLMODIterator(callee);
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> calleeLoc = locIterPtr->current();
            calleeLoc->acceptVisitor(*c2cVisitor);
            callLocIterPtr = c2cVisitor->getCallerLocIterator();
            for ( ; callLocIterPtr->isValid(); (*callLocIterPtr)++ ) {
                OA_ptr<Location> callLoc = callLocIterPtr->current();
                mInterSideEffect->insertLMOD(call, callLoc);
            }
        }
        // MOD
        locIterPtr = mInterSideEffect->getMODIterator(callee);
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> calleeLoc = locIterPtr->current();
            if (debug) { 
              std::cout << "MOD calleeLoc = ";
              calleeLoc->dump(std::cout,mIR);
              std::cout << std::endl;
            }
            calleeLoc->acceptVisitor(*c2cVisitor);
            callLocIterPtr = c2cVisitor->getCallerLocIterator();
            for ( ; callLocIterPtr->isValid(); (*callLocIterPtr)++ ) {
                OA_ptr<Location> callLoc = callLocIterPtr->current();
                mInterSideEffect->insertMOD(call, callLoc);
                if (debug) { 
                  std::cout << "MOD callLoc = ";
                  callLoc->dump(std::cout,mIR);
                  std::cout << std::endl;
                }
            }
        }
        // LDEF
        locIterPtr = mInterSideEffect->getLDEFIterator(callee);
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> calleeLoc = locIterPtr->current();
            calleeLoc->acceptVisitor(*c2cVisitor);
            callLocIterPtr = c2cVisitor->getCallerLocIterator();
            for ( ; callLocIterPtr->isValid(); (*callLocIterPtr)++ ) {
                OA_ptr<Location> callLoc = callLocIterPtr->current();
                mInterSideEffect->insertLDEF(call, callLoc);
            }
        }
        // DEF 
        locIterPtr = mInterSideEffect->getDEFIterator(callee);
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> calleeLoc = locIterPtr->current();
            calleeLoc->acceptVisitor(*c2cVisitor);
            callLocIterPtr = c2cVisitor->getCallerLocIterator();
            for ( ; callLocIterPtr->isValid(); (*callLocIterPtr)++ ) {
                OA_ptr<Location> callLoc = callLocIterPtr->current();
                mInterSideEffect->insertDEF(call, callLoc);
            }
        }
        // LUSE
        locIterPtr = mInterSideEffect->getLUSEIterator(callee);
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> calleeLoc = locIterPtr->current();
            calleeLoc->acceptVisitor(*c2cVisitor);
            callLocIterPtr = c2cVisitor->getCallerLocIterator();
            for ( ; callLocIterPtr->isValid(); (*callLocIterPtr)++ ) {
                OA_ptr<Location> callLoc = callLocIterPtr->current();
                mInterSideEffect->insertLUSE(call, callLoc);
            }
        }
        // USE 
        locIterPtr = mInterSideEffect->getUSEIterator(callee);
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> calleeLoc = locIterPtr->current();
            if (debug) { 
              std::cout << "USE calleeLoc = ";
              calleeLoc->dump(std::cout,mIR);
              std::cout << std::endl;
            }
            calleeLoc->acceptVisitor(*c2cVisitor);
            callLocIterPtr = c2cVisitor->getCallerLocIterator();
            for ( ; callLocIterPtr->isValid(); (*callLocIterPtr)++ ) {
                OA_ptr<Location> callLoc = callLocIterPtr->current();
                mInterSideEffect->insertUSE(call, callLoc);
                if (debug) { 
                  std::cout << "USE callLoc = ";
                  callLoc->dump(std::cout,mIR);
                  std::cout << std::endl;
                }
            }
        }
        // LREF
        locIterPtr = mInterSideEffect->getLREFIterator(callee);
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> calleeLoc = locIterPtr->current();
            calleeLoc->acceptVisitor(*c2cVisitor);
            callLocIterPtr = c2cVisitor->getCallerLocIterator();
            for ( ; callLocIterPtr->isValid(); (*callLocIterPtr)++ ) {
                OA_ptr<Location> callLoc = callLocIterPtr->current();
                mInterSideEffect->insertLREF(call, callLoc);
            }
        }
        // REF
        locIterPtr = mInterSideEffect->getREFIterator(callee);
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> calleeLoc = locIterPtr->current();
            calleeLoc->acceptVisitor(*c2cVisitor);
            callLocIterPtr = c2cVisitor->getCallerLocIterator();
            for ( ; callLocIterPtr->isValid(); (*callLocIterPtr)++ ) {
                OA_ptr<Location> callLoc = callLocIterPtr->current();
                mInterSideEffect->insertREF(call, callLoc);
            }
        }

    // don't have definition for callee
    // just insert locations that IR told us about, see comments about
    // getSideEffect in InterSideEffectIRInterface
    } else {
        OA_ptr<SideEffect::SideEffectStandard> sideEffect 
            = mIR->getSideEffect(caller, mIR->getProcSymHandle(callee));
        mInterSideEffect->mapCallToSideEffect(call, sideEffect);
 
        // LMOD
        OA_ptr<LocIterator> locIterPtr = sideEffect->getLMODIterator();
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> loc = locIterPtr->current();
            mInterSideEffect->insertLMOD(call, loc);
        }
        // MOD
        locIterPtr = sideEffect->getMODIterator();
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> loc = locIterPtr->current();
            mInterSideEffect->insertMOD(call, loc);
        }
        // LDEF
        locIterPtr = sideEffect->getLDEFIterator();
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> loc = locIterPtr->current();
            mInterSideEffect->insertLDEF(call, loc);
        }
        // DEF
        locIterPtr = sideEffect->getDEFIterator();
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> loc = locIterPtr->current();
            mInterSideEffect->insertDEF(call, loc);
        }
        // LUSE
        locIterPtr = sideEffect->getLUSEIterator();
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> loc = locIterPtr->current();
            mInterSideEffect->insertLUSE(call, loc);
        }
        // USE
        locIterPtr = sideEffect->getUSEIterator();
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> loc = locIterPtr->current();
            mInterSideEffect->insertUSE(call, loc);
        }
        // LREF
        locIterPtr = sideEffect->getLREFIterator();
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> loc = locIterPtr->current();
            mInterSideEffect->insertLREF(call, loc);
        }
        // REF
        locIterPtr = sideEffect->getREFIterator();
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            OA_ptr<Location> loc = locIterPtr->current();
            mInterSideEffect->insertREF(call, loc);
        }
    }
    
    // count how many locations are in all sets for this  procedure so we
    // can see if any are added
    // FIXME: don't have way to get count for call yet
    //int count = mInterSideEffect->getLocCount(caller);
    OA_ptr<DataFlow::CountDFSet> retval;
    retval = new DataFlow::CountDFSet();

    return retval;
}
 

  } // end of namespace SideEffect
} // end of namespace OA
