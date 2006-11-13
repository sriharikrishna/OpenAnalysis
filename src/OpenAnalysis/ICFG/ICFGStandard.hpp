/*! \file
  
  \brief Declaration for standard ICFG (interprocedural control flow graph).

  \authors Michelle Strout
  \version $Id: ICFGStandard.hpp,v 1.3 2005/07/25 16:19:30 utke Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#ifndef ICFGStandard_H
#define ICFGStandard_H

//--------------------------------------------------------------------
// STL headers
#include <list>
#include <map>

// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/Utils/DGraph/DGraphStandard.hpp>
#include <OpenAnalysis/Utils/DGraph/Interface.hpp>
#include <OpenAnalysis/CFG/CFGStandard.hpp>
//#include "Interface.hpp"

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
class ICFGStandard : public virtual DGraph::Interface {
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
    Node (OA_ptr<ICFGStandard> pICFG, ProcHandle proc, NodeType pType) 
        : mICFG(pICFG), mProc(proc), mType(pType) { Ctor(); }
    Node (OA_ptr<ICFGStandard> pICFG, ProcHandle proc, NodeType pType, 
          OA_ptr<CFG::CFGStandard::Node> cNode) 
        : mICFG(pICFG), mProc(proc), mType(pType), mCFGNode(cNode) { Ctor(); }

    ~Node () { }
    
    //========================================================
    // Info specific to ICFG
    //========================================================
    
    NodeType getType() const { return mType; }

    ProcHandle getProc() const { return mProc; }
    
    //! number of statements in node
    unsigned int size () const;

    //! create a forward order iterator for the statements in the node
    OA_ptr<CFG::Interface::NodeStatementsIterator> 
        getNodeStatementsIterator() const;

    //! create a reverse order iterator for the statements in the node
    OA_ptr<CFG::Interface::NodeStatementsRevIterator> 
        getNodeStatementsRevIterator() const;

    //========================================================
    // Construction Methods
    //========================================================

    void addCallEdge(OA_ptr<Edge> e) { mCallEdges->push_back(e); }
    void addReturnEdge(OA_ptr<Edge> e) { mReturnEdges->push_back(e); }
    
    //========================================================
    // DGraph::Interface::Node interface
    //========================================================
    
    //! getId: An id unique within instances of ICFG::Node
    unsigned int getId() const { return mId; }
    
    //! number of incoming edges
    int num_incoming () const { return mDGNode->num_incoming(); }
    
    //! number of outgoing edges
    int num_outgoing () const { return mDGNode->num_outgoing(); }

    //! returns true if node is an entry node, IOW  has no incoming edges
    bool isAnEntry() const { return mDGNode->isAnEntry(); }

    //! returns true if node is an exit node, IOW  has no outgoing edges
    bool isAnExit() const { return mDGNode->isAnExit(); }

    bool operator==(DGraph::Interface::Node& other);
    bool operator<(DGraph::Interface::Node& other);
    
    // useful for constructing iterators 
    void setVisitFlag() { mDGNode->setVisitFlag(); }
    void clearVisitFlag() { mDGNode->clearVisitFlag(); }
    bool getVisitFlag() const { return mDGNode->getVisitFlag(); }


    //========================================================
    // Output
    //========================================================
    void dump(std::ostream& os) {} // for full override
    void dumpbase(std::ostream& os) {}
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);
    void dumpdot(ICFGStandard&, 
                 std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);
    void longdump(ICFGStandard& icfg, std::ostream& os, 
                  OA_ptr<IRHandlesIRInterface> ir);
    
  public:
    //========================================================
    // These methods shadow the same named methods in
    // the DGraph::Interface class and allow us to return
    // the more specific subclass
    //========================================================
    OA_ptr<IncomingEdgesIterator> getIncomingEdgesIterator() const;

    OA_ptr<OutgoingEdgesIterator> getOutgoingEdgesIterator() const;

    OA_ptr<SourceNodesIterator> getSourceNodesIterator() const;

    OA_ptr<SinkNodesIterator> getSinkNodesIterator() const;

  protected:
    //========================================================
    // These are the virtual methods in the the DGraph::Interface 
    //========================================================
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

    //------------------------------------------------------------------
  

  private:
     void Ctor();
   
    // if we are a ENTRY_Node and have CallEdges then we
    // are the entry to a CFG and the CallEdges are incoming
    // if we are a EXIT_Node and have ReturnEdges then 
    // we are the exit to a CFG and the return edges are outgoing
    // if we are a Call node then the call edges are outgoing
    // if we are a return node then the call edges are incoming
    OA_ptr<std::list<OA_ptr<Edge> > > mCallEdges;
    OA_ptr<std::list<OA_ptr<Edge> > > mReturnEdges;

    static unsigned int sNextId;

    unsigned int mId;               // 0 is reserved; first instance is 1
    OA_ptr<ICFGStandard> mICFG;     // enclosing ICFG
    ProcHandle mProc;               // enclosing procedure
    NodeType mType;                 // node type
    OA_ptr<CFG::CFGStandard::Node> mCFGNode;        // corresponding CFG node
    OA_ptr<DGraph::DGraphStandard::Node> mDGNode;  // corresponding DGraph node
                                                   // in DGraph that keeps 
                                                   // structure

    friend class ICFGStandard;
    friend class ICFGStandard::Edge;
  };
  
  // lt_Node: function object that compares by id.  Useful for sorting.
  class lt_Node : public DGraph::Interface::lt_Node {
  public:
    // return true if n1 < n2; false otherwise
    bool operator()(const OA_ptr<DGraph::Interface::Node> n1, 
                    const OA_ptr<DGraph::Interface::Node> n2) const {
      return (n1->getId() < n2->getId());
    }
  };
  
  //! returns a node compare object
  OA::OA_ptr<DGraph::Interface::lt_Node> getNodeCompare() const 
  { OA::OA_ptr<DGraph::Interface::lt_Node> retval; retval= new lt_Node();  
    return retval; 
  }
 
 
  //--------------------------------------------------------
  class Edge : public virtual DGraph::Interface::Edge {
  public:
    Edge (OA_ptr<ICFGStandard> pICFG,
          OA_ptr<Node> pNode1, OA_ptr<Node> pNode2, EdgeType pType,
          ExprHandle call); 
    Edge (OA_ptr<ICFGStandard> pICFG,
          OA_ptr<Node> pNode1, OA_ptr<Node> pNode2, EdgeType pType);
    ~Edge () {}
    
    //========================================================
    // Info specific to ICFG
    //========================================================
    
    EdgeType getType() const { return mType; }

    ProcHandle getSourceProc() const { return mNode1->getProc(); }
    ProcHandle getSinkProc() const { return mNode2->getProc(); }
    ExprHandle getCall() const { return mCall; }

    //========================================================
    // DGraph::Interface::Edge interface
    //========================================================
    
    //! getId: An id unique within instances of CallGraph::Edge
    unsigned int getId() const { return mId; }

    //! Returns source node for this edge
    OA_ptr<Node> source () const { return mNode1; }
    // FIXME: tail and head should be done like source and sink
    // in DGraph::Interface and here
    OA_ptr<DGraph::Interface::Node> tail () const { return mNode1; }
    
    //! Returns sink node for this edge
    OA_ptr<Node> sink () const { return mNode2; }
    OA_ptr<DGraph::Interface::Node> head () const { return mNode2; }

    bool operator==(DGraph::Interface::Edge& other);
    bool operator<(DGraph::Interface::Edge& other);

    void dump (std::ostream& os);
    void dumpdot (std::ostream& os);
    void dumpbase (std::ostream& os) {}
    
  protected:
    OA_ptr<DGraph::Interface::Node> getDGraphInterfaceNodeSource() const 
        { return mNode1; }
    OA_ptr<DGraph::Interface::Node> getDGraphInterfaceNodeSink() const 
        { return mNode2; }


  private:
    static unsigned int sNextId;

    OA_ptr<ICFGStandard> mICFG;
    OA_ptr<Node> mNode1, mNode2;
    OA_ptr<DGraph::DGraphStandard::Edge> mDGEdge;

    EdgeType mType;
    ExprHandle mCall;
    unsigned int mId; // 0 is reserved; first instance is 1

    friend class ICFGStandard;
    friend class ICFGStandard::Node;

  }; 
  
  // lt_Edge: function object that compares by source and sink node
  // ids.  Useful for sorting.  Not used to put edges in sets or other
  // STL containers.
  class lt_Edge : public DGraph::Interface::lt_Edge {
  public:
      bool operator()(const OA_ptr<DGraph::Interface::Edge> e1, 
                      const OA_ptr<DGraph::Interface::Edge> e2) const 
      {
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
  OA::OA_ptr<DGraph::Interface::lt_Edge> getEdgeCompare() const 
  { OA::OA_ptr<DGraph::Interface::lt_Edge> retval; retval= new lt_Edge();  
    return retval; 
  }



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
    NodesIterator (OA_ptr<std::list<OA_ptr<Node> > > pSet)
        : mSet(pSet), mIter(pSet->begin()) {}
    ~NodesIterator () {}
    OA_ptr<Node> current() const 
      { OA_ptr<Node> retval; if (isValid()) { retval = *mIter;} return retval; }
    void operator++ () { if (isValid()) { mIter++; } }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return mIter != mSet->end(); }
    void reset() { mIter = mSet->begin(); }

  protected:
    OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const { return current(); }
  private:
    OA_ptr<std::list<OA_ptr<Node> > > mSet;
    std::list<OA_ptr<Node> >::iterator mIter;
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
    EdgesIterator (OA_ptr<std::list<OA_ptr<Edge> > > pSet)
        : mSet(pSet), mIter(pSet->begin()) {}
    ~EdgesIterator () {}
    OA_ptr<Edge> current() const 
      { OA_ptr<Edge> retval; if (isValid()) { retval = *mIter;} return retval; }
    void operator++ () { if (isValid()) { mIter++; } }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return mIter != mSet->end(); }
    void reset() { mIter = mSet->begin(); }

  protected:
    OA_ptr<DGraph::Interface::Edge> 
        getDGraphInterfaceEdgesIteratorCurrent() const { return current(); }
  private:
    OA_ptr<std::list<OA_ptr<Edge> > > mSet;
    std::list<OA_ptr<Edge> >::iterator mIter;
  };
  

//------------------------------------------------------------------
public:
  ICFGStandard (); 
  virtual ~ICFGStandard ();
  
  //-------------------------------------
  // ICFG information access
  //-------------------------------------
  //OA_ptr<Node> getEntry() { return mEntry; }
  //OA_ptr<Node> getExit() { return mExit; }

  //! get ICFGEdge that is associated with given CFG edge in
  //! one of the CFGs we contain
  OA_ptr<Edge> getICFGEdge(OA_ptr<DGraph::DGraphStandard::Edge> dgEdge) const;

  //! get ICFGNode that is associated with given CFG node in
  //! one of the CFGs we contain
  OA_ptr<Node> getICFGNode(OA_ptr<DGraph::DGraphStandard::Node> dgNode) const;
  
  //========================================================
  // Construction
  //========================================================
  void addNode(OA_ptr<Node> pNode);
  void addEdge(OA_ptr<Edge> pEdge);

  // can't do connect by creating a new edge in a member function
  // because edges have to have an OA_ptr to containing ICFG
  //void connect(OA_ptr<Node> pNode1, OA_ptr<Node> pNode2,
  //             EdgeType pType);

  //========================================================
  // DGraph::Interface::Edge interface
  //========================================================
 
  int getNumNodes () { return mDGraph->getNumNodes(); }
  int getNumEdges () { return mDGraph->getNumEdges(); }

  //! get and set the source node or root in the graph
  //! FIXME: return Null if there is more than one source node
  //OA_ptr<DGraph::Interface::Node> getSource() { return mEntry; }
  //void setSource(OA_ptr<Node> n) { mEntry = n; }

  //! get and set the sink node in the graph
  //! FIXME: return Null if there is more than one sink node
  //OA_ptr<DGraph::Interface::Node> getSink() { return mExit; }
  //void setSink(OA_ptr<Node> n) { mExit = n; }
  
 
  void dump (std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);
  void dumpdot (std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);
  void dumpbase(std::ostream& os) {}
  
  //------------------------------------------------------------------
  // Using trick of imitating covariance when getting all iterators
  // so that get an iterator specific to actual subclass
  // This is why have these getBlahIterator methods that shadow those
  // in DGraph::Interface and also have the protected ones
  // that must be defined here which override implementation in 
  // DGraph::Interface
  //------------------------------------------------------------------
  
  OA_ptr<NodesIterator> getNodesIterator() const;

  OA_ptr<EntryNodesIterator> getEntryNodesIterator() const;

  OA_ptr<ExitNodesIterator> getExitNodesIterator() const;

  OA_ptr<EdgesIterator> getEdgesIterator() const;

  OA_ptr<DFSIterator> getDFSIterator();

  OA_ptr<BFSIterator> getBFSIterator();

  OA_ptr<ReversePostDFSIterator> 
    getReversePostDFSIterator(DGraph::DGraphEdgeDirection pOrient);

  OA_ptr<ReversePostDFSIterator>
    getReversePostDFSIterator(OA_ptr<DGraph::Interface::Node> root, 
                              DGraph::DGraphEdgeDirection pOrient);

  OA_ptr<PostDFSIterator> getPostDFSIterator(
          DGraph::DGraphEdgeDirection pOrient);

  OA_ptr<PostDFSIterator>
    getPostDFSIterator(OA_ptr<DGraph::Interface::Node> root, 
                       DGraph::DGraphEdgeDirection pOrient);

protected:
  OA_ptr<DGraph::Interface::NodesIterator> 
  getDGraphInterfaceNodesIterator() const
    // extra conversion needed because NodesIterator is an ambiguous base
    { OA_ptr<NodesIterator> temp = getNodesIterator();
      return temp.convert<DGraph::Interface::DFSIterator>(); 
    }
  OA_ptr<DGraph::Interface::EntryNodesIterator> 
  getDGraphInterfaceEntryNodesIterator() const
    { return getEntryNodesIterator(); }
  OA_ptr<DGraph::Interface::ExitNodesIterator> 
  getDGraphInterfaceExitNodesIterator() const
    { return getExitNodesIterator(); }

  OA_ptr<DGraph::Interface::EdgesIterator> 
  getDGraphInterfaceEdgesIterator() const
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
    { return getReversePostDFSIterator(root,pOrient); }
  OA_ptr<DGraph::Interface::PostDFSIterator> 
  getDGraphInterfacePostDFSIterator( DGraph::DGraphEdgeDirection pOrient)
    { return getPostDFSIterator(pOrient); }
  OA_ptr<DGraph::Interface::PostDFSIterator> 
  getDGraphInterfacePostDFSIterator(
              OA_ptr<DGraph::Interface::Node> root, 
              DGraph::DGraphEdgeDirection pOrient)
    { return getPostDFSIterator(root,pOrient); }

 private:
  // helper routines for creating iterators
  // FIXME: should be moved somewhere were other graph classes
  // can use it
  OA_ptr<std::list<OA_ptr<ICFGStandard::Node> > > 
      convert_to_ICFG(
              OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > pList) const;
  /*
  OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > 
      create_reverse_post_order_list(DGraph::Interface& pDGraph,
                                     OA_ptr<DGraph::Interface::Node> root,
                                     DGraph::DGraphEdgeDirection pOrient);
  void reverse_postorder_recurse(DGraph::Interface& pDGraph, 
                         OA_ptr<DGraph::Interface::Node> pNode,
                         DGraph::DGraphEdgeDirection pOrient,
                         OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > );
                         */

 private:
  OA_ptr<Node> mEntry; 
  OA_ptr<Node> mExit;

  typedef std::map<OA_ptr<DGraph::DGraphStandard::Node>,OA_ptr<Node> > DGNodeToNodeMap;
  typedef std::map<OA_ptr<DGraph::DGraphStandard::Edge>,OA_ptr<Edge> > DGEdgeToEdgeMap;

  // mappings between dgraph nodes to ICFG nodes
  DGNodeToNodeMap mDGNodeToICFGNode;
  DGEdgeToEdgeMap mDGEdgeToICFGEdge;

  // dgraph that will store underlying graph structure
  OA_ptr<DGraph::DGraphStandard> mDGraph;

  // ICFG nodes and edges
  OA_ptr<std::list<OA_ptr<Node> > > mNodes;
  OA_ptr<std::list<OA_ptr<Edge> > > mEdges;

  // using lists instead of sets because some of the iterators
  // need an ordered list of things and only want to have one
  // NodeIterator and EdgesIterator implementation
  OA_ptr<std::list<OA_ptr<Node> > > mCallNodes;
  OA_ptr<std::list<OA_ptr<Edge> > > mCallEdges;
  OA_ptr<std::list<OA_ptr<Node> > > mReturnNodes;
  OA_ptr<std::list<OA_ptr<Edge> > > mReturnEdges;


};
//--------------------------------------------------------------------

  } // end of ICFG namespace
} // end of OA namespace

#endif
