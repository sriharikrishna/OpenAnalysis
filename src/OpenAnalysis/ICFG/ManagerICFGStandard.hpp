/*! \file
  
  \brief Declarations of the AnnotationManager that generates a ICFGStandard
         from the CFG standards for each procedure

  \authors Michelle Strout
  \version $Id: ManagerICFGStandard.hpp,v 1.2 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef ICFGManagerStandard_h
#define ICFGManagerStandard_h

//--------------------------------------------------------------------
// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/CFG/EachCFGInterface.hpp>
#include <OpenAnalysis/CFG/CFGStandard.hpp>
#include <OpenAnalysis/IRInterface/ICFGIRInterface.hpp>
#include "ICFGStandard.hpp"

/*
#include <queue>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>
*/

namespace OA {
  namespace ICFG {

/*! 
   The AnnotationManager for ICFGStandard.
   This class can build an ICFGStandard, 
   (eventually) read one in from a file, and write one out to a file.
*/
class ManagerICFGStandard { 
public:
  ManagerICFGStandard(OA_ptr<ICFGIRInterface> _ir);
 ~ManagerICFGStandard () {}

  OA_ptr<ICFGStandard> performAnalysis( OA_ptr<IRProcIterator>,
                                        OA_ptr<CFG::EachCFGInterface> );

private: // helper functions

  bool stmt_has_call(StmtHandle stmt);

  StmtHandle 
  get_call_stmt(OA_ptr<CFG::CFGStandard::Node> cfgNode);

  OA_ptr<ICFGStandard::Node> create_icfg_node(
        OA_ptr<ICFGStandard> icfg, ProcHandle proc, NodeType pType, 
        OA_ptr<CFG::CFGStandard::Node> cfgNode);

  OA_ptr<ICFGStandard::Node> handle_call_node(OA_ptr<ICFGStandard> icfg, 
        ProcHandle proc, OA_ptr<CFG::CFGStandard::Node> cfgNodeNew,
        OA_ptr<ICFGStandard::Node> prevICFGNode, 
        OA_ptr<ICFGStandard::Node>& firstICFGNode,
        std::list<ProcHandle>& worklist);
    
  OA_ptr<std::list<OA_ptr<CFG::CFGStandard::Node> > >
  break_out_calls(OA_ptr<CFG::CFGStandard::Node> cfgNode);

private: // member variables

  OA_ptr<ICFGIRInterface> mIR;
  OA_ptr<CFG::EachCFGInterface> mEachCFG;
  
  // mapping of CFG nodes to ICFG nodes so that know what
  // ICFG nodes edges should connect
  typedef std::map<OA_ptr<CFG::CFGStandard::Node>,OA_ptr<ICFGStandard::Node> > 
      NodeToNodeMap;
  NodeToNodeMap mCFGNodeToICFGNode;


};

  } // end of ICFG namespace
} // end of OA namespace

#endif
