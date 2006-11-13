/*! \file
  
  \brief An AnnotationManager that generates ICFGDep.

  \authors Michelle Strout
  \version $Id: ManagerProcIterDep.cpp,v 1.1 2005/06/21 15:20:54 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerProcIterDep.hpp"


namespace OA {
  namespace Activity {

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerProcIterDep)
static bool debug = true;
#else
static bool debug = false;
#endif


/*!
*/
ManagerProcIterDep::ManagerProcIterDep(OA_ptr<ActivityIRInterface> _ir) 
    : mIR(_ir)
{
}


/*!
    In transfer function creates a DepDFSet for each stmt.
*/
OA_ptr<ICFGDep> ManagerProcIterDep::performAnalysis(
                    OA_ptr<IRProcIterator> procIter,
                    OA_ptr<DataFlow::ParamBindings> paramBind,
                    OA_ptr<Alias::InterAliasInterface> interAlias)
{
  if (debug) {
    std::cout << "In ManagerProcIterDep::performAnalysis" << std::endl;
  }
  mDep = new ICFGDep();

  // store results that will be needed in callbacks
  mParamBind = paramBind;
  mInterAlias = interAlias;
  
  for (procIter->reset(); procIter->isValid(); (*procIter)++ ) {
    ProcHandle proc = procIter->current();
    
    // get alias results for this procedure
    OA_ptr<Alias::Interface> alias = mInterAlias->getAliasResults(proc);

    OA_ptr<IRStmtIterator> stmtIter = mIR->getStmtIterator(proc);
    for ( ; stmtIter->isValid(); (*stmtIter)++ ) {
      StmtHandle stmt = stmtIter->current();

      if (debug) {
        std::cout << "stmt(hval=" << stmt.hval() << ")= ";
        mIR->dump(stmt,std::cout);
      }

    
      // new DepDFSet for this stmt
      OA_ptr<DepDFSet> stmtDepDFSet;
      stmtDepDFSet = new DepDFSet;
    
      // set of must defs and differentiable uses for this statement
      LocSet mustDefSet;
      LocSet mayDefSet;
      LocSet diffUseSet;

      // set of expressions to analyze for differentiable uses
      std::set<ExprHandle> exprSet;

      // iterate over memref = expr pairs if this is an EXPR_STMT
      OA::Activity::IRStmtType sType = mIR->getActivityStmtType(stmt);
      if (sType==EXPR_STMT) {
        OA_ptr<ExprStmtPairIterator> espIterPtr 
            = mIR->getExprStmtPairIterator(stmt);
        for ( ; espIterPtr->isValid(); ++(*espIterPtr)) {
            // unbundle pair
            MemRefHandle mref = espIterPtr->currentTarget();
            ExprHandle expr = espIterPtr->currentSource();
            if (debug) {
                std::cout << "\tmref = " << mIR->toString(mref) << ", ";
                std::cout << "expr = " << mIR->toString(expr) << std::endl;
            }

            // add this to list of expressions we need to analyze
            exprSet.insert(expr);
            if (debug) { 
                std::cout << "Inserting memref = expr into exprSet" 
                          << std::endl;
                OA_ptr<ExprTree> etree = mIR->getExprTree(expr);
                etree->dump(std::cout,mIR); 
            }

            // keep track of def mustlocs
            OA_ptr<LocIterator> locIterPtr = alias->getMustLocs(mref);
            for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
                mustDefSet.insert(locIterPtr->current());
            }
            // maylocs need to be used for Dep pairs so that we get
            // conservative activity results
            locIterPtr = alias->getMayLocs(mref);
            for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
                mayDefSet.insert(locIterPtr->current());
            }
        }
    
      // all other statement types just get all uses and defs
      } else {
        if (debug) {
            std::cout << "\tstmt is not EXPR_STMT, stmt = ";
        }
    
        OA_ptr<MemRefHandleIterator> mrIterPtr = mIR->getUseMemRefs(stmt);
        for (; mrIterPtr->isValid(); (*mrIterPtr)++ ) {
            MemRefHandle mref = mrIterPtr->current();
            OA_ptr<LocIterator> locIterPtr = alias->getMayLocs(mref);
            for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
                diffUseSet.insert(locIterPtr->current());
            }
        }
        mrIterPtr = mIR->getDefMemRefs(stmt);
        for (; mrIterPtr->isValid(); (*mrIterPtr)++ ) {
            MemRefHandle mref = mrIterPtr->current();
            OA_ptr<LocIterator> locIterPtr = alias->getMustLocs(mref);
            for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
                mustDefSet.insert(locIterPtr->current());
            }
            locIterPtr = alias->getMayLocs(mref);
            for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
                mayDefSet.insert(locIterPtr->current());
            }
        }
      }

      // iterate over all calls in the statement
      // and find all expressions that are callsite params and not 
      // reference parameters
      OA_ptr<IRCallsiteIterator> callsiteItPtr = mIR->getCallsites(stmt);
      for ( ; callsiteItPtr->isValid(); ++(*callsiteItPtr)) {
        ExprHandle call = callsiteItPtr->current();
        if (debug) {
          std::cout << "\nhandling all callsite params, ";
          std::cout << "call = " << mIR->toString(call) << std::endl;
        }

        // looping over actual params
        OA_ptr<IRCallsiteParamIterator> cspIterPtr 
            = mIR->getCallsiteParams(call);
        for ( ; cspIterPtr->isValid(); ++(*cspIterPtr)) {
            ExprHandle param = cspIterPtr->current();

            // determine if dealing with a reference parameter
            bool isRefParam = false;
            OA_ptr<ExprTree> eTreePtr = mIR->getExprTree(param);
            EvalToMemRefVisitor evalVisitor;
            eTreePtr->acceptVisitor(evalVisitor);
            if (debug) { eTreePtr->dump(std::cout,mIR); }
            if ( evalVisitor.isMemRef() ) {
                MemRefHandle memref = evalVisitor.getMemRef();
                if (mParamBind->isRefParam(
                        mParamBind->getCalleeFormal(call,memref)) )
                {
                  isRefParam = true;
                }
            }

            // record expr for those that aren't reference parameters
            if (isRefParam==false) {
                exprSet.insert(param);
                if (debug) { 
                    std::cout << "Inserting param expr into exprSet" 
                              << std::endl;
                    eTreePtr->dump(std::cout,mIR); 
                }
            } 
        }
      }

      // get differentiable locations from all expressions in stmt
      if (debug) { std::cout << "ExprTree's:" << std::endl; }
      DifferentiableLocsVisitor dlVisitor(alias);
      std::set<ExprHandle>::iterator exprIter;
      for (exprIter=exprSet.begin(); exprIter!=exprSet.end(); exprIter++) {
        OA_ptr<ExprTree> etree = mIR->getExprTree(*exprIter);
        if (debug) { etree->dump(std::cout,mIR); }
        etree->acceptVisitor(dlVisitor);
        OA_ptr<LocIterator> locIterPtr 
            = dlVisitor.getDiffLocsIterator();
        for ( ; locIterPtr->isValid(); (*locIterPtr)++ ) {
            diffUseSet.insert(locIterPtr->current());
        }
      }
      
      // store must defs in Dep results as well
      // and remove the associated implicit dep from DepDFSet
      LocSet::iterator useIter, defIter;
      for (defIter=mustDefSet.begin(); defIter!=mustDefSet.end(); defIter++) {
        mDep->insertMustDefForStmt(stmt,*defIter);
        stmtDepDFSet->removeImplicitDep(*defIter,*defIter);
      }
   
      // map all uses to may defs and vice versa for this statement
      // have to do this after removing implicit deps in case it will be
      // explicitly put back in
      OA_ptr<Location> use, def;
      for (useIter=diffUseSet.begin(); useIter!=diffUseSet.end(); useIter++) {
        for (defIter=mayDefSet.begin(); defIter!=mayDefSet.end(); defIter++) {
          stmtDepDFSet->insertDep(*useIter,*defIter);
        }
      }

      // map stmtDepDFSet to stmt in depResults
      mDep->mapStmtToDeps(stmt, stmtDepDFSet);

    } // iterate over stmts
  } // iterate over procedures

  return mDep;
}

  } // end of namespace Activity
} // end of namespace OA
