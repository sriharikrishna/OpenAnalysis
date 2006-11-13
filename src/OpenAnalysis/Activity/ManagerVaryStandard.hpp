/*! \file
  
  \brief Declarations of the AnnotationManager that generates a VaryStandard

  \authors Michelle Strout
  \version $Id: ManagerVaryStandard.hpp,v 1.4 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
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
#include <OpenAnalysis/CFG/Interface.hpp>
#include <OpenAnalysis/DataFlow/CFGDFProblem.hpp>
#include <OpenAnalysis/DataFlow/LocDFSet.hpp>

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
        OA_ptr<CFG::Interface> cfg, OA_ptr<DepStandard> dep,
        OA_ptr<DataFlow::LocDFSet> varyIn);

  //------------------------------------------------------------------
  // Implementing the callbacks for CFGDFProblem
  //------------------------------------------------------------------
private:
  OA_ptr<DataFlow::DataFlowSet> initializeTop();
  OA_ptr<DataFlow::DataFlowSet> initializeBottom();

  void initializeNode(OA_ptr<CFG::Interface::Node> n);

  OA_ptr<DataFlow::DataFlowSet> 
  meet (OA_ptr<DataFlow::DataFlowSet> set1, OA_ptr<DataFlow::DataFlowSet> set2); 

  OA_ptr<DataFlow::DataFlowSet> 
  transfer(OA_ptr<DataFlow::DataFlowSet> in, OA::StmtHandle stmt); 

private: // member variables

  OA_ptr<ActivityIRInterface> mIR;
  OA_ptr<DepStandard> mDep;
  OA_ptr<VaryStandard> mVaryMap;
  OA_ptr<DataFlow::LocDFSet> mVaryIn;
  OA_ptr<CFG::Interface> mCFG;

};

  } // end of Activity namespace
} // end of OA namespace

#endif
