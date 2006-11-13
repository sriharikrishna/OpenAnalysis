/*! \file
  
  \brief Implementation of ExprTree.

  \authors Nathan Tallent, Michelle Strout
  \version $Id: ExprTree.cpp,v 1.14 2005/03/17 21:47:45 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/
// standard headers

#include "ExprTree.hpp"

// included here instead of in hpp file to remove circular reference
#include <OpenAnalysis/ExprTree/ExprTreeVisitor.hpp>

namespace OA {

//--------------------------------------------------------------------
ExprTree::ExprTree() 
{
}

ExprTree::~ExprTree()
{
}

//! visit nodes in require order and then accept visitor on tree
void ExprTree::acceptVisitor(ExprTreeVisitor& pVisitor)
{ 
    pVisitor.visitExprTreeBefore(*this); 
    getRoot()->acceptVisitor(pVisitor);
    pVisitor.visitExprTreeAfter(*this); 
}

/*!
   Copies the given subtree into this tree as a child of the given node
*/

void ExprTree::copyAndConnectSubTree(OA_ptr<Node> src, OA_ptr<ExprTree> subtree)
{
    std::map<OA_ptr<Node>,OA_ptr<Node> > oldToNewNodeMap;

    // iterate over all nodes to get copy
    NodesIterator nIter(*subtree);
    for ( ; nIter.isValid(); ++nIter) {
      oldToNewNodeMap[nIter.current()] = nIter.current()->copy();
    }

    // iterate over all edges of the subtree
    EdgesIterator eIter(*subtree);
    for ( ; eIter.isValid(); ++eIter ) {
        OA_ptr<Edge> edge = eIter.current();
        connect( oldToNewNodeMap[edge->source()],
                 oldToNewNodeMap[edge->sink()] );
    }
    connect(src,oldToNewNodeMap[subtree->getRoot()]);
}


void
ExprTree::dump (std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
  os << "===== ExprTree: =====" << std::endl;
  
  // print the contents of all the nodes
  //PreOrderIterator nodes_iter(*this);
  NodesIterator nodes_iter(*this);
  for ( ; nodes_iter.isValid(); ++nodes_iter) {
    OA_ptr<ExprTree::Node> node = nodes_iter.current();
    node->dump(os,ir);
  }
  
  os << "====================" << std::endl << std::endl;
}

//-------------------------------------------------------------------
// OpNode
//-------------------------------------------------------------------

void ExprTree::OpNode::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{ 
    Node::dump(os,ir);  
    os << "handle: " << ir->toString(mHandle) << std::endl; 
}

void ExprTree::OpNode::acceptVisitor(ExprTreeVisitor& pVisitor)
{ 
    pVisitor.visitOpNode(*this); 
}

//-------------------------------------------------------------------
// CallNode
//-------------------------------------------------------------------
void ExprTree::CallNode::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{ 
  Node::dump(os,ir);  
  os << "handle: " << ir->toString(mHandle) << std::endl; 
}

void ExprTree::CallNode::acceptVisitor(ExprTreeVisitor& pVisitor)
{ 
    pVisitor.visitCallNode(*this); 
}

//-------------------------------------------------------------------
// MemRefNode
//-------------------------------------------------------------------
void ExprTree::MemRefNode::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{ 
  Node::dump(os,ir);  
  os << "handle: " << ir->toString(mHandle) << std::endl; 
}

void ExprTree::MemRefNode::acceptVisitor(ExprTreeVisitor& pVisitor)
{ 
    pVisitor.visitMemRefNode(*this); 
}

//-------------------------------------------------------------------
// ConstSymNode
//-------------------------------------------------------------------
void ExprTree::ConstSymNode::dump(std::ostream& os, 
                                  OA_ptr<IRHandlesIRInterface> ir)
{ 
  Node::dump(os,ir);  
  os << "handle: " << ir->toString(mHandle) << std::endl; 
}

void ExprTree::ConstSymNode::acceptVisitor(ExprTreeVisitor& pVisitor)
{ 
    pVisitor.visitConstSymNode(*this); 
}

//-------------------------------------------------------------------
// ConstValNode
//-------------------------------------------------------------------
void ExprTree::ConstValNode::dump(std::ostream& os, 
                                  OA_ptr<IRHandlesIRInterface> ir)
{ 
  Node::dump(os,ir);  
  os << "handle: " << ir->toString(mHandle) << std::endl; 
}

void ExprTree::ConstValNode::acceptVisitor(ExprTreeVisitor& pVisitor)
{ 
    pVisitor.visitConstValNode(*this); 
}


//--------------------------------------------------------------------
void ExprTree::Edge::dump(std::ostream& os)
{
  std::cout << "{Edge: parent: " << parent() << " child: " << child()
	    << "}" << std::endl;
}

//--------------------------------------------------------------------
void
ExprTree::Node::dump (std::ostream& os, OA_ptr<IRHandlesIRInterface> ir)
{
    os << "{Node: "
	<< "  " << mName;
	//<< ", attr:  " << mAttr;
}

//--------------------------------------------------------------------
/*
void
ExprTree::Node::str(ostream& os){
  if (isSym()) {
    os << symHndl << ":";
  }
  if (isConst()) {
    os << constHndl << ":";
  }
  if (isAttr()) {
    os << attr << ":";
  }
}

//-----------------------------------------------------------------
void ExprTree::str(ostream &os){
  PreOrderIterator nodes_iter(*this);
  for ( ; (bool)nodes_iter; ++nodes_iter) {
    ExprTree::Node* node = 
      dynamic_cast<ExprTree::Node*>((Tree::Node*)nodes_iter);
    node->str(os);
  }
}
*/

} // end of OA namespace
