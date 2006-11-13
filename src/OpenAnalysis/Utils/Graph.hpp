/*! \file
  
  \brief Declarations for Graph class

  \authors Arun Chauhan (2001 was part of Mint)
  \version $Id: Graph.hpp,v 1.17 2005/03/18 18:14:17 ntallent Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef Graph_H
#define Graph_H

#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/Utils/BaseGraph.hpp>

namespace OA {

//--------------------------------------------------------------------
/*!
   Graph is the base class for a general undirected graph (Graph) that
   is in turn derived from BaseGraph.  Algorithms that operate upon
   abstract undirected graphs should, normally, use only this base
   Graph class for maximum portability.
    
   No extra restrictions are placed on nodes and edges in addition to
   those imposed by BaseGraph.  This means that self-edges, and
   multiple edges between two nodes, are allowed.

   An undirected graph, Graph, extends BaseGraph by adding DFS and BFS
   iterators, as well as iterators to enumerate neighboring nodes and
   incident edges for a node.

   NOTE ON friend CLASSES: Many classes (especially Graph,
   Graph::Node, and Graph::Edge) have many friend classes.  This is
   *not* a kludge.  It is simulating "package" visiblity in Java.  We
   want a limited public interface to Node and Edge and yet give more
   permissions to methods within the Graph class.  
*/
//--------------------------------------------------------------------
class Graph : public BaseGraph {
public:
  class Edge;
  class Node;
  class IncidentEdgesIterator;
  class NeighborNodesIterator;
  //------------------------------------------------------------------
  /*! An node in an undirected graph has a list of neighboring nodes
      and a list of incident edges. */
  class Node : public BaseGraph::Node {
  public:
    Node () 
        { incident_edges = new std::list<OA_ptr<Edge> >; }
    virtual ~Node () { incident_edges->clear(); }
  private:
    OA_ptr<std::list<OA_ptr<Edge> > >  incident_edges;
    friend class Graph;
    friend class Graph::Edge;
    friend class Graph::IncidentEdgesIterator;
    friend class Graph::NeighborNodesIterator;
  };
  //------------------------------------------------------------------
  class Edge : public BaseGraph::Edge {
  public:
    Edge (OA_ptr<Node> n1, OA_ptr<Node> n2) : BaseGraph::Edge(n1, n2) {}
    virtual ~Edge () {}
    OA_ptr<Node> node1 () { 
      OA_ptr<BaseGraph::Node> n = n1; // Stupid Sun CC 5.4
      return n.convert<Node>(); 
    }
    OA_ptr<Node> node2 () { 
      OA_ptr<BaseGraph::Node> n = n2; // Stupid Sun CC 5.4
      return n.convert<Node>(); 
    }
    friend class Graph;
    friend class Graph::NeighborNodesIterator;

    // just doing comparison on ptr val
    bool operator == (BaseGraph::Edge& other) { return this==&other; }
  };
  //------------------------------------------------------------------
  /*! Incident edges iterator iterates over all the edges incident on a node. */
  class IncidentEdgesIterator : public Iterator {
  public:
    IncidentEdgesIterator (Node& n) 
        : mList(n.incident_edges), mIter(mList->begin()) {}
    ~IncidentEdgesIterator () {}
    void operator++ () { ++mIter; }
    bool isValid() const { return (mIter != mList->end()); }
    OA_ptr<Edge> current() { return *mIter; }

  private:
    OA_ptr<std::list<OA_ptr<Edge> > > mList;
    std::list<OA_ptr<Edge> >::iterator mIter;
  };
  //------------------------------------------------------------------
  /*! Node iterator iterates over all the neighboring nodes. */
  class NeighborNodesIterator : public Iterator {
  public:
    NeighborNodesIterator (Node& n)
        : mCenter(&n), mList(n.incident_edges), mIter(mList->begin()) {}
    ~NeighborNodesIterator () {}
    void operator++ () { ++mIter; }
    bool isValid() const { return (mIter != mList->end()); }
    OA_ptr<Node> current() 
        { OA_ptr<Edge> e = *mIter; 
          return  (e->node1().ptrEqual(mCenter)) ? e->node2() : e->node1(); }

  private:
    // storing the below as a raw ptr because going to assume that it
    // stays alive for life of iterator and need it in current method,
    // do not make it an OA_ptr because when this iterator goes out of
    // scope the OA_ptr will try to delete the Node
    Node* mCenter;

    OA_ptr<std::list<OA_ptr<Edge> > > mList;
    std::list<OA_ptr<Edge> >::iterator mIter;
  };
  //------------------------------------------------------------------
  /*! The DFSIterator here is just an extension of
      BaseGraph::DFSIterator to allow proper casting. */
  class DFSIterator : public BaseGraph::DFSIterator {
  public:
    DFSIterator (Graph& g) : BaseGraph::DFSIterator(g) {}
    virtual ~DFSIterator () {}
    OA_ptr<Node> current() { 
      OA_ptr<BaseGraph::Node> n = mNode; // Stupid Sun CC 5.4
      return n.convert<Node>(); 
    }

   };
  //------------------------------------------------------------------
  /*! The BFSIterator here is just an extension of
      BaseGraph::DFSIterator to allow proper casting. */
  class BFSIterator : public BaseGraph::BFSIterator {
  public:
    BFSIterator (Graph& g) : BaseGraph::BFSIterator(g) {}
    virtual ~BFSIterator () {}
    OA_ptr<Node> current() { 
      OA_ptr<BaseGraph::Node> n = mNode; // Stupid Sun CC 5.4
      return n.convert<Node>(); 
    }
   };
  //------------------------------------------------------------------
  /*! The NodesIterator is just and extension of BaseGraph::NodesIterator 
      to provide access to Graph nodes. */
  class NodesIterator : public BaseGraph::NodesIterator {
  public:
    NodesIterator (Graph& g) : BaseGraph::NodesIterator(g) {}
    virtual ~NodesIterator () {}
    OA_ptr<Node> current() { OA_ptr<BaseGraph::Node> bnode = (*mIter);
                             return bnode.convert<Node>(); }

  };
  //------------------------------------------------------------------
  /*! The BiDirNodesIterator is just an extension of
      BaseGraph::BiDirNodesIterator to provide access to Graph nodes.
  */
  class BiDirNodesIterator : public BaseGraph::BiDirNodesIterator {
  public:
    BiDirNodesIterator (Graph& g) : BaseGraph::BiDirNodesIterator(g) {}
    BiDirNodesIterator (Graph& g, dirType d) 
        : BaseGraph::BiDirNodesIterator(g, d) {}
    virtual ~BiDirNodesIterator () {}
    OA_ptr<Node> current() { OA_ptr<BaseGraph::Node> bnode = (*mIter);
                             return bnode.convert<Node>(); }

  };
  //------------------------------------------------------------------
  /*! The EdgesIterator is just and extension of BaseGraph::EdgesIterator 
      to provide access to Graph edges. */
  class EdgesIterator : public BaseGraph::EdgesIterator {
  public:
    EdgesIterator (Graph& g) : BaseGraph::EdgesIterator(g) {}
    virtual ~EdgesIterator () {}
    OA_ptr<Edge> current() { OA_ptr<BaseGraph::Edge> bedge = (*mIter);
                             return bedge.convert<Edge>(); }

  };

  //------------------------------------------------------------------

  Graph() : BaseGraph() {}
  Graph(OA_ptr<Node> root) : BaseGraph(root) {}
  virtual ~Graph() {}

  virtual void addEdge(OA_ptr<Graph::Edge> e);
  virtual void addNode(OA_ptr<Graph::Node> n);
  virtual void removeEdge(OA_ptr<Graph::Edge> e);
  virtual void removeNode(OA_ptr<Graph::Node> n);

private:
  OA_ptr<BaseGraph::Node> create_DFS_links (OA_ptr<BaseGraph::Node> start_node);
  OA_ptr<BaseGraph::Node> create_BFS_links (OA_ptr<BaseGraph::Node> start_node);
};
//--------------------------------------------------------------------

} // end of OA namespace

#endif
