/*! \file
  
  \brief Declarations of the AnnotationManager that generates a VaryStandard

  \authors Michelle Strout
  \version $Id: ManagerVaryStandard.hpp,v 1.4 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2005, Rice University <br>
  Copyright (c) 2004-2005, University of Chicago <br>
  Copyright (c) 2006, Contributors <br>
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef VaryManagerStandard_h
#define VaryManagerStandard_h

//--------------------------------------------------------------------
// OpenAnalysis headers
#include "DepStandard.hpp"
#include "VaryStandard.hpp"

#include <OpenAnalysis/IRInterface/ActivityIRInterface.hpp>
#include <OpenAnalysis/CFG/CFGInterface.hpp>
#include <OpenAnalysis/DataFlow/CFGDFProblem.hpp>
#include <OpenAnalysis/DataFlow/LocDFSet.hpp>
#include <OpenAnalysis/DataFlow/CFGDFSolver.hpp>

namespace OA {
  namespace Activity {


/*! 
   The AnnotationManager for VaryStandard.
   This class can build an VaryStandard, 
   (eventually) read one in from a file, and write one out to a file.
*/
class ManagerVaryStandard 
    : private DataFlow::CFGDFProblem { 
      //??? eventually public OA::AnnotationManager
public:
  ManagerVaryStandard(OA_ptr<ActivityIRInterface> _ir);
  ~ManagerVaryStandard () {}

  //! indepLocIter is a location iterator over independent locations
  //! that the algorithm should calculate OutVary from
  OA_ptr<VaryStandard> performAnalysis(ProcHandle, 
        OA_ptr<CFG::CFGInterface> cfg, OA_ptr<DepStandard> dep,
        OA_ptr<DataFlow::LocDFSet> varyIn,
        DataFlow::DFPImplement algorithm);

  //------------------------------------------------------------------
  // Implementing the callbacks for CFGDFProblem
  //------------------------------------------------------------------
private:
  OA_ptr<DataFlow::DataFlowSet> initializeTop();
  OA_ptr<DataFlow::DataFlowSet> initializeBottom();

//  void initializeNode(OA_ptr<CFG::Interface::Node> n);
  // Added by PLM 07/26/06
   //! Should generate an in and out DataFlowSet for node
  OA_ptr<DataFlow::DataFlowSet>
           initializeNodeIN(OA_ptr<CFG::NodeInterface> n);
  OA_ptr<DataFlow::DataFlowSet>
           initializeNodeOUT(OA_ptr<CFG::NodeInterface> n);

  
  OA_ptr<DataFlow::DataFlowSet> 
  meet (OA_ptr<DataFlow::DataFlowSet> set1, OA_ptr<DataFlow::DataFlowSet> set2); 

  OA_ptr<DataFlow::DataFlowSet> 
  transfer(OA_ptr<DataFlow::DataFlowSet> in, OA::StmtHandle stmt); 

private: // member variables

  OA_ptr<ActivityIRInterface> mIR;
  OA_ptr<DepStandard> mDep;
  OA_ptr<VaryStandard> mVaryMap;
  OA_ptr<DataFlow::LocDFSet> mVaryIn;
  OA_ptr<CFG::CFGInterface> mCFG;
  // Added by PLM 07/26/06
  OA_ptr<DataFlow::CFGDFSolver> mSolver;
  
};

  } // end of Activity namespace
} // end of OA namespace

#endif
