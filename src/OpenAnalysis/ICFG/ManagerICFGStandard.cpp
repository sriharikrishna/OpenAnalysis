/*! \file
  
  \brief The AnnotationManager that generates ICFG

  \authors Michelle Strout
  \version $Id: ManagerICFGStandard.cpp,v 1.2 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerICFGStandard.hpp"

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerICFGStandard)
static bool debug = true;
#else
static bool debug = false;
#endif

namespace OA {
  namespace ICFG {


/*!
*/
ManagerICFGStandard::ManagerICFGStandard(OA_ptr<ICFGIRInterface> _ir) 
    : mIR(_ir)
/*!
 */
{ 
}

bool ManagerICFGStandard::stmt_has_call(StmtHandle stmt)
{
    bool callflag = false;
    OA_ptr<IRCallsiteIterator> callsiteItPtr = mIR->getCallsites(stmt);
    for ( ; callsiteItPtr->isValid(); ++(*callsiteItPtr)) {
        ExprHandle call = callsiteItPtr->current();
        SymHandle sym = mIR->getSymHandle(call);
        ProcHandle proc = mIR->getProcHandle(sym);
        if (debug) {
          std::cout << "sym for callee = " 
                    << mIR->toString(sym) << std::endl;
        }
        if (proc!=ProcHandle(0)) {
            callflag = true;
        }
    }
    return callflag;
}

StmtHandle 
ManagerICFGStandard::get_call_stmt(OA_ptr<CFG::CFGStandard::Node> cfgNode)
{
    OA_ptr<StmtHandleIterator> stmtIter;
    stmtIter = cfgNode->getNodeStatementsIterator();
    for ( ; stmtIter->isValid(); (*stmtIter)++ ) {
        StmtHandle stmt = stmtIter->current();
        if ( stmt_has_call(stmt) ) {
            return stmt; 
        }
    }
    return StmtHandle(0);
}

OA_ptr<ICFGStandard::Node> ManagerICFGStandard::create_icfg_node(
        OA_ptr<ICFGStandard> icfg, ProcHandle proc, NodeType pType, 
        OA_ptr<CFG::CFGStandard::Node> cfgNode)
{
  OA_ptr<ICFGStandard::Node> icfgNode; 
  icfgNode = new ICFGStandard::Node(icfg, proc, pType, cfgNode);
  icfg->addNode(icfgNode);
  mCFGNodeToICFGNode[cfgNode] = icfgNode;
  return icfgNode;
}

/*! The returned prevICFGNode (last one in sequence) is returned.
    firstICFGNode is set to first node in sequence.
*/
OA_ptr<ICFGStandard::Node>
ManagerICFGStandard::handle_call_node(OA_ptr<ICFGStandard> icfg, 
        ProcHandle proc, OA_ptr<CFG::CFGStandard::Node> cfgNodeNew,
        OA_ptr<ICFGStandard::Node> prevICFGNode, 
        OA_ptr<ICFGStandard::Node>& firstICFGNode,
        std::list<ProcHandle>& worklist)
{
    OA_ptr<ICFGStandard::Node> callICFGNode;
    OA_ptr<ICFGStandard::Edge> icfgEdge;
    StmtHandle stmt = get_call_stmt(cfgNodeNew);

    // Call node and matched return node
    firstICFGNode = callICFGNode 
        = create_icfg_node(icfg,proc,CALL_NODE,cfgNodeNew);

    // edge between prevICFGNode and callICFGNode 
    if (!prevICFGNode.ptrEqual(0)) {
        icfgEdge = new ICFGStandard::Edge(icfg, 
                prevICFGNode, callICFGNode, CFLOW_EDGE, ExprHandle(0));
        icfg->addEdge(icfgEdge);
    }

    // return node
    OA_ptr<CFG::CFGStandard::Node> emptyNode;
    emptyNode = new CFG::CFGStandard::Node;
    prevICFGNode = create_icfg_node(icfg,proc,RETURN_NODE,emptyNode);

    // edge between callnode and return node
    icfgEdge = new ICFGStandard::Edge(icfg, 
                callICFGNode, prevICFGNode, CFLOW_EDGE, ExprHandle(0));
    icfg->addEdge(icfgEdge);

    // for each call to a defined procedure add a call Edge and return edge
    OA_ptr<IRCallsiteIterator> 
        callsiteItPtr = mIR->getCallsites(stmt);
    for ( ; callsiteItPtr->isValid(); ++(*callsiteItPtr)) {
        ExprHandle call = callsiteItPtr->current();
        SymHandle calleesym = mIR->getSymHandle(call);
        ProcHandle callee = mIR->getProcHandle(calleesym);
        if (callee==ProcHandle(0)) { continue; }
        worklist.push_back(callee);
        // get CFG
        OA_ptr<CFG::Interface> calleeCFGInterface 
            = mEachCFG->getCFGResults(callee);
        OA_ptr<CFG::CFGStandard> calleeCFG 
            = calleeCFGInterface.convert<CFG::CFGStandard>();
        // get entry and exit node
        OA_ptr<CFG::Interface::Node> temp
            = calleeCFG->getEntry();
        OA_ptr<CFG::CFGStandard::Node> entryNode 
            = temp.convert<CFG::CFGStandard::Node>();
        temp = calleeCFG->getExit();
        OA_ptr<CFG::CFGStandard::Node> exitNode 
            = temp.convert<CFG::CFGStandard::Node>();
        // make an ICFG node for these if necessary
        if (mCFGNodeToICFGNode[entryNode].ptrEqual(0)) {
            mCFGNodeToICFGNode[entryNode] 
                = new ICFGStandard::Node(icfg, callee, ENTRY_NODE, entryNode);
            icfg->addNode(mCFGNodeToICFGNode[entryNode]);
        }
        if (mCFGNodeToICFGNode[exitNode].ptrEqual(0)) {
            mCFGNodeToICFGNode[exitNode] 
                = new ICFGStandard::Node(icfg, callee, EXIT_NODE, exitNode);
           icfg->addNode(mCFGNodeToICFGNode[exitNode]);
        }
                             
        // edge to link callICFGNode to entryNode
        icfgEdge = new ICFGStandard::Edge(icfg, 
            callICFGNode, mCFGNodeToICFGNode[entryNode], CALL_EDGE, call);
        icfg->addEdge(icfgEdge);
                    
        // edge to link exitNode to prevICFGNode
        icfgEdge = new ICFGStandard::Edge(icfg, 
            mCFGNodeToICFGNode[exitNode], prevICFGNode, RETURN_EDGE, call);
        icfg->addEdge(icfgEdge);
    }

    return prevICFGNode;
}

OA_ptr<std::list<OA_ptr<CFG::CFGStandard::Node> > > 
ManagerICFGStandard::break_out_calls(OA_ptr<CFG::CFGStandard::Node> cfgNode)
{
    OA_ptr<std::list<OA_ptr<CFG::CFGStandard::Node> > > retval;
    retval = new std::list<OA_ptr<CFG::CFGStandard::Node> >;

    OA_ptr<CFG::CFGStandard::Node> prevNode;
    prevNode = new CFG::CFGStandard::Node;

    // loop through statements in original cfgNode
    // inserting them into prevNode until hit a call stmt
    // when hit a call statement make a new node for the call stmt
    // and then a new prevNode and loop
    OA_ptr<StmtHandleIterator> stmtIter;
    stmtIter = cfgNode->getNodeStatementsIterator();
    for ( ; stmtIter->isValid(); (*stmtIter)++ ) {
        StmtHandle stmt = stmtIter->current();
        if ( stmt_has_call(stmt) ) {
            if (prevNode->size() > 0) {
                retval->push_back(prevNode);
                prevNode = new CFG::CFGStandard::Node;
            }
            prevNode->add(stmt);
            retval->push_back(prevNode);
            prevNode = new CFG::CFGStandard::Node;
        } else {
            prevNode->add(stmt);
        }
    }
    retval->push_back(prevNode);

    return retval;
}

/*!
    Creates an ICFG based upon the CFGs for the program
*/
OA_ptr<ICFGStandard> ManagerICFGStandard::performAnalysis( 
    OA_ptr<IRProcIterator> procIter,
    OA_ptr<CFG::EachCFGInterface> eachCFG)
{
  OA_ptr<ICFGStandard::Edge> icfgEdge;
  OA_ptr<ICFGStandard> icfg; icfg = new ICFGStandard();

  // for use within helper functions
  mEachCFG = eachCFG;

  // list of edges that are needed, the ICFGStandard::Node is the
  // sink of the edge and the CFGNodes are predecessors
  // have to keep these until the end and all CFG nodes are mapped
  // to ICFG nodes
  std::map<OA_ptr<ICFGStandard::Node>,
           std::set<OA_ptr<CFG::CFGStandard::Node> > > edgeMap;


  /*
  // get the CFG for the root procedure
  OA_ptr<CFG::Interface> rootCFGInterface = eachCFG->getCFGResults(rootProc);
  OA_ptr<CFG::CFGStandard> rootCFG 
      = rootCFGInterface.convert<CFG::CFGStandard>();
  if (debug) {
      std::cout << "--- rootCFG" << std::endl;
      rootCFG->dumpdot(std::cout, mIR);
  }

  // get entry and exit node for cfg
  OA_ptr<CFG::Interface::Node> temp = rootCFG->getEntry();
  OA_ptr<CFG::CFGStandard::Node> cfgNode 
      = temp.convert<CFG::CFGStandard::Node>();
  OA_ptr<ICFGStandard::Node> icfgNode 
      = create_icfg_node(icfg, rootProc, ENTRY_NODE, cfgNode);

  temp = rootCFG->getExit();
  cfgNode = temp.convert<CFG::CFGStandard::Node>();
  icfgNode = create_icfg_node(icfg, rootProc, EXIT_NODE, cfgNode);
  */

  // worklist and set of procs that have already been visited
  std::list<ProcHandle> worklist;
  std::set<ProcHandle> alreadyVisited;
  for ( procIter->reset(); procIter->isValid(); (*procIter)++ ) {
    worklist.push_back(procIter->current());
  }

  // while there is a proc in the worklist
  while ( ! worklist.empty() ) {
    
    // get next proc and make sure it hasn't already been visited
    ProcHandle proc = worklist.front();
    worklist.pop_front();
    if (alreadyVisited.find(proc)!=alreadyVisited.end()) {
        continue;
    } else {
        alreadyVisited.insert(proc);
    }

    // get cfg 
    OA_ptr<CFG::Interface> cfgInterface = eachCFG->getCFGResults(proc);
    OA_ptr<CFG::CFGStandard> cfg = cfgInterface.convert<CFG::CFGStandard>();
    if (debug) {
      std::cout << "--- cfg" << std::endl;
      cfg->dumpdot(std::cout, mIR);
      //cfg->dumpdot(std::cout, mIR);
    }

    // for each node in cfg create an icfg node
    OA_ptr<CFG::CFGStandard::NodesIterator> cfgNodeIter
        = cfg->getNodesIterator();
    for ( ; cfgNodeIter->isValid(); (*cfgNodeIter)++ ) {
        OA_ptr<CFG::CFGStandard::Node> cfgNode = cfgNodeIter->current();

        if (debug) {
            std::cout << "cfgNode = ";
            cfgNode->dumpdot(*cfg,std::cout,mIR);
        }

        // determine if any of the statements contain a call
        OA_ptr<StmtHandleIterator> stmtIter;
        stmtIter = cfgNode->getNodeStatementsIterator();
        bool callflag = false;
        for ( ; stmtIter->isValid(); (*stmtIter)++ ) {
            StmtHandle stmt = stmtIter->current();
            if (debug) { std::cout << "stmt = " 
                                   << mIR->toString(stmt) << std::endl;}
            if ( stmt_has_call(stmt) ) {
                callflag = true;
                if (debug) { std::cout << "\tcallflag = true" << std::endl;}
            }
        }

        // if there were any calls in any of the statements
        if (callflag) {
            // get list of new CFG nodes, will NOT have empty ones for
            // return nodes
            OA_ptr<std::list<OA_ptr<CFG::CFGStandard::Node> > > newCFGList
                = break_out_calls(cfgNode);
            std::list<OA_ptr<CFG::CFGStandard::Node> >::iterator newCFGNodeIter;
            newCFGNodeIter = newCFGList->begin();

            // for first one, create associated ICFG node and link that
            // associated ICFG with all predecessors for original CFG node
            OA_ptr<CFG::CFGStandard::Node> firstCFGNode = *newCFGNodeIter;
            if (debug) {
                std::cout << "firstCFGNode = ";
                firstCFGNode->dumpdot(*cfg,std::cout,mIR);
            }
            OA_ptr<ICFGStandard::Node> firstICFGNode, prevICFGNode;
            // Call node and matched return node
            if ( get_call_stmt(firstCFGNode) != StmtHandle(0) ) {
                prevICFGNode = handle_call_node(icfg, proc, firstCFGNode,
                                                prevICFGNode, firstICFGNode,
                                                worklist);
            // regular cfg node
            } else if (firstCFGNode->size() > 0) {
                firstICFGNode = create_icfg_node(icfg,proc,CFLOW_NODE, 
                                                 firstCFGNode);
                                                 
                prevICFGNode = firstICFGNode;
            }

            if (debug) {
                std::cout << "firstICFGNode = ";
                firstICFGNode->dumpdot(*icfg,std::cout,mIR);
            }

            // get predecessors for original CFG node
            OA_ptr<CFG::CFGStandard::PredNodesIterator> predIter
                = cfgNode->getPredNodesIterator();
            for ( ; predIter->isValid(); (*predIter)++ ) {
                OA_ptr<CFG::CFGStandard::Node> predNode = predIter->current();
                // temporarily record
                // edge between predecessor ICFG node and firstICFGNode
                edgeMap[firstICFGNode].insert(predNode);
            }

            // for middle ones create appropriate ICFG and link to previous
            newCFGNodeIter++;
            for ( ; newCFGNodeIter!=newCFGList->end(); newCFGNodeIter++ )
            {
                OA_ptr<CFG::CFGStandard::Node> cfgNodeNew = *newCFGNodeIter;

                // Call node and matched return node
                if ( get_call_stmt(cfgNodeNew) != StmtHandle(0) ) {
                    prevICFGNode = handle_call_node(icfg, proc, cfgNodeNew,
                                                  prevICFGNode, firstICFGNode,
                                                  worklist);

                // regular cfg node
                } else if (cfgNodeNew->size() > 0) {

                    OA_ptr<ICFGStandard::Node> currICFGNode;
                    currICFGNode  
                        = create_icfg_node(icfg,proc,CFLOW_NODE,cfgNodeNew);

                    // edge between prevICFGNode and this one
                    icfgEdge = new ICFGStandard::Edge(icfg, 
                                prevICFGNode, currICFGNode, CFLOW_EDGE,
                                ExprHandle(0));
                    icfg->addEdge(icfgEdge);

                    prevICFGNode = currICFGNode;
                }

            } 
            
            // for last one, associate with original CFG node
            // so correct linkages will occur later when successors
            // are linking with predecessors
            mCFGNodeToICFGNode[cfgNode] = prevICFGNode; 

        // CFG node that doesn't have a call stmt
        } else {
            OA_ptr<ICFGStandard::Node> icfgNode; 
            if (mCFGNodeToICFGNode[cfgNode].ptrEqual(0)) {
                if (cfg->getEntry() == cfgNode) {
                    icfgNode = create_icfg_node(icfg,proc,ENTRY_NODE,cfgNode);
                } else if (cfg->getExit() == cfgNode) {
                    icfgNode = create_icfg_node(icfg,proc,EXIT_NODE,cfgNode);
                } else {
                    icfgNode = create_icfg_node(icfg,proc,CFLOW_NODE,cfgNode);
                }
            } else {
                icfgNode = mCFGNodeToICFGNode[cfgNode];
            }

            OA_ptr<CFG::CFGStandard::PredNodesIterator> predIter
                = cfgNode->getPredNodesIterator();
            for ( ; predIter->isValid(); (*predIter)++ ) {
                OA_ptr<CFG::CFGStandard::Node> predNode = predIter->current();
                // temporarily record
                // edge between predecessor ICFG node and firstICFGNode
                edgeMap[icfgNode].insert(predNode);
            }
        }

    } // end of loop over CFG nodes 


  } // worklist loop for procedures          
  
  // loop over edges and generate ICFG edges
  std::map<OA_ptr<ICFGStandard::Node>,
           std::set<OA_ptr<CFG::CFGStandard::Node> > >::iterator mapIter;
  for (mapIter=edgeMap.begin(); mapIter!=edgeMap.end(); mapIter++ ) {
      OA_ptr<ICFGStandard::Node> icfgNode = mapIter->first;
      std::set<OA_ptr<CFG::CFGStandard::Node> >& nodeSet = mapIter->second;
      std::set<OA_ptr<CFG::CFGStandard::Node> >::iterator nodeIter;
      for (nodeIter=nodeSet.begin(); nodeIter!=nodeSet.end(); nodeIter++) {
          icfgEdge = new ICFGStandard::Edge(icfg, 
                  mCFGNodeToICFGNode[*nodeIter], 
                  icfgNode, CFLOW_EDGE, ExprHandle(0));
          icfg->addEdge(icfgEdge);
      }
  }

  return icfg;
}


  } // end of namespace MPICFG
} // end of namespace OA
