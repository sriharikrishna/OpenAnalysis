/*! \file
  
  \brief Abstract interface that all CFG analysis results must satisfy.

  \authors Nathan Tallent?, Michelle Strout
  \version $Id: Interface.hpp,v 1.13 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef CallGraphInterface_H
#define CallGraphInterface_H

//--------------------------------------------------------------------
// standard headers
#ifdef NO_STD_CHEADERS
# include <string.h>
#else
# include <cstring>
#endif

// STL headers
#include <list>
#include <map>

// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/Utils/DGraph/Interface.hpp>

//--------------------------------------------------------------------

namespace OA {
  namespace CallGraph {

//--------------------------------------------------------------------
/*! This abstract Interface to any CallGraph assumes a directed graph
    interface with nodes and edges.
    Each node corresponds to a function definition or
    reference (or both).  
 */
class Interface : public virtual DGraph::Interface {
public:
  class NodesIterator;
  class BFSIterator;
  class DFSIterator;
  class ReversePostDFSIterator;
  class PostDFSIterator;
  class EdgesIterator;
  class Node;
  class Edge;
  class IncomingEdgesIterator;
  class OutgoingEdgesIterator;
  class SourceNodesIterator;
  class SinkNodesIterator;

  // Changes here must be also reflected in CallGraph.C:edgeTypeToString?
  enum EdgeType { NORMAL_EDGE = 0 };
  
public:
  Interface () {}
  virtual ~Interface () {}
  
  //-------------------------------------
  // CallGraph information access
  //-------------------------------------
 
  //---------------------------------------------------------
  // CallGraph iterators
  //---------------------------------------------------------
  OA_ptr<NodesIterator> getNodesIterator() const
    { OA_ptr<DGraph::Interface::NodesIterator> iter 
        =  getDGraphInterfaceNodesIterator();
       return iter.convert<NodesIterator>(); }
  
  OA_ptr<EdgesIterator> getEdgesIterator() const
    { OA_ptr<DGraph::Interface::EdgesIterator> iter 
        =  getDGraphInterfaceEdgesIterator();
       return iter.convert<EdgesIterator>(); }

  OA_ptr<DFSIterator> getDFSIterator() 
    { OA_ptr<DGraph::Interface::DFSIterator> iter 
        =  getDGraphInterfaceDFSIterator();
       return iter.convert<DFSIterator>(); }

  OA_ptr<BFSIterator> getBFSIterator() 
    { OA_ptr<DGraph::Interface::BFSIterator> iter 
        =  getDGraphInterfaceBFSIterator();
       return iter.convert<BFSIterator>(); }

  OA_ptr<ReversePostDFSIterator> 
    getReversePostDFSIterator(DGraph::DGraphEdgeDirection pOrient)
    { OA_ptr<DGraph::Interface::ReversePostDFSIterator> iter 
        = getDGraphInterfaceReversePostDFSIterator(pOrient);
      return iter.convert<ReversePostDFSIterator>(); }

  OA_ptr<ReversePostDFSIterator> 
    getReversePostDFSIterator(OA_ptr<Node> root, 
            DGraph::DGraphEdgeDirection pOrient)
    { OA_ptr<DGraph::Interface::ReversePostDFSIterator> iter
        = getDGraphInterfaceReversePostDFSIterator( root, pOrient);
      return iter.convert<ReversePostDFSIterator>(); }

  OA_ptr<PostDFSIterator> 
    getPostDFSIterator(DGraph::DGraphEdgeDirection pOrient)
    { OA_ptr<DGraph::Interface::PostDFSIterator> iter 
        = getDGraphInterfacePostDFSIterator(pOrient);
      return iter.convert<PostDFSIterator>(); }

  OA_ptr<PostDFSIterator> 
    getPostDFSIterator(OA_ptr<Node> root, 
            DGraph::DGraphEdgeDirection pOrient)
    { OA_ptr<DGraph::Interface::PostDFSIterator> iter
        = getDGraphInterfacePostDFSIterator( root, pOrient);
      return iter.convert<PostDFSIterator>(); }


  //------------------------------------------------------------------
  /*! The NodesIterator is just and extension of DGraph::NodesIterator 
      to provide access to CFG::Interface nodes. 
  */
  class NodesIterator : public virtual DGraph::Interface::NodesIterator {
  public:
    NodesIterator (const Interface& g) : DGraph::Interface::NodesIterator(g) {}
    NodesIterator () {} 
    virtual ~NodesIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
  };

  //------------------------------------------------------------------
  class ReversePostDFSIterator 
      : public virtual DGraph::Interface::ReversePostDFSIterator 
  {
  public:
    ReversePostDFSIterator (Interface& g) {}
    ReversePostDFSIterator () {}
    virtual ~ReversePostDFSIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
  };
  
  //------------------------------------------------------------------
  class PostDFSIterator 
      : public virtual DGraph::Interface::PostDFSIterator 
  {
  public:
    PostDFSIterator (Interface& g) {}
    PostDFSIterator () {}
    virtual ~PostDFSIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
  };
  
  //------------------------------------------------------------------
  class DFSIterator 
      : public virtual DGraph::Interface::DFSIterator 
  {
  public:
    DFSIterator (Interface& g) {}
    DFSIterator () {}
    virtual ~DFSIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
  };
  
  //------------------------------------------------------------------
  class BFSIterator 
      : public virtual DGraph::Interface::BFSIterator 
  {
  public:
    BFSIterator (Interface& g) {}
    BFSIterator () {}
    virtual ~BFSIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
  };

  //------------------------------------------------------------------
  class EdgesIterator : public virtual DGraph::Interface::EdgesIterator {
  public:
    EdgesIterator (const Interface& g) {}
    EdgesIterator () {} 
    virtual ~EdgesIterator () {}
    OA_ptr<Edge> current() const
      { OA_ptr<DGraph::Interface::Edge> n 
          = getDGraphInterfaceEdgesIteratorCurrent();
        return n.convert<Edge>(); }
  };
  
  //------------------------------------------------------------------
  class IncomingEdgesIterator : public virtual EdgesIterator,
          public virtual DGraph::Interface::IncomingEdgesIterator 
  {
  public:
    IncomingEdgesIterator (const Node& n) {}
    IncomingEdgesIterator () {}
    virtual ~IncomingEdgesIterator () {}
    OA_ptr<Edge> current() const
      { OA_ptr<DGraph::Interface::Edge> n 
          = getDGraphInterfaceEdgesIteratorCurrent();
        return n.convert<Edge>(); }
  };
  
  //------------------------------------------------------------------
  class OutgoingEdgesIterator 
      : public virtual EdgesIterator,
        public virtual DGraph::Interface::OutgoingEdgesIterator 
  {
  public:
    OutgoingEdgesIterator (const Node& n) {}
    OutgoingEdgesIterator () {}
    virtual ~OutgoingEdgesIterator () {}
    OA_ptr<Edge> current() const
      { OA_ptr<DGraph::Interface::Edge> n 
          = getDGraphInterfaceEdgesIteratorCurrent();
        return n.convert<Edge>(); }
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
 


  class NodeCallsIterator;
  //--------------------------------------------------------
  class Node : public virtual DGraph::Interface::Node {
  public:
    Node () {} 
    virtual ~Node () { }
    
    //! getId: An id unique within instances of CallGraph::Node
    virtual unsigned int getId() const = 0;
    
    //! resetIds: reset id numbering
    //virtual static void resetIds() = 0;

    virtual bool isDefined() const = 0;
    virtual bool isCalled() const  = 0;
    
    virtual ProcHandle getProc() const = 0;
    virtual SymHandle getProcSym() const = 0;
    
    // Other Iterators
    OA_ptr<IncomingEdgesIterator> getIncomingEdgesIterator() const
      { OA_ptr<DGraph::Interface::IncomingEdgesIterator> iter 
          = getDGraphInterfaceIncomingEdgesIterator();
        return iter.convert<IncomingEdgesIterator>(); }

    OA_ptr<OutgoingEdgesIterator> getOutgoingEdgesIterator() const
      { OA_ptr<DGraph::Interface::OutgoingEdgesIterator> iter
          = getDGraphInterfaceOutgoingEdgesIterator();
        return iter.convert<OutgoingEdgesIterator>(); }

    OA_ptr<SourceNodesIterator> getSourceNodesIterator() const
      { OA_ptr<DGraph::Interface::SourceNodesIterator> iter
          = getDGraphInterfaceSourceNodesIterator();
        return iter.convert<SourceNodesIterator>(); }

    OA_ptr<SinkNodesIterator> getSinkNodesIterator() const
      { OA_ptr<DGraph::Interface::SinkNodesIterator> iter
          = getDGraphInterfaceSinkNodesIterator();
        return iter.convert<SinkNodesIterator>(); }


    //void dump(std::ostream& os);
    //void shortdump(CallGraph* cgraph, std::ostream& os);
    //void longdump(CallGraph* cgraph, std::ostream& os);
  };
  
  //--------------------------------------------------------
  class Edge : public virtual DGraph::Interface::Edge {
  public:
    Edge () {}
    virtual ~Edge () {}
    
    //! getId: An id unique within instances of CallGraph::Edge
    virtual unsigned int getId() const = 0;
    
    //! Returns the handle for the callsite that induces this edge
    virtual ExprHandle getCallHandle() const = 0;

    virtual EdgeType getType() const = 0;
       
    //! Return source node for this edge.    
    //! This method must have a default implementation because
    //! covariant returns are not implemented in many compilers.    
    //! Classes that inherit from CFG::Interface will shadow
    //! this method and implement the private polymorphic method.
    OA_ptr<Node> source () const { return getCallGraphInterfaceNodeSource(); }
    
    //! Return sink node for this edge.    
    //! This method must have a default implementation because
    //! covariant returns are not implemented in many compilers.    
    //! Classes that inherit from CFG::Interface will shadow
    //! this method and implement the private polymorphic method.
    OA_ptr<Node> sink () const { return getCallGraphInterfaceNodeSink(); }
    
  protected:
    virtual OA_ptr<Node> getCallGraphInterfaceNodeSource() const = 0;
    virtual OA_ptr<Node> getCallGraphInterfaceNodeSink() const = 0;
  
    //void dump (std::ostream& os);

  };  
  //--------------------------------------------------------
  
  class NodeCallsIterator //: public Iterator {
  {
  public:
    NodeCallsIterator () {}
    virtual ~NodeCallsIterator () {}

    virtual void operator ++ () = 0;
    virtual bool isValid() = 0;
    virtual ExprHandle current() = 0;
  };
  
  virtual const SymHandle subprog_name ()  = 0;
  
  //virtual static void resetIds() = 0;
  
  virtual void dump (std::ostream& os, OA_ptr<IRHandlesIRInterface>) = 0;

 
};
//--------------------------------------------------------------------

  } // end of CallGraph namespace
} // end of OA namespace

#endif
