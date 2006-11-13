/*! \file
  
  \brief Abstract directed graph class.

  \authors Michelle Strout abstracted the DGraph class written by
           Arun Chauhan (2001 was part of Mint)
  \version $Id: Interface.hpp,v 1.17 2005/06/10 02:32:05 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef DGraphInterface_H
#define DGraphInterface_H

#include <OpenAnalysis/Utils/OA_ptr.hpp>

namespace OA {
  namespace DGraph {

/*!
   When performing traversals, the DGraphEdgeDirection indicates
   whether the traversal should use the original direction of
   edges in the directed graph (DEdgeOrg) or 
   the reverse direction (DEdgeRev).
 */
typedef enum {DEdgeOrg = 0, DEdgeRev = 1} DGraphEdgeDirection;

//--------------------------------------------------------------------
/*!
   DGraph::Interface is the abstract base class for a general directed
   graph (DGraph).  Algorithms that operate upon abstract directed
   graphs should, normally require only this base DGraph class for
   maximum portability.

   Self-edges, and
   multiple edges between two nodes, are allowed.

   A directed graph is expected to have the following subclasses:
   Node, Edge, NodesIterator, EdgesIterator, DFSIterator, BFSIterator,
   ReversePostDFSIterator, IncomingEdgesIterator, OutgoingEdgesIterator,
   SourceNodesIterator, and SinkNodesIterator.
   The interfaces for these classes are specfied here as well.

*/
//--------------------------------------------------------------------
class Interface {
public:
  class NodesIterator;
  class EdgesIterator;
  class DFSIterator;
  class BFSIterator;
  class ReversePostDFSIterator;
  class PostDFSIterator;
  class IncomingEdgesIterator;
  class OutgoingEdgesIterator;
  class SourceNodesIterator;
  class SinkNodesIterator;
  class Edge;
  
  //------------------------------------------------------------------
  class Node {
  public:
    Node () {}
    virtual ~Node () {}

    //! a unique identifier for each node
    virtual unsigned int getId () const = 0;

    //! number of incoming edges
    virtual int num_incoming () const = 0;
    
    //! number of outgoing edges
    virtual int num_outgoing () const = 0;

    //! returns true if node is an entry node, IOW  has no incoming edges
    virtual bool isAnEntry() const = 0;

    //! returns true if node is an exit node, IOW  has no outgoing edges
    virtual bool isAnExit() const = 0;

    virtual bool operator==(Node& other) = 0;
    virtual bool operator<(Node& other) = 0;

    //! dump the node (named thusly to allow subclasses to have a dump
    // function with different arguments)
    virtual void dumpbase(std::ostream& os) = 0;

    // useful for constructing iterators 
    virtual void setVisitFlag() = 0;
    virtual void clearVisitFlag() = 0;
    virtual bool getVisitFlag() const = 0;

    // Other Iterators
    OA_ptr<IncomingEdgesIterator> getIncomingEdgesIterator() const
      { return getDGraphInterfaceIncomingEdgesIterator(); }

    OA_ptr<OutgoingEdgesIterator> getOutgoingEdgesIterator() const
      { return getDGraphInterfaceOutgoingEdgesIterator(); }

    OA_ptr<SourceNodesIterator> getSourceNodesIterator() const
      { return getDGraphInterfaceSourceNodesIterator(); }

    OA_ptr<SinkNodesIterator> getSinkNodesIterator() const
      { return getDGraphInterfaceSinkNodesIterator(); }


  protected:
    virtual OA_ptr<IncomingEdgesIterator> 
        getDGraphInterfaceIncomingEdgesIterator() const = 0;

    virtual OA_ptr<OutgoingEdgesIterator> 
        getDGraphInterfaceOutgoingEdgesIterator() const = 0;

    virtual OA_ptr<SourceNodesIterator> 
        getDGraphInterfaceSourceNodesIterator() const = 0;

    virtual OA_ptr<SinkNodesIterator> 
        getDGraphInterfaceSinkNodesIterator() const = 0;
  };

  // lt_Node: function object that compares by node id.  Useful for sorting.
  class lt_Node {
  public:
    // return true if n1 < n2; false otherwise
    virtual bool operator()(const OA_ptr<Node> n1, const OA_ptr<Node> n2) const = 0;
  };
  
  //! returns an node compare object
  virtual OA::OA_ptr<lt_Node> getNodeCompare() const = 0;

  //------------------------------------------------------------------
  class Edge {
  public:
    Edge() {}
    virtual ~Edge () {}
    
    //! a unique identifier for each edge
    virtual unsigned int getId () const = 0;

    //! Return source node for this edge.
    //! This method must have a default implementation because
    //! covariant returns are not implemented in many compilers. 
    //! Classes that inherit from DGraph::Interface will shadow
    //! this method and implement the private polymorphic method.
    OA_ptr<Node> source () const { return getDGraphInterfaceNodeSource(); }

    //! Return sink node for this edge
    //! This method must have a default implementation because
    //! covariant returns are not implemented in many compilers. 
    //! Classes that inherit from DGraph::Interface will shadow
    //! this method and implement the private polymorphic method.
    OA_ptr<Node> sink () const { return getDGraphInterfaceNodeSink(); }

    //! dump the edge (named thusly to allow subclasses to have a dump
    // function with different arguments)
    // FIXME: want to deprecate this
    virtual void dumpbase(std::ostream& os) = 0;

  protected:
    virtual OA_ptr<Node> getDGraphInterfaceNodeSource() const = 0;
    virtual OA_ptr<Node> getDGraphInterfaceNodeSink() const = 0;

  public:
//    virtual OA_ptr<Node> source () const = 0;
//    virtual OA_ptr<Node> sink () const = 0;
    virtual OA_ptr<Node> head () const = 0;
    virtual OA_ptr<Node> tail () const = 0;

    virtual bool operator==(Edge& other) = 0;
    virtual bool operator<(Edge& other) = 0;

  };
  
  // lt_Edge: function object that compares by id.  Useful for sorting.
  class lt_Edge {
  public:
    // return true if e1 < e2; false otherwise
    virtual bool operator()(const OA_ptr<Edge> e1, const OA_ptr<Edge> e2) const = 0;
  };

  //! returns an edge compare object
  virtual OA::OA_ptr<lt_Edge> getEdgeCompare() const = 0;

  //------------------------------------------------------------------
  class NodesIterator {
  public:
    NodesIterator (const Interface& g) {}
    NodesIterator () {} 
    virtual ~NodesIterator () {}
    OA_ptr<Node> current() const 
      { return getDGraphInterfaceNodesIteratorCurrent();}
    virtual void operator++ () = 0;
    void operator++(int) { ++*this; }  // postfix
    virtual bool isValid() const = 0;
    virtual void reset() = 0;
  protected:
    virtual OA_ptr<Node> getDGraphInterfaceNodesIteratorCurrent() const = 0;
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all entry nodes.
      Subclasses that implement this interface must implement methods 
      in NodesIterator 
  */
  class EntryNodesIterator : public virtual NodesIterator {
  public:
    EntryNodesIterator(const Interface& g) {}
    EntryNodesIterator() {}
    virtual ~EntryNodesIterator () {}
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all exit nodes.
      Subclasses that implement this interface must implement methods 
      in NodesIterator 
  */
  class ExitNodesIterator : public virtual NodesIterator {
  public:
    ExitNodesIterator(const Interface& g) {}
    ExitNodesIterator() {}
    virtual ~ExitNodesIterator () {}
  };
   //------------------------------------------------------------------
  /*! Iterator to enumerate all nodes in reverse post DFS order.
      Subclasses that implement this interface must implement methods 
      in NodesIterator 
  */
  class ReversePostDFSIterator : public virtual NodesIterator {
  public:
    ReversePostDFSIterator(const Interface& g) {}
    ReversePostDFSIterator() {}
    virtual ~ReversePostDFSIterator () {}
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all nodes in reverse post DFS order.
      Subclasses that implement this interface must implement methods 
      in NodesIterator 
  */
  class PostDFSIterator : public virtual NodesIterator {
  public:
    PostDFSIterator(const Interface& g) {}
    PostDFSIterator() {}
    virtual ~PostDFSIterator () {}
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all nodes in DFS order (postorder traversal
      over the depth-first-search spanning tree.
      Subclasses that implement this interface must implement methods 
      in NodesIterator 
  */
  class DFSIterator  : public virtual NodesIterator {
  public:
    DFSIterator (const Interface& g) {}
    DFSIterator () {}
    virtual ~DFSIterator () {}
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all nodes in BFS order.
      Subclasses that implement this interface must implement methods 
      in NodesIterator 
  */
  class BFSIterator : public virtual NodesIterator {
  public:
    BFSIterator (const Interface& g) {}
    BFSIterator () {}
    virtual ~BFSIterator () {}
   };
  //------------------------------------------------------------------
  class EdgesIterator {
  public:
    EdgesIterator (const Interface& g) {}
    EdgesIterator () {}
    virtual ~EdgesIterator () {}
    OA_ptr<Edge> current() const
      { return getDGraphInterfaceEdgesIteratorCurrent(); }
    virtual void operator++ () = 0;
    void operator++(int) { ++*this; }  // postfix
    virtual bool isValid() const = 0;
    virtual void reset() = 0;
  protected:
    virtual OA_ptr<Edge> getDGraphInterfaceEdgesIteratorCurrent() const = 0;
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all the incoming edges into a node.
      Subclasses of this must implement methods in EdgesIterator 
  */
  class IncomingEdgesIterator : public virtual EdgesIterator {
  public:
    IncomingEdgesIterator (const Node& n) {}
    IncomingEdgesIterator () {}
    virtual ~IncomingEdgesIterator () {}
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all the outgoing edges from a node. 
      Subclasses of this must implement methods in EdgesIterator 
  */
  class OutgoingEdgesIterator : public virtual EdgesIterator {
  public:
    OutgoingEdgesIterator (const Node& n) {}
    OutgoingEdgesIterator () {}
    virtual ~OutgoingEdgesIterator () {}
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all the source nodes for a particular Node. 

      NOTE:  Inherits from NodesIterator so that one can dynamically
             select between a SourceNodesIterator or a SinkNodesIterator
             and still be able to use the current method.
   */
  class SourceNodesIterator : public NodesIterator {
  public:
    SourceNodesIterator (const Node& n) {}
    SourceNodesIterator () {}
    virtual ~SourceNodesIterator () {}
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all the sink nodes. */
  class SinkNodesIterator : public NodesIterator {
  public:
    SinkNodesIterator (const Node& n) {}
    SinkNodesIterator () {}
    virtual ~SinkNodesIterator () {}
  };
 
 //------------------------------------------------------------------
  Interface () {}
  Interface (OA_ptr<Node> root) {}
  virtual ~Interface () {}

  virtual int getNumNodes () = 0;
  virtual int getNumEdges () = 0;


  //MMS, 5/16/05, took these out because not all DGraph's have a single
  //source and sink
  //! get and set the source node or root in the graph
  //! return Null if there is more than one source node
  //virtual OA_ptr<Node> getSource() = 0;
  //! get and set the sink node in the graph
  //! return Null if there is more than one sink node
  //virtual OA_ptr<Node> getSink() = 0;

  //! dump the graph (named thusly to allow subclasses to have a dump
  // function with different arguments)
  virtual void dumpbase(std::ostream& os) = 0;
 
  //------------------------------------------------------------------
  // Using trick of imitating covariance when getting all iterators
  // so that get an iterator specific to actual subclass
  //------------------------------------------------------------------
  
  OA_ptr<NodesIterator> getNodesIterator() const
    { return getDGraphInterfaceNodesIterator(); }

  OA_ptr<NodesIterator> getEntryNodesIterator() const
    { return getDGraphInterfaceEntryNodesIterator(); }

  OA_ptr<NodesIterator> getExitNodesIterator() const
    { return getDGraphInterfaceExitNodesIterator(); }

  OA_ptr<EdgesIterator> getEdgesIterator() const
    { return getDGraphInterfaceEdgesIterator(); }

  OA_ptr<DFSIterator> getDFSIterator() 
    { return getDGraphInterfaceDFSIterator(); }

  OA_ptr<BFSIterator> getBFSIterator() 
    { return getDGraphInterfaceBFSIterator(); }

  OA_ptr<ReversePostDFSIterator> 
    getReversePostDFSIterator(DGraphEdgeDirection pOrient)
    { return getDGraphInterfaceReversePostDFSIterator(pOrient); }

  OA_ptr<ReversePostDFSIterator>
    getReversePostDFSIterator(OA_ptr<Node> root, 
                              DGraphEdgeDirection pOrient)
    { return getDGraphInterfaceReversePostDFSIterator( root, pOrient); }

  OA_ptr<PostDFSIterator> 
    getPostDFSIterator(DGraphEdgeDirection pOrient)
    { return getDGraphInterfacePostDFSIterator(pOrient); }

  OA_ptr<PostDFSIterator>
    getPostDFSIterator(OA_ptr<Node> root, 
                              DGraphEdgeDirection pOrient)
    { return getDGraphInterfacePostDFSIterator( root, pOrient); }

protected:
  virtual OA_ptr<NodesIterator> getDGraphInterfaceNodesIterator() const = 0;
  virtual OA_ptr<EntryNodesIterator> getDGraphInterfaceEntryNodesIterator() const = 0;
  virtual OA_ptr<ExitNodesIterator> getDGraphInterfaceExitNodesIterator() const = 0;
  virtual OA_ptr<EdgesIterator> getDGraphInterfaceEdgesIterator() const = 0;
  virtual OA_ptr<DFSIterator> getDGraphInterfaceDFSIterator() = 0;
  virtual OA_ptr<BFSIterator> getDGraphInterfaceBFSIterator() = 0;
  virtual OA_ptr<ReversePostDFSIterator> 
      getDGraphInterfaceReversePostDFSIterator(
              DGraphEdgeDirection pOrient) = 0;
  virtual OA_ptr<ReversePostDFSIterator> 
      getDGraphInterfaceReversePostDFSIterator(
              OA_ptr<Node> root, DGraphEdgeDirection pOrient) = 0;
  virtual OA_ptr<PostDFSIterator> 
      getDGraphInterfacePostDFSIterator(
              DGraphEdgeDirection pOrient) = 0;
  virtual OA_ptr<PostDFSIterator> 
      getDGraphInterfacePostDFSIterator(
              OA_ptr<Node> root, DGraphEdgeDirection pOrient) = 0;
 

};
//--------------------------------------------------------------------

  } // end of DGraph namespace
} // end of OA namespace

#endif
