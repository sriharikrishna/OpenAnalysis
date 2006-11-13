/*! \file
  
  \brief Abstract interface that all CFG analysis results must satisfy.

  \authors Arun Chauhan (2001 as part of Mint), Nathan Tallent, Michelle Strout
  \version $Id: Interface.hpp,v 1.28 2005/06/21 15:20:54 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef CFGINTERFACE_H
#define CFGINTERFACE_H

#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/Utils/DGraph/Interface.hpp>

namespace OA {
  namespace CFG {


//--------------------------------------------------------------------
/*! This abstract Interface to any CFG assumes a directed graph with
    nodes and edges.  The nodes contain an ordered list of statements.
    The entire program would be represented by a set of CFGs, one for
    each subroutine, and one for the main program.
*/
//--------------------------------------------------------------------
class Interface : public virtual DGraph::Interface {
public:
  enum EdgeType { TRUE_EDGE = 0, FALLTHROUGH_EDGE, FALSE_EDGE,
		  BACK_EDGE, MULTIWAY_EDGE, BREAK_EDGE,
		  CONTINUE_EDGE, RETURN_EDGE };

  //------------------------------------------------------------------
  typedef StmtHandleIterator NodeStatementsIterator;
  typedef StmtHandleIterator NodeStatementsRevIterator;

  class IncomingEdgesIterator;
  class OutgoingEdgesIterator;
  class SourceNodesIterator;
  typedef SourceNodesIterator PredNodesIterator;
  class SinkNodesIterator;
  typedef SinkNodesIterator SuccNodesIterator;
  class NodesIterator;
  class EdgesIterator;
  class DFSIterator;
  class BFSIterator;
  class ReversePostDFSIterator;
  class PostDFSIterator;


  //------------------------------------------------------------------
  /*! Node in the CFG must satisfy the following interface.
  */
  //------------------------------------------------------------------
  class Node : public virtual DGraph::Interface::Node {
  public:
    Node () {}
    
    //! create a node with a statement
    Node (OA::StmtHandle n)  {}
    //virtual ~Node () = 0;
    virtual ~Node () {} 
    
    //! get an id unique within instances of CFG::Node
    virtual unsigned int getId() const = 0;
    
    //! number of statements in node
    virtual unsigned int size () const = 0;

    //! create a forward order iterator for the statements in the node
    virtual OA_ptr<NodeStatementsIterator> getNodeStatementsIterator() const = 0;

    //! create a reverse order iterator for the statements in the node
    virtual OA_ptr<NodeStatementsRevIterator> 
        getNodeStatementsRevIterator() const = 0;

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

    //! Get iterator over predecessor nodes
    OA_ptr<PredNodesIterator> getPredNodesIterator() const { 
      OA_ptr<DGraph::Interface::SourceNodesIterator> it =
        getDGraphInterfaceSourceNodesIterator();
      return it.convert<SourceNodesIterator>(); 
    }


    OA_ptr<SinkNodesIterator> getSinkNodesIterator() const { 
      OA_ptr<DGraph::Interface::SinkNodesIterator> it =
        getDGraphInterfaceSinkNodesIterator();
      return it.convert<SinkNodesIterator>(); 
    }

    //! Get iterator over successor nodes
    OA_ptr<SuccNodesIterator> getSuccNodesIterator() const { 
      OA_ptr<DGraph::Interface::SinkNodesIterator> it =
        getDGraphInterfaceSinkNodesIterator();
      return it.convert<SinkNodesIterator>(); 
    }


    //???
    //ExprHandle getEndExpr () const { return end; }
    
    //??? what will the outputBuffer stuff look like, part
    //???  of annotation interface?
    //void dump (std::ostream& os) { }
    //void longdump (CFG *, std::ostream& os);
    //void longdump (CFG * _cfg) { longdump(_cfg, std::cout); } 
    
  };
  
  //------------------------------------------------------------------
  /*! Edge in the CFG must satisfy the following interface.
  */
  //------------------------------------------------------------------
  class Edge : public virtual DGraph::Interface::Edge {
  public:
    Edge () {}
    virtual ~Edge () {}
    
    //! Return source node for this edge.
    //virtual OA_ptr<Node> getSource () const = 0;
   // virtual OA_ptr<Node> source () const = 0;

    //! Return source node for this edge.    
    //! This method must have a default implementation because
    //! covariant returns are not implemented in many compilers.    
    //! Classes that inherit from CFG::Interface will shadow
    //! this method and implement the private polymorphic method.
    OA_ptr<Node> source () const { return getCFGInterfaceNodeSource(); }
    
    //! Return sink node for this edge.    
    //! This method must have a default implementation because
    //! covariant returns are not implemented in many compilers.    
    //! Classes that inherit from CFG::Interface will shadow
    //! this method and implement the private polymorphic method.
    OA_ptr<Node> sink () const { return getCFGInterfaceNodeSink(); }
    
  protected:
    virtual OA_ptr<Node> getCFGInterfaceNodeSource() const = 0;
    virtual OA_ptr<Node> getCFGInterfaceNodeSink() const = 0;
    
    //! Return sink node for this edge
    //virtual OA_ptr<Node> getSink () const = 0;

  public:
    // get an id unique within instances of Interface::Edge
    virtual unsigned int getId() const = 0;

    //??? resetIds: reset id numbering
    //static void resetIds() { nextId = 1; }

    //! get type associated with edge: TRUE_EDGE, FALLTHROUGH_EDGE, etc.
    virtual EdgeType getType() const = 0;
    
    //! get expression associated with edge, specifies branching val
    virtual ExprHandle getExpr() const = 0;

    //???
    virtual void dump (std::ostream& os) = 0;
    //void dump () { dump(std::cout); }

  };  
 
public:
  Interface() {}
  virtual ~Interface () {}

  //-------------------------------------
  // CFG information access
  //-------------------------------------
  
  //! pointer to entry node
  virtual OA_ptr<Node> getEntry() const = 0;
  
  //! pointer to entry node
  virtual OA_ptr<Node> getExit() const = 0;

  //! get SymHandle for ProcHandle (i.e., function name)
  virtual SymHandle getName () const = 0; 
    
  //! Will eventually use an OutputBuilder, for now
  //! just uses Beatta's dump code, node and edge should have
  //! an output call as well, but don't want to have
  //! an OutputBuilder associated to each one so pass in CFG's
  //! OutputBuilder, but what if someone has just a CFG::Node?
  //void output() const = 0;
  //void configOutput(OutputBuilder& ob) { mOB = ob; }

  virtual void dump(std::ostream&, OA::OA_ptr<IRHandlesIRInterface>) = 0;


  //---------------------------------------------------------
  // CFG iterators
  //---------------------------------------------------------
  OA_ptr<NodesIterator> getNodesIterator() const { 
    OA_ptr<DGraph::Interface::NodesIterator> it =
      getDGraphInterfaceNodesIterator();
    return it.convert<NodesIterator>(); 
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
      getDGraphInterfaceReversePostDFSIterator(root, pOrient);
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
      getDGraphInterfacePostDFSIterator(root, pOrient);
    return it.convert<PostDFSIterator>(); 
  }



  //------------------------------------------------------------------
  /*! The NodesIterator is just and extension of DGraph::NodesIterator 
      to provide access to CFG::Interface nodes. 
  */
  class NodesIterator : public virtual DGraph::Interface::NodesIterator {
  public:
    NodesIterator (const Interface& g) : DGraph::Interface::NodesIterator(g) {}
    NodesIterator () {} 
    virtual ~NodesIterator () {}
    OA_ptr<Node> current() const { 
      OA_ptr<DGraph::Interface::Node> n =
        getDGraphInterfaceNodesIteratorCurrent();
      return n.convert<Node>(); 
    }
  };
  
  //------------------------------------------------------------------
  class ReversePostDFSIterator 
      : public virtual DGraph::Interface::ReversePostDFSIterator,
        public virtual NodesIterator
  {
  public:
    ReversePostDFSIterator (Interface& g) {}
    ReversePostDFSIterator () {}
    virtual ~ReversePostDFSIterator () {}
    OA_ptr<Node> current() const { 
      OA_ptr<DGraph::Interface::Node> n =
	getDGraphInterfaceNodesIteratorCurrent();
      return n.convert<Node>(); 
    }
  };

  //------------------------------------------------------------------
  class PostDFSIterator 
      : public virtual DGraph::Interface::PostDFSIterator,
        public virtual NodesIterator
  {
  public:
    PostDFSIterator (Interface& g) {}
    PostDFSIterator () {}
    virtual ~PostDFSIterator () {}
    OA_ptr<Node> current() const { 
      OA_ptr<DGraph::Interface::Node> n =
	getDGraphInterfaceNodesIteratorCurrent();
      return n.convert<Node>(); 
    }
  };
  
  //------------------------------------------------------------------
  class DFSIterator 
      : public virtual DGraph::Interface::DFSIterator,
        public virtual NodesIterator
  {
  public:
    DFSIterator (Interface& g) {}
    DFSIterator () {}
    virtual ~DFSIterator () {}
    OA_ptr<Node> current() const { 
      OA_ptr<DGraph::Interface::Node> n =
	getDGraphInterfaceNodesIteratorCurrent();
      return n.convert<Node>(); 
    }
  };
  
  //------------------------------------------------------------------
  class BFSIterator 
      : public virtual DGraph::Interface::BFSIterator,
        public virtual NodesIterator
  {
  public:
    BFSIterator (Interface& g) {}
    BFSIterator () {}
    virtual ~BFSIterator () {}
    OA_ptr<Node> current() const { 
      OA_ptr<DGraph::Interface::Node> n =
	getDGraphInterfaceNodesIteratorCurrent();
      return n.convert<Node>(); 
    }
  };

  //------------------------------------------------------------------
  class EdgesIterator : public virtual DGraph::Interface::EdgesIterator {
  public:
    EdgesIterator (const Interface& g) {}
    EdgesIterator () {} 
    virtual ~EdgesIterator () {}
    OA_ptr<Edge> current() const { 
      OA_ptr<DGraph::Interface::Edge> e = 
        getDGraphInterfaceEdgesIteratorCurrent();
      return e.convert<Edge>(); 
    }
  };
  
  //------------------------------------------------------------------
  class IncomingEdgesIterator 
      : public virtual DGraph::Interface::IncomingEdgesIterator,
        public virtual EdgesIterator
  {
  public:
    IncomingEdgesIterator (const Node& n) {}
    IncomingEdgesIterator () {}
    virtual ~IncomingEdgesIterator () {}
    OA_ptr<Edge> current() const { 
      OA_ptr<DGraph::Interface::Edge> e = 
	getDGraphInterfaceEdgesIteratorCurrent();
      return e.convert<Edge>();
    }
  };
  
  //------------------------------------------------------------------
  class OutgoingEdgesIterator 
      : public virtual DGraph::Interface::OutgoingEdgesIterator,
        public virtual EdgesIterator
  {
  public:
    OutgoingEdgesIterator (const Node& n) {} 
    OutgoingEdgesIterator () {} 
    virtual ~OutgoingEdgesIterator () {}
    OA_ptr<Edge> current() const { 
      OA_ptr<DGraph::Interface::Edge> e = 
	getDGraphInterfaceEdgesIteratorCurrent();
      return e.convert<Edge>(); 
    }
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
  /*
  class NodeList;
  class NodeListIterator {
  public:
    NodeListIterator (NodeList* nl) {}
    virtual ~NodeListIterator () {}
    virtual void operator ++ () = 0;

    //! is the iterator at the end
    virtual bool isValid() const = 0;

    //! get a pointer to the current node
    virtual OA_ptr<Node> current() = 0;
  };
  */
  
  
  //--------------------------------------------------------------------
  /*! An iterator that enumerates the nodes in the CFG in depth first order.
  */
  //--------------------------------------------------------------------
  /*
  class DFSIterator {
  public:
    DFSIterator (CFG::Interface  &g) {}
    virtual ~DFSIterator () {}
    virtual void operator++ () = 0;
    
    //! is the iterator at the end
    virtual operator bool () const = 0;
    
    //! return current node
    virtual operator Node* () const = 0;

  };
 */ 
  
  //--------------------------------------------------------------------
  /*! An iterator for the statements in a node.
  */
  //--------------------------------------------------------------------
  /*
  class NodeStatementsIterator {
  public:
    NodeStatementsIterator () {}
    virtual ~NodeStatementsIterator () {}
    virtual void operator ++ () = 0;
    
    //! is the iterator at the end
    virtual bool isValid() const  = 0;
    
    //! return current node
    virtual StmtHandle current() = 0;
  };
  */
   
  //--------------------------------------------------------------------
  /*! An iterator that iterates of the statements in the node in
   *  reverse order.
  */
  //--------------------------------------------------------------------
  /*
  class NodeStatementsRevIterator {
  public:
    NodeStatementsRevIterator () {}
    virtual ~NodeStatementsRevIterator () {}
    virtual void operator ++ () = 0;
    
    //! is the iterator at the end
    virtual bool isValid() const  = 0;
    
    //! return current node
    virtual StmtHandle current() = 0;
  };
  */

};

  } // end of CFG namespace
} // end of OA namespace

#endif

