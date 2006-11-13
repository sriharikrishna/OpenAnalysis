/*! \file
  
  \brief Declarations of the AnnotationManager that generates a ReachDefsStandard

  \authors Michelle Strout
  \version $Id: ManagerReachDefsStandard.hpp,v 1.15 2005/03/18 18:14:16 ntallent Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef ReachDefsManagerStandard_h
#define ReachDefsManagerStandard_h

//--------------------------------------------------------------------
// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/ReachDefsIRInterface.hpp>
#include <OpenAnalysis/ReachDefs/ReachDefsStandard.hpp>

#include <OpenAnalysis/CFG/Interface.hpp>
#include <OpenAnalysis/Alias/Interface.hpp>

#include <OpenAnalysis/DataFlow/CFGDFProblem.hpp>
#include <OpenAnalysis/Location/Location.hpp>
#include <OpenAnalysis/DataFlow/IRHandleDataFlowSet.hpp>
#include <OpenAnalysis/SideEffect/InterSideEffectInterface.hpp>

namespace OA {
  namespace ReachDefs {


/*! 
   The AnnotationManager for ReachDefsStandard.
   This class can build an ReachDefsStandard, 
   (eventually) read one in from a file, and write one out to a file.
*/
class ManagerStandard 
    : private DataFlow::CFGDFProblem { 
      //??? eventually public OA::AnnotationManager
public:
  ManagerStandard(OA_ptr<ReachDefsIRInterface> _ir);
  //{ CFGDFProblem<IRHandleDataFlowSet<StmtHandle> >( DataFlow::Forward ); } 
  //{ CFGDFProblem( DataFlow::Forward ); } 
  ~ManagerStandard () {}

  //! Used to perform analysis when not using AQM
  OA_ptr<ReachDefsStandard> performAnalysis(ProcHandle, 
        OA_ptr<CFG::Interface> cfg, OA_ptr<Alias::Interface> alias, 
        OA_ptr<SideEffect::InterSideEffectInterface> interSE);

  //! this method will be used when the AQM is working because the CFG
  //! and MemRefExpr information will be queried from AnnotationQueryManager
  //virtual Alias::AliasMap* performAnalysis(ProcHandle);

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

  OA_ptr<ReachDefsIRInterface> mIR;
  OA_ptr<Alias::Interface> mAlias;
  OA_ptr<ReachDefsStandard> mReachDefMap;
  //std::set<MemRefHandle> mMemRefSet;    // set of all memrefs seen so far

  // the statements that may define this MemRefHandle
  //std::map<MemRefHandle,std::set<StmtHandle> > mMayDefMap;

  // all memrefs that a statement may and must define
  //std::map<StmtHandle,DataFlow::IRHandleDataFlowSet<MemRefHandle> > mStmtMayDefMap;
  //std::map<StmtHandle,DataFlow::IRHandleDataFlowSet<MemRefHandle> > mStmtMustDefMap;
  std::map<StmtHandle,std::set<OA_ptr<Location> > > mStmtMayDefMap;
  std::map<StmtHandle,std::set<OA_ptr<Location> > > mStmtMustDefMap;

};

  } // end of ReachDefs namespace
} // end of OA namespace

#endif
