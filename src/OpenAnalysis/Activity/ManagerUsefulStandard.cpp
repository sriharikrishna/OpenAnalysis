/*! \file
  
  \brief The AnnotationManager that generates UsefulStandard.

  \authors Michelle Strout
  \version $Id: ManagerUsefulStandard.cpp,v 1.10 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerUsefulStandard.hpp"


namespace OA {
  namespace Activity {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerUsefulStandard)
static bool debug = true;
#else
static bool debug = false;
#endif


/*!
*/
ManagerUsefulStandard::ManagerUsefulStandard(OA_ptr<ActivityIRInterface> _ir) 
    : DataFlow::CFGDFProblem( DataFlow::Backward ), mIR(_ir)
{
}

OA_ptr<DataFlow::DataFlowSet> ManagerUsefulStandard::initializeTop()
{
    OA_ptr<DataFlow::LocDFSet>  retval;
    retval = new DataFlow::LocDFSet;
    return retval;
}

OA_ptr<DataFlow::DataFlowSet> ManagerUsefulStandard::initializeBottom()
{
    // FIXME: really should be all accessible locations, but I don't think
    // this is used, if it is there will be an assertian
    OA_ptr<DataFlow::LocDFSet>  retval;
    //retval = new DataFlow::LocDFSet;
    return retval;
}

/*!
    Uses the CFGDFProblem::solve functionality to get In and Out
    Useful sets for each basic block.   In transfer function 
    put InUseful locs in UsefulStandard as well.
*/
OA_ptr<UsefulStandard> ManagerUsefulStandard::performAnalysis(ProcHandle proc, 
    OA_ptr<CFG::Interface> cfg, OA_ptr<DepStandard> dep,
    OA_ptr<DataFlow::LocDFSet> depLocSet)
{
  if (debug) {
    std::cout << "In ManagerUsefulStandard::performAnalysis" << std::endl;
    cfg->dump(std::cout,mIR);
  }
  mUsefulMap = new UsefulStandard(proc);

  // store Dep information for use within the transfer function
  mDep = dep;
  
  // store iterator over independent locations
  OA_ptr<LocIterator> locIter = depLocSet->getLocIterator();
  mDepLocIter = locIter;

  // store CFG for use in initialization
  mCFG = cfg;

  // use the dataflow solver to get the In and Out sets for the BBs
  DataFlow::CFGDFProblem::solve(cfg);

  // get the final useful set from the entry node
  OA_ptr<CFG::Interface::Node> entry = cfg->getEntry();
  OA_ptr<DataFlow::DataFlowSet> dfset = mNodeOutSetMap[entry];
  mUsefulMap->mapFinalUseful(dfset.convert<DataFlow::LocDFSet>());
  
  return mUsefulMap;

}

//------------------------------------------------------------------
// Implementing the callbacks for CFGDFProblem
//------------------------------------------------------------------
void ManagerUsefulStandard::initializeNode(OA_ptr<CFG::Interface::Node> n)
{
    mNodeInSetMap[n] = new DataFlow::LocDFSet;
    mNodeOutSetMap[n] = new DataFlow::LocDFSet;

    // put independent variables in the In and Out Set for last node
    // and into UsefulStandard that we are creating
    if (n.ptrEqual(mCFG->getExit())) {
        OA_ptr<DataFlow::LocDFSet> temp;
        temp = new DataFlow::LocDFSet;
        
        for (mDepLocIter->reset(); mDepLocIter->isValid(); ++(*mDepLocIter) ) {
          OA_ptr<Location> loc = mDepLocIter->current();
          mUsefulMap->insertDepLoc(loc);
          temp->insert(loc);
       }
       mNodeInSetMap[n] = temp->clone();
       mNodeOutSetMap[n] = temp->clone();
    }
}

OA_ptr<DataFlow::DataFlowSet> 
ManagerUsefulStandard::meet (OA_ptr<DataFlow::DataFlowSet> set1orig, 
                             OA_ptr<DataFlow::DataFlowSet> set2orig)
{
    OA_ptr<DataFlow::LocDFSet> set1
        = set1orig.convert<DataFlow::LocDFSet>();
    if (debug) {
        std::cout << "ManagerUsefulStandard::meet" << std::endl;
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

/*!
    \brief Core of analysis algorithm.  Transfer function for a stmt.

    KILL is the set of locations that are must defined in this stmt

    GEN is all locations whose use might is needed for locations in 
        OutUseful that may be defined in the statement
*/

OA_ptr<DataFlow::DataFlowSet> 
ManagerUsefulStandard::transfer(OA_ptr<DataFlow::DataFlowSet> out, 
                              OA::StmtHandle stmt) 
{
    OA_ptr<DataFlow::LocDFSet> outRecast 
        = out.convert<DataFlow::LocDFSet>();
    OA_ptr<DataFlow::LocDFSet> inUseful;
    inUseful = new DataFlow::LocDFSet;

    if (debug) {
        std::cout << "In transfer, stmt(hval=" << stmt.hval() << ")= ";
        mIR->dump(stmt,std::cout);
    }
    
    // loop through all calls in the statement and record the OutVary
    // set for them
    OA_ptr<IRCallsiteIterator> callsiteItPtr = mIR->getCallsites(stmt);
    for ( ; callsiteItPtr->isValid(); ++(*callsiteItPtr)) {
        ExprHandle call = callsiteItPtr->current();

        mUsefulMap->copyIntoOutUseful(call, outRecast);
    }

    // use dep pairs to determine what locations should be in  InUseful 
    DataFlow::LocDFSetIterator outLocIter(*outRecast);
    for ( ; outLocIter.isValid(); ++outLocIter ) {
      OA_ptr<Location> outLoc = outLocIter.current();
      if (debug) { std::cout << "\toutLoc = ";  outLoc->dump(std::cout,mIR); }
      OA_ptr<LocIterator> locIterPtr;
      locIterPtr = mDep->getDiffUseIterator(stmt,outLoc);
      for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
          inUseful->insert(locIterPtr->current());
          if (debug) { 
            std::cout << "\tinserting into diffUseSet = ";  
            locIterPtr->current()->dump(std::cout,mIR); 
          }
      }
    }

    // map to statement in results
    mUsefulMap->copyIntoInUseful(stmt, inUseful);
   
    return inUseful;
}

  } // end of namespace Activity
} // end of namespace OA
