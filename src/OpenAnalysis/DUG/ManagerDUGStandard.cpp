/*! \file
  
  \brief The AnnotationManager that generates DUG

  \authors Michelle Strout
  \version $Id: ManagerDUGStandard.cpp,v 1.2 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "ManagerDUGStandard.hpp"

#if defined(DEBUG_ALL) || defined(DEBUG_ManagerDUGStandard)
static bool debug = true;
#else
static bool debug = false;
#endif

namespace OA {
  namespace DUG {


/*!
   Visitor over memory reference expressions that creates an
   appropriate Location data structure for any MemRefExpr.
   
   Conservatively handles addressOf (only way UnnamedRefs happen) and 
   derefs.  Therefore resulting mLoc will be UnknownLoc for those.
 */
class CreateLocationVisitor : public virtual MemRefExprVisitor {
  public:
    OA_ptr<Location> mLoc;
    CreateLocationVisitor(OA_ptr<DUGIRInterface> ir,
                          ProcHandle proc) : mIR(ir),mProc(proc) {}
    ~CreateLocationVisitor() {}
    void visitNamedRef(NamedRef& ref) 
      {
          mLoc = mIR->getLocation(mProc,ref.getSymHandle());

          // if the memory reference doesn't have full accuracy then
          // create a partial subset of the base memory location
          if (!ref.hasFullAccuracy()) {
              mLoc = new LocSubSet(mLoc,false);
          }

          // if this is an addressOf just switch it to UnknownLoc
          if (ref.hasAddressTaken()) {
              mLoc = new UnknownLoc();
          }
      }
    void visitUnnamedRef(UnnamedRef& ref) { mLoc = new UnknownLoc; }
    void visitUnknownRef(UnknownRef& ref) { mLoc = new UnknownLoc; }
    void visitDeref(Deref& ref) { mLoc = new UnknownLoc; }
    // default handling of more specific SubSet specificiations
    void visitSubSetRef(SubSetRef& ref) 
      {
          // will set mLoc to our base location
          ref.getMemRefExpr()->acceptVisitor(*this);
          if (mLoc->isaNamed()) {
            mLoc = new LocSubSet(mLoc,false);
          }
          // if this is an addressOf just switch it to UnknownLoc
          if (ref.hasAddressTaken()) {
              mLoc = new UnknownLoc();
          }
      }

  private:
    OA_ptr<DUGIRInterface> mIR;
    ProcHandle mProc;

};

/*!
*/
ManagerDUGStandard::ManagerDUGStandard(OA_ptr<DUGIRInterface> _ir, 
                                       OA_ptr<Activity::ActivityIRInterface> _air) 
    : mIR(_ir), mActIR(_air)
/*!
 */
{ 
}


bool ManagerDUGStandard::stmt_has_call(StmtHandle stmt)
{
  bool callflag = false;
  OA_ptr<IRCallsiteIterator> callsiteItPtr = mIR->getCallsites(stmt);
  for ( ; callsiteItPtr->isValid(); ++(*callsiteItPtr)) {
    ExprHandle call = callsiteItPtr->current();
    SymHandle sym = mIR->getSymHandle(call);
    ProcHandle proc = mIR->getProcHandle(sym);
    if (debug) {
      std::cout << "sym for callee = " 
                << mIR->toString(sym) << std::endl;
    }
    if (proc!=ProcHandle(0)) {
      callflag = true;
    }
  }
  return callflag;
}

/*!
    Creates a DUG for the program
*/
void ManagerDUGStandard::insertEdge( 
    SymHandle        from,
    SymHandle        to,
    EdgeType         etype,
    ExprHandle       expr,
    ProcHandle       fromProc,
    ProcHandle       toProc)
{
  if (from == to) return;
  // duplicate edges
  if (etype == CFLOW_EDGE && mMatrix[etype][from][to]) return;
  mMatrix[etype][from][to] = true;

  OA_ptr<Location> fromLoc = mIR->getLocation(fromProc, from);
  OA_ptr<Location> toLoc = mIR->getLocation(toProc, to);
#if 0
  static const char *sEdgeTypeToString[] = { 
    "CFLOW",
    "CALL",
    "RETURN",
    "PARAM"
  };
  if (etype == PARAM_EDGE){
    std::cout << "insertEdge: ";
    fromLoc->dump(std::cout, mIR);
    std::cout << " -> ";
    toLoc->dump(std::cout, mIR);
    std::cout << " (" << sEdgeTypeToString[etype] << ")" << std::endl;
  }
#endif
  ProcHandle proc = (etype == RETURN_EDGE ? toProc : fromProc);

  OA_ptr<DUGStandard::Node> fromNode = mDUG->getNode(from, fromLoc, fromProc);
  OA_ptr<DUGStandard::Node> toNode   = mDUG->getNode(to, toLoc, toProc);

  // edge between 'from' and 'to' node
  OA_ptr<DUGStandard::Edge> dugEdge;
  dugEdge = new DUGStandard::Edge(mDUG, 
                                  fromNode, toNode, 
                                  etype, expr, proc);
  mDUG->addEdge(dugEdge);
}



/*!
    Creates an DUG for the program
*/
void ManagerDUGStandard::labelCallRetEdges(
  StmtHandle stmt, ProcHandle proc)
{
#ifdef DEBUG_DUAA
  std::cout << "labelCallRetEdges:" << std::endl;
#endif
  // get alias results for this procedure
  OA_ptr<Alias::Interface> alias;
  alias = mInterAlias->getAliasResults(proc);
    
  // for each call to a defined procedure,
  // add a call Edge and return edge
  OA_ptr<IRCallsiteIterator> callsiteItPtr;
  callsiteItPtr = mIR->getCallsites(stmt);
  for ( ; callsiteItPtr->isValid(); ++(*callsiteItPtr)) {
    ExprHandle call = callsiteItPtr->current();
    SymHandle calleesym = mIR->getSymHandle(call);
    ProcHandle callee = mIR->getProcHandle(calleesym);
    // Undefined procedures are not processed
    if (callee==ProcHandle(0)) { continue; }

    // to process only reachable procedures
    mProcsOfInterest.insert(callee);

    mProcToCallsiteSet[callee].insert(call); 
    mCallsiteToProc[call] = proc;

    // iterate over formal parameters for callee procedure
    OA_ptr<SymHandleIterator> formalIter;
    formalIter = mParamBind->getFormalIterator(callee);
    // iterate over actual parameters at the same time
    OA_ptr<IRCallsiteParamIterator> actualIter;
    actualIter = mIR->getCallsiteParams(call);
    for ( ; formalIter->isValid() && actualIter->isValid(); (*formalIter)++, ++(*actualIter)) 
    {
      // formal
      SymHandle formalSym = formalIter->current();
      mProcToFormalSet[callee].insert(formalSym); 

      // actual
      ExprHandle param = actualIter->current();
      OA_ptr<ExprTree> eTreePtr; eTreePtr = mIR->getExprTree(param);
      ExprTree::NodesIterator nodes_iter(*eTreePtr);
      for ( ; nodes_iter.isValid(); ++nodes_iter) {
        OA_ptr<ExprTree::Node> exprTreeNode; 
        exprTreeNode = nodes_iter.current();
        if ( exprTreeNode->isaMemRefNode() ) {
          OA_ptr<ExprTree::MemRefNode> memRefNode;
          memRefNode = exprTreeNode.convert<ExprTree::MemRefNode>();
          MemRefHandle memref = memRefNode->getHandle();

          // get the memory reference expressions for this handle
          OA_ptr<MemRefExprIterator> mreIter;
          mreIter = mIR->getMemRefExprIterator(memref);
      
          // for each mem-ref-expr associated with this memref
          for (; mreIter->isValid(); (*mreIter)++) {
            OA_ptr<OA::MemRefExpr> mre; mre = mreIter->current();

            // only named memref
            if (mre->isaNamed()){
              OA_ptr<NamedRef> namedRef = mre.convert<NamedRef>();
              SymHandle actualSym = namedRef->getSymHandle();

              mFormalToActualMap[call][formalSym].insert(actualSym);
              insertEdge(actualSym, formalSym, CALL_EDGE, call, proc, callee);
              if (mParamBind->isRefParam(formalSym) )
              {
                insertEdge(formalSym, actualSym, RETURN_EDGE, call, callee, proc);
              }
            }
          }
        }
      }
    }
  }
}


/*!
    collect independent variables
*/
void
ManagerDUGStandard::collectIndependentSyms( ProcHandle proc)
{
#if 0
  std::cout << "collectIndependentSyms: ---";
#endif
  // get iterator over indep  locations for procedure
  OA_ptr<LocIterator> indepIter; indepIter = mIR->getIndepLocIter(proc);
  for ( indepIter->reset(); indepIter->isValid(); (*indepIter)++ ) {
    OA_ptr<Location> loc = indepIter->current();
    assert(loc->isaNamed());
    OA_ptr<NamedLoc> namedLoc = loc.convert<NamedLoc>();
    mDUG->insertIndepSymSet(namedLoc->getSymHandle());
    mProcsOfInterest.insert(proc);
#if 0
    std::cout << " ";
    loc->dump(std::cout, mIR);
#endif
  }
#if 0
  std::cout << std::endl;
#endif
}



/*!
    collect dependent variables
*/
void
ManagerDUGStandard::collectDependentSyms( ProcHandle proc)
{
#if 0
  std::cout << "collectDependentSyms: ---";
#endif
  // get iterator over indep  locations for procedure
  OA_ptr<LocIterator> depIter; depIter = mIR->getDepLocIter(proc);
  for ( depIter->reset(); depIter->isValid(); (*depIter)++ ) {
    OA_ptr<Location> loc = depIter->current();
    assert(loc->isaNamed());
    OA_ptr<NamedLoc> namedLoc = loc.convert<NamedLoc>();
    mDUG->insertDepSymSet(namedLoc->getSymHandle());
#if 0
    std::cout << " ";
    loc->dump(std::cout, mIR);
#endif
  }
#if 0
  std::cout << std::endl;
#endif
}



// for each (use, def) pair of a stmt
void ManagerDUGStandard::labelUseDefEdges(
  StmtHandle stmt, ProcHandle proc)
{
#ifdef DEBUG_DUAA
  std::cout << "labelUseDefEdges:" << std::endl;
#endif
  std::set<SymHandle> useSet, defSet, allSyms;

  // collect all locations in the stmt
  OA_ptr<MemRefHandleIterator> mrIterPtr;
  mrIterPtr = mIR->getAllMemRefs(stmt);
  for (; mrIterPtr->isValid(); (*mrIterPtr)++ )
  {
    MemRefHandle memref = mrIterPtr->current();

    // get the memory reference expressions for this handle
    OA_ptr<MemRefExprIterator> mreIter;
    mreIter = mIR->getMemRefExprIterator(memref);
      
    // for each mem-ref-expr associated with this memref
    for (; mreIter->isValid(); (*mreIter)++) {
      OA_ptr<OA::MemRefExpr> mre; mre = mreIter->current();

      // only named memref
      if (mre->isaNamed()){
        OA_ptr<NamedRef> namedRef = mre.convert<NamedRef>();
        SymHandle sym = namedRef->getSymHandle();

        allSyms.insert(sym);
      }
    }
  }

  // collect uses
  mrIterPtr = mIR->getUseMemRefs(stmt);
  for (; mrIterPtr->isValid(); (*mrIterPtr)++ ) {
    MemRefHandle mref = mrIterPtr->current();

    // get the memory reference expressions for this handle
    OA_ptr<MemRefExprIterator> mreIter; 
    mreIter = mIR->getMemRefExprIterator(mref);
      
    // for each mem-ref-expr associated with this memref
    for (; mreIter->isValid(); (*mreIter)++) {
      OA_ptr<OA::MemRefExpr> mre; mre = mreIter->current();

      if (mre->isaNamed()){
        OA_ptr<NamedRef> namedRef = mre.convert<NamedRef>();
        SymHandle use = namedRef->getSymHandle();

        useSet.insert(use);
      }
    }
  }

  // collect defs
  mrIterPtr = mIR->getDefMemRefs(stmt);
  for (; mrIterPtr->isValid(); (*mrIterPtr)++ ) {
    MemRefHandle mref = mrIterPtr->current();

    // get the memory reference expressions for this handle
    OA_ptr<MemRefExprIterator> mreIter; 
    mreIter = mIR->getMemRefExprIterator(mref);
      
    // for each mem-ref-expr associated with this memref
    for (; mreIter->isValid(); (*mreIter)++) {
      OA_ptr<OA::MemRefExpr> mre; mre = mreIter->current();


      if (mre->isaNamed()){
        OA_ptr<NamedRef> namedRef = mre.convert<NamedRef>();
        SymHandle def = namedRef->getSymHandle();

        if (allSyms.find(def) != allSyms.end())
        {
          defSet.insert(def);
        }
      }
    }
  }

  // map all uses to may defs and vice versa for this statement
  // have to do this after removing implicit deps in case it will be
  // explicitly put back in
  std::set<SymHandle>::iterator useIter, defIter;
  for (useIter=useSet.begin(); useIter!=useSet.end(); useIter++) {
    SymHandle use = *useIter;
    for (defIter=defSet.begin(); defIter!=defSet.end(); defIter++) {
      SymHandle def = *defIter;
      insertEdge(use, def, CFLOW_EDGE, ExprHandle(0), proc, proc);

      // build dependence matrix
      setDepMatrix(proc, use, def);
    }
  }
}


/*!
  Creates a DUG for the program
*/
OA_ptr<DUGStandard> ManagerDUGStandard::performAnalysis( 
  OA_ptr<IRProcIterator> procIter,
  OA_ptr<ICFG::ICFGStandard> icfg,
  OA_ptr<DataFlow::ParamBindings> paramBind,
  OA_ptr<OA::Alias::InterAliasMap> interAlias,
  OA_ptr<OA::CallGraph::CallGraphStandard> cgraph)
{

  mInterAlias = interAlias;
  mParamBind  = paramBind;

  // create a Manager that generates dep information for each statement in
  // ICFG
  OA_ptr<Activity::ManagerICFGDep> depman;
  depman = new Activity::ManagerICFGDep(mActIR);
  mICFGDep = depman->performAnalysis(icfg, paramBind, interAlias);
  OA_ptr<DUGStandard> dug; mDUG = dug = new DUGStandard(mICFGDep, mIR);
  
#if 0
  std::cout << "ManagerDUGStandard::performAnalysis: ---" << std::endl;
#endif
  // Mark 'independent' and 'dependent' variables
  for ( procIter->reset(); procIter->isValid(); (*procIter)++ ) {
    collectIndependentSyms(procIter->current());
    collectDependentSyms  (procIter->current());
  }

  OA_ptr<OA::CallGraph::CallGraphStandard::ReversePostDFSIterator> callGraphIter;
  callGraphIter = cgraph->getReversePostDFSIterator(DGraph::DEdgeOrg);

  for ( ; callGraphIter->isValid(); ++(*callGraphIter)) {
    OA_ptr<CallGraph::CallGraphStandard::Node> node; 
    node = callGraphIter->current();
#if 0
    node->dump(std::cout, mIR);
#endif
    ProcHandle proc = node->getProc();
    // skip unreachable procedures
    if(mProcsOfInterest.find(proc) == mProcsOfInterest.end()) continue;
#ifdef DEBUG_DUAA_LAST
    if(mProcsOfInterest.find(proc) == mProcsOfInterest.end()) {
      std::cout << "Not Interesting: ";
      node->dump(std::cout, mIR);
      std::cout << std::endl;
    }
#endif  

    OA_ptr<OA::IRStmtIterator> sItPtr; 
    sItPtr = mIR->getStmtIterator(proc);
    for ( ; sItPtr->isValid(); (*sItPtr)++) {
      StmtHandle stmt = sItPtr->current();
      labelUseDefEdges(stmt, proc);
      if (stmt_has_call(stmt)){
        labelCallRetEdges(stmt, proc);
      }
    }
  }

  return dug;
}




/*!
  Set the dependence matrix entry to 'true'
*/
void ManagerDUGStandard::setDepMatrix( 
  ProcHandle proc, 
  SymHandle  use, 
  SymHandle  def)
{
  if (use == def) return;
  mProcToSymSet[proc].insert(use);
  mProcToSymSet[proc].insert(def);
  mProcToMatrix[proc][use][def] = true;
#if 0
  std::cout << mIR->toString(proc) << ": ";
  OA_ptr<Location> useLoc = mIR->getLocation(proc, use);
  useLoc->dump(std::cout, mIR);
  std::cout << " ---> ";
  OA_ptr<Location> defLoc = mIR->getLocation(proc, def);
  defLoc->dump(std::cout, mIR);
  std::cout << std::endl;
#endif
}




/*!
  Warshall's transitive closure algorithm on the dependence matrix
*/
void ManagerDUGStandard::transitiveClosureDepMatrix(
  OA_ptr<OA::CallGraph::CallGraphStandard> cgraph
  )
{
  // use 'DEdgeRev' to get PostDFSIterator
  // 'getPostDFSIterator' is not implemented
  OA_ptr<OA::CallGraph::CallGraphStandard::ReversePostDFSIterator> iter;
  iter = cgraph->getReversePostDFSIterator(DGraph::DEdgeRev);

  for ( ; iter->isValid(); ++(*iter)) {
    OA_ptr<CallGraph::CallGraphStandard::Node> node; node = iter->current();
#if 0
    node->dump(std::cout, mIR);
    std::cout << " --------------" << std::endl;
#endif
    ProcHandle proc = node->getProc();
    transitiveClosure(proc);
    edgesBetweenActuals(proc);
  }
}



/*!
  Warshall's transitive closure algorithm on the dependence matrix 
  of one procedure. This makes edges between formal parameters too.
*/
void ManagerDUGStandard::transitiveClosure(ProcHandle proc)
{
  std::map<SymHandle, std::map<SymHandle, bool> >&
    depMat = mProcToMatrix[proc];

  std::set<SymHandle>& symSet = mProcToSymSet[proc];
  std::set<SymHandle>::iterator i, j, k;
  SymHandle use, def, var;
  for (i=symSet.begin(); i!=symSet.end(); i++){
    use = *i;
    for (j=symSet.begin(); j!=symSet.end(); j++){
      def = *j;
      if (use != def && depMat[use][def]){
#if 0
        std::cout << "\t" << mIR->toString(proc) << ": ";
        OA_ptr<Location> useLoc = mIR->getLocation(proc, use);
        useLoc->dump(std::cout, mIR);
        std::cout << " ---> ";
        OA_ptr<Location> defLoc = mIR->getLocation(proc, def);
        defLoc->dump(std::cout, mIR);
        std::cout << "(true)" << std::endl;
#endif
        for (k=symSet.begin(); k!=symSet.end(); k++){
          var = *k;
          depMat[use][var] = depMat[use][var] || depMat[def][var];
        }
      }
    }
  }
}




/*!
  This makes edges between actual parameters.
*/
void ManagerDUGStandard::edgesBetweenActuals(ProcHandle proc)
{
  std::set<ExprHandle>& callsites = mProcToCallsiteSet[proc];
  std::set<ExprHandle>::iterator callIter;

  std::set<SymHandle>& formals = mProcToFormalSet[proc];
  std::set<SymHandle>::iterator i, j;
  
  for (i=formals.begin(); i!=formals.end(); i++){
    SymHandle formal1 = *i;
    for (j=formals.begin(); j!=formals.end(); j++){
      SymHandle formal2 = *j;
#if 0
      if (formal1 != formal2 && !mProcToMatrix[proc][formal1][formal2]){
        std::cout << "\t" << mIR->toString(proc) << ": ";
        OA_ptr<Location> formal1Loc = mIR->getLocation(proc, formal1);
        formal1Loc->dump(std::cout, mIR);
        std::cout << " ---> ";
        OA_ptr<Location> formal2Loc = mIR->getLocation(proc, formal2);
        formal2Loc->dump(std::cout, mIR);
        std::cout << "(false)" << std::endl;
      }
#endif
      if (formal1 != formal2 && mProcToMatrix[proc][formal1][formal2]){
        insertEdge(formal1, formal2, PARAM_EDGE, ExprHandle(0), proc, proc);

        for (callIter = callsites.begin(); callIter!=callsites.end(); callIter++){
          ExprHandle call = *callIter;
          // get calling procedure
          ProcHandle caller = mCallsiteToProc[call];
          assert(caller != ProcHandle(0));
          std::set<SymHandle>& set1 = mFormalToActualMap[call][formal1];
          std::set<SymHandle>& set2 = mFormalToActualMap[call][formal2];
          std::set<SymHandle>::iterator i1;
          std::set<SymHandle>::iterator i2;
          for (i1=set1.begin(); i1 != set1.end(); i1++){
            for (i2=set2.begin(); i2 != set2.end(); i2++){
              setDepMatrix(caller, *i1, *i2);
            }
          }
        }
      }
    }
  }
}


  } // end of namespace MPDUG
} // end of namespace OA
