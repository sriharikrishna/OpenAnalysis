/*! \file
  
  \brief Declaration for BaseGraph class

  \authors Arun Chauhan (2001 was part of Mint)
  \version $Id: BaseGraph.hpp,v 1.19 2005/03/14 23:49:26 ntallent Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef BaseGraph_H
#define BaseGraph_H

// standard headers
#include <iostream>
using std::ostream;

// STL headers
#include <list>
#include <set>

// Mint headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/Utils/Iterator.hpp>
#include <OpenAnalysis/Utils/Exception.hpp>

namespace OA {

//--------------------------------------------------------------------
/*! 
   BaseGraph is the abstract base class (the "interface") for a
   general graph.  It defines graph properties common to directed and
   undirected graphs.  It leaves out some loose ends in the interface:
     -# A node has no notion of incident edges or neighboring nodes
     because the number of kinds of incident edges or neighboring
     nodes is dependent upon the graph being directed or undirected.
     -# For the same reason, the method delete(node) cannot delete any
     incident edges.  Therefore the directed or undirected graph
     *must* override it with a more complete version.  Similarly, the
     method delete(edge) cannot delete the edge from the list(s) of
     the nodes involved.  
     -# In an analogous manner, the method add(edge) must be
     overridden with a more complete version that adds the edge into
     the records of the nodes involved, if needed.

   The only restriction on nodes and edges is that they should be
   unique objects, meaning, an edge or a node cannot be shared between
   two graphs.  A node or edge can also not be inserted twice.  Nodes
   and edges are identified by their pointer values (BaseGraph::Node*
   and BaseGraph::Edge*).

   Following exceptions are thrown by the class (all are subclasses of
   Exception):
     -# BaseGraph::EmptyEdge                       -- attempt to add, or remove, an empty edge (null pointer)
     -# BaseGraph::DuplicateEdge                   -- attempt to add an edge more than once
     -# BaseGraph::NonexistentEdge                 -- attempt to remove an edge that does not belong to the graph
     -# BaseGraph::EdgeInUse                       -- attempt to add an edge that is already a part of another graph
     -# BaseGraph::EmptyNode                       -- attempt to add, or remove, an empty node (null pointer)
     -# BaseGraph::DuplicateNode                   -- attempt to add a node more than once
     -# BaseGraph::NonexistentNode                 -- attempt to remove a node that does not belong to the graph
     -# BaseGraph::NodeInUse                       -- attempt to add a node that is already a part of another graph
     -# BaseGraph::DeletingRootOfNonSingletonGraph -- attempt to delete the root node when graph has more nodes & edges
    
   NOTE ON friend CLASSES: Many classes (especially BaseGraph,
   BaseGraph::Node and BaseGraph::Edge) have many friend classes.
   This is *not* a kludge.  It is simulating "package" visiblity in
   Java.  We want a limited public interface to Node and Edge and yet
   give more permissions to methods within the BaseGraph class.  
*/
//--------------------------------------------------------------------
class BaseGraph {
public:
  class Node;
  class Edge;
  class DFSIterator;
  class BFSIterator;
  class NodesIterator;
  class BiDirNodesIterator;
  class EdgesIterator;
  friend class DFSIterator;
  friend class BFSIterator;
  friend class NodesIterator;
  friend class BiDirNodesIterator;
  friend class EdgesIterator;
  //------------------------------------------------------------------
  /*! EmptyEdge exception is thrown if an edge being added is null (0) */
  class EmptyEdge : public Exception {
  public:
    EmptyEdge () {}
    ~EmptyEdge () {}
    void report (std::ostream& o) const { o << "E!  Adding a null edge to a graph." << std::endl; }
  };
  //------------------------------------------------------------------
  /*! DuplicateEdge exception is thrown if an edge being added is 
      already a part of the graph. */
  class DuplicateEdge : public Exception {
  public:
    DuplicateEdge (OA_ptr<BaseGraph::Edge> e) { offending_edge = e; }
    ~DuplicateEdge () {}
    void report (std::ostream& o) const;
  private:
    OA_ptr<BaseGraph::Edge> offending_edge;
  };
  //------------------------------------------------------------------
  /*! NonexistentEdge exception is thrown if an edge being deleted is 
      not a part of the graph. */
  class NonexistentEdge : public Exception {
  public:
    NonexistentEdge (OA_ptr<BaseGraph::Edge> e) { offending_edge = e; }
    ~NonexistentEdge () {}
    void report (std::ostream& o) const { o << "E!  Removing a non-existent edge from a graph." << std::endl; }
  private:
    OA_ptr<BaseGraph::Edge> offending_edge;
  };
  //------------------------------------------------------------------
  /*! EdgeInUse exception is thrown if an edge being added is already a 
      part of another graph. */
  class EdgeInUse : public Exception {
  public:
    EdgeInUse (OA_ptr<BaseGraph::Edge> e) { offending_edge = e; }
    ~EdgeInUse () {}
    void report (std::ostream& o) const { o << "E!  Adding an edge that is already a part of another graph." << std::endl; }
  private:
    OA_ptr<BaseGraph::Edge> offending_edge;
  };
  //------------------------------------------------------------------
  /*! EmptyNode exception is thrown if a node being added is null (0) */
  class EmptyNode : public Exception {
  public:
    EmptyNode () {}
    ~EmptyNode () {}
    void report (std::ostream& o) const { o << "E!  Adding a null node to a graph." << std::endl; }
  };
  //------------------------------------------------------------------
  /*! DuplicateNode exception is thrown if a node being added is already 
      a part of the graph. */
  class DuplicateNode : public Exception {
  public:
    DuplicateNode (OA_ptr<BaseGraph::Node> n) { offending_node = n; }
    ~DuplicateNode () {}
    void report (std::ostream& o) const { o << "E!  Adding a duplicate node to a graph." << std::endl; }
  private:
    OA_ptr<BaseGraph::Node> offending_node;
  };
  //------------------------------------------------------------------
  /*! NonexistentNode exception is thrown if a node being deleted is 
      not a part of the graph. */
  class NonexistentNode : public Exception {
  public:
    NonexistentNode (OA_ptr<BaseGraph::Node> n) { offending_node = n; }
    ~NonexistentNode () {}
    void report (std::ostream& o) const { o << "E!  Removing a non-existent node from a graph." << std::endl; }
  private:
    OA_ptr<BaseGraph::Node> offending_node;
  };
  //------------------------------------------------------------------
  /*! NodeInUse exception is thrown if a node being added is already a 
      part of another graph. */
  class NodeInUse : public Exception {
  public:
    NodeInUse (OA_ptr<BaseGraph::Node> n) { offending_node = n; }
    ~NodeInUse () {}
    void report (std::ostream& o) const { o << "E!  Addiing a node that is already a part of another graph." << std::endl; }
  private:
    OA_ptr<BaseGraph::Node> offending_node;
  };
  //------------------------------------------------------------------
  class DeletingRootOfNonSingletonGraph : public Exception {
  public:
    DeletingRootOfNonSingletonGraph (OA_ptr<BaseGraph::Node> n) 
        { offending_node = n; }
    ~DeletingRootOfNonSingletonGraph () {}
    void report (std::ostream& o) const { o << "E!  Deleting the root node of a non-singleton graph." << std::endl; }
  private:
    OA_ptr<BaseGraph::Node> offending_node;
  };
  //------------------------------------------------------------------
  class BaseNode {
  public:
    BaseNode() { }
    virtual ~BaseNode() { }
    
    virtual unsigned int getId () const = 0;
  };

  class Node : public BaseNode {
  public:
    Node () : in_use(false) {  dfs_succ = NULL; bfs_succ = NULL; }
    virtual ~Node () { }

    // getId: an id that is defined by derived classes; 0 is NULL.
    // This allows algorithms to be written on base classes.
    virtual unsigned int getId() const { return 0; }

    //! dump the node (named thusly to allow subclasses to have a dump
    // function with different arguments)
    virtual void dumpbase(std::ostream& os) { /*os << this;*/ }

    virtual bool operator==( Node& other) { return this == &other; }
    virtual bool operator<( Node& other) { return this < &other; }

  protected:
    bool in_use;
    OA_ptr<Node> dfs_succ;
    OA_ptr<Node> bfs_succ;
    
  private:
    friend class BaseGraph;
    friend class BaseGraph::DFSIterator;
    friend class BaseGraph::BFSIterator;
  };

  // lt_Node: function object that compares by id.  Useful for sorting.
  class lt_Node {
  public:
    // return true if n1 < n2; false otherwise
    bool operator()(const OA_ptr<Node> n1, const OA_ptr<Node> n2) const {
      return (n1->getId() < n2->getId());
    }
  };

  //------------------------------------------------------------------
  class BaseEdge {
  public:
    BaseEdge() { }
    virtual ~BaseEdge() { }
    
    virtual unsigned int getId () const = 0;
  };
  
  class Edge : public BaseEdge {
  public:
    Edge (OA_ptr<Node> _n1, OA_ptr<Node> _n2) 
        : in_use(false), n1(_n1), n2(_n2) { }
    virtual ~Edge () { }
    
    // getId: an id that is defined by derived classes; 0 is NULL.
    // This allows algorithms to be written on base classes.
    virtual unsigned int getId() const { return 0; }

    //! dump the edge (named thusly to allow subclasses to have a dump
    // function with different arguments)
    virtual void dumpbase(std::ostream& os) { os << this; }
    virtual bool operator==( Edge& other) 
        { return this == &other; }
    virtual bool operator<( Edge& other) 
        { return this < &other; }

  protected:
    bool in_use;
    OA_ptr<Node> n1;
    OA_ptr<Node> n2;
    
  private:
    friend class BaseGraph;
    friend class BaseGraph::DFSIterator;
    friend class BaseGraph::BFSIterator;
    friend class BaseGraph::DuplicateEdge;
  };
  
  // lt_Edge: function object that compares by id.  Useful for sorting.
  class lt_Edge {
  public:
    // return true if e1 < e2; false otherwise
    bool operator()(const OA_ptr<Edge> e1, const OA_ptr<Edge> e2) const {
      return (e1->getId() < e2->getId());
    }
  };

  //------------------------------------------------------------------
  /*! The DFSiterator calls the virtual function create_DFS_links the 
      first time it is called, or if the graph has
      been changed since the last call.  
  */
  class DFSIterator : public Iterator {
  public:
    DFSIterator (BaseGraph& g);
    virtual ~DFSIterator () {}
    virtual void operator++ () { if (!mNode.ptrEqual(0)) {
                                   mNode = mNode->dfs_succ; }
                               }
    virtual bool isValid() const { return (!mNode.ptrEqual(0)); }
    virtual void reset() { mNode = mRootNode; }
  protected:
    OA_ptr<Node> mRootNode;
    OA_ptr<Node> mNode;
  };
  //------------------------------------------------------------------
  /*! The BFSiterator calls the virtual function create_BFS_links the
      first time it is called, or if the graph has
      been changed since the last call. 
  */
  class BFSIterator : public Iterator {
  public:
    BFSIterator (BaseGraph& g);
    virtual ~BFSIterator () {}
    virtual void operator++ () { if (!mNode.ptrEqual(0)) {
                                   mNode = mNode->bfs_succ; }
                               }
    virtual bool isValid() const { return (!mNode.ptrEqual(0)); }
    virtual void reset() { mNode = mRootNode; }
  protected:
    OA_ptr<Node> mRootNode;
    OA_ptr<Node> mNode;
  };
  //------------------------------------------------------------------
  /*! The node iterator iterates over all the nodes in the graph in no 
      particular order. 
   */
  class NodesIterator : public Iterator {
  public:
    NodesIterator (const BaseGraph& g) 
        : mNodeSet(g.node_set), mIter(mNodeSet->begin()) {}
    NodesIterator () { } 
    virtual ~NodesIterator () {}
    virtual void operator++ () { ++mIter; }
    virtual bool isValid() const { return (mIter != mNodeSet->end()); }
    virtual void reset() { mIter = mNodeSet->begin(); }

  protected:
    OA_ptr<std::set<OA_ptr<Node> > > mNodeSet;
    std::set<OA_ptr<Node> >::iterator mIter;
  };
  //------------------------------------------------------------------
  /*! The bi-directional node iterator iterates over all the nodes in 
      the graph in no particular order -- except
      that the Forward direction is guaranteed to be opposite of the 
      Reverse direction.  
  */
  class BiDirNodesIterator : public Iterator {
  public:
    enum dirType { Forward, Reverse }; 
    // Default to forward direction if client doesn't specify the direction.
    BiDirNodesIterator (const BaseGraph& g) : mNodeSet(g.node_set), mDir(Forward)
        { mIter = mNodeSet->begin(); } 
    BiDirNodesIterator (const BaseGraph& g, dirType d) 
        : mNodeSet(g.node_set), mDir(d) { reset(); }
    virtual ~BiDirNodesIterator () {}
    virtual void operator++ () { ++mIter; }
    virtual void operator-- () { --mIter; }
    virtual bool isValid() const {
      if (mDir == Forward) {
        return mIter != mNodeSet->end();
      } else {
        return mIter != mPre_begin;
      }
    }
    virtual void reset() { 
      if (mDir == Forward) {
        mIter = mNodeSet->begin();
      } else {
        mIter = mNodeSet->end();
        mIter--;
        mPre_begin = mNodeSet->begin();
        --mPre_begin;
      }
    }
  protected:
    OA_ptr<std::set<OA_ptr<Node> > > mNodeSet;
    std::set<OA_ptr<Node> >::iterator mIter;
    std::set<OA_ptr<Node> >::iterator mPre_begin;
    dirType mDir;
  };
  //------------------------------------------------------------------
  /*! The edge iterator iterates over all the edges in the graph in 
      no particular order. */
  class EdgesIterator : public Iterator {
  public:
    EdgesIterator (const BaseGraph& g) 
        : mEdgeSet(g.edge_set), mIter(mEdgeSet->begin()) { }
    EdgesIterator () {}
    virtual ~EdgesIterator () {}
    virtual void operator++ () { ++mIter; }
    virtual bool isValid() const { return (mIter != mEdgeSet->end()); }
    virtual void reset() { mIter = mEdgeSet->begin(); }
  protected:
    OA_ptr<std::set<OA_ptr<Edge> > > mEdgeSet;
    std::set<OA_ptr<Edge> >::iterator mIter;
  };
  //------------------------------------------------------------------
  BaseGraph() 
      { node_set = new std::set<OA_ptr<Node> >;
        edge_set = new std::set<OA_ptr<Edge> >;
        root_node = 0; }
  BaseGraph(OA_ptr<Node> root) 
      { node_set = new std::set<OA_ptr<Node> >;
        edge_set = new std::set<OA_ptr<Edge> >; 
        root_node = 0;
        addNode(root); }
  virtual ~BaseGraph (); 
  
  OA_ptr<Node> getRoot() { return root_node; }
  void  setRoot(OA_ptr<Node> n) { root_node = n; }

  int getNumNodes() { return node_set->size(); }
  int getNumEdges() { return edge_set->size(); }

  bool isEmpty() { return (root_node.ptrEqual(0)); }

  //! dump the graph (named thusly to allow subclasses to have a dump
  // function with different arguments)
  void dumpbase(std::ostream&);

protected:
  OA_ptr<std::set<OA_ptr<Node> > > node_set; // the set of all the graph nodes
  OA_ptr<std::set<OA_ptr<Edge> > > edge_set; // the set of all the graph edges
  OA_ptr<Node> root_node;          // the root node
  bool DFS_needed, BFS_needed;     // has a DFS / BFS been done on this graph?
  void addEdge (OA_ptr<Edge> e) throw (DuplicateEdge, EdgeInUse, EmptyEdge,
			    DuplicateNode, NodeInUse, EmptyNode);
  void addNode (OA_ptr<Node> n) throw (DuplicateNode, NodeInUse, EmptyNode);
  void removeEdge (OA_ptr<Edge> e) throw (NonexistentEdge, EmptyEdge);
  void removeNode (OA_ptr<Node> n) throw (NonexistentNode, 
          DeletingRootOfNonSingletonGraph, EmptyNode);
  virtual OA_ptr<Node> create_DFS_links (OA_ptr<Node> start_node) = 0;
  virtual OA_ptr<Node> create_BFS_links (OA_ptr<Node> start_node) = 0;
};
//--------------------------------------------------------------------

} // end of OA namespace
#endif
