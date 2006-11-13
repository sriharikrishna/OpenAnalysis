/*! \file
  
  \brief Implementation of DUGStandard.

  \authors Michelle Strout
  \version $Id: DUGStandard.cpp,v 1.3 2005/10/17 21:21:41 garvin Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

//--------------------------------------------------------------------
//--------------------------------------------------------------------

// #define DEBUG_DUAA_LAST 1
// standard headers

#include <string.h>
#include <stdlib.h>
#include <iostream>
using std::ostream;
using std::endl;
using std::cout;
using std::cerr;

#include "DUGStandard.hpp"

namespace OA {
  namespace DUG {

//--------------------------------------------------------------------
unsigned int DUGStandard::Node::sNextId = 1;
unsigned int DUGStandard::Edge::sNextId = 1;

//--------------------------------------------------------------------
// must be updated any time DUG::Interface::EdgeType changes
static const char *sEdgeTypeToString[] = { 
  "F",
  "C",
  "R",
  "P"
//   "CFLOW",
//   "CALL",
//   "RETURN",
//   "PARAM"
};
// must be updated any time DUG::Interface::EdgeType changes
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
void DUGStandard::Node::Ctor() { 
    mId = sNextId++; 

    mDGNode = new DGraph::DGraphStandard::Node;

    mVaried = false;
    mUseful = false;
}



//--------------------------------------------------------------------
void
DUGStandard::Node::dump (ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
  os << sNodeTypeToString[mType] << std::endl;
}

bool DUGStandard::Node::operator==(DGraph::Interface::Node& other) 
{ 
    Node& recastOther = dynamic_cast<Node&>(other);
    return mDGNode == recastOther.mDGNode; 
}

bool DUGStandard::Node::operator<(DGraph::Interface::Node& other) 
{ 
    Node& recastOther = dynamic_cast<Node&>(other);
    return mDGNode < recastOther.mDGNode; 
}

void
DUGStandard::Node::longdump (DUGStandard& icfg, 
                              ostream& os, 
                              OA_ptr<IRHandlesIRInterface> ir)
{
  // print the node ID
  os << "DUGStandard Node: ";
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
DUGStandard::Node::dumpdot (DUGStandard &currcfg, ostream& os, 
                             OA_ptr<IRHandlesIRInterface> ir)
{
  // print the node
  os << getId() << " [ label=\"==  DUG ";
  os << sNodeTypeToString[getType()] << " " << getId() << " ==";
  
  if (isAnEntry()) {
    os << " (entry)";
  } 
  else if (isAnExit()) {
    os << " (exit)"; 
  }
  os << "\\n"; 


  os << "\" ];" << endl;
  os.flush();
}


//--------------------------------------------------------------------

OA_ptr<DUGStandard::IncomingEdgesIterator> 
    DUGStandard::Node::getIncomingEdgesIterator() const
{ 
    OA_ptr<IncomingEdgesIterator> retval; 

    // get incoming edges for associated dgraph node
    OA_ptr<DGraph::DGraphStandard::IncomingEdgesIterator> dgEdgeIter
        = mDGNode->getIncomingEdgesIterator();
    // put them all in a set
    OA_ptr<std::list<OA_ptr<Edge> > > tempSet;
    tempSet = new std::list<OA_ptr<Edge> >;
    for ( ; dgEdgeIter->isValid(); (*dgEdgeIter)++ ) {
        tempSet->push_back(mDUG->getDUGEdge(dgEdgeIter->current()));
    }

    // create an iterator over the set of edges
    retval = new EdgesIterator(tempSet); 
    return retval;
}

OA_ptr<DUGStandard::OutgoingEdgesIterator> 
    DUGStandard::Node::getOutgoingEdgesIterator() const
{ 
    OA_ptr<OutgoingEdgesIterator> retval; 

    // get outgoing edges for associated dgraph node
    OA_ptr<DGraph::DGraphStandard::OutgoingEdgesIterator> dgEdgeIter
        = mDGNode->getOutgoingEdgesIterator();
    // put them all in a set
    OA_ptr<std::list<OA_ptr<Edge> > > tempSet;
    tempSet = new std::list<OA_ptr<Edge> >;
    for ( ; dgEdgeIter->isValid(); (*dgEdgeIter)++ ) {
        tempSet->push_back(mDUG->getDUGEdge(dgEdgeIter->current()));
    }

    // create an iterator over the set of edges
    retval = new EdgesIterator(tempSet); 
    return retval;
}

OA_ptr<DUGStandard::SourceNodesIterator> 
    DUGStandard::Node::getSourceNodesIterator() const
{ 
    OA_ptr<SourceNodesIterator> retval; 

    // get incoming edges for associated cfg node
    OA_ptr<DGraph::DGraphStandard::SourceNodesIterator> dgNodeIter
        = mDGNode->getSourceNodesIterator();
    // put them all in a set
    OA_ptr<std::list<OA_ptr<Node> > > tempSet; 
    tempSet = new std::list<OA_ptr<Node> >;
    for ( ; dgNodeIter->isValid(); (*dgNodeIter)++ ) {
        tempSet->push_back(mDUG->getDUGNode(dgNodeIter->current()));
    }
    // create an iterator over the set of nodes
    retval = new NodesIterator(tempSet); 
    return retval;
}

OA_ptr<DUGStandard::SinkNodesIterator> 
    DUGStandard::Node::getSinkNodesIterator() const
{ 
    OA_ptr<SinkNodesIterator> retval; 

    // get incoming edges for associated cfg node
    OA_ptr<DGraph::DGraphStandard::SinkNodesIterator> dgNodeIter
        = mDGNode->getSinkNodesIterator();
    // put them all in a set
    OA_ptr<std::list<OA_ptr<Node> > > tempSet; 
    tempSet = new std::list<OA_ptr<Node> >;
    for ( ; dgNodeIter->isValid(); (*dgNodeIter)++ ) {
        tempSet->push_back(mDUG->getDUGNode(dgNodeIter->current()));
    }
    // create an iterator over the set of nodes
    retval = new NodesIterator(tempSet); 
    return retval;
}



//--------------------------------------------------------------------
//--------------------------------------------------------------------
DUGStandard::Edge::Edge (OA_ptr<DUGStandard> pDUG,
                          OA_ptr<Node> pNode1, 
                          OA_ptr<Node> pNode2, 
                          EdgeType pType, ExprHandle call,
                         ProcHandle proc)
  : mDUG(pDUG), mNode1(pNode1), mNode2(pNode2), mType(pType), 
    mCall(call), mProc(proc)
{
  mId = sNextId++;

  // create DGraphEdge for associated DGraph
  mDGEdge = new DGraph::DGraphStandard::Edge(pNode1->mDGNode,
                                             pNode2->mDGNode);

}

// DUGStandard::Edge::Edge (OA_ptr<DUGStandard> pDUG,
//                           OA_ptr<Node> pNode1, 
//                           OA_ptr<Node> pNode2, 
//                           EdgeType pType)
//   : mDUG(pDUG), mNode1(pNode1), mNode2(pNode2), mType(pType), 
//     mCall(ExprHandle(0)), mProc(ProcHandle(0))
// {
//   mId = sNextId++;

//   // create DGraphEdge for associated DGraph
//   mDGEdge = new DGraph::DGraphStandard::Edge(pNode1->mDGNode,
//                                              pNode2->mDGNode);

// }


//--------------------------------------------------------------------
bool DUGStandard::Edge::operator==(DGraph::Interface::Edge& other) 
{ 
    Edge& recastOther = dynamic_cast<Edge&>(other);
    return mDGEdge == recastOther.mDGEdge; 
}

bool DUGStandard::Edge::operator<(DGraph::Interface::Edge& other) 
{ 
    Edge& recastOther = dynamic_cast<Edge&>(other);
    return mDGEdge < recastOther.mDGEdge; 
}



void DUGStandard::Edge::dump(ostream& os)
{
  os << sEdgeTypeToString[mType];
}

void DUGStandard::Edge::dumpdot(ostream& os)
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
DUGStandard::DUGStandard(OA_ptr<Activity::ICFGDep> icfgDep,
                         OA_ptr<IRHandlesIRInterface> pIR) 
    : mICFGDep(icfgDep), mIR(pIR)
{
  mEntry       = mExit = NULL;
  mDGraph      = new DGraph::DGraphStandard;
  mCallNodes   = new std::list<OA_ptr<Node> >;
  mReturnNodes = new std::list<OA_ptr<Node> >;

  mEdges = new std::list<OA_ptr<Edge> >;
  mNodes = new std::list<OA_ptr<Node> >;

  mActiveLocSet    = new LocSet;
  mActiveSymSet    = new std::set<SymHandle>;
  mActiveStmtSet   = new std::set<StmtHandle>;
  mActiveMemRefSet = new std::set<MemRefHandle>;

  mUnknownLocActive = false;
}


DUGStandard::~DUGStandard()
{
  mEntry = NULL;
  mExit = NULL;
}

//--------------------------------------------------------------------
// DUGStandard Methods
//--------------------------------------------------------------------

OA_ptr<DUGStandard::Edge> 
DUGStandard::getDUGEdge(
        const OA_ptr<DGraph::DGraphStandard::Edge> dgEdge) const
{
  OA_ptr<DUGStandard::Edge> retval;

  DGEdgeToEdgeMap::const_iterator pos;
  pos = mDGEdgeToDUGEdge.find(dgEdge);
  if (pos != mDGEdgeToDUGEdge.end()) {
      retval =  pos->second;
  } else {
      assert(0); // querying about DGG edge that doesn't have a
                 // corresponding DUG edge, Manager should make
                 // sure this doesn't happen
  }

  return retval;
}

OA_ptr<DUGStandard::Node> 
DUGStandard::getDUGNode(
        const OA_ptr<DGraph::DGraphStandard::Node> dgNode) const
{
  OA_ptr<DUGStandard::Node> retval;

  DGNodeToNodeMap::const_iterator pos;
  pos = mDGNodeToDUGNode.find(dgNode);
  if (pos != mDGNodeToDUGNode.end()) {
      retval =  pos->second;
  } else {
      assert(0); // querying about DG node that doesn't have a
                 // corresponding DUG node, Manager should make
                 // sure this doesn't happen
  }

  return retval;
}

void DUGStandard::addEdge(OA_ptr<DUGStandard::Edge> pEdge)
{
  assert(!pEdge.ptrEqual(0));

  // associate the DGEdge with given DUG edge
  mDGEdgeToDUGEdge[pEdge->mDGEdge] = pEdge;

  // add associated DGraph edge to underlying DGraph
  mDGraph->addEdge(pEdge->mDGEdge);

  // add to complete list of edges
  mEdges->push_back(pEdge);
}

void DUGStandard::addNode(OA_ptr<DUGStandard::Node> pNode)
{
    if ( pNode->mDGNode.ptrEqual(0) ) {
        assert(0);
    } else {
        mDGNodeToDUGNode[pNode->mDGNode] = pNode;
    }
    mDGraph->addNode(pNode->mDGNode);
    
    // add to complete list of edges
    mNodes->push_back(pNode);

}

OA_ptr<DUGStandard::NodesIterator> DUGStandard::getNodesIterator() const
{ 
    OA_ptr<NodesIterator> retval; 
    retval = new NodesIterator(mNodes); 
    return retval;
}

OA_ptr<DUGStandard::NodesIterator>
DUGStandard::getEntryNodesIterator( ) const
{
    OA_ptr<ReversePostDFSIterator> retval;
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList
        = DGraph::create_entry_list(*mDGraph);
    OA_ptr<std::list<OA_ptr<DUGStandard::Node> > > retlist 
        = convert_to_DUG(dgList);
    retval = new NodesIterator(retlist);
    return retval;
}

OA_ptr<DUGStandard::NodesIterator>
DUGStandard::getExitNodesIterator( ) const
{
    OA_ptr<ReversePostDFSIterator> retval;
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList
        = DGraph::create_exit_list(*mDGraph);
    OA_ptr<std::list<OA_ptr<DUGStandard::Node> > > retlist 
        = convert_to_DUG(dgList);
    retval = new NodesIterator(retlist);
    return retval;
}


OA_ptr<DUGStandard::EdgesIterator> DUGStandard::getEdgesIterator() const
{ 
    OA_ptr<EdgesIterator> retval; 
    retval = new EdgesIterator(mEdges); 
    return retval;
}

OA_ptr<DUGStandard::DFSIterator> DUGStandard::getDFSIterator() 
{ 
    assert(0);
    OA_ptr<DFSIterator> retval; 
    return retval;
}

OA_ptr<DUGStandard::BFSIterator> DUGStandard::getBFSIterator() 
{ 
    assert(0);
    OA_ptr<BFSIterator> retval; 
    return retval;
}


OA_ptr<DUGStandard::ReversePostDFSIterator>
DUGStandard::getReversePostDFSIterator( DGraph::DGraphEdgeDirection pOrient)
{
    OA_ptr<ReversePostDFSIterator> retval;
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList
        = DGraph::create_reverse_post_order_list(*mDGraph, pOrient);
    OA_ptr<std::list<OA_ptr<DUGStandard::Node> > > retlist 
        = convert_to_DUG(dgList);
    retval = new NodesIterator(retlist);
    return retval;
}

OA_ptr<DUGStandard::ReversePostDFSIterator>
DUGStandard::getReversePostDFSIterator(OA_ptr<DGraph::Interface::Node> root, 
                                        DGraph::DGraphEdgeDirection pOrient)
{
    OA_ptr<ReversePostDFSIterator> retval;
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList
        = DGraph::create_reverse_post_order_list(*mDGraph, root, pOrient);
    OA_ptr<std::list<OA_ptr<DUGStandard::Node> > > retlist 
        = convert_to_DUG(dgList);
    retval = new NodesIterator(retlist);
    return retval;
}

/*! FIXME: for now just returning a ReversePostDFSIterator that
    starts at exit.  For data-flow analysis this is actually better
    but DGraphDFProblem has to get the PostDFSIterator because
    not all graphs (eg. callgraph) have an exit node.
*/
OA_ptr<DUGStandard::PostDFSIterator> 
DUGStandard::getPostDFSIterator(DGraph::DGraphEdgeDirection pOrient)
{
    if (pOrient == DGraph::DEdgeOrg) {
        return getReversePostDFSIterator(DGraph::DEdgeRev);
    } else {
        return getReversePostDFSIterator(DGraph::DEdgeOrg);
    }
}

OA_ptr<DUGStandard::PostDFSIterator>
DUGStandard::getPostDFSIterator(OA_ptr<DGraph::Interface::Node> root, 
                                 DGraph::DGraphEdgeDirection pOrient)
{
    assert(0);
    OA_ptr<DUGStandard::PostDFSIterator> retval;
    return retval;
}


/*! converts more general list to list of subclass type
    needed so the helper functions that generate orderings operate
    on the more general DGraph::Interface
 */
OA_ptr<std::list<OA_ptr<DUGStandard::Node> > > 
DUGStandard::convert_to_DUG(
        OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > pList) const
{
    OA_ptr<std::list<OA_ptr<DUGStandard::Node> > > retval;
    retval = new std::list<OA_ptr<DUGStandard::Node> >;

    std::list<OA_ptr<DGraph::Interface::Node> >::iterator nodeIter;
    for (nodeIter=pList->begin(); nodeIter!=pList->end(); nodeIter++ ) {
        OA_ptr<DGraph::Interface::Node> dgNode = *nodeIter;
        //retval->push_back(dgNode.convert<DUGStandard::Node>());
        OA_ptr<DGraph::DGraphStandard::Node> dgStdNode 
            = dgNode.convert<DGraph::DGraphStandard::Node>();
        DGNodeToNodeMap::const_iterator pos = mDGNodeToDUGNode.find(dgStdNode);
        if (pos != mDGNodeToDUGNode.end()) {
            retval->push_back(pos->second);
        }
    }

    return retval;
}

/*
OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > 
DUGStandard::create_reverse_post_order_list(DGraph::Interface& pDGraph,
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

void DUGStandard::reverse_postorder_recurse(DGraph::Interface& pDGraph, 
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
DUGStandard::dump (ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
  os << "===== DUGStandard: =====\n"
     << endl;
  
  // print the contents of all the nodes
  OA_ptr<NodesIterator> nodeIter = getNodesIterator();
  for ( ; nodeIter->isValid(); ++(*nodeIter)) {
    OA_ptr<DUGStandard::Node> node = nodeIter->current();
    node->longdump(*this,os, ir);
    os << endl;
  }
  
  os << "====================" << endl;

}

void
DUGStandard::dumpdot(ostream& os, OA_ptr<DUGIRInterface> ir)
{

  os << "digraph OA_DUG {" << endl;
  os << "node [shape=rectangle];" << endl;

  // then output nodes and edges by procedure
  std::list<OA_ptr<Edge> >::iterator iter;
  for (iter=mEdges->begin(); iter!=mEdges->end(); iter++ ) {
    OA_ptr<Edge> edge = *iter;
    OA_ptr<Node> srcNode  = edge->source();
    OA_ptr<Node> sinkNode = edge->sink();

    OA_ptr<Location> srcLoc  = srcNode->getLoc();
    OA_ptr<Location> sinkLoc = sinkNode->getLoc();
    srcLoc->dump(os, mIR); 
    os << "_" << srcNode->getId();
    os << "->";
    sinkLoc->dump(os, mIR);
    os << "_" << sinkNode->getId();

    EdgeType etype = edge->getType();
    os << "[label=\"" << sEdgeTypeToString[etype] << "_";
    ExprHandle call = edge->getCall();
    if (call != ExprHandle(0)){
      SymHandle calleesym = ir->getSymHandle(call);
      os << mIR->toString(calleesym);
      os << "\\l" << (unsigned)edge->getCall().hval() << "\"];" << endl;
    }
    else{
      ProcHandle proc = sinkNode->getProc();
      os << mIR->toString(proc);
      os << "\"];" << endl;
    }
  }

  os << "}" << endl;
  os.flush();
}

OA_ptr<DUGStandard::Node> 
DUGStandard::getNode(SymHandle sym, OA_ptr<Location> loc, ProcHandle proc){
  assert(sym);
  OA_ptr<Node> retval = mSymToNode[sym];

  if (retval.ptrEqual(0)){
    OA_ptr<DUGStandard> temp; temp = this;
    retval = new DUGStandard::Node(temp, proc, loc, sym);
    mSymToNode[sym] = retval;
    addNode(retval);
#ifdef DEBUG_DUAA
    std::cout << "added" << std::endl;
#endif
  }
#ifdef DEBUG_DUAA
  else {
    std::cout << "found" << std::endl;
  }
#endif

  return retval;
}

// true if a node exists for 'loc'
bool
DUGStandard::isNode(SymHandle sym){
  assert(sym);
  OA_ptr<Node> retval = mSymToNode[sym];
  
  return !retval.ptrEqual(0);
}

void 
DUGStandard::insertActiveSymSet(OA_ptr<Location> loc)
{ 
  // Unknown
  if (loc->isaUnknown()) {
    mUnknownLocActive = true;

    // Named
  } else if (loc->isaNamed()) {
    OA_ptr<NamedLoc> nloc = loc.convert<NamedLoc>();
#ifdef DEBUG_DUAA
    std::cout << "DUGStandard::insertActiveSymSet:(loc)"; 
    loc->dump(std::cout, mIR);
    std::cout << std::endl;
#endif  
    insertActiveSymSet( nloc->getSymHandle() );

    OA_ptr<SymHandleIterator> symIter = nloc->getFullOverlapIter();
    for ( ; symIter->isValid(); (*symIter)++) {
#ifdef DEBUG_DUAA
      std::cout << "\tFullOverlap: " << mIR->toString(symIter->current()); 
      std::cout << std::endl;
#endif  
      insertActiveSymSet( symIter->current());
    }
    symIter = nloc->getPartOverlapIter();
    for ( ; symIter->isValid(); (*symIter)++) {
#ifdef DEBUG_DUAA
      std::cout << "\tPartialOverlap: "; 
      std::cout << std::endl;
#endif  
      insertActiveSymSet(symIter->current());
    }
 
    // Unnamed
  } else if (loc->isaUnnamed()) {
    assert(0);
    // not handling this yet

    // Invisible
  } else if (loc->isaInvisible()) {
    OA_ptr<InvisibleLoc> invLoc 
      = loc.convert<InvisibleLoc>();
    OA_ptr<MemRefExpr> mre = invLoc->getMemRefExpr();

    // get symbol from memory reference expression if no derefs
    // FIXME: here need another visitor for MemRefExpr, for now assuming
    // only named ones
    if (mre->isaNamed()) {
      OA_ptr<NamedRef> namedRef 
        = namedRef.convert<NamedRef>();
      insertActiveSymSet( namedRef->getSymHandle() );
    } else {
      assert(0);
    }

    // LocSubSet
  } else if (loc->isaSubSet()) {
    OA_ptr<Location> baseLoc = loc->getBaseLoc();
    // FIXME: now really want to call visitor on this guy but instead will just
    // call this function recursively
    insertActiveSymSet(baseLoc);

  } else {
    assert(0);
  }
}


void 
DUGStandard::Node::setUseful()
{ 
  mUseful = true;
  if (!mVaried) return;
  mDUG->insertActiveLocSet(mLoc);
  mDUG->insertActiveSymSet(mSym);

  OA_ptr<std::set<MemRefHandle> > mrefSet;
  mrefSet = mDUG->mICFGDep->getMemRefSet(mLoc);
  if (!mrefSet.ptrEqual(0)){
    std::set<MemRefHandle>::iterator i = mrefSet->begin();
    for(; i != mrefSet->end(); i++)
      mDUG->insertActiveMemRefSet(*i);
  }


  OA_ptr<std::set<StmtHandle> > stmtSet;
  stmtSet = mDUG->mICFGDep->getStmtSet(mLoc);
  if (!stmtSet.ptrEqual(0)){
    std::set<StmtHandle>::iterator i = stmtSet->begin();
    for(; i != stmtSet->end(); i++)
      mDUG->insertActiveStmtSet(*i);
  }
}


void 
DUGStandard::Node::markVaried(std::list<ExprHandle>& callStack,
                              OA_ptr<Activity::ActivityIRInterface> ir,
                              std::set<OA_ptr<DUG::DUGStandard::Edge> >& visited,
                              std::set<unsigned>& onPath,
                              ProcHandle proc)
{ 
#ifdef DEBUG_DUAA_LAST
  std::cout << "-v->"; 
  OA_ptr<Location> loc = getLoc();
  loc->dump(std::cout, mDUG->mIR);
  std::cout << "(" << getId() << ")" << std::endl;
#endif  
  setVaried();

  bool valueThroughGlobals = false;
  if (callStack.front() == ExprHandle(1)) 
    valueThroughGlobals = true;

  // set up iterator for successor edges
  OA_ptr<DUG::DUGStandard::EdgesIterator> succIterPtr;
  succIterPtr = getOutgoingEdgesIterator();
  // iterate over the successors
  for (; succIterPtr->isValid(); ++(*succIterPtr)) {
    OA_ptr<DUG::DUGStandard::Edge> succEdge = succIterPtr->current();
    OA_ptr<DUG::DUGStandard::Node> succNode = succEdge->sink();
    unsigned succId = succNode->getId();
    if (visited.find(succEdge) != visited.end() ||
        onPath.find(succId)    != onPath.end()  ) continue;
    onPath.insert(succId);
    EdgeType etype = succEdge->getType();

#ifdef DEBUG_DUAA_LAST
    if (succEdge->getType() != RETURN_EDGE || 
        callStack.front() == succEdge->getCall() || valueThroughGlobals){
      OA_ptr<Location> loc = getLoc();
      loc->dump(std::cout, mDUG->mIR);
      std::cout << "(" << getId() << ":" << sEdgeTypeToString[etype];
      ExprHandle call = succEdge->getCall();
      if (call != ExprHandle(0)){
        SymHandle calleesym = ir->getSymHandle(call);
        std::cout << ":" << mDUG->mIR->toString(calleesym);
        std::cout << ":" << (unsigned)succEdge->getCall().hval();
      }
      else {
        std::cout << ":" << mDUG->mIR->toString(succEdge->getProc());
      }
      std::cout << ")";
    }
#endif  
    switch(etype) {
      case (DUG::CALL_EDGE):
        callStack.push_front(succEdge->getCall());
        visited.insert(succEdge);
        succNode->markVaried(callStack, ir, visited, onPath, succEdge->getSinkProc());
        callStack.pop_front();
        break;
      case (DUG::RETURN_EDGE):
        if (callStack.front() == succEdge->getCall() || valueThroughGlobals){
          if (!valueThroughGlobals) callStack.pop_front();
          visited.insert(succEdge);
          succNode->markVaried(callStack, ir, visited, onPath, succEdge->getProc());
          if (!valueThroughGlobals) callStack.push_front(succEdge->getCall());
        }
#ifdef DEBUG_DUAA_LAST
        else{
          std::cout << "markVaried: " << (callStack.front() == succEdge->getCall())
                    << (unsigned)callStack.front().hval() << "<->"
                    << (unsigned)succEdge->getCall().hval() << std::endl;
        }
#endif  
        break;
      default: // for both CFLOW_EDGE and PARAM_EDGE
        if (succEdge->getType() != PARAM_EDGE) 
          visited.insert(succEdge);
        // to prevent value passing through global variables between procedures
        ProcHandle succProc = succEdge->getProc();
        if (proc != succProc) {
          callStack.push_front(ExprHandle(1));
          succNode->markVaried(callStack, ir, visited, onPath, succProc);
          callStack.pop_front();
        }
        else{
          succNode->markVaried(callStack, ir, visited, onPath, proc);
        }
#if 0
        else
        {
          std::cout << "-v->"; 
          OA_ptr<Location> loc = succNode->getLoc();
          loc->dump(std::cout, mDUG->mIR);
          std::cout << "(" << succNode->getId() << "): ";
          std::cout << mDUG->mIR->toString(proc) << "<->" 
                    << mDUG->mIR->toString(succEdge->getProc()) <<std::endl;
        }
#endif  
          
        break;
    }

    onPath.erase(succId);
  }
#ifdef DEBUG_DUAA_LAST
  std::cout << "<-" << std::endl;
#endif  
}

void
DUGStandard::Node::markUseful(std::list<ExprHandle>& callStack,
                              OA_ptr<Activity::ActivityIRInterface> ir,
                              std::set<OA_ptr<DUG::DUGStandard::Edge> >& visited,
                              std::set<unsigned>& onPath,
                              ProcHandle proc)
{ 
  if (!isVaried()) return;
#ifdef DEBUG_DUAA_LAST
  std::cout << "-u->"; 
  OA_ptr<Location> loc = getLoc();
  loc->dump(std::cout, mDUG->mIR);
  std::cout << "(" << getId() << ")" << std::endl;
#endif  
  setUseful();

  bool valueThroughGlobals = false;
  if (callStack.front() == ExprHandle(1)) {
    valueThroughGlobals = true;
#ifdef DEBUG_DUAA_LAST
    std::cout << "val thru globals" << std::endl;
#endif  
  }

  // set up iterator for predecessor edges
  OA_ptr<DUG::DUGStandard::EdgesIterator> predIterPtr;
  predIterPtr = getIncomingEdgesIterator();
  // iterate over the predecessors
  for (; predIterPtr->isValid(); ++(*predIterPtr)) {
    OA_ptr<DUG::DUGStandard::Edge> predEdge = predIterPtr->current();
    OA_ptr<DUG::DUGStandard::Node> predNode = predEdge->source();
    unsigned predId = predNode->getId();
    if (visited.find(predEdge) != visited.end() ||
        onPath.find(predId)    != onPath.end()  ) continue;
    onPath.insert(predId);
    EdgeType etype = predEdge->getType();

#ifdef DEBUG_DUAA_LAST
    if (predEdge->getType() != CALL_EDGE || 
        callStack.front() == predEdge->getCall() || valueThroughGlobals){
      OA_ptr<Location> loc = getLoc();
      loc->dump(std::cout, mDUG->mIR);
      std::cout << "(" << getId() << ":" << sEdgeTypeToString[etype];
      ExprHandle call = predEdge->getCall();
      if (call != ExprHandle(0)){
        SymHandle calleesym = ir->getSymHandle(call);
        std::cout << mDUG->mIR->toString(calleesym);
      }
      std::cout << ":" << (unsigned)predEdge->getCall().hval() << ")";
    }
#endif  
    switch(etype) {
      case (DUG::CALL_EDGE):
        if (callStack.front() == predEdge->getCall() || valueThroughGlobals ){
          if (!valueThroughGlobals) callStack.pop_front();
          visited.insert(predEdge);
          predNode->markUseful(callStack, ir, visited, onPath, predEdge->getProc());
          if (!valueThroughGlobals) callStack.push_front(predEdge->getCall());
        }
        break;
      case (DUG::RETURN_EDGE):
        callStack.push_front(predEdge->getCall());
        visited.insert(predEdge);
        predNode->markUseful(callStack, ir, visited, onPath, predEdge->getSourceProc());
        callStack.pop_front();
        break;
      default: // for both CFLOW_EDGE and PARAM_EDGE
        if (predEdge->getType() != PARAM_EDGE) visited.insert(predEdge);
        ProcHandle predProc = predEdge->getProc();
        if (proc != predProc) {
          callStack.push_front(ExprHandle(1));
#ifdef DEBUG_DUAA_LAST
          std::cout << "valthruglobals: begin" << std::endl;
#endif  
          predNode->markUseful(callStack, ir, visited, onPath, predProc);
          callStack.pop_front();
#ifdef DEBUG_DUAA_LAST
          std::cout << "valthruglobals: end" << std::endl;
#endif  
        }
        else
        {
          predNode->markUseful(callStack, ir, visited, onPath, proc);
        }
        break;
    }

    onPath.erase(predId);
  }
#ifdef DEBUG_DUAA_LAST
  std::cout << "<-" << std::endl;
#endif  
}

//! Indicate whether the given sym is active or not
bool DUGStandard::isActive(SymHandle sym)
{
  // an unknown location is active, therefore all symbols are active
  if (mUnknownLocActive) {
    return true;
  } else if (mActiveSymSet->find(sym) != mActiveSymSet->end()) {
    return true;
  } else {
    return false;
  }  
}

//! Indicate whether the given stmt is active or not
bool DUGStandard::isActive(StmtHandle stmt)
{
  if (mActiveStmtSet->find(stmt) != mActiveStmtSet->end()) {
    return true;
  } else {
    return false;
  }  
}

//! Indicate whether the given memref is active or not
bool DUGStandard::isActive(MemRefHandle memref)
{
  if (mActiveMemRefSet->find(memref) != mActiveMemRefSet->end()) {
    return true;
  } else {
    return false;
  }  
}


  } // end namespace DUGStandard
} // end namespace OA
