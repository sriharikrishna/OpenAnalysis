/*! \file
  
  \brief Declaration for standard CFGStandard.

  \authors Arun Chauhan (2001 as part of Mint), Nathan Tallent, Michelle Strout
  \version $Id: CFGStandard.hpp,v 1.48 2005/08/18 14:06:57 johnmc Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef CFGStandard_H
#define CFGStandard_H

//--------------------------------------------------------------------
// standard headers
#ifdef NO_STD_CHEADERS
# include <string.h>
#else
# include <cstring>
#endif

// STL headers
#include <list>
#include <set>
#include <map>

// OpenAnalysis headers
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/Utils/DGraph/DGraphStandard.hpp>
#include <OpenAnalysis/IRInterface/CFGIRInterface.hpp>
#include <OpenAnalysis/CFG/Interface.hpp>

namespace OA {
  namespace CFG {


//--------------------------------------------------------------------
/*! CFGStandard implements the CFG::Interface using the DGraph class.
*/
//--------------------------------------------------------------------
class CFGStandard : public virtual Interface, public DGraph::DGraphStandard {

public:
  class Node;
  friend class Node;

  //------------------------------------------------------------------
  class NodeStatementsIterator;
  class NodeStatementsRevIterator;
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

  //--------------------------------------------------------------------
  /*! CFGStandard::Node implements the CFG::Interface::Node
  */
  //--------------------------------------------------------------------
  class Node : public virtual Interface::Node, 
               public DGraph::DGraphStandard::Node {
  public:
    Node() : DGraph::DGraphStandard::Node() { Ctor(); }
    Node(StmtHandle n) : DGraph::DGraphStandard::Node() 
        { Ctor(); mStmt_list->push_back(n); }
    ~Node() { mStmt_list->clear(); }
    
    //! An id unique within instances of CFGStandard::Node
    virtual unsigned int getId() const { return mId; }
    
    //! return number of statements in block
    unsigned int size() const { return mStmt_list->size(); }

    //! reset id numbering
    static void resetIds() { sNextId = 1; }

    //! add statement to block
    void add(StmtHandle h) { mStmt_list->push_back(h); }
    void add_front(StmtHandle h) { mStmt_list->push_front(h); }

    //! erase statement from block, Careful: linear time!
    StmtHandle erase(StmtHandle h); 

    //! remove all statements from block
    bool empty() const { return mStmt_list->empty(); }
    
    void split(StmtHandle splitPoint, OA_ptr<Node> newBlock);
    
    // These still might be needed
    //void addEndExpr (ExprHandle expr) { end = expr; }
    //ExprHandle getEndExpr () const { return end; }
    void dump(std::ostream& os) { } // BaseGraph::Node::dump override, must be here
    void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);
    void longdump(CFGStandard&, std::ostream& os, 
                  OA_ptr<IRHandlesIRInterface> ir);
    //void longdump (CFG * _cfg) { longdump(_cfg, std::cout); } 
    
    void dumpdot(CFGStandard &currcfg, std::ostream& os, 
                 OA_ptr<IRHandlesIRInterface> ir);
    
    friend class CFGStandard;
    friend class CFGStandard::NodeStatementsIterator;
    friend class CFGStandard::NodeStatementsRevIterator;

    OA_ptr<CFG::Interface::NodeStatementsIterator> 
          getNodeStatementsIterator() const
      { OA_ptr<NodeStatementsIterator> retval; 
        retval = new NodeStatementsIterator(*this); 
        return retval;
      }

    OA_ptr<CFG::Interface::NodeStatementsRevIterator> 
        getNodeStatementsRevIterator()  const
      { OA_ptr<NodeStatementsRevIterator> retval; 
        retval = new NodeStatementsRevIterator(*this); 
        return retval;
      }

    
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

    OA_ptr<PredNodesIterator> getPredNodesIterator() const
      { return getSourceNodesIterator(); }

    OA_ptr<SinkNodesIterator> getSinkNodesIterator() const
      { OA_ptr<SinkNodesIterator> retval; 
        retval = new SinkNodesIterator(*this); 
        return retval;
      }

    OA_ptr<SuccNodesIterator> getSuccNodesIterator() const
      { return getSinkNodesIterator(); }

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
    void Ctor() { mId = sNextId++; mStmt_list = new std::list<StmtHandle>; }

  protected:
    //! gives inheriters read access to mStmt_list (a)
    std::list<StmtHandle>::iterator getStmtListBegin () {
      return mStmt_list->begin();
    }
    //! gives inheriters read access to mStmt_list (b)
    std::list<StmtHandle>::iterator getStmtListEnd () {
      return mStmt_list->end();
    }
    //! gives inheriters read access to mStmt_list (c)
    std::list<StmtHandle>::reverse_iterator getStmtListRBegin () {
      return mStmt_list->rbegin();
    }
    //! gives inheriters read access to mStmt_list (d)
    std::list<StmtHandle>::reverse_iterator getStmtListREnd () {
      return mStmt_list->rend();
    }

    ////! gives inheriters read access to sNextId
    //unsigned int getNextId () {
    //  return sNextId;
    //}
    ////
    ////! gives inheriters write access to sNextId
    //void setNextId(unsigned int nId) {
    //  sNextId = nId;
    //}

  private:
    static unsigned int sNextId;

    // OA_ptr because want a reference to it in NodeStatementsIterator
    OA_ptr<std::list<StmtHandle> > mStmt_list;
    //std::list<StmtHandle> mStmt_list;
    ExprHandle mEnd;
    unsigned int mId; // 0 is reserved; first instance is 1

  };
  //--------------------------------------------------------------------
  /*! CFGStandard::Edge implements the CFG::Interface::Edge
  */
  //--------------------------------------------------------------------
  class Edge : public virtual Interface::Edge, 
               public DGraph::DGraphStandard::Edge {
  public:
    Edge(OA_ptr<Interface::Node> n1, OA_ptr<Interface::Node> n2, 
         Interface::EdgeType type, 
	 OA::ExprHandle expr);
    ~Edge() {}
    
    friend class CFGStandard;

    OA_ptr<Node> source () const 
        { OA_ptr<Interface::Node> n = getCFGInterfaceNodeSource();
          return n.convert<Node>(); }
    OA_ptr<Node> sink () const 
        { OA_ptr<Interface::Node> n = getCFGInterfaceNodeSink();
          return n.convert<Node>(); }

    void dumpdot(std::ostream &os, OA_ptr<IRHandlesIRInterface> ir);

  protected:
    OA_ptr<Interface::Node> getCFGInterfaceNodeSource() const 
        { OA_ptr<DGraph::Interface::Node> n = getDGraphInterfaceNodeSource();
          return n.convert<Interface::Node>(); }
    OA_ptr<Interface::Node> getCFGInterfaceNodeSink() const 
        { OA_ptr<DGraph::Interface::Node> n = getDGraphInterfaceNodeSink();
          return n.convert<Interface::Node>(); }

  public:
    //! get an id unique within instances of CFGStandard::Edge
    virtual unsigned int getId() const { return mId; }
    
    //! get type of edge: FALLTHROUGH, etc.
    EdgeType getType() const { return mType; }

    //! get expression associated with edge, specifies branching val
    ExprHandle getExpr() const { return mExpr; }
    
    // These still might be needed
    // resetIds: reset id numbering
    static void resetIds() { sNextId = 1; }

    //! used to be in cfg debugging section
    const char* edgeTypeToString(EdgeType et) const;

    void dump(std::ostream& os);
    //void dump () { dump(std::cout); }

  protected:

    //! used by inheriters to gain write access to mId
    void setId(unsigned int id) { mId = id; }

    //! used by inheriters to gain write access to mType
    void setType(EdgeType type) { mType = type; }

    //! used by inheriters to gain write access to mExpr
    void setExpr(ExprHandle expr) { mExpr = expr; }

    ////! used by inheriters to gain write access to sNextId
    //virtual void setNextId(unsigned int nId) { sNextId=nId; }
    
    ////! used by inheriters to gain read access to sNextId
    //virtual unsigned int getNextId() const { return sNextId; }
    
  private:
    EdgeType mType;
    ExprHandle mExpr;
    unsigned int mId; // 0 is reserved; first instance is 1
    static unsigned int sNextId;
    static const char* sEdgeTypeToString[];
  };  

//-------------------------------------------------------------------
// CFGStandard methods
//-------------------------------------------------------------------
public:
  CFGStandard(SymHandle _name);
  ~CFGStandard();

  //-------------------------------------
  // CFG information access
  //-------------------------------------
  //! get ptr to unique entry node
  OA_ptr<Interface::Node> getEntry() const { return mEntry; }
  OA_ptr<Interface::Node> getExit() const { return mExit; }
  SymHandle getName() const { return mName; }

  void dump(std::ostream&, OA_ptr<IRHandlesIRInterface> ); 

  void dumpdot(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);
  //void output() const; 
  
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
  //-------------------------------------
  // CFG building methods
  //-------------------------------------
  void setEntry(OA_ptr<Node> n) { mEntry = n; }
  void setExit(OA_ptr<Node> n) { mExit = n; }

  OA_ptr<Node> splitBlock(OA_ptr<Node>, StmtHandle);

  OA_ptr<Edge> connect(OA_ptr<Interface::Node> src, 
                       OA_ptr<Interface::Node> dst,
                       EdgeType type) {
      OA_ptr<Edge> e;
      e = new Edge (src, dst, type, 0);
      addEdge (e);
      return e;
  }
  OA_ptr<Edge> connect(OA_ptr<Interface::Node> src, 
                       OA_ptr<Interface::Node> dst,
		       EdgeType type, ExprHandle expr) {
      OA_ptr<Edge> e;
      e = new Edge (src, dst, type, expr);
      addEdge (e);
      return e;
  }

  class NodeLabel;
  typedef std::list<NodeLabel> NodeLabelList;
  void connect(OA_ptr<Interface::Node>, NodeLabelList&);
  void connect(NodeLabelList&, OA_ptr<Interface::Node>);
  void disconnect(OA_ptr<Edge> e) 
    { removeEdge(e); }
  void disconnect(OA_ptr<Node> n) 
    { removeNode(n); }
  
  //-------------------------------------
  // CFG attaching labels to nodes
  //-------------------------------------
  OA_ptr<CFGStandard::Node> node_from_label(OA::StmtLabel);
  OA_ptr<CFGStandard::Node> getLabelBlock(StmtLabel lab); 
  //! associate given label with given node
  void mapLabelToNode(OA::StmtLabel lab, OA_ptr<Node> n)
    { mlabel_to_node_map[lab] = n; }
  //! determine if the given label is currently mapped to a node
  bool isLabelMappedToNode(StmtLabel lab) 
    { return (mlabel_to_node_map.find(lab) != mlabel_to_node_map.end()) 
      ? true: false; }
    
  //-------------------------------------
  // CFG debugging
  //-------------------------------------
  //const char* edgeTypeToString(EdgeType et) const; // moved to edge


  //------------------------------------------------------------------
  //! A list of CFGStandard::Node pointers
  //typedef std::list<OA_ptr<Node> > NodeList;
  
  //--------------------------------------------------------------------
  /*! An iterator for the NodeList given to this class' constructor.
  */
  //--------------------------------------------------------------------
  /*
  class NodeListIterator : public Iterator {
  public:
    NodeListIterator(OA_ptr<NodeList> nl) 
        { mList = nl; mIter = mList->begin(); }
    virtual ~NodeListIterator() {}
    void operator++() { if (mIter != mList->end()) ++mIter; }
    bool isValid() const { return (mIter != mList->end()); }
    OA_ptr<Node> current() { return *mIter; }
  private:
    OA_ptr<NodeList> mList;
    std::list<OA_ptr<Node> >::iterator mIter;
  };
  */
  
  //--------------------------------------------------------------------
  /*! An iterator for the statements in a node.
  */
  //--------------------------------------------------------------------
  class NodeStatementsIterator : public virtual 
      CFG::Interface::NodeStatementsIterator {
  public:
    NodeStatementsIterator(const Node& node) 
        { mList = node.mStmt_list; 
          mIter = mList->begin();
        }
    ~NodeStatementsIterator() {}
    void operator++() { if (mIter != mList->end()) { mIter++; } }
    void operator++(int) { ++*this; }  // postfix
    //! returns true if we are not past the end of the list
    bool isValid() const { return (mIter != mList->end()); }
    //! returns the statement handle that the iterator is currently pointing 
    StmtHandle current() const { return *mIter; }
    void reset() { mIter = mList->begin(); }
  private:
    OA_ptr<std::list<StmtHandle> > mList;
    std::list<StmtHandle>::iterator mIter;
  };
  
  //--------------------------------------------------------------------
  /*! An iterator for the statements in a node.
  */
  //--------------------------------------------------------------------
  class NodeStatementsRevIterator : public virtual 
      CFG::Interface::NodeStatementsRevIterator {
  public:
    NodeStatementsRevIterator(const Node& node) 
        { mList = node.mStmt_list; 
          mRevIter = mList->rbegin();
        }
    ~NodeStatementsRevIterator() {}
    void operator++() { if (mRevIter != mList->rend()) ++mRevIter; }
    void operator++(int) { ++*this; }  // postfix
    //! returns true if we are not past the end of the list
    bool isValid() const { return (mRevIter != mList->rend()); }
    //! returns the statement handle that the iterator is currently pointing 
    StmtHandle current() const { return *mRevIter; }
    void reset() { mRevIter = mList->rbegin(); }
  private:
    OA_ptr<std::list<StmtHandle> > mList;
    std::list<StmtHandle>::reverse_iterator mRevIter;
  };
  
  class NodeLabelListIterator;
  friend class NodeLabelListIterator;
  /*----------------------------------------------------------------*/
  /*! A label for a node that contains information about edgetype?,
   * and an expression for each node?
   *//*-------------------------------------------------------------*/
  class NodeLabel {
  public:
    NodeLabel(OA_ptr<Node> _n, EdgeType _et, ExprHandle _eh = 0) :
      mN(_n), mEh(_eh), mEt(_et) {}
    OA_ptr<CFGStandard::Node> getNode() { return mN; }
    Interface::EdgeType getEdgeType() { return mEt; }
    OA::ExprHandle getExpr() { return mEh; }

  private:
    OA_ptr<CFGStandard::Node> mN;
    OA::ExprHandle mEh;
    Interface::EdgeType mEt;
  };

  /*----------------------------------------------------------------*/
  /*! An iterator over a list of Node labels.
   * and an expression for each node?
   *//*-------------------------------------------------------------*/
  class NodeLabelListIterator {
  public:
    NodeLabelListIterator(NodeLabelList& nl)  
        : mList(nl), mIter(mList.begin()) {}
    virtual ~NodeLabelListIterator() {}
    void operator++() { if (mIter != mList.end()) ++mIter; }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const { return (mIter != mList.end()); }
    NodeLabel current() const { return *mIter; }
  private:
    NodeLabelList& mList;
    std::list<NodeLabel>::iterator mIter;
  };

  //------------------------------------------------------------------
  /*! The NodesIterator is just an extension of DGraphStandard::NodesIterator 
      and Interface::NodesIterator
      to provide access to CFG::CFGStandard nodes. 
  */
  class NodesIterator : public virtual Interface::NodesIterator,
                        public DGraph::DGraphStandard::NodesIterator {
  public:
    NodesIterator (const CFGStandard& g) : DGraph::DGraphStandard::NodesIterator(g) {}
    NodesIterator () {} 
    virtual ~NodesIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () { DGraph::DGraphStandard::NodesIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
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
    ReversePostDFSIterator (CFGStandard& g, 
                            DGraph::DGraphEdgeDirection pOrient)
        : DGraph::DGraphStandard::ReversePostDFSIterator(g,pOrient) {}
    ReversePostDFSIterator (CFGStandard& g, 
                            OA_ptr<Node> root, 
                            DGraph::DGraphEdgeDirection pOrient)
        : DGraph::DGraphStandard::ReversePostDFSIterator(g,
                root.convert<Node>(),pOrient) {}
    virtual ~ReversePostDFSIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () 
      { DGraph::DGraphStandard::ReversePostDFSIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const 
      { return DGraph::DGraphStandard::ReversePostDFSIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::ReversePostDFSIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { return DGraph::DGraphStandard::ReversePostDFSIterator::getDGraphInterfaceNodesIteratorCurrent(); }
  };

  //------------------------------------------------------------------
  class PostDFSIterator 
      : public virtual Interface::PostDFSIterator,
        public DGraph::DGraphStandard::PostDFSIterator 
  {
  public:
    PostDFSIterator (CFGStandard& g, 
                            DGraph::DGraphEdgeDirection pOrient)
        : DGraph::DGraphStandard::PostDFSIterator(g,pOrient) {}
    PostDFSIterator (CFGStandard& g, 
                            OA_ptr<Node> root, 
                            DGraph::DGraphEdgeDirection pOrient)
        : DGraph::DGraphStandard::PostDFSIterator(g,
                root.convert<Node>(),pOrient) {}
    virtual ~PostDFSIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () 
      { DGraph::DGraphStandard::PostDFSIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const 
      { return DGraph::DGraphStandard::PostDFSIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::PostDFSIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { return DGraph::DGraphStandard::PostDFSIterator::getDGraphInterfaceNodesIteratorCurrent(); }
  };


  //------------------------------------------------------------------
  class DFSIterator 
      : public virtual Interface::DFSIterator,
        public DGraph::DGraphStandard::DFSIterator 
  {
  public:
    DFSIterator (CFGStandard& g) 
        : DGraph::DGraphStandard::DFSIterator(g) {}
    virtual ~DFSIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () 
      { DGraph::DGraphStandard::DFSIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
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
    BFSIterator (CFGStandard& g) 
        : DGraph::DGraphStandard::BFSIterator(g) {}
    virtual ~BFSIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () 
      { DGraph::DGraphStandard::BFSIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
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
    EdgesIterator (const CFGStandard& g) 
        : DGraph::DGraphStandard::EdgesIterator(g) {}
    virtual ~EdgesIterator () {}
    OA_ptr<Edge> current() const
      { OA_ptr<DGraph::Interface::Edge> n 
          = getDGraphInterfaceEdgesIteratorCurrent();
        return n.convert<Edge>(); }
    void operator++ () 
      { DGraph::DGraphStandard::EdgesIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
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
    virtual ~IncomingEdgesIterator () {}
    OA_ptr<Edge> current() const
      { OA_ptr<DGraph::Interface::Edge> n 
          = getDGraphInterfaceEdgesIteratorCurrent();
        return n.convert<Edge>(); }
    void operator++ () 
      { DGraph::DGraphStandard::IncomingEdgesIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
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
    virtual ~OutgoingEdgesIterator () {}
    OA_ptr<Edge> current() const
      { OA_ptr<DGraph::Interface::Edge> n 
          = getDGraphInterfaceEdgesIteratorCurrent();
        return n.convert<Edge>(); }
    void operator++ () 
      { DGraph::DGraphStandard::OutgoingEdgesIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
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
    virtual ~SourceNodesIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () 
      { DGraph::DGraphStandard::SourceNodesIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
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
    virtual ~SinkNodesIterator () {}
    OA_ptr<Node> current() const
      { OA_ptr<DGraph::Interface::Node> n 
          = getDGraphInterfaceNodesIteratorCurrent();
        return n.convert<Node>(); }
    void operator++ () 
      { DGraph::DGraphStandard::SinkNodesIterator::operator++(); }
    void operator++(int) { ++*this; }  // postfix
    bool isValid() const 
      { return DGraph::DGraphStandard::SinkNodesIterator::isValid(); } 
    void reset() { DGraph::DGraphStandard::SinkNodesIterator::reset(); }

  protected:
    OA_ptr<DGraph::Interface::Node> 
        getDGraphInterfaceNodesIteratorCurrent() const 
    { return DGraph::DGraphStandard::SinkNodesIterator::getDGraphInterfaceNodesIteratorCurrent(); }
   };
 

// members for CFGStandard
private:
  //static const char* sEdgeTypeToString[]; //moved to edge class
  SymHandle mName;
  OA_ptr<Node> mEntry;
  OA_ptr<Node> mExit;
  std::map<OA::StmtLabel, OA_ptr<CFGStandard::Node> > mlabel_to_node_map;

};

  } // end of CFG namespace
} // end of OA namespace

#endif
