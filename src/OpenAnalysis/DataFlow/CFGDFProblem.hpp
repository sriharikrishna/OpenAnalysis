/*! \file
  
  \brief Abstract class for solving dataflow analysis problems on CFGs.

  \authors Michelle Strout (April 2004)
           similar to John Mellor-Crummey's CallGraphDFProblem.h
  \version $Id: CFGDFProblem.hpp,v 1.17 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  To use this for performing an intra-procedural dataflow analysis
  problem, privately inherit from the CFGDFProblem class and
  implement the callbacks.
*/

#ifndef CFGDFProblem_h
#define CFGDFProblem_h

#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/DataFlow/DGraphIterativeDFP.hpp>
#include <OpenAnalysis/DataFlow/DataFlowSet.hpp>
#include <OpenAnalysis/CFG/Interface.hpp>
#include <OpenAnalysis/CFG/CFGStandard.hpp>
#include <OpenAnalysis/IRInterface/CFGIRInterface.hpp>
#include <map>

namespace OA {
  namespace DataFlow {

typedef enum { Forward, Backward } DFDirectionType;
  
//*********************************************************************
// class CFGDFProblem
//*********************************************************************
//template <class DataFlowSet>
class CFGDFProblem  : private DataFlow::DGraphIterativeDFP {
  OA_ptr<DataFlowSet> mTop;
  //OA_ptr<DataFlowSet> mBottom;
  DFDirectionType mDirection;

public:
  //--------------------------------------------------------
  // constructor/destructor
  //--------------------------------------------------------
  CFGDFProblem(DFDirectionType pDirection) : mDirection(pDirection) {}
  virtual ~CFGDFProblem() {}

  //! returns DataFlowSet for Exit for a forward problem 
  //! and for Entry for a backward proble
  OA_ptr<DataFlowSet> solve(OA_ptr<CFG::Interface> cfg);

private:
  //--------------------------------------------------------
  // initialization callbacks
  //--------------------------------------------------------

  //! Return an initialized top set
  virtual OA_ptr<DataFlowSet>  initializeTop() = 0;

  //! Return an initialized bottom set
  virtual OA_ptr<DataFlowSet>  initializeBottom() = 0;

  //! Should generate an in and out DataFlowSet for node
  //! and store these in mNodeInSetMap and mNodeOutSetMap.
  virtual void initializeNode(OA_ptr<CFG::Interface::Node> n)  = 0;
  //virtual void initializeEdge(DGraph::Interface::Edge *e)  = 0;

  //--------------------------------------------------------
  // solver callbacks 
  //--------------------------------------------------------
  
  //! OK to modify set1 and return it as result, because solver
  //! only passes a tempSet in as set1
  virtual OA_ptr<DataFlowSet> meet(OA_ptr<DataFlowSet> set1, 
                                   OA_ptr<DataFlowSet> set2) = 0; 

  //! OK to modify in set and return it again as result because
  //! solver clones the BB in sets
  virtual OA_ptr<DataFlowSet> transfer(OA_ptr<DataFlowSet> in, 
                                       OA::StmtHandle stmt) = 0; 
                            
private:
  //========================================================
  // implementation of DGraphIterativeDFP callbacks
  // These are implemented in CFGDFProblem.cpp for a CFG.
  //========================================================
  
  //--------------------------------------------------------
  // initialization upcall 
  //--------------------------------------------------------
  void initialize(OA_ptr<DGraph::Interface> dg);
  
  //--------------------------------------------------------
  // solver upcalls
  //--------------------------------------------------------
  bool atDGraphNode(OA_ptr<DGraph::Interface::Node> node, 
                    DGraph::DGraphEdgeDirection pOrient);
  
  //--------------------------------------------------------
  // finalization upcalls
  //--------------------------------------------------------
  void finalizeNode(OA_ptr<DGraph::Interface::Node> node);


  /*! 
     transfer function for a CFG::Interface::Node
     Will clone in set to block before passing it to this function
     so ok to return a modified in set
  */
  OA_ptr<DataFlowSet> transfer(OA_ptr<DataFlowSet> in, 
                               OA_ptr<DGraph::Interface::Node> n);

  //--------------------------------------------------------
  // debugging
  //--------------------------------------------------------
public:
  void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);

protected:
  //std::map<OA::StmtHandle,DataFlowSet*> mStmtInSetMap;
  //std::map<OA::StmtHandle,DataFlowSet*> mStmtOutSetMap;
  std::map<OA_ptr<CFG::Interface::Node>,OA_ptr<DataFlowSet> > mNodeInSetMap;
  std::map<OA_ptr<CFG::Interface::Node>,OA_ptr<DataFlowSet> > mNodeOutSetMap;

  // whether or not the node has had the transfer function applied
  // to all statements at least once
  std::map<OA_ptr<CFG::Interface::Node>,bool> mNodeInitTransApp;

};

  } // end of DataFlow namespace
}  // end of OA namespace

#endif
