/*! \file
  
  \brief Implementation of OutputExprTreeVisitor class.
  
  \authors Luis Ramos
  \version $Id: OutputExprTreeVisitor.cpp,v 1.5 2005/03/17 21:47:45 mstrout Exp $

  Copyright (c) 2002-2005, Rice University <br>
  Copyright (c) 2004-2005, University of Chicago <br>
  Copyright (c) 2006, Contributors <br>
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include <OpenAnalysis/ExprTree/OutputExprTreeVisitor.hpp>

static bool debug = false;

namespace OA {


OutputExprTreeVisitor::OutputExprTreeVisitor(IRHandlesIRInterface& ir)
	:mIR(ir)
{
  mId = 0;
}

//! if we don't know how to handle a node then the default action
//! is to change the eval result to NULL
void OutputExprTreeVisitor::visitNode(ExprTree::Node&)
{
    std::cout << "visitNode\n";
}

void OutputExprTreeVisitor::visitOpNode(ExprTree::OpNode& n)
{
  int Id = mId;
  OpHandle opH = n.getHandle();
  ostringstream label;
  label << "ET OpNode ( " << mIR.toString(opH) << " )";
  sOutBuild->graphNodeStart(mId);
    sOutBuild->graphNodeLabel(label.str());
      //sOutBuild->graphNodeLabelStart();
      //sOutBuild->graphNodeLabelEnd();
  //sOutBuild->graphNodeEnd();
  mId++;

  if ( n.num_children() <= 2 ) {

    // if so then visit each child
    OA_ptr<ExprTree::Node> cetNodePtr;
    ExprTree::ChildNodesIterator cNodesIter(n);

    // child1
    if (cNodesIter.isValid()) {
      cetNodePtr = cNodesIter.current();

      //sOutBuild->graphEdgeLabelStart();
      sOutBuild->graphEdgeStart();
        sOutBuild->graphEdgeLabelStart();
        sOutBuild->graphEdgeSourceNode(Id);
          cetNodePtr->acceptVisitor(*this);
        sOutBuild->graphEdgeLabelEnd(); 
      //sOutBuild->graphEdgeEnd();
    }
    ++cNodesIter;
    ++mId;

    // child2
    if (cNodesIter.isValid()) {
      cetNodePtr = cNodesIter.current();

      sOutBuild->graphEdgeStart();
        sOutBuild->graphEdgeLabelStart();
        sOutBuild->graphEdgeSourceNode(Id);
          cetNodePtr->acceptVisitor(*this);
        sOutBuild->graphEdgeLabelEnd(); 
      //sOutBuild->graphEdgeEnd();
    }
  }

}

void OutputExprTreeVisitor::visitCallNode(ExprTree::CallNode& n)
{
    CallHandle call = n.getHandle();
    ostringstream label;
    label << "ET CallNode ( " << mIR.toString(call) << " )";

    sOutBuild->graphNodeStart(mId);
      sOutBuild->graphNodeLabel(label.str());
        //sOutBuild->graphNodeLabelStart();
        //sOutBuild->graphNodeLabelEnd();
    //sOutBuild->graphNodeEnd();
}

void OutputExprTreeVisitor::visitMemRefNode(ExprTree::MemRefNode& n)
{
    MemRefHandle memref = n.getHandle();
    ostringstream label;
    label << "ET MemRefNode ( " << mIR.toString(memref) << " )";

    sOutBuild->graphNodeStart(mId);
      sOutBuild->graphNodeLabel(label.str());
        //sOutBuild->graphNodeLabelStart();
        //sOutBuild->graphNodeLabelEnd();
    //sOutBuild->graphNodeEnd();

}

void OutputExprTreeVisitor::visitConstSymNode(ExprTree::ConstSymNode& n)
{
    ConstSymHandle simnode = n.getHandle();
    ostringstream label;
    label << "ET ConstSymNode ( " << mIR.toString(simnode) << " )";

    sOutBuild->graphNodeStart(mId);
      sOutBuild->graphNodeLabel(label.str());
        //sOutBuild->graphNodeLabelStart();
        //sOutBuild->graphNodeLabelEnd();
    //sOutBuild->graphNodeEnd();
}

void OutputExprTreeVisitor::visitConstValNode(ExprTree::ConstValNode& n)
{
    ConstValHandle constval = n.getHandle();
    ostringstream label;
    label << "ET ConstValNode ( " << mIR.toString(constval) << " )";

    sOutBuild->graphNodeStart(mId);
      sOutBuild->graphNodeLabel(label.str());
        //sOutBuild->graphNodeLabelStart();
        //sOutBuild->graphNodeLabelEnd();
    //sOutBuild->graphNodeEnd();

}

void OutputExprTreeVisitor::output(IRHandlesIRInterface&)
{
}



} // end of OA namespace

