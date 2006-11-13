/*! \file
  
  \brief Abstract interface that all ICFG analysis results must satisfy.

  \authors Michelle Strout
  \version $Id: ICFGInterface.hpp,v 1.1 2005/06/21 15:20:55 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

FIXME: Not actually being used yet and needs fixed

*/

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#ifndef ICFGInterface_H
#define ICFGInterface_H

#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/Utils/DGraph/Interface.hpp>

//--------------------------------------------------------------------

namespace OA {
  namespace ICFG {
      
// Changes here must be also reflected in edgeTypeToString 
// and nodeTypeToString.
enum EdgeType { CFLOW_EDGE, CALL_EDGE, RETURN_EDGE };
enum NodeType { CFLOW_NODE, CALL_NODE, RETURN_NODE, ENTRY_NODE, EXIT_NODE };
 
//--------------------------------------------------------------------
/*! ICFG implements the DGraph::Interface (directed graph) but has
    enhanced nodes and edges.  Each node corresponds to a basic block in one of
    the procedures and contains a list of statments and each edge is either a
    control flow edge, call edge, or return edge.
*/
class ICFGInterface : public virtual DGraph::Interface {
public:
  class Node;
  class Edge;

  class NodesIterator;
  typedef NodesIterator EntryNodesIterator;
  typedef NodesIterator ExitNodesIterator;
  typedef NodesIterator SourceNodesIterator;
  typedef NodesIterator SinkNodesIterator;
  typedef NodesIterator DFSIterator;
  typedef NodesIterator BFSIterator;
  typedef NodesIterator ReversePostDFSIterator;
  typedef NodesIterator PostDFSIterator;

  class EdgesIterator;
  typedef EdgesIterator IncomingEdgesIterator;
  typedef EdgesIterator OutgoingEdgesIterator;

 
  //--------------------------------------------------------
  class Node : public virtual DGraph::Interface::Node {
  public:
    Node () {}
    ~Node () { }
    
    //========================================================
    // Info specific to ICFG
    //========================================================
    
    virtual NodeType getType() const = 0;

    virtual ProcHandle getProc() const = 0;
    
    //! number of statements in node
    virtual unsigned int size () const = 0;

    //! create a forward order iterator for the statements in the node
    virtual OA_ptr<CFG::Interface::NodeStatementsIterator> 
        getNodeStatementsIterator() const = 0;

    //! create a reverse order iterator for the statements in the node
    virtual OA_ptr<CFG::Interface::NodeStatementsRevIterator> 
        getNodeStatementsRevIterator() const = 0;

    //========================================================
    // Output
    //========================================================
//    virtual void dump(std::ostream& os) =  0;
//    virtual void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir) = 0;
//    virtual void dumpdot(ICFGStandard&, 
//                 std::ostream& os, OA_ptr<IRHandlesIRInterface> ir) = 0;
//    void longdump(ICFGStandard& icfg, std::ostream& os, 
//                  OA_ptr<IRHandlesIRInterface> ir) = 0;
    
  public:
    //========================================================
    // These methods shadow the same named methods in
    // the DGraph::Interface class and allow us to return
    // the more specific subclass
    //========================================================
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

    //------------------------------------------------------------------
  };
  
  //--------------------------------------------------------
  class Edge : public virtual DGraph::Interface::Edge {
  public:
    Edge () {}
    ~Edge () {}
    
    //========================================================
    // Info specific to ICFG
    //========================================================
    
    virtual EdgeType getType() const = 0;

    virtual ProcHandle getSourceProc() const = 0;
    virtual ProcHandle getSinkProc() const = 0;
    virtual ExprHandle getCall() const = 0;

  }; 
  
 
  //------------------------------------------------------------------
  /*! An iterator over ICFG::Nodes that satisfies the interface
      for all different iterator types.
  */
  class NodesIterator : 
                      public virtual DGraph::Interface::EntryNodesIterator,
                      public virtual DGraph::Interface::ExitNodesIterator,
                      public virtual DGraph::Interface::DFSIterator,
                      public virtual DGraph::Interface::BFSIterator,
                      public virtual DGraph::Interface::ReversePostDFSIterator,
                      public virtual DGraph::Interface::PostDFSIterator,
                      public virtual DGraph::Interface::SourceNodesIterator,
                      public virtual DGraph::Interface::SinkNodesIterator
  {
  public:
    NodesIterator () {}
    ~NodesIterator () {}
    OA_ptr<Node> current() const { 
      OA_ptr<DGraph::Interface::Node> n =
        getDGraphInterfaceNodesIteratorCurrent();
      return n.convert<Node>(); 
    }
  };
  
  //------------------------------------------------------------------
  /*! An iterator over ICFG::Edges that satisfies the interface
      for all different iterator types.
  */
  class EdgesIterator : public virtual DGraph::Interface::EdgesIterator,
                      public virtual DGraph::Interface::IncomingEdgesIterator,
                      public virtual DGraph::Interface::OutgoingEdgesIterator
  {
  public:
    EdgesIterator () {}
    ~EdgesIterator () {}
    OA_ptr<Edge> current() const { 
      OA_ptr<DGraph::Interface::Edge> e = 
        getDGraphInterfaceEdgesIteratorCurrent();
      return e.convert<Edge>(); 
    }
  };
  

//------------------------------------------------------------------
public:
  ICFGInterface (); 
  virtual ~ICFGInterface ();
  
  //void dump (std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);
  //void dumpdot (std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);
  //void dumpbase(std::ostream& os) {}
  
  //------------------------------------------------------------------
  // Using trick of imitating covariance when getting all iterators
  // so that get an iterator specific to actual subclass
  // This is why have these getBlahIterator methods that shadow those
  // in DGraph::Interface and also have the protected ones
  // that must be defined here which override implementation in 
  // DGraph::Interface
  //------------------------------------------------------------------
  OA_ptr<NodesIterator> getNodesIterator() const { 
    OA_ptr<DGraph::Interface::NodesIterator> it =
      getDGraphInterfaceNodesIterator();
    return it.convert<NodesIterator>(); 
  }
  
  virtual OA_ptr<EntryNodesIterator> getEntryNodesIterator() const = 0;

  virtual OA_ptr<ExitNodesIterator> getExitNodesIterator() const = 0;

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
  
};
//--------------------------------------------------------------------

  } // end of ICFG namespace
} // end of OA namespace

#endif
