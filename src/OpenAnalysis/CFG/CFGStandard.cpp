/*! \file
  
  \brief Implementation of CFGStandard.

  Implements the CFG::Interface for viewing the contents and structure
  of a CFG.

  Also implements methods useful for constructing a CFG including methods
  that access a mapping of labels (OA::StmtLabel's) to Nodes.

  \authors Arun Chauhan (2001 as part of Mint), Nathan Tallent, Michelle Strout
  \version $Id: CFGStandard.cpp,v 1.25 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/


// standard headers

#ifdef NO_STD_CHEADERS
# include <stdlib.h>
# include <string.h>
# include <assert.h>
#else
# include <cstdlib>
# include <cstring>
# include <cassert>
using namespace std; // For compatibility with non-std C headers
#endif

#include <iostream>
using std::ostream;
using std::endl;
using std::cout;
using std::cerr;

// Mint headers
#include "CFGStandard.hpp"

namespace OA {
  namespace CFG {

//--------------------------------------------------------------------
unsigned int CFGStandard::Node::sNextId = 1;
unsigned int CFGStandard::Edge::sNextId = 1;

// must be updated any time CFG::Interface::EdgeType changes
const char *CFGStandard::Edge::sEdgeTypeToString[] = {
  "TRUE_EDGE", "FALLTHROUGH_EDGE", "FALSE_EDGE",
  "BACK_EDGE", "MULTIWAY_EDGE", "BREAK_EDGE",
  "CONTINUE_EDGE", "RETURN_EDGE" 
};

//--------------------------------------------------------------------
const char* CFGStandard::Edge::edgeTypeToString(EdgeType et) const 
{
    return sEdgeTypeToString[et];
}

void CFGStandard::Edge::dump (ostream& os)
{
  os << "{ " << mId << " " 
     << edgeTypeToString(mType) << " " 
     << mExpr << " }";
  os.flush();
}

//--------------------------------------------------------------------
CFGStandard::CFGStandard (SymHandle _name) : mName(_name)
/*!
 * \brief Constructs an empty standard CFGStandard
 *
 * \param _name    SymbolHandle for procedure name
 */
{ 
}

CFGStandard::~CFGStandard()
/*!
 * \brief Destructor for CFGStandard
 */
{
  // FIXME: should iterate through all nodes and edges to delete them
  
  mEntry = NULL;
  mExit = NULL;
  mlabel_to_node_map.clear();
}



//--------------------------------------------------------------------
//--------------------------------------------------------------------
CFGStandard::Edge::Edge (OA_ptr<Interface::Node> n1, 
                         OA_ptr<Interface::Node> n2, 
                         EdgeType _type, ExprHandle _expr) 
    : DGraph::DGraphStandard::Edge(n1.convert<Node>(), 
                                   n2.convert<Node>()), 
      mType(_type), mExpr(_expr)
/*!
 * \brief Edge constructor
 *
 */
{
  mId = sNextId++;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
/*! 
   \brief Given a label, determine whether it has been encountered already.
   If so, return its associated block (obtained from the mlabel_to_node_map).
   If not, return 0
*/
OA_ptr<CFGStandard::Node> CFGStandard::getLabelBlock(OA::StmtLabel lab)
{
  OA_ptr<CFGStandard::Node> retval; retval = 0;

  if (mlabel_to_node_map.find(lab) != mlabel_to_node_map.end()) {
    retval = mlabel_to_node_map[lab];
  }
  return retval;
}

//--------------------------------------------------------------------
/*! 
   \brief Given a label, determine whether it has been encountered already.
   If so, return its associated block (obtained from the mlabel_to_node_map).
   If not, create a new block and update the mlabel_to_node_map.
*/ 
OA_ptr<CFGStandard::Node> CFGStandard::node_from_label (OA::StmtLabel lab)
{
  OA_ptr<CFGStandard::Node> node; node = 0;
  if (mlabel_to_node_map.find(lab) != mlabel_to_node_map.end()) {
    // This label has been encountered already, thus it already has an associated block.
    node = mlabel_to_node_map[lab];
  }
  else {
    // This label has not been encountered yet, so create a new block.
    node = new CFGStandard::Node();
    addNode(node);
    mlabel_to_node_map[lab] = node; 
  }
  return node;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*!
    \brief Transfer all statements beginning at (and including) splitPoint to 
           the new basic block, 'newBlock'
*/
void CFGStandard::Node::split(OA::StmtHandle splitPoint, OA_ptr<Node> newBlock)
{
  std::list<StmtHandle>::iterator splitPointIter;
  std::list<StmtHandle> tempList;
  std::list<StmtHandle>::iterator tempIter;
  
  for (splitPointIter = mStmt_list->begin();
       (splitPointIter != mStmt_list->end() && *splitPointIter != splitPoint);
       ++splitPointIter) {
  }
  // 'splitPointIter' now points to 'splitPoint'

  // splice them into temp list
  tempList.splice(tempList.end(), *mStmt_list, splitPointIter, mStmt_list->end());

  // then copy them into newBlock, can't splice to newBlock, because can't
  // assume we now how newBlock stores data
  for (tempIter=tempList.begin(); tempIter != tempList.end(); tempIter++) {
      newBlock->add(*tempIter);
  }
}


/*!
    \brief  Remove 'h' from the statement list; return the removed handle or 
            0 if not found.
*/
StmtHandle CFGStandard::Node::erase(StmtHandle h)
{
  std::list<StmtHandle>::iterator it;
  for (it = mStmt_list->begin(); (it != mStmt_list->end()); ++it) {
    StmtHandle stmt = *it;
    if (stmt == h) {
      mStmt_list->erase(it);
      return h;
    }
  }
  return 0;
}

/*!
   \brief Split the basic block 'block' at the given split point and return
          the new basic block.  
          
   \param splitPoint The first statement in the new basic block.  
   
   After the operation is done, the new block
   contains all the outgoing edges from 'block'; in other words,
  'block' will have *no* outgoing edges.
*/
OA_ptr<CFGStandard::Node>
CFGStandard::splitBlock (OA_ptr<CFGStandard::Node> block, StmtHandle splitPoint)
{
  // TODO: if a NOP is in the middle of being processed split into 
  // 2 NOPs so they can be split. ???

  OA_ptr<Node> newBlock; newBlock = new Node;
  addNode(newBlock);

  // 1. Move all instructions beginning at 'splitPoint' from 'block'
  // to the new block
  block->split((StmtHandle)splitPoint, newBlock);

  // 2. Change edge information
  std::list<OA_ptr<CFGStandard::Edge> > del_list;
  
  // 2a. Outgoing edges of 'block' must become outgoing edges of 'newBlock'
  for (OutgoingEdgesIterator ei(*block); ei.isValid(); ++ei) {
      // Add successor edges for new block
      OA_ptr<CFGStandard::Edge> edge 
          = ei.current();
      OA_ptr<CFGStandard::Node> targBlock 
          = edge->sink();
      connect(newBlock, targBlock, edge->getType(), edge->getExpr());
      del_list.push_back(edge);

      // 2b. Update fallthrough, mFallThrough is datastructure used
      // by ManagerStandard, no longer used here, MMS 4/7/04
  }
  
  // 2c. Remove outgoing edges of 'block'
  std::list<OA_ptr<CFGStandard::Edge> >::iterator dli = del_list.begin();
  for ( ; dli != del_list.end(); dli++) {
    disconnect(*dli);
  }

#if 0 // FIXME: Splitting won't change any label's block??
  // Update label->block maps.
  for (NodeStatementsIterator si(newBlock); (StmtHandle)si; ++si) {
    StmtHandle stmt = (StmtHandle)si;
    if (ir->GetLabel(stmt) != 0) {
      label_to_node_map.erase(ir->GetLabel(stmt));
      label_to_node_map[ir->GetLabel(stmt)] = newBlock;
    }
  }
#endif
  
  return newBlock;
}

//--------------------------------------------------------------------
//! Connect the src to each of the nodes in dst_list.
void
CFGStandard::connect (OA_ptr<Interface::Node> src, 
                      CFGStandard::NodeLabelList& dst_list)
{
    CFGStandard::NodeLabelListIterator dst_list_iter(dst_list);
    while (dst_list_iter.isValid()) {
        NodeLabel nl = dst_list_iter.current();
        connect(src, nl.getNode(), nl.getEdgeType(), nl.getExpr());
        ++dst_list_iter;
    }
}


//! Connect each node in src_list to dst.
void
CFGStandard::connect (CFGStandard::NodeLabelList& src_list, 
                      OA_ptr<Interface::Node> dst)
{
  NodeLabelListIterator src_list_iter(src_list);
  while (src_list_iter.isValid()) {
    NodeLabel nl = src_list_iter.current();
    connect(nl.getNode(), dst, nl.getEdgeType(), nl.getExpr());
    ++src_list_iter;
  }
}

//! Nathan's
void
CFGStandard::dump (ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
  os << "========== CFG dump ==========" << endl 
     << ir->toString(mName) << endl 
     << "------------------------------" << endl;

  // print the contents of all the nodes
  OA_ptr<NodesIterator> nodes_iter = getNodesIterator();
  for ( ; nodes_iter->isValid(); ++(*nodes_iter)) {
    OA_ptr<Node> node = nodes_iter->current();
    // node->dump(std::cout, ir);
    node->longdump(*this, os, ir);
    os << endl;
  }
  
  os << "==============================" << endl;
  dumpdot(os,ir);
  os << "==============================" << endl;
}

void
CFGStandard::Node::dump (ostream& os, OA_ptr<IRHandlesIRInterface> rir)
{
  // print the node ID
  os << "CFG Node: " << getId();
  if (num_incoming() == 0)
    os << " (root)";
  os << endl;

  // print the node contents
  CFGStandard::NodeStatementsIterator stmtIt(*this);
  for ( ; stmtIt.isValid(); ++stmtIt) {
    os << "    ";
    StmtHandle s = stmtIt.current();
    rir->dump(s, os);
    os << endl;
  }

  // print the sink(s)
  OutgoingEdgesIterator out_iter(*this);
  if (out_iter.isValid()) {
    OA_ptr<Edge> e = out_iter.current();
    os << " --> (" << (e->sink())->getId();
    os << " [";
    e->dump(os);
    os << "]";
    ++out_iter;
    for ( ; out_iter.isValid(); ++out_iter) {
      e = out_iter.current();
      os << ", " << (e->sink())->getId();
      os << " [";
      e->dump(os);
      os << "]";
    }
    os << ")" << endl;
  }

}

void
CFGStandard::Node::longdump (CFGStandard& currcfg, ostream& os, 
                             OA_ptr<IRHandlesIRInterface> rir)
{
  // print the node ID
  os << "CFG Node: " << getId();
  if (num_incoming() == 0)
    os << " (root)";
  if (currcfg.getEntry().ptrEqual(this)) {
    os << " [ENTRY]";
  } 
  else if (currcfg.getExit().ptrEqual(this)) {
    os << " [EXIT]"; 
  }
  os << endl;
  

  // print the node contents
  CFGStandard::NodeStatementsIterator stmtIt(*this);
  for ( ; stmtIt.isValid(); ++stmtIt) {
    os << "    ";
    StmtHandle s = stmtIt.current();
    rir->dump(s, os);
    os << endl;
  }
  
  // print the sources(s)
  IncomingEdgesIterator in_iter(*this);
  if (in_iter.isValid()) {
    OA_ptr<Edge> e = in_iter.current();
    os << " <-- (" << (e->source())->getId();
    os << " [";
    e->dump(os);
    os << "]";
    ++in_iter;
    for ( ; in_iter.isValid(); ++in_iter) {
      e = in_iter.current();
      os << ", " << (e->source())->getId();
      os << " [";
      e->dump(os);
      os << "]";
    }
    os << ")" << endl;
  }
  
  // print the sink(s)
  OutgoingEdgesIterator out_iter(*this);
  if (out_iter.isValid()) {
    OA_ptr<Edge> e = out_iter.current();
    os << " --> (" << (e->sink())->getId();
    os << " [";
    e->dump(os);
    os << "]";
    ++out_iter;
    for ( ; out_iter.isValid(); ++out_iter) {
      e = out_iter.current();
      os << ", " << (e->sink())->getId();
      os << " [";
      e->dump(os);
      os << "]";
    }
    os << ")" << endl;
  }  
}

void
CFGStandard::dumpdot(ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
  //const char* nm = ir.GetSymNameFromSymHandle(name);

  os << "digraph OA_CFG_" << ir->toString(mName) << " {" << endl;
  os << "node [shape=rectangle];" << endl;

  // print the contents of all the nodes (including edges)
  OA_ptr<NodesIterator> nodesIterPtr = getNodesIterator();
  for ( ; nodesIterPtr->isValid(); ++(*nodesIterPtr) ) {
    nodesIterPtr->current()->dumpdot(*this,os,ir);
  }

  // print all edges
  OA_ptr<EdgesIterator> edgesIterPtr = getEdgesIterator();
  for ( ; edgesIterPtr->isValid(); ++(*edgesIterPtr) ) {
    edgesIterPtr->current()->dumpdot(os,ir);
  }
  
  os << "}" << endl;
  os.flush();
}

void
CFGStandard::Node::dumpdot (CFGStandard &currcfg, ostream& os, 
                            OA_ptr<IRHandlesIRInterface> ir)
{
  // print the node
  os << getId() << " [ label=\"====== CFG node " << getId() << " ======";
  
  if (currcfg.getEntry().ptrEqual(this)) {
    os << " (entry)";
  } 
  else if (currcfg.getExit().ptrEqual(this)) {
    os << " (exit)"; 
  }
  os << "\\n";
  // print the node contents
  CFGStandard::NodeStatementsIterator stmtIt(*this);
  for ( ; stmtIt.isValid(); ++stmtIt) {
    StmtHandle s = stmtIt.current();
    os << std::endl << ir->toString(s) << "\\n";
  }
  os << "\" ];" << endl;
  os.flush();
}

void CFGStandard::Edge::dumpdot(ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
  os << source()->getId() << " -> " << sink()->getId() << ";" << endl;
  os.flush();
}




  } // end of CFG namespace
} // end of OA namespace
