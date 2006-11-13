/*! \file
  
  \brief Declaration for DGraphStandard class.  A standard implementation
         of the DGraph::Interface

  \authors Arun Chauhan (2001 was part of Mint) edited by Michelle Strout
  \version $Id: DGraphStandard.hpp,v 1.29 2005/06/10 02:32:05 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef DGraphStandard_H
#define DGraphStandard_H

#include <cassert>
#include <OpenAnalysis/Utils/BaseGraph.hpp>  // implementation base
#include <OpenAnalysis/Utils/DGraph/Interface.hpp>     // interface base
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/Utils/DGraph/DGraphInterfaceIterators.hpp>

namespace OA {
  namespace DGraph {

//--------------------------------------------------------------------
/*!
   DGraphStandard is a general directed graph implementation that
   implements DGraph::Interface and inherits some of its implementation
   from BaseGraph.

   No extra restrictions are placed on nodes and edges in addition to
   those imposed by BaseGraph.  This means that self-edges, and
   multiple edges between two nodes, are allowed.

   A directed graph, DGraph, extends BaseGraph by adding DFS and BFS
   iterators, as well as iterators to enumerate source nodes, sink
   nodes, incoming edges, and outgoing edges for a node.

   NOTE ON friend CLASSES: Many classes (especially DGraph,
   DGraph::Node, and DGraph::Edge) have many friend classes.  This is
   *not* a kludge.  It is simulating "package" visiblity in Java.  We
   want a limited public interface to Node and Edge and yet give more
   permissions to methods within the Graph class.  
*/
//--------------------------------------------------------------------
class DGraphStandard : public virtual Interface, public BaseGraph {
public:
  class NodesIterator;
  class EdgesIterator;
  class DFSIterator;
  class BFSIterator;
  class IncomingEdgesIterator;
  class ReversePostDFSIterator;
  class PostDFSIterator;
  class OutgoingEdgesIterator;
  class SourceNodesIterator;
  class SinkNodesIterator;
  class ExitNodesIterator;
  class EntryNodesIterator;
  class Edge;
  class Node;
  friend class DFSIterator;
  friend class BFSIterator;
  friend class ReversePostDFSIterator;
  friend class PostDFSIterator;
  friend class ExitNodesIterator;
  friend class EntryNodesIterator;
  //------------------------------------------------------------------
  /*! An node in an undirected graph has a list of neighboring nodes 
   *  and a list of incident edges. */
  class Node : public virtual Interface::Node, public BaseGraph::Node {
  public:
    Node () 
        { incoming_edges = new std::list<OA_ptr<Edge> >;
          outgoing_edges = new std::list<OA_ptr<Edge> >; }
    ~Node () {
      incoming_edges->clear();
      outgoing_edges->clear();
    }
    virtual unsigned int getId () const { return BaseGraph::Node::getId(); }
    int num_incoming () const { return incoming_edges->size(); }
    int num_outgoing () const { return outgoing_edges->size(); }

    //! returns true if node is an entry node, IOW  has no incoming edges
    bool isAnEntry() const { return (num_incoming() == 0); }

    //! returns true if node is an exit node, IOW  has no outgoing edges
    bool isAnExit() const { return (num_outgoing() == 0); }

    //======== operator== and operator< essentially inlining what is in BaseGraph
    // we don't treat this subclass special, but the compiler doesn't know
    // which base class to use
    virtual bool operator==(Node& other) { return this==&other; }
    virtual bool operator<(Node& other) { return this<&other; }

    // definition for abstract super class
    virtual bool operator==(Interface::Node& other) { return this==&other; }
    virtual bool operator<(Interface::Node& other) { return this<&other; }

    // due to multiple inheritance 
    // need to clarify both of these
    virtual bool operator==(BaseGraph::Node& other) { return this==&other; }
    virtual bool operator<(BaseGraph::Node& other) { return this<&other; }
    
    // indicate that want to use operator== from BaseGraph as is
    // doesn't work, probably because of multiple inheritance
    //using BaseGraph::Node::operator==;
    //using BaseGraph::Node::operator<;

    virtual void dumpbase(std::ostream& os);

    // useful for constructing iterators 
    virtual void setVisitFlag() { visited = true; }
    virtual void clearVisitFlag() { visited = false; }
    virtual bool getVisitFlag() const { return visited; }

    // Other Iterators
    OA_ptr<IncomingEdgesIterator> getIncomingEdgesIterator() const { 
      OA_ptr<DGraph::Interface::IncomingEdgesIterator> it =
	getDGraphInterfaceIncomingEdgesIterator();
      return it.convert<IncomingEdgesIterator>(); 
    }

    OA_ptr<OutgoingEdgesIterator> getOutgoingEdgesIterator() const { 
      OA_ptr<DGraph::Interface::OutgoingEdgesIterator> it =
	getDGraphInterfaceOutgoingEdgesIterator();
      return it.convert<OutgoingEdgesIterator>(); 
    }

    OA_ptr<SourceNodesIterator> getSourceNodesIterator() const { 
      OA_ptr<DGraph::Interface::SourceNodesIterator> it =
	getDGraphInterfaceSourceNodesIterator();
      return it.convert<SourceNodesIterator>(); 
    }

    OA_ptr<SinkNodesIterator> getSinkNodesIterator() const {
      OA_ptr<DGraph::Interface::SinkNodesIterator> it =
        getDGraphInterfaceSinkNodesIterator();
      return it.convert<SinkNodesIterator>(); 
    }


  protected:
    virtual OA_ptr<Interface::IncomingEdgesIterator> 
      getDGraphInterfaceIncomingEdgesIterator() const
      { OA_ptr<Interface::IncomingEdgesIterator> retval; 
        retval = new IncomingEdgesIterator(*this); 
        return retval;
      }

    virtual OA_ptr<Interface::OutgoingEdgesIterator> 
      getDGraphInterfaceOutgoingEdgesIterator() const
      { OA_ptr<Interface::OutgoingEdgesIterator> retval; 
        retval = new OutgoingEdgesIterator(*this); 
        return retval;
      }

    virtual OA_ptr<Interface::SourceNodesIterator> 
      getDGraphInterfaceSourceNodesIterator() const
      { OA_ptr<Interface::SourceNodesIterator> retval; 
        retval = new SourceNodesIterator(*this); 
        return retval;
      }

    virtual OA_ptr<Interface::SinkNodesIterator> 
      getDGraphInterfaceSinkNodesIterator() const
      { OA_ptr<Interface::SinkNodesIterator> retval; 
        retval = new SinkNodesIterator(*this); 
        return retval;
      }


  private:
    bool visited;
    OA_ptr<std::list<OA_ptr<Edge> > > incoming_edges;
    OA_ptr<std::list<OA_ptr<Edge> > > outgoing_edges;
    friend class DGraphStandard;
    friend class DGraphStandard::DFSIterator;
    friend class DGraphStandard::BFSIterator;
    friend class DGraphStandard::ReversePostDFSIterator;
    friend class DGraphStandard::PostDFSIterator;
    friend class DGraphStandard::IncomingEdgesIterator;
    friend class DGraphStandard::OutgoingEdgesIterator;
    friend class DGraphStandard::SourceNodesIterator;
    friend class DGraphStandard::SinkNodesIterator;
  };
  
  // lt_Node: function object that compares by id.  Useful for sorting.
  class lt_Node : public Interface::lt_Node {
  public:
    // return true if n1 < n2; false otherwise
    bool operator()(const OA_ptr<Interface::Node> n1, 
                    const OA_ptr<Interface::Node> n2) const {
      return (n1->getId() < n2->getId());
    }
  };

  //! returns an node compare object
  OA::OA_ptr<Interface::lt_Node> getNodeCompare() const 
    { OA::OA_ptr<Interface::lt_Node> retval; retval= new lt_Node();  return retval; }

  //------------------------------------------------------------------
  class Edge : public virtual Interface::Edge, public BaseGraph::Edge {
  public:
    Edge (OA_ptr<Node> _source, OA_ptr<Node> _sink) 
        : BaseGraph::Edge(_source, _sink) {}
    virtual ~Edge () {}
    virtual unsigned int getId () const { return BaseGraph::Edge::getId(); }
    OA_ptr<Node> source () const { 
      OA_ptr<DGraph::Interface::Node> n = getDGraphInterfaceNodeSource();
      return n.convert<Node>(); 
    }
    OA_ptr<Node> sink () const { 
      OA_ptr<DGraph::Interface::Node> n = getDGraphInterfaceNodeSink();
      return n.convert<Node>(); 
    }

  protected:
    OA_ptr<Interface::Node> getDGraphInterfaceNodeSource() const { 
      OA_ptr<BaseGraph::Node> n = n1; // Stupid Sun CC 5.4
      return n.convert<Interface::Node>(); 
    }
    OA_ptr<Interface::Node> getDGraphInterfaceNodeSink() const {
      OA_ptr<BaseGraph::Node> n = n2; // Stupid Sun CC 5.4
      return n.convert<Interface::Node>(); 
    }

  public:
    OA_ptr<Interface::Node> head () const { 
      OA_ptr<Node> n = sink();
      return n.convert<Interface::Node>(); 
    }
    OA_ptr<Interface::Node> tail () const { 
      OA_ptr<Node> n = source();
      return n.convert<Interface::Node>(); 
    }
    friend class DGraphStandard;

    //=================== operator== and operator<
    // have to define for comparing against ourselves and
    // both parents
    
    // DGraph::Interface::Edge
    // just comparing edges by their ptr values, used to remove edge from a list
    virtual bool operator== (DGraph::Interface::Edge& other) 
        { return this==&other; }
    // comparing by ptr values because don't want two edges to map to same
    // location in the set containing edges, sorting is done by ids
    virtual bool operator<(DGraph::Interface::Edge& other) 
        { return this<&other; }
    
    // BaseGraph::Edge
    virtual bool operator== (BaseGraph::Edge& other) { return this==&other; }
    virtual bool operator<(BaseGraph::Edge& other) 
        { return BaseGraph::Edge::operator<(other); }

    // Edge
    virtual bool operator== (Edge& other) { return this==&other; }
    virtual bool operator<(Edge& other) 
        { return BaseGraph::Edge::operator<(other); }
    
    virtual void dumpbase(std::ostream& os);

  };
  
  // lt_Edge: function object that compares by source and sink node
  // ids.  Useful for sorting.  Not used to put edges in sets or other
  // STL containers.
  class lt_Edge : public Interface::lt_Edge {
  public:
    // DO NOT change this to use edge Id because 
    // code exists that assumes this compares by source and sink node ids
    // if another one is wanted then just make a new functor
    bool operator()(const OA_ptr<Interface::Edge> e1, 
                    const OA_ptr<Interface::Edge> e2) const {
      unsigned int src1 = e1->source()->getId();
      unsigned int src2 = e2->source()->getId();
      if (src1 == src2) { 
        return (e1->sink()->getId() < e2->sink()->getId()); 
      } else {
        return (src1 < src2);
      }
    }
  };

  //! returns an edge compare object
  OA::OA_ptr<Interface::lt_Edge> getEdgeCompare() const 
    { OA::OA_ptr<Interface::lt_Edge> retval; retval= new lt_Edge();  return retval; }

  //------------------------------------------------------------------
  /*! The NodesIterator is just and extension of BaseGraph::NodesIterator 
      to provide access to DGraph nodes. 
  */
  class NodesIterator : public virtual Interface::NodesIterator,
                        public BaseGraph::NodesIterator {
  public:
    NodesIterator (const DGraphStandard& g) : BaseGraph::NodesIterator(g) {}
    NodesIterator () {} 
    virtual ~NodesIterator () {}
    OA_ptr<Node> current() const { 
      OA_ptr<DGraph::Interface::Node> n =
        getDGraphInterfaceNodesIteratorCurrent();
      return n.convert<Node>(); 
    }
    void operator++ () { BaseGraph::NodesIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return BaseGraph::NodesIterator::isValid(); } 
    void reset() { BaseGraph::NodesIterator::reset(); }

  protected:
    virtual OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { 
      OA_ptr<BaseGraph::Node> n = (*mIter);
      return n.convert<Node>(); 
    }
  };
  //------------------------------------------------------------------
  /*! The EntryNodesIterator iterates over all entry nodes into graph.
      IOW, nodes that have no incoming edges.

      FIXME: There should only be one NodesIterator and all the getBlahIterator
      should generate an appropriate list of nodes and then pass that to
      the constructor of the generic NodesIterator.
  */
  class EntryNodesIterator : public virtual Interface::EntryNodesIterator,
                             public NodesIterator {
  public:
    EntryNodesIterator (const DGraphStandard& g) 
      {
        OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList 
            = create_entry_list(g);
        mList = g.convert_to_DGraphStandard(dgList);
        mIter = mList->begin();
      }
    EntryNodesIterator () {} 
    virtual ~EntryNodesIterator () {}
    OA_ptr<Node> current() const { 
        OA_ptr<Node> retval;
        if (isValid()) { retval = *mIter; } 
        return retval;
      }
    void operator++ () { if (isValid()) { mIter++; } }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return mIter != mList->end(); }
    void reset() { mIter = mList->begin(); }

  protected:
    virtual OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { 
      return current();
    }
  private:
    OA_ptr<std::list<OA_ptr<Node> > > mList;
    std::list<OA_ptr<Node> >::iterator mIter;
  };
  //------------------------------------------------------------------
  /*! The ExitNodesIterator iterates over all entry nodes into graph.
      IOW, nodes that have no incoming edges.

      FIXME: There should only be one NodesIterator and all the getBlahIterator
      should generate an appropriate list of nodes and then pass that to
      the constructor of the generic NodesIterator.
  */
  class ExitNodesIterator : public virtual Interface::ExitNodesIterator,
                             public NodesIterator {
  public:
    ExitNodesIterator (const DGraphStandard& g) 
      {
        OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > dgList 
            = create_exit_list(g);
        mList = g.convert_to_DGraphStandard(dgList);
        mIter = mList->begin();
      }
    ExitNodesIterator () {} 
    virtual ~ExitNodesIterator () {}
    OA_ptr<Node> current() const { 
        OA_ptr<Node> retval;
        if (isValid()) { retval = *mIter; } 
        return retval;
      }
    void operator++ () { if (isValid()) mIter++;  }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return mIter != mList->end(); }
    void reset() { mIter = mList->begin(); }

  protected:
    virtual OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { 
      return current();
    }
  private:
    OA_ptr<std::list<OA_ptr<Node> > > mList;
    std::list<OA_ptr<Node> >::iterator mIter;
  };
  //------------------------------------------------------------------
  /*! The ReversePostDFSIterator constructor creates a list of nodes
      in reverse postorder either based on the original orientation
      of the directed edges or the reverse orientation of the edges.
      The ReversePostDFSIterator is an iterator for the constructed list.
  */
  class ReversePostDFSIterator 
      : public virtual Interface::ReversePostDFSIterator {
  public:
    ReversePostDFSIterator (DGraphStandard& g, 
                            DGraphEdgeDirection pOrient);
    ReversePostDFSIterator (DGraphStandard& g, 
                            OA_ptr<Interface::Node> root, 
                            DGraphEdgeDirection pOrient);
    virtual ~ReversePostDFSIterator () {}
    OA_ptr<Node> current() const { return *mIter; }
    void operator++ () { mIter++; }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return (mNodeList->end() != mIter); }
    void reset() { mIter = mNodeList->begin(); }

  protected:
    virtual OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { return *mIter; }
  private:
    OA_ptr<std::list<OA_ptr<Node> > > mNodeList;
    std::list<OA_ptr<Node> >::iterator mIter;
    //void createNodeList (DGraphStandard& g, 
    //                     OA_ptr<Interface::Node> root, 
    //                     DGraphEdgeDirection pOrient);
    //void reverse_postorder_recurse(DGraphStandard& g, 
    //                               OA_ptr<Node> pNode, 
    //                               DGraphEdgeDirection pOrient);
  };
  //------------------------------------------------------------------
  /*! FIXME: not working right now, make it work like ReversePostDFSIterator
      The PostDFSIterator constructor creates a list of nodes
      in postorder either based on the original orientation
      of the directed edges or the reverse orientation of the edges.
      Does require a root for original orientation and a sink for reverse.
      The PostDFSIterator is an iterator for the constructed list.
  */
  class PostDFSIterator 
      : public virtual Interface::PostDFSIterator {
  public:
    PostDFSIterator (DGraphStandard& g, 
                     DGraphEdgeDirection pOrient);
    PostDFSIterator (DGraphStandard& g, 
                     OA_ptr<Interface::Node> root, 
                     DGraphEdgeDirection pOrient);
    virtual ~PostDFSIterator () {}
    OA_ptr<Node> current() const { return *mIter; }
    void operator++ () { mIter++; }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return mNodeList.end() != mIter; }
    void reset() { mIter = mNodeList.begin(); }

  protected:
    virtual OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { return *mIter; }
  private:
    std::list<OA_ptr<Node> > mNodeList;
    std::list<OA_ptr<Node> >::iterator mIter;
  };
  //------------------------------------------------------------------
  /*! FIXME: Doesn't actually work.
      The DFSIterator here is just an extension of BaseGraph::DFSIterator 
      to allow proper casting. */
  class DFSIterator : public virtual Interface::DFSIterator,
                      public BaseGraph::DFSIterator {
  public:
    DFSIterator (DGraphStandard& g) : Interface::DFSIterator(g),
                                      BaseGraph::DFSIterator(g) {}
    virtual ~DFSIterator () {}
    OA_ptr<Node> current() const { 
      OA_ptr<DGraph::Interface::Node> n =
	getDGraphInterfaceNodesIteratorCurrent();
      return n.convert<Node>(); 
    }
    void operator++ () { BaseGraph::DFSIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return BaseGraph::DFSIterator::isValid(); } 
    void reset() { BaseGraph::DFSIterator::reset(); }
  protected:
    virtual OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const { 
      OA_ptr<BaseGraph::Node> n = mNode; // Stupid Sun CC 5.4
      return n.convert<Node>(); 
    }
   };
  //------------------------------------------------------------------
  /*! The BFSIterator here is just an extension of BaseGraph::BFSIterator 
      to allow proper casting. */
  class BFSIterator : public virtual Interface::BFSIterator,
                      public BaseGraph::BFSIterator {
  public:
    BFSIterator (DGraphStandard& g) : Interface::BFSIterator(g),
                                      BaseGraph::BFSIterator(g) {}
    virtual ~BFSIterator () {}
    OA_ptr<Node> current() const { 
      OA_ptr<DGraph::Interface::Node> n =
	getDGraphInterfaceNodesIteratorCurrent();
      return n.convert<Node>(); 
    }
    void operator++ () { BaseGraph::BFSIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return BaseGraph::BFSIterator::isValid(); } 
    void reset() { BaseGraph::BFSIterator::reset(); }
  protected:
    virtual OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const { 
      OA_ptr<BaseGraph::Node> n = mNode; // Stupid Sun CC 5.4
      return n.convert<Node>(); 
    }
   };
  //------------------------------------------------------------------
  /*! The EdgesIterator is just and extension of BaseGraph::EdgesIterator 
      to provide access to DGraph edges. */
  class EdgesIterator : public virtual Interface::EdgesIterator,
                        public BaseGraph::EdgesIterator {
  public:
    EdgesIterator (const DGraphStandard& g) : BaseGraph::EdgesIterator(g) {}
    EdgesIterator () {}
    virtual ~EdgesIterator () {}
    OA_ptr<Edge> current() const { 
      OA_ptr<DGraph::Interface::Edge> e =
	getDGraphInterfaceEdgesIteratorCurrent();
      return e.convert<Edge>(); 
    }
    void operator++ () { BaseGraph::EdgesIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return BaseGraph::EdgesIterator::isValid(); } 
    void reset() { BaseGraph::EdgesIterator::reset(); }
  protected:
    virtual OA_ptr<DGraph::Interface::Edge> 
        getDGraphInterfaceEdgesIteratorCurrent() const { 
      OA_ptr<BaseGraph::Edge> e = (*mIter);
      return e.convert<Edge>(); 
    }
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all the incoming edges into a node. */
  class IncomingEdgesIterator 
      : public virtual Interface::IncomingEdgesIterator,
        public EdgesIterator {
  public:
    IncomingEdgesIterator(const Node& n) 
        : Interface::IncomingEdgesIterator(n)
    { //Node& recastN = dynamic_cast<Node&>(n);
      mList = n.incoming_edges;
      mIter = mList->begin();
    }
    virtual ~IncomingEdgesIterator () {}
    OA_ptr<Edge> current() const { return *mIter; }
    void operator++ () { ++mIter; }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return (mIter != mList->end()); }
    void reset() { mIter = mList->begin(); }

  protected:
    virtual OA_ptr<DGraph::Interface::Edge> 
        getDGraphInterfaceEdgesIteratorCurrent() const 
    { return *mIter; }

  private:
    OA_ptr<std::list<OA_ptr<Edge> > > mList;
    std::list<OA_ptr<Edge> >::iterator mIter;
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all the outgoing edges from a node. */
  class OutgoingEdgesIterator 
      : public virtual Interface::OutgoingEdgesIterator,
        public EdgesIterator {
  public:
    OutgoingEdgesIterator (const Node& n) 
        : Interface::OutgoingEdgesIterator(n)
    { //Node& recastN = dynamic_cast<Node&>(n);
      mList = n.outgoing_edges;
      mIter = mList->begin();
    }
    virtual ~OutgoingEdgesIterator () {}
    void operator++ () { ++mIter; }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return (mIter != mList->end()); }
    OA_ptr<Edge> current() const { return *mIter; }
    void reset() { mIter = mList->begin(); }

  protected:
    virtual OA_ptr<DGraph::Interface::Edge> 
        getDGraphInterfaceEdgesIteratorCurrent() const 
    { return *mIter; }

  private:
    OA_ptr<std::list<OA_ptr<Edge> > > mList;
    std::list<OA_ptr<Edge> >::iterator mIter;
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all the source nodes for a particular Node. 

      NOTE:  Inherits from NodesIterator so that one can dynamically
             select between a SourceNodesIterator or a SinkNodesIterator
             and still be able to use the current method.
   */
  class SourceNodesIterator : public virtual Interface::SourceNodesIterator,
                              public NodesIterator {
  public:
    SourceNodesIterator(const Node& n) 
        : Interface::SourceNodesIterator(n)
    { //Node& recastN = dynamic_cast<Node&>(n);
      mList = n.incoming_edges;
      mIter = mList->begin();
    }
    virtual ~SourceNodesIterator () {}
    void operator++ () { ++mIter; }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return (mIter != mList->end()); }
    OA_ptr<Node> current() const { 
      OA_ptr<DGraph::Interface::Node> n =
	getDGraphInterfaceNodesIteratorCurrent();
      return n.convert<Node>();
    }
    void reset() { mIter = mList->begin(); }
  protected:
    OA_ptr<Interface::Node> getDGraphInterfaceNodesIteratorCurrent() const
        { OA_ptr<Edge> e = *mIter; return e->source(); }
  private:
    OA_ptr<std::list<OA_ptr<Edge> > > mList;
    std::list<OA_ptr<Edge> >::iterator mIter;
  };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all the sink nodes. */
  class SinkNodesIterator : public virtual Interface::SinkNodesIterator,
                            public NodesIterator {
  public:
    SinkNodesIterator(const Node& n)  
        : Interface::SinkNodesIterator(n)
    { //Node& recastN = dynamic_cast<Node&>(n);
      mList = n.outgoing_edges;
      mIter = mList->begin();
    }
    virtual ~SinkNodesIterator () {}
    void operator++ () { ++mIter; }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return (mIter != mList->end()); }
    OA_ptr<Node> current() const { 
      OA_ptr<DGraph::Interface::Node>  n =
        getDGraphInterfaceNodesIteratorCurrent();
      return n.convert<Node>();
    }
    void reset() { mIter = mList->begin(); }

  protected:
    OA_ptr<Interface::Node> getDGraphInterfaceNodesIteratorCurrent() const
        { OA_ptr<Edge> e = *mIter; return e->sink(); }

  private:
    OA_ptr<std::list<OA_ptr<Edge> > > mList;
    std::list<OA_ptr<Edge> >::iterator mIter;
  };
  //------------------------------------------------------------------
  /*! The BiDirNodesIterator is just an extension of 
      BaseGraph::BiDirNodesIterator to provide access to DGraph nodes. */
/*
  class BiDirNodesIterator : public Interface::BiDirNodesIterator,
                             public BaseGraph::BiDirNodesIterator {
  public:
    BiDirNodesIterator (DGraph& g) : BaseGraph::BiDirNodesIterator(g) {}
    BiDirNodesIterator (DGraph& g, dirType d) : BaseGraph::BiDirNodesIterator(g, d) {}
    virtual ~BiDirNodesIterator () {}
    Node* current() { return dynamic_cast<Node*>(*iter); }
    void operator++ () { BaseGraph::EdgesIterator::operator++(); }
    operator bool () { BaseGraph::EdgesIterator::operator bool(); } 
    void reset() { BaseGraph::EdgesIterator::reset(); }
  };
  */
  //------------------------------------------------------------------
  DGraphStandard() : BaseGraph() {}
  DGraphStandard(OA_ptr<Node> root) : BaseGraph (root) {}
  virtual ~DGraphStandard() {}

  virtual void addEdge(OA_ptr<Interface::Edge> e);
  virtual void addNode(OA_ptr<Interface::Node> n);
  virtual void removeEdge(OA_ptr<Interface::Edge> e);
  virtual void removeNode(OA_ptr<Interface::Node> n);

  int getNumNodes() { return BaseGraph::getNumNodes(); }
  int getNumEdges() { return BaseGraph::getNumEdges(); }

  virtual void dumpbase(std::ostream& os);
 
  //------------------------------------------------------------------
  OA_ptr<NodesIterator> getNodesIterator() const { 
    OA_ptr<DGraph::Interface::NodesIterator> it =
      getDGraphInterfaceNodesIterator();
    return it.convert<NodesIterator>(); 
  }

  OA_ptr<EntryNodesIterator> getEntryNodesIterator() const { 
    OA_ptr<DGraph::Interface::EntryNodesIterator> it =
      getDGraphInterfaceEntryNodesIterator();
    return it.convert<EntryNodesIterator>(); 
  }

  OA_ptr<ExitNodesIterator> getExitNodesIterator() const { 
    OA_ptr<DGraph::Interface::ExitNodesIterator> it =
      getDGraphInterfaceExitNodesIterator();
    return it.convert<ExitNodesIterator>(); 
  }

  OA_ptr<EdgesIterator> getEdgesIterator() const { 
    OA_ptr<DGraph::Interface::EdgesIterator> it =
      getDGraphInterfaceEdgesIterator();
    return it.convert<EdgesIterator>(); 
  }

  OA_ptr<DFSIterator> getDFSIterator() { 
    OA_ptr<DGraph::Interface::DFSIterator> it =
      getDGraphInterfaceDFSIterator();
    return it.convert<DFSIterator>(); 
  }

  OA_ptr<BFSIterator> getBFSIterator() { 
    OA_ptr<DGraph::Interface::BFSIterator> it =
      getDGraphInterfaceBFSIterator();
    return it.convert<BFSIterator>(); 
  }

  OA_ptr<ReversePostDFSIterator> 
      getReversePostDFSIterator(DGraph::DGraphEdgeDirection pOrient) { 
    OA_ptr<DGraph::Interface::ReversePostDFSIterator> it =
      getDGraphInterfaceReversePostDFSIterator(pOrient);
    return it.convert<ReversePostDFSIterator>(); 
  }

  OA_ptr<ReversePostDFSIterator>
      getReversePostDFSIterator(OA_ptr<Node> root, 
                                DGraph::DGraphEdgeDirection pOrient) { 
    OA_ptr<DGraph::Interface::ReversePostDFSIterator> it =
      getDGraphInterfaceReversePostDFSIterator(root,pOrient);
    return it.convert<ReversePostDFSIterator>(); 
  }

  OA_ptr<PostDFSIterator> 
      getPostDFSIterator(DGraph::DGraphEdgeDirection pOrient) { 
    OA_ptr<DGraph::Interface::PostDFSIterator> it =
      getDGraphInterfacePostDFSIterator(pOrient);
    return it.convert<PostDFSIterator>(); 
  }

  OA_ptr<PostDFSIterator>
      getPostDFSIterator(OA_ptr<Node> root, 
                                DGraph::DGraphEdgeDirection pOrient) { 
    OA_ptr<DGraph::Interface::PostDFSIterator> it =
      getDGraphInterfacePostDFSIterator(root,pOrient);
    return it.convert<PostDFSIterator>(); 
  }


protected:
  virtual OA_ptr<DGraph::Interface::NodesIterator> 
      getDGraphInterfaceNodesIterator() const
  { OA_ptr<NodesIterator> retval; retval = new NodesIterator(*this);
    return retval;
  }
  virtual OA_ptr<DGraph::Interface::EntryNodesIterator> 
      getDGraphInterfaceEntryNodesIterator() const
  { OA_ptr<EntryNodesIterator> retval; retval = new EntryNodesIterator(*this);
    return retval;
  }
  virtual OA_ptr<DGraph::Interface::ExitNodesIterator> 
      getDGraphInterfaceExitNodesIterator() const
  { OA_ptr<ExitNodesIterator> retval; retval = new ExitNodesIterator(*this);
    return retval;
  }
  virtual OA_ptr<DGraph::Interface::EdgesIterator> 
      getDGraphInterfaceEdgesIterator() const
  { OA_ptr<EdgesIterator> retval; retval = new EdgesIterator(*this);
    return retval;
  }
  virtual OA_ptr<DGraph::Interface::DFSIterator> getDGraphInterfaceDFSIterator()
  { OA_ptr<DFSIterator> retval; retval = new DFSIterator(*this);
    return retval;
  }
  virtual OA_ptr<DGraph::Interface::BFSIterator> getDGraphInterfaceBFSIterator()
  { OA_ptr<BFSIterator> retval; retval = new BFSIterator(*this);
    return retval;
  }
  virtual OA_ptr<DGraph::Interface::ReversePostDFSIterator> 
      getDGraphInterfaceReversePostDFSIterator(
              DGraphEdgeDirection pOrient)
  { OA_ptr<ReversePostDFSIterator> retval; retval 
      = new ReversePostDFSIterator(*this,pOrient);
    return retval;
  }
  virtual OA_ptr<DGraph::Interface::ReversePostDFSIterator> 
      getDGraphInterfaceReversePostDFSIterator(
              OA_ptr<Interface::Node> root, DGraphEdgeDirection pOrient)
  { OA_ptr<ReversePostDFSIterator> retval; retval 
      = new ReversePostDFSIterator(*this,root,pOrient);
    return retval;
  }
  virtual OA_ptr<DGraph::Interface::PostDFSIterator> 
      getDGraphInterfacePostDFSIterator(
              DGraphEdgeDirection pOrient)
  { OA_ptr<PostDFSIterator> retval; retval 
      = new PostDFSIterator(*this,pOrient);
    return retval;
  }
  virtual OA_ptr<DGraph::Interface::PostDFSIterator> 
      getDGraphInterfacePostDFSIterator(
              OA_ptr<Interface::Node> root, DGraphEdgeDirection pOrient)
  { OA_ptr<PostDFSIterator> retval; retval 
      = new PostDFSIterator(*this,root,pOrient);
    return retval;
  }
 
private:
  // Helper routines
  OA_ptr<std::list<OA_ptr<DGraphStandard::Node> > > 
  convert_to_DGraphStandard( 
          OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > pList) const;


  OA_ptr<BaseGraph::Node> create_DFS_links (OA_ptr<BaseGraph::Node> start_node);
  OA_ptr<BaseGraph::Node> create_BFS_links (OA_ptr<BaseGraph::Node> start_node);
};
//--------------------------------------------------------------------

  } // end of DGraph namespace
} // end of OA namespace

#endif
