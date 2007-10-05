
// ManagerDFAGenLivenessStandard.cpp

#include "ManagerDFAGenLivenessStandard.hpp"

namespace OA {
  namespace DFAGenLiveness {

static bool debug = false;


/*!
*/
ManagerDFAGenLivenessStandard::ManagerDFAGenLivenessStandard(OA_ptr<DFAGenLivenessIRInterface> _ir) 
    : mIR(_ir)
{
   OA_DEBUG_CTRL_MACRO("DEBUG_ManagerDFAGenLivenessStandard:ALL", debug);
   mSolver = new DataFlow::CFGDFSolver(DataFlow::CFGDFSolver::Backward,*this);
}

OA_ptr<DataFlow::DataFlowSet> ManagerDFAGenLivenessStandard::initializeTop()
{
    OA_ptr<DataFlow::LocDFSet>  retval;
    retval = new DataFlow::LocDFSet;
    return retval;
}

OA_ptr<DataFlow::DataFlowSet> ManagerDFAGenLivenessStandard::initializeBottom()
{
    OA_ptr<DataFlow::LocDFSet>  retval;
    retval = new DataFlow::LocDFSet;
    return retval;
}


OA_ptr<DFAGenLivenessStandard> ManagerDFAGenLivenessStandard::performAnalysis(ProcHandle proc, 
    OA_ptr<CFG::CFGInterface> cfg, OA_ptr<Alias::Interface> alias,
    OA_ptr<SideEffect::InterSideEffectInterface> interSE)
{
  if (debug) {
    std::cout << "In DFAGenLiveness::ManagerDFAGenLivenessStandard::performAnalysis" << std::endl;
  }
  mDFAGenLivenessMap = new DFAGenLivenessStandard(proc);

  // store Alias information for use within the transfer function
  mAlias = alias;

  // get mapping of stmt to locations they may and must define
  OA_ptr<OA::IRStmtIterator> sIt = mIR->getStmtIterator(proc);
  for ( ; sIt->isValid(); (*sIt)++) {
    OA::StmtHandle stmt = sIt->current();
    if (debug) {
      std::cout<< "\tstmt (" << stmt.hval() << ") = ";
      mIR->dump(stmt,std::cout);
    } 
    
    // initialize each stmt to define an empty set of locations
    mStmtMustDefMap[stmt];
    mStmtMustUseMap[stmt];
    mStmtMayDefMap[stmt];
    mStmtMayUseMap[stmt];

    // locations that each statement may or must def
    OA_ptr<MemRefHandleIterator> defIterPtr = mIR->getDefMemRefs(stmt);
    for (; defIterPtr->isValid(); (*defIterPtr)++) {
        MemRefHandle ref = defIterPtr->current();
        if (debug) {
          std::cout << "\t\tdef ref (" << ref.hval() << ") = ";
          mIR->dump(ref,std::cout);
        }

        OA_ptr<LocIterator> locIterPtr = alias->getMustLocs(ref);
        for (; locIterPtr->isValid(); ++(*locIterPtr)) {
            mStmtMustDefMap[stmt].insert(locIterPtr->current());
        }

        locIterPtr = alias->getMayLocs(ref);
        for (; locIterPtr->isValid(); ++(*locIterPtr)) {
            mStmtMayDefMap[stmt].insert(locIterPtr->current());
        }
    }
 
    // must or may defines from procedure calls
    OA_ptr<IRCallsiteIterator> callsiteItPtr = mIR->getCallsites(stmt);
    for ( ; callsiteItPtr->isValid(); ++(*callsiteItPtr)) {
      CallHandle expr = callsiteItPtr->current();

      OA_ptr<LocIterator> locIterPtr;
      
      // MOD
      locIterPtr = interSE->getMODIterator(expr);
      for ( ; locIterPtr->isValid(); (*locIterPtr)++) {
          OA_ptr<Location> modLocPtr = locIterPtr->current();
          mStmtMayDefMap[stmt].insert(modLocPtr);
      }
      
      // DEF
      locIterPtr = interSE->getDEFIterator(expr);
      for ( ; locIterPtr->isValid(); (*locIterPtr)++) {
          OA_ptr<Location> defLocPtr = locIterPtr->current();
          mStmtMustDefMap[stmt].insert(defLocPtr);
      }
    }
    
    
       OA_ptr<MemRefHandleIterator> useIterPtr = mIR->getUseMemRefs(stmt);
        for (; useIterPtr->isValid(); (*useIterPtr)++ ) {
            MemRefHandle useref = useIterPtr->current();
             if (debug) {
                   std::cout << "\t\tuse ref (" << useref.hval() << ") = ";
                   mIR->dump(useref,std::cout);
             }

            OA_ptr<LocIterator> locIterPtr = mAlias->getMayLocs(useref);
            for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
               // mLiveMap->insertLive(stmt,locIterPtr->current());
               mStmtMayUseMap[stmt].insert(locIterPtr->current());
            }
        }
        
  } // loop over statements

  // use the dataflow solver to get the In and Out sets for the BBs
  //DataFlow::CFGDFProblem::solve(cfg);
  //
  mSolver->solve(cfg);  
  
  // get exit node for CFG and determine what definitions reach that node
  OA_ptr<CFG::NodeInterface> node;
  
  node = cfg->getExit();

  OA_ptr<DataFlow::DataFlowSet> x = mSolver->getOutSet(node);
      
  OA_ptr<DataFlow::LocDFSet> inSet 
    = x.convert<DataFlow::LocDFSet>();

  return mDFAGenLivenessMap;

}

OA_ptr<DataFlow::DataFlowSet>
ManagerDFAGenLivenessStandard::initializeNodeIN(OA_ptr<CFG::NodeInterface> n)
{
     OA_ptr<DataFlow::LocDFSet >  retval;
     retval = new DataFlow::LocDFSet;
     return retval;
}

OA_ptr<DataFlow::DataFlowSet>
ManagerDFAGenLivenessStandard::initializeNodeOUT(OA_ptr<CFG::NodeInterface> n)
{
     OA_ptr<DataFlow::LocDFSet>  retval;
     retval = new DataFlow::LocDFSet;
     return retval;
}

OA_ptr<DataFlow::DataFlowSet> 
ManagerDFAGenLivenessStandard::meet (OA_ptr<DataFlow::DataFlowSet> set1orig, 
                       OA_ptr<DataFlow::DataFlowSet> set2orig)
{
    OA_ptr<DataFlow::LocDFSet> set1
        = set1orig.convert<DataFlow::LocDFSet>();
    if (debug) {
        std::cout << "ManagerDFAGenLivenessStandard::meet" << std::endl;
        std::cout << "\tset1 = ";
        set1->dump(std::cout,mIR);
        std::cout << ", set2 = ";
        set2orig->dump(std::cout,mIR);
    }
     
    
    DataFlow::LocDFSet retval 
        = set1->setUnion(*set2orig);
    if (debug) {
        std::cout << std::endl << "\tretval set = ";
        retval.dump(std::cout,mIR);
        std::cout << std::endl;
    }
       
    return retval.clone();
}


OA_ptr<DataFlow::DataFlowSet> 
ManagerDFAGenLivenessStandard::transfer(OA_ptr<DataFlow::DataFlowSet> in, OA::StmtHandle stmt) 
{
    OA_ptr<DataFlow::LocDFSet> inRecast 
        = in.convert<LocDFSet>();

    if (debug) {
        std::cout << "In transfer, stmt(hval=" << stmt.hval() << ")= ";
        mIR->dump(stmt,std::cout);
    }
        
    
  //IN
         
    DataFlow::LocDFSetIterator inIter(*inRecast);
    for (; inIter.isValid(); ++inIter) {
        OA_ptr<Location> dfagenliveness = inIter.current();
        if (debug) {
          std::cout << "\tdfagenliveness in Inset = ";
          mIR->dump(dfagenliveness,std::cout);
        }

        // insert DFAGenLiveness into results
        mDFAGenLivenessMap->insertDFAGenLiveness(stmt,dfagenliveness);
   }      


//KILL

OA_ptr<MemRefHandleIterator> useIterPtr = mIR->getUseMemRefs(stmt);
    for (; useIterPtr->isValid(); (*useIterPtr)++) {
        MemRefHandle ref = useIterPtr->current();
        OA_ptr<LocIterator> locIterPtr = alias->getMayLocs(ref);
        for (; locIterPtr->isValid(); ++(*locIterPtr)) {
            mStmtMayUseMap[stmt].insert(locIterPtr->current());
            mLivenessMap->insertLive(stmt,locIterPtr->current());
            inRecast->insert(locIterPtr->current());
        }
  }
//GEN

    return inRecast;
}

  } // end of namespace DFAGenLiveness
} // end of namespace OA