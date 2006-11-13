/*! \file
  
  \brief Implementation of CallGraphStandard.

  Implements the CallGraph::Interface for viewing the contents and structure
  of a CallGraph.

  Also implements methods useful for constructing a CallGraph.

  \authors Arun Chauhan (2001 as part of Mint), Nathan Tallent, Michelle Strout
  \version $Id: CallGraphStandard.cpp,v 1.14 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

//--------------------------------------------------------------------
//--------------------------------------------------------------------

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
#include "CallGraphStandard.hpp"

namespace OA {
  namespace CallGraph {

//--------------------------------------------------------------------
unsigned int CallGraphStandard::Node::sNextId = 1;
unsigned int CallGraphStandard::Edge::sNextId = 1;

//--------------------------------------------------------------------
// must be updated any time CallGraph::Interface::EdgeType changes
static const char *sEdgeTypeToString[] = { 
  "NORMAL_EDGE" // FIXME (make CFG's version static)
};

//--------------------------------------------------------------------
CallGraphStandard::CallGraphStandard(const SymHandle name) 
  : mName(name)
{
}

CallGraphStandard::CallGraphStandard() 
  : mName(SymHandle(0))
{
}


CallGraphStandard::~CallGraphStandard()
{
}

//--------------------------------------------------------------------


//--------------------------------------------------------------------
CallGraphStandard::Edge::Edge (OA_ptr<Interface::Node> n1, 
                               OA_ptr<Interface::Node> n2, 
                               Interface::EdgeType _type,
                               ExprHandle call) 
  : DGraph::DGraphStandard::Edge(n1.convert<Node>(), 
                                 n2.convert<Node>()), mType(_type),
                                 mCallsiteExpr(call)
{
  mId = sNextId++;
}
//--------------------------------------------------------------------


void CallGraphStandard::Edge::dump(ostream& os)
{
  os << sEdgeTypeToString[mType];
}
//--------------------------------------------------------------------

void CallGraphStandard::connect (OA_ptr<Interface::Node> src, 
              OA_ptr<Interface::Node> dst, 
              Interface::EdgeType type, ExprHandle call) 
{
    OA_ptr<Edge> e; e = new Edge (src, dst, type, call);
    addEdge (e);
}

OA_ptr<CallGraphStandard::Node> 
CallGraphStandard::findOrAddNode(SymHandle sym)
{
  OA_ptr<CallGraphStandard::Node> node; node = NULL;

  SymToNodeMapType::iterator it = mSymToNodeMap.find(sym);
  if (it == mSymToNodeMap.end()) {
    node = new CallGraphStandard::Node(sym);
    addNode(node);
    mSymToNodeMap.insert(SymToNodeMapType::value_type(sym, node));
  } else {
    node = (*it).second;
  }
  assert(!node.ptrEqual(NULL));
  
  return node;
}

//--------------------------------------------------------------------
void
CallGraphStandard::dump (ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
  os << "===== CallGraph: " << ir->toString(mName) << " =====\n"
     << endl;
  
  // print the contents of all the nodes
  NodesIterator nodes_iter(*this);
  for ( ; nodes_iter.isValid(); ++nodes_iter) {
    OA_ptr<CallGraphStandard::Node> node = nodes_iter.current();
    node->longdump(os, ir);
    os << endl;
  }
  
  os << "====================" << endl;

}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void
CallGraphStandard::dumpdot (ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{

  os << "digraph OA_CallGraph {" << endl;
  os << "node [shape=rectangle];" << endl;

  // then output nodes and edges by procedure
  OA_ptr<EdgesIterator> iter = getEdgesIterator();
  for (; iter->isValid(); ++(*iter) ) {
    OA_ptr<Edge> edge = iter->current();
    OA_ptr<Node> srcNode  = edge->source();
    OA_ptr<Node> sinkNode = edge->sink();

    SymHandle srcSym  = srcNode->getProcSym();
    SymHandle sinkSym = sinkNode->getProcSym();
    
    os << ir->toString(srcSym);
    os << "->";
    os << ir->toString(sinkSym);
    os << ";" << std::endl;
  }

  os << "}" << endl;
  os.flush();
}
//--------------------------------------------------------------------



//--------------------------------------------------------------------
void
CallGraphStandard::Node::dump (ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
  if (mSym.hval() == 0) {
    os << "<no-symbol>";
  } else {
    os << ir->toString(mSym);
  }
  os << " " << (isDefined() ? "[defined]" : "[referenced]");
}

/*
void
CallGraph::Node::shortdump (CallGraph *cgraph, ostream& os)
{
  os << mSym;
  IRInterface& ir = cgraph->GetIRInterface();
  if (sym == 0) {
    os << "<no-symbol>";
  } else {
    os << ir->GetSymNameFromSymHandle(sym);
  }
  os << " " << (IsDefined() ? "[defined]" : "[referenced]");
}
*/


void
CallGraphStandard::Node::longdump ( ostream& os, 
                                   OA_ptr<IRHandlesIRInterface> ir)
{
  // print the node ID
  os << "CallGraph Node: ";
  dump(os,ir);
  
  if (isAnEntry()) {
    os << " (ENTRY)";
  } else if (isAnExit()) {
    os << " (EXIT)"; 
  }
  os << endl;
  
  // print the source(s)
  unsigned int count = 0;
  SourceNodesIterator srcIt(*this);
  for ( ; srcIt.isValid(); ++srcIt, ++count) {
    OA_ptr<Node> node = srcIt.current();
    if (count == 0) { os << " <-- ("; }
    else            { os << ", "; }

    node->dump(os,ir);
  }
  if (count > 0) { os << ")" << endl; }
  
  // print the sink(s)
  count = 0;
  //OutgoingEdgesIterator outIt(*this);
  SinkNodesIterator outIt(*this);
  for ( ; outIt.isValid(); ++outIt, ++count) {
    //OA_ptr<Edge> edge = outIt.current();
    //OA_ptr<Node> node = edge->sink().convert<Node>();
    OA_ptr<Node> node = outIt.current();
    if (count == 0) { os << " --> ("; } 
    else            { os << ", "; }
    
    node->dump(os,ir);
    //os << " [";
    //edge->shortdump(os);
    //os << "]";
  }
  if (count > 0) { os << ")" << endl; }

}

//--------------------------------------------------------------------

  } // end namespace CallGraph
} // end namespace OA
