/*! \file
  
  \brief Declarations of the AnnotationManager that generates a CFGStandard.

  \authors Arun Chauhan (2001 as part of Mint), Nathan Tallent, Michelle Strout
  \version $Id: ManagerCFGStandard.hpp,v 1.7 2005/10/17 21:21:41 garvin Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef MANAGERCFGSTANDARD_H
#define MANAGERCFGSTANDARD_H

//--------------------------------------------------------------------
// standard headers
#ifdef NO_STD_CHEADERS
# include <string.h>
#else
# include <cstring>
#endif

// STL headers
#include <list>
#include <set>
#include <map>

// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/CFG/CFGStandard.hpp>
#include <OpenAnalysis/IRInterface/CFGIRInterface.hpp>

namespace OA {
  namespace CFG {


//--------------------------------------------------------------------
class Worklist;


//--------------------------------------------------------------------
/*! 
   The AnnotationManager for a CFGStandard Annotation.  Knows how to
   build a CFGStandard, read one in from a file, and write one out to a file.
*/
class ManagerStandard { //??? eventually public OA::AnnotationManager
public:
  ManagerStandard(OA_ptr<CFGIRInterface> _ir, bool _build_stmt_level_cfg = false);
  virtual ~ManagerStandard ();

  //??? don't think this guy need AQM, but will eventually have
  //to have one so is standard with other AnnotationManagers
  virtual OA_ptr<CFG::CFGStandard> performAnalysis(ProcHandle);

  //-------------------------------------
  // information access
  //-------------------------------------
  OA_ptr<CFGIRInterface> getIRInterface() { return mIR; }

  //------------------------------------------------------------------
  // Exceptions
  //------------------------------------------------------------------
  class Unexpected_Break : public Exception {
  public:
    void report (std::ostream& os) const 
    { os << "E!  Unexpected break statement." << std::endl; }
  };
  //------------------------------------------------------------------
  class Unexpected_Return : public Exception {
  public:
    void report (std::ostream& os) const 
    { os << "E!  Unexpected return statement." << std::endl; }
  };
  //------------------------------------------------------------------
  class Unexpected_Continue : public Exception {
  public:
    void report (std::ostream& os) const 
    { os << "E!  Unexpected continue statement." << std::endl; }
  };
  //------------------------------------------------------------------

private:
 
  //------------------------------------------------------------------
  // Methods that recursively build CFGStandard
  //------------------------------------------------------------------
  IRStmtType build_block (OA_ptr<CFGStandard::Node> prev_node, 
    OA_ptr<IRRegionStmtIterator> si,
    CFGStandard::NodeLabelList& exit_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> break_nodes,
    OA_ptr<CFGStandard::NodeLabelList> return_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> continue_nodes);

  IRStmtType build_stmt (OA_ptr<CFGStandard::Node> prev_node, OA::StmtHandle, 
    CFGStandard::NodeLabelList& exit_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> break_nodes,
    OA_ptr<CFGStandard::NodeLabelList> return_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> continue_nodes)
    throw (Unexpected_Break, Unexpected_Return, Unexpected_Continue);
  
  IRStmtType build_CFG_loop (OA_ptr<CFGStandard::Node> prev_node, 
    OA::StmtHandle th, 
    CFGStandard::NodeLabelList& exit_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> return_nodes);
  
  IRStmtType build_CFG_end_tested_loop (OA_ptr<CFGStandard::Node> prev_node, 
    OA::StmtHandle th, 
    CFGStandard::NodeLabelList& exit_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> return_nodes);

  IRStmtType build_CFG_twoway_branch (OA_ptr<CFGStandard::Node> prev_node, 
    OA::StmtHandle th, 
    CFGStandard::NodeLabelList& exit_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> break_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> return_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> continue_nodes);

  IRStmtType build_CFG_multiway_branch (OA_ptr<CFGStandard::Node> prev_node, 
    OA::StmtHandle th, 
    CFGStandard::NodeLabelList& exit_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> break_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> return_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> continue_nodes);

  IRStmtType build_CFG_multiway_branch_with_fallthrough (
    OA_ptr<CFGStandard::Node> prev_node, 
    OA::StmtHandle th, CFGStandard::NodeLabelList& exit_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> return_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> continue_nodes);

  IRStmtType build_CFG_unconditional_jump (OA_ptr<CFGStandard::Node> prev_node, 
    OA::StmtHandle stmt);

  IRStmtType build_CFG_unconditional_jump_i (
    OA_ptr<CFGStandard::Node> prev_node, 
    OA::StmtHandle stmt);

  IRStmtType build_CFG_ustruct_twoway_branch (
    OA_ptr<CFGStandard::Node> prev_node, 
    OA::StmtHandle stmt, CFGStandard::NodeLabelList& exit_nodes);

  IRStmtType build_CFG_ustruct_multiway_branch (
    OA_ptr<CFGStandard::Node> prev_node, 
    OA::StmtHandle stmt);

  //******************************************
  // Support for building cfgs with delay slots.
  //******************************************
  void HandleDelayedBranches();
  bool isInternalBranch(StmtHandle);
  void processBlock(OA_ptr<CFGStandard::Node> );
  void createBasicCFG();
  void processBlock();

private:
  OA_ptr<CFGIRInterface> mIR;
  OA_ptr<CFGStandard> mCFG;
  bool mIsStmtLevel;

  //------------------------------------------
  // data structures for handling delay slots
  //------------------------------------------
  OA_ptr<Worklist> mThe_worklist;
  std::map<OA_ptr<CFGStandard::Node>, OA_ptr<CFGStandard::Node> > mFallThrough;

  //---------------------------------------------------------------
  // build a CFG for individual statements rather than basic blocks
  //---------------------------------------------------------------
  const bool mBuildStmtLevelCFG; 
};
//--------------------------------------------------------------------

  } // end of CFG namespace
} // end of OA namespace

#endif
