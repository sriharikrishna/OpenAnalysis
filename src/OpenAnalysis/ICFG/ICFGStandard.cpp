/*! \file
  
  \brief Implementation of ICFGStandard.

  \authors Michelle Strout
  \version $Id: ICFGStandard.cpp,v 1.3 2005/10/17 21:21:41 garvin Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

//--------------------------------------------------------------------
//--------------------------------------------------------------------

// standard headers

#include <string.h>
#include <stdlib.h>
#include <iostream>
using std::ostream;
using std::endl;
using std::cout;
using std::cerr;

#include "ICFGStandard.hpp"

namespace OA {
  namespace ICFG {

//--------------------------------------------------------------------
unsigned int ICFGStandard::Node::sNextId = 1;
unsigned int ICFGStandard::Edge::sNextId = 1;

//--------------------------------------------------------------------
// must be updated any time ICFG::Interface::EdgeType changes
static const char *sEdgeTypeToString[] = { 
  "CFLOW_EDGE",
  "CALL_EDGE",
  "RETURN_EDGE"
};
// must be updated any time ICFG::Interface::EdgeType changes
static const char *sNodeTypeToString[] = { 
  "CFLOW_NODE",
  "CALL_NODE",
  "RETURN_NODE",
  "ENTRY_NODE",
  "EXIT_NODE"
};

//--------------------------------------------------------------------
/*! called by all Node constructors
 */
void ICFGStandard::Node::Ctor() { 
    mCallEdges = new std::list<OA_ptr<Edge> >;
    mReturnEdges = new std::list<OA_ptr<Edge> >;
    mId = sNextId++; 

    mDGNode = new DGraph::DGraphStandard::Node;

    if (mCFGNode.ptrEqual(0)) {
        assert(0);  // must pass ina CFG node
    }
}

//! number of statements in node
unsigned int ICFGStandard::Node::size () const
{
    return mCFGNode->size(); 
}

//! create a forward order iterator for the statements in the node
OA_ptr<CFG::Interface::NodeStatementsIterator> 
ICFGStandard::Node::getNodeStatementsIterator() const
{
    return mCFGNode->getNodeStatementsIterator();
}

//! create a reverse order iterator for the statements in the node
OA_ptr<CFG::Interface::NodeStatementsRevIterator> 
ICFGStandard::Node::getNodeStatementsRevIterator() const
{
    return mCFGNode->getNodeStatementsRevIterator();
}


//--------------------------------------------------------------------
void
ICFGStandard::Node::dump (ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
  os << sNodeTypeToString[mType] << std::endl;
  mCFGNode->dump(os,ir);
}

bool ICFGStandard::Node::operator==(DGraph::Interface::Node& other) 
{ 
    Node& recastOther = dynamic_cast<Node&>(other);
    return mDGNode == recastOther.mDGNode; 
}

bool ICFGStandard::Node::operator<(DGraph::Interface::Node& other) 
{ 
    Node& recastOther = dynamic_cast<Node&>(other);
    return mDGNode < recastOther.mDGNode; 
}

void
ICFGStandard::Node::longdump (ICFGStandard& icfg, 
                              ostream& os, 
                              OA_ptr<IRHandlesIRInterface> ir)
{
  // print the node ID
  os << "ICFGStandard Node: ";
  dump(os,ir);
  
  if (isAnEntry()) {
    os << " (ENTRY)";
  } else if (isAnExit()) {
    os << " (EXIT)"; 
  }
  os << endl;
  
  // print the source(s)
  unsigned int count = 0;
  OA_ptr<SourceNodesIterator> srcIter = getSourceNodesIterator();
  for ( ; srcIter->isValid(); ++(*srcIter), ++count) {
    OA_ptr<Node> node = srcIter->current();
    if (count == 0) { os << " <-- ("; }
    else            { os << ", "; }

    node->dump(os,ir);
  }
  if (count > 0) { os << ")" << endl; }
  
  // print the sink(s)
  count = 0;
  OA_ptr<SinkNodesIterator> outIter = getSinkNodesIterator();
  for ( ; outIter->isValid(); ++(*outIter), ++count) {
    OA_ptr<Node> node = outIter->current();
    if (count == 0) { os << " --> ("; } 
    else            { os << ", "; }
    
    node->dump(os,ir);
  }
  if (count > 0) { os << ")" << endl; }
}

void
ICFGStandard::Node::dumpdot (ICFGStandard &currcfg, ostream& os, 
                             OA_ptr<IRHandlesIRInterface> ir)
{
  // print the node
  os << getId() << " [ label=\"==  ICFG ";
  os << sNodeTypeToString[getType()] << " " << getId() << " ==";
  
  if (isAnEntry()) {
    os << " (entry)";
  } 
  else if (isAnExit()) {
    os << " (exit)"; 
  }
  os << "\\n"; 

  // print the node contents
  OA_ptr<CFG::Interface::NodeStatementsIterator> stmtIter 
      = getNodeStatementsIterator();
/*
  for ( ; stmtIter->isValid(); ++(*stmtIter)) {
    StmtHandle s = stmtIter->current();
    os << std::endl << ir->toString(s) << "\\n";
  }
*/
  if (stmtIter->isValid()) {
    os << "\\n"; // internal dot line break
  }
  for ( ; stmtIter->isValid(); ++(*stmtIter)) {
    StmtHandle s = stmtIter->current();
    std::string st = ir->toString(s);

    // replace any '\"' in fortran code with '\''
    // because it messes up the dot files
    // (could replace each '\"' with two characters: '\\' '\"' )
    char * cst;
    cst = (char *)malloc(strlen(st.c_str()) + 1);
    strcpy(cst, st.c_str());
    for (unsigned int j = 0; j < strlen(cst); j++) {
      if (cst[j]=='\"') {
        cst[j]='\'';
      }
    }

    os << cst << "\\n";
  }

  os << "\" ];" << endl;
  os.flush();
}


//--------------------------------------------------------------------

OA_ptr<ICFGStandard::IncomingEdgesIterator> 
    ICFGStandard::Node::getIncomingEdgesIterator() const
{ 
    OA_ptr<IncomingEdgesIterator> retval; 

    // get incoming edges for associated dgraph node
    OA_ptr<DGraph::DGraphStandard::IncomingEdgesIterator> dgEdgeIter
        = mDGNode->getIncomingEdgesIterator();
    // put them all in a set
    OA_ptr<std::list<OA_ptr<Edge> > > tempSet;
    tempSet = new std::list<OA_ptr<Edge> >;
    for ( ; dgEdgeIter->isValid(); (*dgEdgeIter)++ ) {
        tempSet->push_back(mICFG->getICFGEdge(dgEdgeIter->current()));
    }

    // create an iterator over the set of edges
    retval = new EdgesIterator(tempSet); 
    return retval;
}

OA_ptr<ICFGStandard::OutgoingEdgesIterator> 
    ICFGStandard::Node::getOutgoingEdgesIterator() const
{ 
    OA_ptr<OutgoingEdgesIterator> retval; 

    // get outgoing edges for associated dgraph node
    OA_ptr<DGraph::DGraphStandard::OutgoingEdgesIterator> dgEdgeIter
        = mDGNode->getOutgoingEdgesIterator();
    // put them all in a set
    OA_ptr<std::list<OA_ptr<Edge> > > tempSet;
    tempSet = new std::list<OA_ptr<Edge> >;
    for ( ; dgEdgeIter->isValid(); (*dgEdgeIter)++ ) {
        tempSet->push_back(mICFG->getICFGEdge(dgEdgeIter->current()));
    }

    // create an iterator over the set of edges
    retval = new EdgesIterator(tempSet); 
    return retval;
}

OA_ptr<ICFGStandard::SourceNodesIterator> 
    ICFGStandard::Node::getSourceNodesIterator() const
{ 
    OA_ptr<SourceNodesIterator> retval; 

    // get incoming edges for associated cfg node
    OA_ptr<DGraph::DGraphStandard::SourceNodesIterator> dgNodeIter
        = mDGNode->getSourceNodesIterator();
    // put them all in a set
    OA_ptr<std::list<OA_ptr<Node> > > tempSet; 
    tempSet = new std::list<OA_ptr<Node> >;
    for ( ; dgNodeIter->isValid(); (*dgNodeIter)++ ) {
        tempSet->push_back(mICFG->getICFGNode(dgNodeIter->current()));
    }
    // create an iterator over the set of nodes
    retval = new NodesIterator(tempSet); 
    return retval;
}

OA_ptr<ICFGStandard::SinkNodesIterator> 
    ICFGStandard::Node::getSinkNodesIterator() const
{ 
    OA_ptr<SinkNodesIterator> retval; 

    // get incoming edges for associated cfg node
    OA_ptr<DGraph::DGraphStandard::SinkNodesIterator> dgNodeIter
        = mDGNode->getSinkNodesIterator();
    // put them all in a set
    OA_ptr<std::list<OA_ptr<Node> > > tempSet; 
    tempSet = new std::list<OA_ptr<Node> >;
    for ( ; dgNodeIter->isValid(); (*dgNodeIter)++ ) {
        tempSet->push_back(mICFG->getICFGNode(dgNodeIter->current()));
    }
    // create an iterator over the set of nodes
    retval = new NodesIterator(tempSet); 
    return retval;
}



//--------------------------------------------------------------------
//--------------------------------------------------------------------
ICFGStandard::Edge::Edge (OA_ptr<ICFGStandard> pICFG,
                          OA_ptr<Node> pNode1, 
                          OA_ptr<Node> pNode2, 
                          EdgeType pType, ExprHandle call)
  : mICFG(pICFG), mNode1(pNode1), mNode2(pNode2), mType(pType), mCall(call)
{
  mId = sNextId++;

  // create DGraphEdge for associated DGraph
  mDGEdge = new DGraph::DGraphStandard::Edge(pNode1->mDGNode,
                                             pNode2->mDGNode);

}

ICFGStandard::Edge::Edge (OA_ptr<ICFGStandard> pICFG,
                          OA_ptr<Node> pNode1, 
                          OA_ptr<Node> pNode2, 
                          EdgeType pType)
  : mICFG(pICFG), mNode1(pNode1), mNode2(pNode2), mType(pType), 
    mCall(ExprHandle(0))
{
  mId = sNextId++;

  // create DGraphEdge for associated DGraph
  mDGEdge = new DGraph::DGraphStandard::Edge(pNode1->mDGNode,
                                             pNode2->mDGNode);

}


//--------------------------------------------------------------------
bool ICFGStandard::Edge::operator==(DGraph::Interface::Edge& other) 
{ 
    Edge& recastOther = dynamic_cast<Edge&>(other);
    return mDGEdge == recastOther.mDGEdge; 
}

bool ICFGStandard::Edge::operator<(DGraph::Interface::Edge& other) 
{ 
    Edge& recastOther = dynamic_cast<Edge&>(other);
    return mDGEdge < recastOther.mDGEdge; 
}



void ICFGStandard::Edge::dump(ostream& os)
{
  os << sEdgeTypeToString[mType];
}

void ICFGStandard::Edge::dumpdot(ostream& os)
{
    os << source()->getId() << " -> " << sink()->getId(); 
    switch(mType) {
        case (CALL_EDGE):
            os << " [label=\"CALL_EDGE\", style=dashed];" << std::endl;
            break;
        case (RETURN_EDGE):
            os << " [label=\"RETURN_EDGE\", style=dashed];" << std::endl;
            break;
        case (CFLOW_EDGE):
            os << ";" << std::endl;
            break;
    }

    os.flush();
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
ICFGStandard::ICFGStandard() 
{
  mEntry = mExit = NULL;
  mDGraph = new DGraph::DGraphStandard;
  mCallNodes = new std::list<OA_ptr<Node> >;
  mReturnNodes = new std::list<OA_ptr<Node> >;
  mCallEdges = new std::list<OA_ptr<Edge> >;
  mReturnEdges = new std::list<OA_ptr<Edge> >;

  mEdges = new std::list<OA_ptr<Edge> >;
  mNodes = new std::list<OA_ptr<Node> >;
}


ICFGStandard::~ICFGStandard()
{
  mEntry = NULL;
  mExit = NULL;
}

//--------------------------------------------------------------------
// ICFGStandard Methods
//--------------------------------------------------------------------

OA_ptr<ICFGStandard::Edge> 
ICFGStandard::getICFGEdge(
        const OA_ptr<DGraph::DGraphStandard::Edge> dgEdge) const
{
  OA_ptr<ICFGStandard::Edge> retval;

  DGEdgeToEdgeMap::const_iterator pos;
  pos = mDGEdgeToICFGEdge.find(dgEdge);
  if (pos != mDGEdgeToICFGEdge.end()) {
      retval =  pos->second;
  } else {
      assert(0); // querying about DGG edge that doesn't have a
                 // corresponding ICFG edge, Manager should make
                 // sure this doesn't happen
  }

  return retval;
}

OA_ptr<ICFGStandard::Node> 
ICFGStandard::getICFGNode(
        const OA_ptr<DGraph::DGraphStandard::Node> dgNode) const
{
  OA_ptr<ICFGStandard::Node> retval;

  DGNodeToNodeMap::const_iterator pos;
  pos = mDGNodeToICFGNode.find(dgNode);
  if (pos != mDGNodeToICFGNode.end()) {
      retval =  pos->second;
  } else {
      assert(0); // querying about DG node that doesn't have a
                 // corresponding ICFG node, Manager should make
                 // sure this doesn't happen
  }

  return retval;
}

void ICFGStandard::addEdge(OA_ptr<ICFGStandard::Edge> pEdge)
{
  assert(!pEdge.ptrEqual(0));

  // associate the DGEdge with given ICFG edge
  mDGEdgeToICFGEdge[pEdge->mDGEdge] = pEdge;

  // add associated DGraph edge to underlying DGraph
  mDGraph->addEdge(pEdge->mDGEdge);

  // add to complete list of edges
  mEdges->push_back(pEdge);

  OA_ptr<Node> pNode1, pNode2;
  switch (pEdge->getType()) {
      // a call edge
      case CALL_EDGE:
        // put in set of call edges for pNode1 and pNode2
        pNode1 = pEdge->source();
        pNode1->addCallEdge(pEdge);
        pNode2 = pEdge->source();
        pNode2->addCallEdge(pEdge);

        // store in call edge set
        mCallEdges->push_back(pEdge);
        break;

      // a return edge
      case RETURN_EDGE:
        // put in set of return edges for pNode1 and pNode2
        pNode1 = pEdge->source();
        pNode1->addReturnEdge(pEdge);
        pNode2 = pEdge->source();
        pNode2->addReturnEdge(pEdge);

        // store in return edge set
        mReturnEdges->push_back(pEdge);
        break;

      default:
        break;
  }

}

void ICFGStandard::addNode(OA_ptr<ICFGStandard::Node> pNode)
{
    if ( pNode->mDGNode.ptrEqual(0) ) {
        assert(0);
    } else {
        mDGNodeToICFGNode[pNode->mDGNode] = pNode;
    }
    mDGraph->addNode(pNode->mDGNode);
    
    // add to complete list of edges
    mNodes->push_back(pNode);

}

OA_ptr<ICFGStandard::NodesIterator> ICFGStandard::getNodesIterator() const
{ 
    OA_ptr<NodesIterator> retval; 
    retval = new NodesIterator(mNodes); 
    return retval;
}

OA_ptr<ICFGStandard::NodesIterator>
ICFGStandard::getEntryNodesIterator( ) const
{
    OA_ptr<ReversePostDFSIterator> retval;
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList
        = DGraph::create_entry_list(*mDGraph);
    OA_ptr<std::list<OA_ptr<ICFGStandard::Node> > > retlist 
        = convert_to_ICFG(dgList);
    retval = new NodesIterator(retlist);
    return retval;
}

OA_ptr<ICFGStandard::NodesIterator>
ICFGStandard::getExitNodesIterator( ) const
{
    OA_ptr<ReversePostDFSIterator> retval;
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList
        = DGraph::create_exit_list(*mDGraph);
    OA_ptr<std::list<OA_ptr<ICFGStandard::Node> > > retlist 
        = convert_to_ICFG(dgList);
    retval = new NodesIterator(retlist);
    return retval;
}


OA_ptr<ICFGStandard::EdgesIterator> ICFGStandard::getEdgesIterator() const
{ 
    OA_ptr<EdgesIterator> retval; 
    retval = new EdgesIterator(mEdges); 
    return retval;
}

OA_ptr<ICFGStandard::DFSIterator> ICFGStandard::getDFSIterator() 
{ 
    assert(0);
    OA_ptr<DFSIterator> retval; 
    return retval;
}

OA_ptr<ICFGStandard::BFSIterator> ICFGStandard::getBFSIterator() 
{ 
    assert(0);
    OA_ptr<BFSIterator> retval; 
    return retval;
}


OA_ptr<ICFGStandard::ReversePostDFSIterator>
ICFGStandard::getReversePostDFSIterator( DGraph::DGraphEdgeDirection pOrient)
{
    OA_ptr<ReversePostDFSIterator> retval;
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList
        = DGraph::create_reverse_post_order_list(*mDGraph, pOrient);
    OA_ptr<std::list<OA_ptr<ICFGStandard::Node> > > retlist 
        = convert_to_ICFG(dgList);
    retval = new NodesIterator(retlist);
    return retval;
}

OA_ptr<ICFGStandard::ReversePostDFSIterator>
ICFGStandard::getReversePostDFSIterator(OA_ptr<DGraph::Interface::Node> root, 
                                        DGraph::DGraphEdgeDirection pOrient)
{
    OA_ptr<ReversePostDFSIterator> retval;
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList
        = DGraph::create_reverse_post_order_list(*mDGraph, root, pOrient);
    OA_ptr<std::list<OA_ptr<ICFGStandard::Node> > > retlist 
        = convert_to_ICFG(dgList);
    retval = new NodesIterator(retlist);
    return retval;
}

/*! FIXME: for now just returning a ReversePostDFSIterator that
    starts at exit.  For data-flow analysis this is actually better
    but DGraphDFProblem has to get the PostDFSIterator because
    not all graphs (eg. callgraph) have an exit node.
*/
OA_ptr<ICFGStandard::PostDFSIterator> 
ICFGStandard::getPostDFSIterator(DGraph::DGraphEdgeDirection pOrient)
{
    if (pOrient == DGraph::DEdgeOrg) {
        return getReversePostDFSIterator(DGraph::DEdgeRev);
    } else {
        return getReversePostDFSIterator(DGraph::DEdgeOrg);
    }
}

OA_ptr<ICFGStandard::PostDFSIterator>
ICFGStandard::getPostDFSIterator(OA_ptr<DGraph::Interface::Node> root, 
                                 DGraph::DGraphEdgeDirection pOrient)
{
    assert(0);
    OA_ptr<ICFGStandard::PostDFSIterator> retval;
    return retval;
}


/*! converts more general list to list of subclass type
    needed so the helper functions that generate orderings operate
    on the more general DGraph::Interface
 */
OA_ptr<std::list<OA_ptr<ICFGStandard::Node> > > 
ICFGStandard::convert_to_ICFG(
        OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > pList) const
{
    OA_ptr<std::list<OA_ptr<ICFGStandard::Node> > > retval;
    retval = new std::list<OA_ptr<ICFGStandard::Node> >;

    std::list<OA_ptr<DGraph::Interface::Node> >::iterator nodeIter;
    for (nodeIter=pList->begin(); nodeIter!=pList->end(); nodeIter++ ) {
        OA_ptr<DGraph::Interface::Node> dgNode = *nodeIter;
        //retval->push_back(dgNode.convert<ICFGStandard::Node>());
        OA_ptr<DGraph::DGraphStandard::Node> dgStdNode 
            = dgNode.convert<DGraph::DGraphStandard::Node>();
        DGNodeToNodeMap::const_iterator pos = mDGNodeToICFGNode.find(dgStdNode);
        if (pos != mDGNodeToICFGNode.end()) {
            retval->push_back(pos->second);
        }
    }

    return retval;
}

/*
OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > 
ICFGStandard::create_reverse_post_order_list(DGraph::Interface& pDGraph,
                                     OA_ptr<DGraph::Interface::Node> root,
                                     DGraph::DGraphEdgeDirection pOrient)
{
    // loop over all nodes and set their visit field to false
    OA_ptr<DGraph::Interface::NodesIterator> nodeIter
        = pDGraph.getNodesIterator();
    for ( ; nodeIter->isValid(); (*nodeIter)++ ) {
        OA_ptr<DGraph::Interface::Node> node = nodeIter->current();
        node->clearVisitFlag();
    }

    // generate a list of nodes in the requested ordering
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > retval;
    retval = new std::list<OA_ptr<DGraph::Interface::Node> >;
    
    reverse_postorder_recurse(pDGraph, root, pOrient, retval);

    return retval;

}

void ICFGStandard::reverse_postorder_recurse(DGraph::Interface& pDGraph, 
                 OA_ptr<DGraph::Interface::Node> pNode,
                 DGraph::DGraphEdgeDirection pOrient,
                 OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > pList )
{
    OA_ptr<DGraph::Interface::NodesIterator> neighIter;
    
    // mark as visited so that we don't get in an infinite
    // loop on cycles in the graph
    pNode->setVisitFlag();

    // loop over the successors or predecessors based on orientation
    if (pOrient==DGraph::DEdgeOrg) {
      neighIter =  pNode->getSinkNodesIterator();
    } else {
      neighIter = pNode->getSourceNodesIterator();
    }
    for (; neighIter->isValid(); ++(*neighIter)) {
        OA_ptr<DGraph::Interface::Node> n; 
        n = neighIter->current();

        // if the node hasn't been visited then call recursively 
        if (!n->getVisitFlag()) {
            reverse_postorder_recurse(pDGraph, n, pOrient, pList);
        }
    }

    // add ourselves to the beginning of the list
    pList->push_front(pNode);
}
*/


void
ICFGStandard::dump (ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
  os << "===== ICFGStandard: =====\n"
     << endl;
  
  // print the contents of all the nodes
  OA_ptr<NodesIterator> nodeIter = getNodesIterator();
  for ( ; nodeIter->isValid(); ++(*nodeIter)) {
    OA_ptr<ICFGStandard::Node> node = nodeIter->current();
    node->longdump(*this,os, ir);
    os << endl;
  }
  
  os << "====================" << endl;

}

void
ICFGStandard::dumpdot(ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{

  os << "digraph OA_ICFG {" << endl;
  os << "node [shape=rectangle];" << endl;

  // print the contents of all the nodes 
  // first sort group nodes by procedure
  std::map<ProcHandle,std::set<OA_ptr<Node> > > procNodeSet;
  OA_ptr<NodesIterator> nodesIterPtr = getNodesIterator();
  for ( ; nodesIterPtr->isValid(); ++(*nodesIterPtr) ) {
    OA_ptr<Node> node = nodesIterPtr->current();
    ProcHandle proc = node->getProc();
    procNodeSet[proc].insert(node);
  }

  // sort edges by procedure, call and return edges will be put in 
  // a separate set
  std::set<OA_ptr<Edge> > sepSet;
  std::map<ProcHandle,std::set<OA_ptr<Edge> > > procEdgeSet;
  OA_ptr<EdgesIterator> edgeIterPtr = getEdgesIterator();
  for ( ; edgeIterPtr->isValid(); ++(*edgeIterPtr) ) {
    OA_ptr<Edge> edge = edgeIterPtr->current();
    if (edge->getSinkProc() == edge->getSourceProc()) {
        procEdgeSet[edge->getSinkProc()].insert(edge);
    } else {
        sepSet.insert(edge);
    }
  }

  // then output nodes and edges by procedure
  std::map<ProcHandle,std::set<OA_ptr<Node> > >::iterator mapIter;
  for (mapIter=procNodeSet.begin(); mapIter!=procNodeSet.end(); mapIter++ ) {
      // subgraph
      os << "subgraph cluster_" << mapIter->first.hval() << " {" << endl;
      
      // set of nodes
      std::set<OA_ptr<Node> > nodeSet = mapIter->second;
      std::set<OA_ptr<Node> >::iterator nodeIter;
      for (nodeIter=nodeSet.begin(); nodeIter!=nodeSet.end(); nodeIter++ ) {
          (*nodeIter)->dumpdot(*this,os,ir);
      }
      
      // set of edges
      std::set<OA_ptr<Edge> > edgeSet = procEdgeSet[mapIter->first];
      std::set<OA_ptr<Edge> >::iterator edgeIter;
      for (edgeIter=edgeSet.begin(); edgeIter!=edgeSet.end(); edgeIter++ ) {
          (*edgeIter)->dumpdot(os);
      }
      os << "}" << endl;
  }

  // print rest of edges
  std::set<OA_ptr<Edge> >::iterator edgeIter;
  for (edgeIter=sepSet.begin(); edgeIter!=sepSet.end(); edgeIter++ ) {
      (*edgeIter)->dumpdot(os);
  }
  
  os << "}" << endl;
  os.flush();
}


  } // end namespace ICFGStandard
} // end namespace OA
