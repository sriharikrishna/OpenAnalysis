/*! \file
  
  \brief Definition of DGraph class, directed graph

  \authors Arun Chauhan (2001 was part of Mint)
  \version $Id: DGraphStandard.cpp,v 1.20 2005/06/10 02:32:05 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  DGraph is the base class (the "interface") for a general directed graph
  (DGraph).  Algorithms that operate upon abstract directed graphs should,
  normally, use only this base DGraph class for maximum portability.

*/

#include "DGraphStandard.hpp"


namespace OA {
  namespace DGraph {

#if defined(DEBUG_ALL) || defined(DEBUG_DGraphStandard)
static bool debug = true;
#else
static bool debug = false;
#endif

//--------------------------------------------------------------------
/*! This routine must be called before a DFS iterator can be used.  It traverses the graph in DFS order and adds, in
    each node, a pointer to the next node in DFS ordering. */
OA_ptr<BaseGraph::Node>
DGraphStandard::create_DFS_links (OA_ptr<BaseGraph::Node> n)
{
  SinkNodesIterator neighIter(*(n.convert<Node>()));
  OA_ptr<DGraphStandard::Node> last = n.convert<DGraphStandard::Node>();
  for (; neighIter.isValid(); ++neighIter) {
    OA_ptr<DGraphStandard::Node> neigh = neighIter.current();
    if (neigh->dfs_succ.ptrEqual(0)) {    
      last->dfs_succ = neigh.convert<OA::BaseGraph::Node>(); 
      OA_ptr<BaseGraph::Node> bnode = create_DFS_links(neigh);
      last = bnode.convert<DGraphStandard::Node>();
    }
  }
  last->dfs_succ = 0; 
  return last;

}
//--------------------------------------------------------------------

/*! converts more general list to list of subclass type
    needed so the helper functions that generate orderings operate
    on the more general DGraph::Interface
 */
OA_ptr<std::list<OA_ptr<DGraphStandard::Node> > > 
DGraphStandard::convert_to_DGraphStandard(
        OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > pList) const
{
    OA_ptr<std::list<OA_ptr<DGraphStandard::Node> > > retval;
    retval = new std::list<OA_ptr<DGraphStandard::Node> >;

    std::list<OA_ptr<DGraph::Interface::Node> >::iterator nodeIter;
    for (nodeIter=pList->begin(); nodeIter!=pList->end(); nodeIter++ ) {
        OA_ptr<DGraph::Interface::Node> dgNode = *nodeIter;
        retval->push_back(dgNode.convert<DGraphStandard::Node>());
    }

    return retval;
}

//--------------------------------------------------------------------
/*
DGraphStandard::ExitNodesIterator::ExitNodesIterator (const DGraphStandard& g) 
{
  OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList 
      = create_exit_list(g);
  mList = g.convert_to_DGraphStandard(dgList);
  mIter = mList->begin();
}
*/

//--------------------------------------------------------------------
/*!
   Creates a reverse postorder list of the graph nodes.  
 */
DGraphStandard::ReversePostDFSIterator::ReversePostDFSIterator(
    DGraphStandard& g, DGraphEdgeDirection pOrient)
{
    // use helper routines that create such an ordering on a DGraph::Interface
    // use the source or sink as the root depending on the orientation
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList 
        = create_reverse_post_order_list(g, pOrient);
    mNodeList = g.convert_to_DGraphStandard(dgList);
    mIter = mNodeList->begin();

    if (debug) {
      std::cout << "ReversePostDFSIterator constructor: mNodeList = { ";
      std::list<OA_ptr<Node> >::iterator iter;
      for (iter=mNodeList->begin(); iter!=mNodeList->end(); iter++) {
        OA_ptr<Node> node = *iter;
        std::cout << " ( node Id = " << node->getId() << "), ";
      }
      std::cout << " }" << std::endl;
    }
}
/*!
   Creates a reverse postorder list of the graph nodes.  Nodes that
   precede / succeed  the given startNode will not be in the ordering.

   \param n      where ordering should start
 */
DGraphStandard::ReversePostDFSIterator::ReversePostDFSIterator(
        DGraphStandard& g, OA_ptr<Interface::Node> startNode, 
        DGraphEdgeDirection pOrient)
{
    // use helper routines that create such an ordering on a DGraph::Interface
    // use the source or sink as the root depending on the orientation
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList 
        = create_reverse_post_order_list(g, startNode, pOrient);
    mNodeList = g.convert_to_DGraphStandard(dgList);
    mIter = mNodeList->begin();
}

/*
void DGraphStandard::ReversePostDFSIterator::createNodeList(
        DGraphStandard& g, OA_ptr<Interface::Node> root, 
        DGraphEdgeDirection pOrient)
{
    assert(!root.ptrEqual(NULL));

    // loop over all nodes and set their visit field to false
    DGraphStandard::NodesIterator nIter(g);
    for (; nIter.isValid(); ++nIter) {
      OA_ptr<DGraphStandard::Node> n = nIter.current();
      n->visited = false;
    }

    // generate a list of the nodes in the requested ordering
    reverse_postorder_recurse( g, root.convert<Node>(), pOrient );

    //reset();
    mIter = mNodeList.begin();
}
*/

/*!
   Recursive helper method for creating the reverse postorder list.

   \param n        root of subgraph
   \param pOrient  use original (DEdgeOrg) or reverse orientation 
                   (DEdgeRev) of directed edges

   Assumption: Before calling this method at the top level, all Nodes should 
   have their visit field set to false. 
 */
/*
void
DGraphStandard::ReversePostDFSIterator::reverse_postorder_recurse(
    DGraphStandard& g, OA_ptr<Node> pNode, DGraphEdgeDirection pOrient)
{
    OA_ptr<DGraphStandard::NodesIterator> neighIterPtr;

    // mark as visited so that we don't get in an infinite
    // loop on cycles in the graph
    pNode->visited = true;

    // loop over the successors or predecessors based on orientation
    if (pOrient==DEdgeOrg) {
      OA_ptr<DGraphStandard::SinkNodesIterator> it = 
        pNode->getSinkNodesIterator();
      neighIterPtr = it.convert<DGraphStandard::NodesIterator>();
    } else {
      OA_ptr<DGraphStandard::SourceNodesIterator> it = 
        pNode->getSourceNodesIterator();
      neighIterPtr = it.convert<DGraphStandard::NodesIterator>();
    }
    for (; neighIterPtr->isValid(); ++(*neighIterPtr)) {
        OA_ptr<DGraphStandard::Node> n; 
        n = neighIterPtr->current();

        // if the node hasn't been visited then call recursively 
        if (!n->visited) {
            reverse_postorder_recurse(g, n, pOrient);
        }
    }

    // add ourselves to the beginning of the list
    mNodeList.push_front(pNode);
}
*/
//--------------------------------------------------------------------


/*!
   Creates a postorder list of the graph nodes.  
 */
DGraphStandard::PostDFSIterator::PostDFSIterator(
    DGraphStandard& g, DGraphEdgeDirection pOrient)
{
    // FIXME: need to put appropriate helper routines in
    // DGraphInterfaceIterators.*pp, see commented out createNodeList
    // below for how it should work
    assert(0);
}
/*!
   Creates a postorder list of the graph nodes.  

   \param n      root of the graph
 */
DGraphStandard::PostDFSIterator::PostDFSIterator(
        DGraphStandard& g, OA_ptr<Interface::Node> root, 
        DGraphEdgeDirection pOrient)
{
    assert(0);
}

/*
void DGraphStandard::PostDFSIterator::createNodeList(
        DGraphStandard& g, OA_ptr<Interface::Node> root, 
        DGraphEdgeDirection pOrient)
{
    assert(!root.ptrEqual(NULL));

    // loop over all nodes and set their visit field to false
    DGraphStandard::NodesIterator nIter(g);
    for (; nIter.isValid(); ++nIter) {
      OA_ptr<DGraphStandard::Node> n = nIter.current();
      n->visited = false;
    }

    // generate a list of the nodes in the requested ordering
    postorder_recurse( g, root.convert<Node>(), pOrient );

    //reset();
    mIter = mNodeList.begin();
}
*/

/*!
   Recursive helper method for creating the postorder list.

   \param n        root of subgraph
   \param pOrient  use original (DEdgeOrg) or reverse orientation 
                   (DEdgeRev) of directed edges

   Assumption: Before calling this method at the top level, all Nodes should 
   have their visit field set to false. 
 */
/*
void
DGraphStandard::PostDFSIterator::postorder_recurse(
    DGraphStandard& g, OA_ptr<Node> pNode, DGraphEdgeDirection pOrient)
{
    OA_ptr<DGraphStandard::NodesIterator> neighIterPtr;

    // mark as visited so that we don't get in an infinite
    // loop on cycles in the graph
    pNode->visited = true;

    // loop over the successors or predecessors based on orientation
    if (pOrient==DEdgeOrg) {
      OA_ptr<DGraphStandard::SinkNodesIterator> it = 
        pNode->getSinkNodesIterator();
      neighIterPtr = it.convert<DGraphStandard::NodesIterator>();
    } else {
      OA_ptr<DGraphStandard::SourceNodesIterator> it = 
        pNode->getSourceNodesIterator();
      neighIterPtr = it.convert<DGraphStandard::NodesIterator>();
    }
    for (; neighIterPtr->isValid(); ++(*neighIterPtr)) {
        OA_ptr<DGraphStandard::Node> n; 
        n = neighIterPtr->current();

        // if the node hasn't been visited then call recursively 
        if (!n->visited) {
            // mark as visited so that we don't get in an infinite
            // loop on cycles in the graph
            postorder_recurse(g, n, pOrient);
        }
    }

    // add ourselves to the beginning of the list
    mNodeList.push_back(pNode);
}
*/


//--------------------------------------------------------------------
/*! This routine must be called before a BFS iterator can be used.  
    It traverses the graph in BFS order and adds, in
    each node, a pointer to the next node in BFS ordering. 

    NOTE: doesn't appear to be implemented
 */
OA_ptr<BaseGraph::Node>
DGraphStandard::create_BFS_links (OA_ptr<BaseGraph::Node> n)
{
  assert(0);
  return n;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! Add the given node to the set of nodes. */
void
DGraphStandard::addNode (OA_ptr<DGraph::Interface::Node> n)
{
  BaseGraph::addNode(n.convert<DGraphStandard::Node>());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! Add the edge to the set of edges, as well as to the list of incoming 
    edges of its sink node and to the list of
    outgoing edges of its source node.  Further, add the two nodes to 
    each other's source and sink lists, as
    appropriate. 
 */
void
DGraphStandard::addEdge (OA_ptr<DGraph::Interface::Edge> pEdge)
{
  OA_ptr<DGraphStandard::Edge> e = pEdge.convert<DGraphStandard::Edge>();
  OA_ptr<DGraphStandard::Node> sink = e->sink();
  OA_ptr<DGraphStandard::Node> source = e->source();
  BaseGraph::addEdge(e);
  source->outgoing_edges->push_back(e);
  sink->incoming_edges->push_back(e);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! Remove this edge from the BaseGraph as well as from the list of 
    incoming or outgoing edges of the two nodes that
    form this edge. */
void
DGraphStandard::removeEdge (OA_ptr<DGraph::Interface::Edge> pEdge)
{
  OA_ptr<DGraphStandard::Edge> e = pEdge.convert<DGraphStandard::Edge>();
  BaseGraph::removeEdge(e);
  OA_ptr<DGraphStandard::Node> sink = e->sink();
  OA_ptr<DGraphStandard::Node> source = e->source();

  // remove this edge from the incoming edges of the sink
  sink->incoming_edges->remove(e);

  // remove this edge from the outgoing edges of the source
  source->outgoing_edges->remove(e);
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/*! Remove the given node and all the incoming and outgoing edges incident on it. */
void
DGraphStandard::removeNode (OA_ptr<DGraph::Interface::Node> pNode)
{
  OA_ptr<DGraphStandard::Node> n = pNode.convert<DGraphStandard::Node>();
  BaseGraph::removeNode(n);

  IncomingEdgesIterator incoming(*n);
  while (incoming.isValid()) {
    OA_ptr<DGraphStandard::Edge> e = incoming.current();
    OA_ptr<DGraphStandard::Node> source = e->source();
    // remove this edge from the list of outgoing edges of the source node
    source->outgoing_edges->remove(e);
    // remove this edge
    BaseGraph::removeEdge(e);
    ++incoming;
  }

  OutgoingEdgesIterator outgoing(*n);
  while (outgoing.isValid()) {
    OA_ptr<DGraphStandard::Edge> e = outgoing.current();
    OA_ptr<DGraphStandard::Node> sink = e->sink();
    // remove this edge from the list of outgoing edges of the source node
    sink->incoming_edges->remove(e);
    // remove this edge
    BaseGraph::removeEdge(e);
    ++outgoing;
  }
}
//--------------------------------------------------------------------------------------------------------------------

void
DGraphStandard::dumpbase(std::ostream& os)
{
  os << "DGraphStandard" << std::endl;
}


//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

void
DGraphStandard::Node::dumpbase(std::ostream& os)
{
  os << getId();
}


void
DGraphStandard::Edge::dumpbase(std::ostream& os)
{
  os << getId();
}



  } // end of DGraph namespace
} // end of OA namespace
