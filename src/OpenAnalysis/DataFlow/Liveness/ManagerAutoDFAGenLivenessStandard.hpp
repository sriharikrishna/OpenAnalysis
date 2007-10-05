
// ManagerDFAGenLivenessStandard.hpp

#ifndef DFAGenLivenessManagerStandard_h
#define DFAGenLivenessManagerStandard_h

//--------------------------------------------------------------------
// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/DFAGenLivenessIRInterface.hpp>
#include <OpenAnalysis/DFAGenLiveness/DFAGenLivenessStandard.hpp>


#include <OpenAnalysis/Alias/Interface.hpp>

#include <OpenAnalysis/DataFlow/CFGDFProblem.hpp>
#include <OpenAnalysis/Location/Location.hpp>
#include <OpenAnalysis/DataFlow/IRHandleDataFlowSet.hpp>
#include <OpenAnalysis/SideEffect/InterSideEffectInterface.hpp>
#include <OpenAnalysis/DataFlow/CFGDFSolver.hpp>

#include <OpenAnalysis/CFG/CFG.hpp>
#include <OpenAnalysis/CFG/CFGInterface.hpp>

namespace OA {
  namespace DFAGenLiveness {


class ManagerDFAGenLivenessStandard 
    : public virtual DataFlow::CFGDFProblem { 
public:
  ManagerDFAGenLivenessStandard(OA_ptr<DFAGenLivenessIRInterface> _ir);
  ~ManagerDFAGenLivenessStandard () {}

  OA_ptr<DFAGenLivenessStandard> performAnalysis(ProcHandle, 
        OA_ptr<CFG::CFGInterface> cfg, OA_ptr<Alias::Interface> alias, 
        OA_ptr<SideEffect::InterSideEffectInterface> interSE);

private:
  OA_ptr<DataFlow::DataFlowSet> initializeTop();
  OA_ptr<DataFlow::DataFlowSet> initializeBottom();

  OA_ptr<DataFlow::DataFlowSet>
           initializeNodeIN(OA_ptr<CFG::NodeInterface> n);
  
  OA_ptr<DataFlow::DataFlowSet>
           initializeNodeOUT(OA_ptr<CFG::NodeInterface> n);
                     

  OA_ptr<DataFlow::DataFlowSet> 
  meet (OA_ptr<DataFlow::DataFlowSet> set1, OA_ptr<DataFlow::DataFlowSet> set2); 

  OA_ptr<DataFlow::DataFlowSet> 
  transfer(OA_ptr<DataFlow::DataFlowSet> in, OA::StmtHandle Stmt); 

private: // member variables

  OA_ptr<DFAGenLivenessIRInterface> mIR;
  OA_ptr<Alias::Interface> mAlias;
  OA_ptr<DFAGenLivenessStandard> mDFAGenLivenessMap;
  std::map<StmtHandle, OA_ptr<LocSet> > mStmtMustDefMap;
  std::map<StmtHandle, OA_ptr<LocSet> > mStmtMustUseMap;
  std::map<StmtHandle, OA_ptr<LocSet> > mStmtMayDefMap;
  std::map<StmtHandle, OA_ptr<LocSet> > mStmtMayUseMap;
  OA_ptr<DataFlow::CFGDFSolver> mSolver;
    
};

  } // end of DFAGenLiveness namespace
} // end of OA namespace

#endif