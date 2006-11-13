/*! \file
  
  \brief Definition for BaseGraph class

  \authors Arun Chauhan (2001 was part of Mint)
  \version $Id: BaseGraph.cpp,v 1.10 2005/03/14 23:49:26 ntallent Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  BaseGraph is the abstract base class (the "interface") for a general graph.
  It defines graph properties common to directed and undirected graphs.

*/


//--------------------------------------------------------------------
// standard headers
#include <iostream>
using std::ostream;
using std::endl;

// Mint headers
#include "BaseGraph.hpp"

namespace OA {

//--------------------------------------------------------------------

BaseGraph::~BaseGraph ()
{
  node_set->clear();
  edge_set->clear();
  root_node = NULL;
}

//--------------------------------------------------------------------
// Exceptions
void
BaseGraph::DuplicateEdge::report (ostream& o) const
{
  o << "E!  Attempting to add an already existing edge ";
  offending_edge->dumpbase(o);
  o << " (";
  offending_edge->n1->dumpbase(o);
  o << ", ";
  offending_edge->n2->dumpbase(o);
  o << ") to the graph." << endl;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
/*! ReversePostDFSIterator creates an iterator to enumerate nodes in 
    the reverse DFS postorder 
    starting at the root.  If this is the first
    time such an iterator has been demanded for this graph, or the graph 
    has changed since the last ReversePostDFS traversal, a ReversePostDFS
    is actually carried out to fill in the ReversePostDFS pointers.  
    Otherwise, the nodes are simply traversed using the existing
    ReversePostDFS pointers. 
*/
/*
BaseGraph::ReversePostDFSIterator::ReversePostDFSIterator (BaseGraph& g)
{
  if (g.root_node != 0) {
    if (g.ReversePostDFS_needed) {
      // reset all the ReversePostDFS succ links and call the recursive 
      // routine to re-create them
      std::set<Node*>::iterator ni = g.node_set.begin();
      while (ni != g.node_set.end()) {
	Node* n = *ni;
	n->rev_post_dfs_succ = 0;
	++ni;
      }
      g.create_ReversePostDFS_links(g.root_node);
      g.ReversePostDFS_needed = false;
    }
  }
  p = g.root_node;
}
*/
//--------------------------------------------------------------------
/*! DFSIterator creates an iterator to enumerate nodes in the DFS order 
    starting at the root.  If this is the first
    time a DFS iterator has been demanded for this graph, or the graph 
    has changed since the last DFS traversal, a DFS
    is actually carried out to fill in the DFS pointers.  Otherwise, 
    the nodes are simply traversed using the existing
    DFS pointers. 
*/
BaseGraph::DFSIterator::DFSIterator(BaseGraph& g) 
    : mRootNode(g.root_node), mNode(g.root_node)
{
  if (!g.root_node.ptrEqual(0)) {
    if (g.DFS_needed) {
      // reset all the DFS succ links and call the recursive routine to re-create them
      std::set<OA_ptr<Node> >::iterator ni = g.node_set->begin();
      while (ni != g.node_set->end()) {
        OA_ptr<Node> n = *ni;
        n->dfs_succ = 0;
        ++ni;
      }
      g.create_DFS_links(g.root_node);
      g.DFS_needed = false;
    }
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! DBFSIterator creates a node_iterator to enumerate nodes in the BFS
    order starting at the root.  If this is the first time a BFS
    iterator has been demanded for this graph, or the graph has
    changed since the last BFS traversal, a BFS is actually carried
    out to fill in the BFS pointers.  Otherwise, the nodes are simply
    traversed using the existing BFS pointers. 
*/
BaseGraph::BFSIterator::BFSIterator (BaseGraph& g) :
  mRootNode(g.root_node), mNode(g.root_node)
{
  if (!g.root_node.ptrEqual(0)) {
    if (!g.BFS_needed) {
      // reset all the BFS succ links and call the recursive routine to re-create them
      std::set<OA_ptr<Node> >::iterator ni = g.node_set->begin();
      while (ni != g.node_set->end()) {
        OA_ptr<Node> n = *ni;
        n->bfs_succ = 0;
        ++ni;
      }
      g.create_BFS_links(g.root_node);
      g.BFS_needed = false;
    }
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! Add an edge, checking for duplication and making sure that the
 *  edge does not belong to another graph. */
void
BaseGraph::addEdge (OA_ptr<BaseGraph::Edge> e)
  throw (BaseGraph::DuplicateEdge, BaseGraph::EdgeInUse, BaseGraph::EmptyEdge,
	 /* indirect: from BaseGraph::add(BaseGraph::Node) */
	 BaseGraph::DuplicateNode, BaseGraph::NodeInUse, BaseGraph::EmptyNode)
	 
{
  if (e.ptrEqual(0))
    throw EmptyEdge();
  if (edge_set->find(e) != edge_set->end())
    throw DuplicateEdge(e);
  if (e->in_use)
    throw EdgeInUse(e);

  // insert the nodes if they don't already exist in the graph
  if (node_set->find(e->n1) == node_set->end()) {
    addNode(e->n1);
  }
  if (node_set->find(e->n2) == node_set->end()) {
    addNode(e->n2);
  }
  e->in_use = true;
  edge_set->insert(e);
  DFS_needed = BFS_needed = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! Add a node checking for duplication and making sure the node does 
    not belong to another graph. */
void
BaseGraph::addNode (OA_ptr<BaseGraph::Node> n)
  throw (BaseGraph::DuplicateNode, BaseGraph::NodeInUse, BaseGraph::EmptyNode)
{
  if (n.ptrEqual(0))
    throw EmptyNode();
  if (node_set->find(n) != node_set->end())
    throw DuplicateNode(n);
  if (n->in_use)
    throw NodeInUse(n);
  if (root_node.ptrEqual(0))
    root_node = n;
  n->in_use = true;
  node_set->insert(n);
  DFS_needed = BFS_needed = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! Remove an edge, checking for non-existence. */
void
BaseGraph::removeEdge (OA_ptr<BaseGraph::Edge> e)
  throw (BaseGraph::NonexistentEdge, BaseGraph::EmptyEdge)
{
  if (e.ptrEqual(0))
    throw EmptyEdge();
  if (edge_set->erase(e) == 0)
    throw NonexistentEdge(e); 
  e->in_use = false;
  DFS_needed = BFS_needed = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! Remove a node, checking for non-existence.  Throw 
    DeletingRootOfNonEmptyGraph if attempted to remove the root node
    while the graph still has other nodes and edges. */
void
BaseGraph::removeNode (OA_ptr<BaseGraph::Node> n)
  throw (BaseGraph::NonexistentNode, BaseGraph::DeletingRootOfNonSingletonGraph, BaseGraph::EmptyNode)
{
  if (n.ptrEqual(0))
    throw EmptyNode();
  if ((n.ptrEqual(root_node)) && (!edge_set->empty() || (node_set->size() > 1)))
    throw DeletingRootOfNonSingletonGraph(n);
  if (node_set->erase(n) == 0)
    throw NonexistentNode(n);
  n->in_use = false;
  DFS_needed = BFS_needed = true;
  if (n.ptrEqual(root_node))
    root_node = 0;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void
BaseGraph::dumpbase(ostream& os)
{
  os << "List of Nodes:" << endl;
  std::set<OA_ptr<Node> >::iterator nodes_iter = node_set->begin();
  while (nodes_iter != node_set->end()) {
    OA_ptr<Node> n = *nodes_iter;
    os << "\t";
    n->dumpbase(os);
    if ((*nodes_iter).ptrEqual(root_node))
      os << " (root)";
    os << endl;
    ++nodes_iter;
  }

  os << "List of Edges:" << endl;
  std::set<OA_ptr<Edge> >::iterator edges_iter = edge_set->begin();
  while (edges_iter != edge_set->end()) {
    OA_ptr<Edge> e = *edges_iter;
    os << "\t";
    e->dumpbase(os);
    os << " (";
    e->n1->dumpbase(os);
    os << " -- "; 
    e->n2->dumpbase(os);
    os << ")" << endl;
    ++edges_iter;
  }
}
//--------------------------------------------------------------------

} // end of OA namespace
