/*! \file
  
  \brief Declaration for standard CallGraphStandard.

  \authors Nathan Tallent?, Michelle Strout
  \version $Id: CallGraphStandard.hpp,v 1.21 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#ifndef CallGraphStandard_H
#define CallGraphStandard_H

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
#include <OpenAnalysis/Utils/DGraph/DGraphStandard.hpp>
#include <OpenAnalysis/IRInterface/CallGraphIRInterface.hpp>
#include "Interface.hpp"

//--------------------------------------------------------------------

namespace OA {
  namespace CallGraph {

//--------------------------------------------------------------------
/*! CallGraph is a DGraph (directed graph) with enhanced nodes and
    edges.  Each node corresponds to a function definition or
    reference (or both).  Each node contains a list of places where it
    is defined or referenced. */
class CallGraphStandard : public virtual Interface, public DGraph::DGraphStandard {
public:
  class Node;
  class IncomingEdgesIterator;
  class OutgoingEdgesIterator;
  class SourceNodesIterator;
  class SinkNodesIterator;
  class NodesIterator;
  class EdgesIterator;
  class DFSIterator;
  class BFSIterator;
  class ReversePostDFSIterator;
  class PostDFSIterator;

  // Changes here must be also reflected in CallGraph.C:edgeTypeToString.
  enum EdgeType { NORMAL_EDGE = 0 };
  
public:
  CallGraphStandard (const SymHandle name);
  CallGraphStandard (); 
  virtual ~CallGraphStandard ();
  
  //-------------------------------------
  // CallGraph information access
  //-------------------------------------

  //--------------------------------------------------------
  
  class NodeCallsIterator;
  //--------------------------------------------------------
  class Node : public virtual Interface::Node,
               public DGraph::DGraphStandard::Node {
  public:
    Node () : DGraph::DGraphStandard::Node(), mSym(0), mProc(0) { Ctor(); }
    Node (SymHandle s) : DGraph::DGraphStandard::Node(), mSym(s), mProc(0) 
      { Ctor(); }
    ~Node () { }
    
    //! getId: An id unique within instances of CallGraph::Node
    virtual unsigned int getId() const { return mId; }
    //! resetIds: reset id numbering
    static void resetIds() { sNextId = 1; }

    bool isDefined() const { return (mProc.hval() != 0); }
    bool isCalled() const { return (mCalls.size() != 0); }
    
    ProcHandle getProc() const { return mProc; }
    SymHandle getProcSym() const { return mSym; }

    void dump(std::ostream& os) {} // for full override
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);
    //void shortdump(CallGraph* cgraph, std::ostream& os);
    void longdump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);
    
    friend class CallGraphStandard::NodeCallsIterator;

  public: // be careful
    void add_def(ProcHandle h) { mProc = h; }
    void add_call(ExprHandle h) { mCalls.push_back(h); }

  public:
    // Other Iterators
    OA_ptr<IncomingEdgesIterator> getIncomingEdgesIterator() const
      { OA_ptr<IncomingEdgesIterator> retval; 
        retval = new IncomingEdgesIterator(*this); 
        return retval;
      }

    OA_ptr<OutgoingEdgesIterator> getOutgoingEdgesIterator() const
      { OA_ptr<OutgoingEdgesIterator> retval; 
        retval = new OutgoingEdgesIterator(*this); 
        return retval;
      }

    OA_ptr<SourceNodesIterator> getSourceNodesIterator() const
      { OA_ptr<SourceNodesIterator> retval; 
        retval = new SourceNodesIterator(*this); 
        return retval;
      }

    OA_ptr<SinkNodesIterator> getSinkNodesIterator() const
      { OA_ptr<SinkNodesIterator> retval; 
        retval = new SinkNodesIterator(*this); 
        return retval;
      }

  protected:
    virtual OA_ptr<DGraph::Interface::IncomingEdgesIterator> 
      getDGraphInterfaceIncomingEdgesIterator() const
      { return getIncomingEdgesIterator(); }

    virtual OA_ptr<DGraph::Interface::OutgoingEdgesIterator> 
      getDGraphInterfaceOutgoingEdgesIterator() const
      { return getOutgoingEdgesIterator(); }

    virtual OA_ptr<DGraph::Interface::SourceNodesIterator> 
      getDGraphInterfaceSourceNodesIterator() const
      { return getSourceNodesIterator(); }

    virtual OA_ptr<DGraph::Interface::SinkNodesIterator> 
      getDGraphInterfaceSinkNodesIterator() const
      { return getSinkNodesIterator(); }

  private:
    void Ctor() { mId = sNextId++; }
    
    static unsigned int sNextId;

    SymHandle mSym;      // should be identical across def and uses
    ProcHandle mProc;             // where this function is defined (if any)
    std::list<ExprHandle> mCalls; // places this function is called (if any)
    unsigned int mId;            // 0 is reserved; first instance is 1
  };
  
  //--------------------------------------------------------
  class Edge : public virtual Interface::Edge,
               public DGraph::DGraphStandard::Edge {
  public:
    Edge (OA_ptr<Interface::Node> n1, OA_ptr<Interface::Node> n2, 
          Interface::EdgeType type, ExprHandle call);
    ~Edge () {}
    
    //! getId: An id unique within instances of CallGraph::Edge
    virtual unsigned int getId() const { return mId; }
    //! resetIds: reset id numbering
    static void resetIds() { sNextId = 1; }
    
    Interface::EdgeType getType() const { return mType; }

    //! Returns the handle for the callsite that induces this edge
    ExprHandle getCallHandle() const { return mCallsiteExpr; }
    //void setCallHandle(ExprHandle call) { mCallsiteExpr = call; }

    //! Returns source node for this edge
    OA_ptr<Node> source () const 
        { OA_ptr<Interface::Node> n = getCallGraphInterfaceNodeSource();
          return n.convert<Node>(); }
    
    //! Returns sink node for this edge
    OA_ptr<Node> sink () const 
        { OA_ptr<Interface::Node> n = getCallGraphInterfaceNodeSink();
          return n.convert<Node>(); }

    void dump (std::ostream& os);

  protected:
    OA_ptr<Interface::Node> getCallGraphInterfaceNodeSource() const 
        { OA_ptr<DGraph::Interface::Node> n = getDGraphInterfaceNodeSource();
          return n.convert<Interface::Node>(); }
    OA_ptr<Interface::Node> getCallGraphInterfaceNodeSink() const 
        { OA_ptr<DGraph::Interface::Node> n = getDGraphInterfaceNodeSink();
          return n.convert<Interface::Node>(); }


  private:
    static unsigned int sNextId;

    Interface::EdgeType mType;
    unsigned int mId; // 0 is reserved; first instance is 1
    ExprHandle mCallsiteExpr;
  };  
  //--------------------------------------------------------
  
  class NodeCallsIterator : public Iterator {
  public:
    NodeCallsIterator (OA_ptr<Interface::Node> node_) 
        { mNode = node_.convert<Node>(); 
          mIter = mNode->mCalls.begin(); }
    ~NodeCallsIterator () {}
    void operator ++ () { if (mIter != mNode->mCalls.end()) { ++mIter; } }
    bool isValid() const { return (mIter != mNode->mCalls.end()); }
    ExprHandle current() const { return *mIter; }
  private:
    OA_ptr<Node> mNode;
    std::list<ExprHandle>::iterator mIter;
  };
  
  const OA::SymHandle subprog_name () { return mName; }
  
  static void resetIds() {
    Node::resetIds();
    Edge::resetIds();
  }
  

public:
  class NodeLabelListIterator;
  friend class NodeLabelListIterator;

  void connect (OA_ptr<Interface::Node> src, OA_ptr<Interface::Node> dst, 
                Interface::EdgeType type, ExprHandle call);
  void disconnect (OA_ptr<Interface::Edge> e) 
    { removeEdge(e.convert<DGraph::Interface::Edge>()); }

  /*
  void build_graph(IRProcIterator* funcIter);
  void build_graph_from_block(Interface::Node* currProcNode, 
			                  IRRegionStmtIterator* stmtIter);
                              */

  OA_ptr<Node> findOrAddNode(SymHandle sym);

  typedef std::map<SymHandle, OA_ptr<Node> > SymToNodeMapType;

  void dump (std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);

  //-------------------------------------
  // Iterators
  // Have same code here as in DGraphStandard.hpp because
  // these methods are meant to shadow those in DGraphStandard.hpp
  // so that we get subclass specific return values
  //-------------------------------------
  OA_ptr<NodesIterator> getNodesIterator() const
  { OA_ptr<NodesIterator> retval; retval = new NodesIterator(*this);
    return retval;
  }

  OA_ptr<EdgesIterator> getEdgesIterator() const
  { OA_ptr<EdgesIterator> retval; retval = new EdgesIterator(*this);
    return retval;
  }

  OA_ptr<DFSIterator> getDFSIterator()
    { OA_ptr<DFSIterator> retval; retval = new DFSIterator(*this);
      return retval;
    }

  OA_ptr<BFSIterator> getBFSIterator()
    { OA_ptr<BFSIterator> retval; retval = new BFSIterator(*this);
      return retval;
    }

  OA_ptr<ReversePostDFSIterator> 
      getReversePostDFSIterator(DGraph::DGraphEdgeDirection pOrient)
    { OA_ptr<ReversePostDFSIterator> retval; retval 
        = new ReversePostDFSIterator(*this,pOrient);
      return retval;
    }

  OA_ptr<ReversePostDFSIterator>
      getReversePostDFSIterator(OA_ptr<DGraph::Interface::Node> root, 
                                DGraph::DGraphEdgeDirection pOrient)
    { OA_ptr<ReversePostDFSIterator> retval; retval 
        = new ReversePostDFSIterator(*this,root.convert<Node>(),pOrient);
      return retval;
    }

  OA_ptr<PostDFSIterator> 
      getPostDFSIterator(DGraph::DGraphEdgeDirection pOrient)
    { OA_ptr<PostDFSIterator> retval; retval 
        = new PostDFSIterator(*this,pOrient);
      return retval;
    }

  OA_ptr<PostDFSIterator>
      getPostDFSIterator(OA_ptr<DGraph::Interface::Node> root, 
                                DGraph::DGraphEdgeDirection pOrient)
    { OA_ptr<PostDFSIterator> retval; retval 
        = new PostDFSIterator(*this,root.convert<Node>(),pOrient);
      return retval;
    }

protected:
  OA_ptr<DGraph::Interface::NodesIterator> getDGraphInterfaceNodesIterator() const
    { return getNodesIterator(); }
  OA_ptr<DGraph::Interface::EdgesIterator> getDGraphInterfaceEdgesIterator() const
    { return getEdgesIterator(); }
  OA_ptr<DGraph::Interface::DFSIterator> getDGraphInterfaceDFSIterator() 
    { return getDFSIterator(); }
  OA_ptr<DGraph::Interface::BFSIterator> getDGraphInterfaceBFSIterator()
    { return getBFSIterator(); }
  OA_ptr<DGraph::Interface::ReversePostDFSIterator> 
      getDGraphInterfaceReversePostDFSIterator(
              DGraph::DGraphEdgeDirection pOrient) 
     { return getReversePostDFSIterator(pOrient); }
  OA_ptr<DGraph::Interface::ReversePostDFSIterator> 
      getDGraphInterfaceReversePostDFSIterator(
              OA_ptr<DGraph::Interface::Node> root, 
              DGraph::DGraphEdgeDirection pOrient)
     { return getReversePostDFSIterator(root, pOrient); }
  OA_ptr<DGraph::Interface::PostDFSIterator> 
      getDGraphInterfacePostDFSIterator(
              DGraph::DGraphEdgeDirection pOrient) 
     { return getPostDFSIterator(pOrient); }
  OA_ptr<DGraph::Interface::PostDFSIterator> 
      getDGraphInterfacePostDFSIterator(
              OA_ptr<DGraph::Interface::Node> root, 
              DGraph::DGraphEdgeDirection pOrient)
     { return getPostDFSIterator(root, pOrient); }

public:
  //------------------------------------------------------------------
  /*! The NodesIterator is just an extension of DGraphStandard::NodesIterator 
      and Interface::NodesIterator
      to provide access to CallGraph::CallGraphStandard nodes. 
  */
  class NodesIterator : public virtual Interface::NodesIterator,
                        public DGraph::DGraphStandard::NodesIterator {
  public:
    NodesIterator (const CallGraphStandard& g) : DGraph::DGraphStandard::NodesIterator(g) {}
    NodesIterator () {} 
    ~NodesIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () { DGraph::DGraphStandard::NodesIterator::operator++(); }
    bool isValid() const 
      { return DGraph::DGraphStandard::NodesIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::NodesIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { return DGraph::DGraphStandard::NodesIterator::getDGraphInterfaceNodesIteratorCurrent(); }
  };

  //------------------------------------------------------------------
  class ReversePostDFSIterator 
      : public virtual Interface::ReversePostDFSIterator,
        public DGraph::DGraphStandard::ReversePostDFSIterator 
  {
  public:
    ReversePostDFSIterator (CallGraphStandard& g, 
                            DGraph::DGraphEdgeDirection pOrient)
        : DGraph::DGraphStandard::ReversePostDFSIterator(g,pOrient) {}
    ReversePostDFSIterator (CallGraphStandard& g, 
                            OA_ptr<Node> root, 
                            DGraph::DGraphEdgeDirection pOrient)
        : DGraph::DGraphStandard::ReversePostDFSIterator(g,root.convert<Node>(),pOrient) {}
    //NodesIterator () {} 
    ~ReversePostDFSIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () 
      { DGraph::DGraphStandard::ReversePostDFSIterator::operator++(); }
    bool isValid() const 
      { return DGraph::DGraphStandard::ReversePostDFSIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::ReversePostDFSIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceReverseNodesIteratorCurrent() const 
    { return DGraph::DGraphStandard::ReversePostDFSIterator::getDGraphInterfaceNodesIteratorCurrent(); }
  };


  //------------------------------------------------------------------
  class PostDFSIterator 
      : public virtual Interface::PostDFSIterator,
        public DGraph::DGraphStandard::PostDFSIterator 
  {
  public:
    PostDFSIterator (CallGraphStandard& g, 
                            DGraph::DGraphEdgeDirection pOrient)
        : DGraph::DGraphStandard::PostDFSIterator(g,pOrient) {}
    PostDFSIterator (CallGraphStandard& g, 
                            OA_ptr<Node> root, 
                            DGraph::DGraphEdgeDirection pOrient)
        : DGraph::DGraphStandard::PostDFSIterator(g,root.convert<Node>(),pOrient) {}
    //NodesIterator () {} 
    //PostDFSIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () 
      { DGraph::DGraphStandard::PostDFSIterator::operator++(); }
    bool isValid() const 
      { return DGraph::DGraphStandard::PostDFSIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::PostDFSIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceReverseNodesIteratorCurrent() const 
    { return DGraph::DGraphStandard::PostDFSIterator::getDGraphInterfaceNodesIteratorCurrent(); }
  };

  //------------------------------------------------------------------
  class DFSIterator 
      : public virtual Interface::DFSIterator,
        public DGraph::DGraphStandard::DFSIterator 
  {
  public:
    DFSIterator (CallGraphStandard& g) 
        : DGraph::DGraphStandard::DFSIterator(g) {}
    //NodesIterator () {} 
    ~DFSIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () 
      { DGraph::DGraphStandard::DFSIterator::operator++(); }
    bool isValid() const 
      { return DGraph::DGraphStandard::DFSIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::DFSIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { return DGraph::DGraphStandard::DFSIterator::getDGraphInterfaceNodesIteratorCurrent(); }
  };
   
  //------------------------------------------------------------------
  class BFSIterator 
      : public virtual Interface::BFSIterator,
        public DGraph::DGraphStandard::BFSIterator 
  {
  public:
    BFSIterator (CallGraphStandard& g) 
        : DGraph::DGraphStandard::BFSIterator(g) {}
    //NodesIterator () {} 
    ~BFSIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () 
      { DGraph::DGraphStandard::BFSIterator::operator++(); }
    bool isValid() const 
      { return DGraph::DGraphStandard::BFSIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::BFSIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { return DGraph::DGraphStandard::BFSIterator::getDGraphInterfaceNodesIteratorCurrent(); }
  };
  
  //------------------------------------------------------------------
  class EdgesIterator 
      : public virtual Interface::EdgesIterator,
        public DGraph::DGraphStandard::EdgesIterator 
  {
  public:
    EdgesIterator (const CallGraphStandard& g) 
        : DGraph::DGraphStandard::EdgesIterator(g) {}
    //NodesIterator () {} 
    ~EdgesIterator () {}
    OA_ptr<Edge> current() const
      { OA_ptr<DGraph::Interface::Edge> n 
          = getDGraphInterfaceEdgesIteratorCurrent();
        return n.convert<Edge>(); }
    void operator++ () 
      { DGraph::DGraphStandard::EdgesIterator::operator++(); }
    bool isValid() const 
      { return DGraph::DGraphStandard::EdgesIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::EdgesIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Edge> 
        getDGraphInterfaceEdgesIteratorCurrent() const 
    { return DGraph::DGraphStandard::EdgesIterator::getDGraphInterfaceEdgesIteratorCurrent(); }
  };
   
  //------------------------------------------------------------------
  class IncomingEdgesIterator 
      : public virtual Interface::IncomingEdgesIterator,
        public DGraph::DGraphStandard::IncomingEdgesIterator 
  {
  public:
    IncomingEdgesIterator (const Node& g) 
        : DGraph::DGraphStandard::IncomingEdgesIterator(g) {}
    ~IncomingEdgesIterator () {}
    OA_ptr<Edge> current() const
      { OA_ptr<DGraph::Interface::Edge> n 
          = getDGraphInterfaceEdgesIteratorCurrent();
        return n.convert<Edge>(); }
    void operator++ () 
      { DGraph::DGraphStandard::IncomingEdgesIterator::operator++(); }
    bool isValid() const 
      { return DGraph::DGraphStandard::IncomingEdgesIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::IncomingEdgesIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Edge> 
        getDGraphInterfaceEdgesIteratorCurrent() const 
    { return DGraph::DGraphStandard::IncomingEdgesIterator::getDGraphInterfaceEdgesIteratorCurrent(); }
  };
  
  //------------------------------------------------------------------
  class OutgoingEdgesIterator 
      : public virtual Interface::OutgoingEdgesIterator,
        public DGraph::DGraphStandard::OutgoingEdgesIterator 
  {
  public:
    OutgoingEdgesIterator (const Node& g) 
        : DGraph::DGraphStandard::OutgoingEdgesIterator(g) {}
    ~OutgoingEdgesIterator () {}
    OA_ptr<Edge> current() const
      { OA_ptr<DGraph::Interface::Edge> n 
          = getDGraphInterfaceEdgesIteratorCurrent();
        return n.convert<Edge>(); }
    void operator++ () 
      { DGraph::DGraphStandard::OutgoingEdgesIterator::operator++(); }
    bool isValid() const 
      { return DGraph::DGraphStandard::OutgoingEdgesIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::OutgoingEdgesIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Edge> 
        getDGraphInterfaceEdgesIteratorCurrent() const 
    { return DGraph::DGraphStandard::OutgoingEdgesIterator::getDGraphInterfaceEdgesIteratorCurrent(); }
  };

  //------------------------------------------------------------------
  /*! Iterator to enumerate all the source nodes for a particular Node. 

      NOTE:  Inherits from NodesIterator so that one can dynamically
             select between a SourceNodesIterator or a SinkNodesIterator
             and still be able to use the current method.
   */
  class SourceNodesIterator : public virtual Interface::SourceNodesIterator,
                              public DGraph::DGraphStandard::SourceNodesIterator
  {
  public:
    SourceNodesIterator (const Node& n) 
        : DGraph::DGraphStandard::SourceNodesIterator(n) {}
    ~SourceNodesIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () 
      { DGraph::DGraphStandard::SourceNodesIterator::operator++(); }
    bool isValid() const 
      { return DGraph::DGraphStandard::SourceNodesIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::SourceNodesIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { return DGraph::DGraphStandard::SourceNodesIterator::getDGraphInterfaceNodesIteratorCurrent(); }
   };
  //------------------------------------------------------------------
  /*! Iterator to enumerate all the sink nodes. */
  class SinkNodesIterator : public virtual Interface::SinkNodesIterator,
                            public DGraph::DGraphStandard::SinkNodesIterator
  {
  public:
    SinkNodesIterator (const Node& n) 
        : DGraph::DGraphStandard::SinkNodesIterator(n) {}
    ~SinkNodesIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () 
      { DGraph::DGraphStandard::SinkNodesIterator::operator++(); }
    bool isValid() const 
      { return DGraph::DGraphStandard::SinkNodesIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::SinkNodesIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { return DGraph::DGraphStandard::SinkNodesIterator::getDGraphInterfaceNodesIteratorCurrent(); }
   };
 



private:
  const SymHandle mName;
  
  SymToNodeMapType mSymToNodeMap;
  
};
//--------------------------------------------------------------------

  } // end of CallGraph namespace
} // end of OA namespace

#endif
