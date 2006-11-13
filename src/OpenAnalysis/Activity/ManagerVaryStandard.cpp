/*! \file
  
  \brief The AnnotationManager that generates VaryStandard.

  \authors Michelle Strout
  \version $Id: ManagerVaryStandard.cpp,v 1.9 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerVaryStandard.hpp"


namespace OA {
  namespace Activity {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerVaryStandard)
static bool debug = true;
#else
static bool debug = false;
#endif



/*!
*/
ManagerVaryStandard::ManagerVaryStandard(OA_ptr<ActivityIRInterface> _ir) 
    : DataFlow::CFGDFProblem( DataFlow::Forward ), mIR(_ir)
{
}

OA_ptr<DataFlow::DataFlowSet> ManagerVaryStandard::initializeTop()
{
    OA_ptr<DataFlow::LocDFSet>  retval;
    retval = new DataFlow::LocDFSet;
    return retval;
}

OA_ptr<DataFlow::DataFlowSet> ManagerVaryStandard::initializeBottom()
{
    OA_ptr<DataFlow::LocDFSet>  retval;
    retval = new DataFlow::LocDFSet;
    return retval;
}

/*!
    Uses the CFGDFProblem::solve functionality to get In and Out
    Vary sets for each basic block.   In transfer function 
    put OutVary locs in VaryStandard as well.
*/
OA_ptr<VaryStandard> ManagerVaryStandard::performAnalysis(ProcHandle proc, 
    OA_ptr<CFG::Interface> cfg, OA_ptr<DepStandard> dep,
    OA_ptr<DataFlow::LocDFSet> varyIn)
{
  if (debug) {
    std::cout << "In ManagerVaryStandard::performAnalysis" << std::endl;
  }
  mVaryMap = new VaryStandard(proc);

  // store Dep information for use within the transfer function
  mDep = dep;
 
  // store set of independent locations
  mVaryIn = varyIn;

  // store CFG for use in initialization
  mCFG = cfg;

  // use the dataflow solver to get the In and Out sets for the BBs
  DataFlow::CFGDFProblem::solve(cfg);
  
  return mVaryMap;

}

//------------------------------------------------------------------
// Implementing the callbacks for CFGDFProblem
//------------------------------------------------------------------
void ManagerVaryStandard::initializeNode(OA_ptr<CFG::Interface::Node> n)
{
    mNodeInSetMap[n] = new DataFlow::LocDFSet;
    mNodeOutSetMap[n] = new DataFlow::LocDFSet;

    // put independent variables in the In and Out Set for first node
    // and into VaryStandard that we are creating
    if (n.ptrEqual(mCFG->getEntry())) {
        mNodeInSetMap[n] = mVaryIn->clone();
        mNodeOutSetMap[n] = mVaryIn->clone();
    }
}

OA_ptr<DataFlow::DataFlowSet> 
ManagerVaryStandard::meet (OA_ptr<DataFlow::DataFlowSet> set1orig, 
                           OA_ptr<DataFlow::DataFlowSet> set2orig)
{
    OA_ptr<DataFlow::LocDFSet> set1
        = set1orig.convert<DataFlow::LocDFSet>();
    if (debug) {
        std::cout << "ManagerVaryStandard::meet" << std::endl;
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

    GEN is all locations whose may definition might depend on 
        locations in InVary
*/

OA_ptr<DataFlow::DataFlowSet> 
ManagerVaryStandard::transfer(OA_ptr<DataFlow::DataFlowSet> in, 
                              OA::StmtHandle stmt) 
{
    OA_ptr<DataFlow::LocDFSet> inRecast 
        = in.convert<DataFlow::LocDFSet>();
    OA_ptr<DataFlow::LocDFSet> outVary;
    outVary = new DataFlow::LocDFSet;

    if (debug) {
        std::cout << "In transfer, stmt(hval=" << stmt.hval() << ")= ";
        mIR->dump(stmt,std::cout);
    }

    // loop through all calls in the statement and record the InVary
    // set for them
    // FIXME: eventually need to include all defines that might happen
    // in any order in relation to uses and defines in the subroutine
    OA_ptr<IRCallsiteIterator> callsiteItPtr = mIR->getCallsites(stmt);
    for ( ; callsiteItPtr->isValid(); ++(*callsiteItPtr)) {
        ExprHandle call = callsiteItPtr->current();

        mVaryMap->copyIntoInVary(call, inRecast);
    }

    // use dep pairs to determine what locations should be in OutVary
    DataFlow::LocDFSetIterator inLocIter(*inRecast);
    for ( ; inLocIter.isValid(); ++inLocIter ) {
      OA_ptr<Location> inLoc = inLocIter.current();
      if (debug) {
          std::cout << "\tinLoc = ";
          inLoc->dump(std::cout, mIR);
      }
      OA_ptr<LocIterator> locIterPtr;
      locIterPtr = mDep->getMayDefIterator(stmt,inLoc);
      for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
          outVary->insert(locIterPtr->current());
      }
    }
    
    // store outVary for stmt into Vary results
    mVaryMap->copyIntoOutVary(stmt,outVary);

    if (debug) {
        std::cout << "\toutVary = ";
        outVary->dump(std::cout,mIR);
    }

    return outVary;
}

  } // end of namespace Activity
} // end of namespace OA
