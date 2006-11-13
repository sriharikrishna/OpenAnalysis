/*! \file
  
  \brief Abstract class for solving dataflow analysis problems on CallGraphs

  \authors Michelle Strout (Nov 2004)
           similar to John Mellor-Crummey's CallGraphDFProblem.h
  \version $Id: CallGraphDFProblem.hpp,v 1.5 2005/06/10 02:32:03 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

  To use this for performing interprocedural dataflow analysis,
  privately inherit from the CallGraphDFProblem class and
  implement the callbacks.
*/

#ifndef CallGraphDFProblem_h
#define CallGraphDFProblem_h

#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/DataFlow/DGraphIterativeDFP.hpp>
#include <OpenAnalysis/DataFlow/DataFlowSet.hpp>
#include <OpenAnalysis/CallGraph/Interface.hpp>
#include <map>

#include <OpenAnalysis/ExprTree/EvalToMemRefVisitor.hpp>
#include <OpenAnalysis/IRInterface/CallGraphDFProblemIRInterface.hpp>
#include <OpenAnalysis/Alias/InterAliasInterface.hpp>

namespace OA {
  namespace DataFlow {

  
//*********************************************************************
// class CallGraphDFProblem
//*********************************************************************
class CallGraphDFProblem  : private DataFlow::DGraphIterativeDFP {
public:
  typedef enum { TopDown, BottomUp } CallGraphDirectionType;
private:
  OA_ptr<DataFlowSet> mTop;
  OA_ptr<DataFlowSet> mBottom;
  CallGraphDirectionType mDirection;

public:
  //--------------------------------------------------------
  // constructor/destructor
  //--------------------------------------------------------
  CallGraphDFProblem(CallGraphDirectionType pDirection, 
                     OA_ptr<DataFlow::CallGraphDFProblemIRInterface> ir)
      : mDirection(pDirection), mIR(ir) {}
  virtual ~CallGraphDFProblem() {}

  void solve(OA_ptr<CallGraph::Interface> callGraph,
             OA_ptr<Alias::InterAliasInterface> interAlias);

protected:
  //--------------------------------------------------------
  // Methods for subclasses to get information about mapping
  // of locations in Callee to locations in Caller
  //--------------------------------------------------------
  OA_ptr<LocIterator> 
      getCalleeToCallerMayLocs(ExprHandle call, OA_ptr<Location> loc);
  OA_ptr<LocIterator> 
      getCalleeToCallerMustLocs(ExprHandle call, OA_ptr<Location> loc);

private:
  //--------------------------------------------------------
  // initialization callbacks
  //--------------------------------------------------------

  //! Return an initialized top set
  //virtual OA_ptr<DataFlowSet>  initializeTop() = 0;

  //! Return an initialized bottom set
  //virtual OA_ptr<DataFlowSet>  initializeBottom() = 0;

  //! Should generate an in and out DataFlowSet for a function
  //! and store these in mNodeInSetMap and mNodeOutSetMap.
  //virtual void initializeNode(OA_ptr<CFG::Interface::Node> n)  = 0;
  //virtual void initializeEdge(DGraph::Interface::Edge *e)  = 0;

  //--------------------------------------------------------
  // solver callbacks 
  //--------------------------------------------------------
  
  //! OK to modify set1 and return it as result, because solver
  //! only passes a tempSet in as set1
  //virtual OA_ptr<DataFlowSet> meet(OA_ptr<DataFlowSet> set1, 
  //                                 OA_ptr<DataFlowSet> set2) = 0; 

  //! OK to modify in set and return it again as result because
  //! solver clones the BB in sets
  //virtual OA_ptr<DataFlowSet> transfer(OA_ptr<DataFlowSet> in, 
  //                                     OA::StmtHandle stmt) = 0; 
                            
protected:
  //========================================================
  // implementation of DGraphIterativeDFP callbacks
  // These are implemented in CallGraphDFProblem.cpp for a 
  // CallGraph.
  //========================================================
  
  //--------------------------------------------------------
  // initialization upcall 
  //--------------------------------------------------------
  void initialize(OA_ptr<DGraph::Interface> dg);
  
  //--------------------------------------------------------
  // solver upcalls
  //--------------------------------------------------------
  //bool atDGraphNode(OA_ptr<DGraph::Interface::Node> node, 
  //                  DGraph::DGraphEdgeDirection pOrient);
  virtual bool atDGraphEdge(OA_ptr<DGraph::Interface::Edge>, 
                            DGraph::DGraphEdgeDirection);
  
  //--------------------------------------------------------
  // finalization upcalls
  //--------------------------------------------------------
  void finalizeNode(OA_ptr<DGraph::Interface::Node> node);
  void finalizeEdge(OA_ptr<DGraph::Interface::Edge> edge);


  /*! 
     transfer function for a CFG::Interface::Node
     Will clone in set to block before passing it to this function
     so ok to return a modified in set
  */
  //OA_ptr<DataFlowSet> transfer(OA_ptr<DataFlowSet> in, 
  //                             OA_ptr<DGraph::Interface::Node> n);

  //--------------------------------------------------------
  // debugging
  //--------------------------------------------------------
public:
  void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir);


private:
  void mapFormalToActualMay(ExprHandle call,
                            OA_ptr<Location> formalLoc,
                            OA_ptr<Location> actualLoc)
    {
        mFormalToActualLocMay[call][formalLoc].insert(actualLoc);
    }
  void mapFormalToActualMust(ExprHandle call,
                             OA_ptr<Location> formalLoc,
                             OA_ptr<Location> actualLoc)
    {
        mFormalToActualLocMust[call][formalLoc].insert(actualLoc);
    }

private:
  typedef std::map<ExprHandle,
                   std::map<OA_ptr<Location>,LocSet> >
      CallToLocToLocSetMap;
  CallToLocToLocSetMap mFormalToActualLocMay;
  CallToLocToLocSetMap mFormalToActualLocMust;

  OA_ptr<Alias::InterAliasInterface> mInterAlias;
  OA_ptr<CallGraphDFProblemIRInterface> mIR;

  //std::map<OA_ptr<CFG::Interface::Node>,OA_ptr<DataFlowSet> > mNodeInSetMap;
  //std::map<OA_ptr<CFG::Interface::Node>,OA_ptr<DataFlowSet> > mNodeOutSetMap;

  // whether or not the node has had the transfer function applied
  // to all statements at least once
  //std::map<OA_ptr<CFG::Interface::Node>,bool> mNodeInitTransApp;

};

  } // end of DataFlow namespace
}  // end of OA namespace

#endif
