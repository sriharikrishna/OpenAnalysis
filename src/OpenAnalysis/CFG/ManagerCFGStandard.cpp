/*! \file
  
  \brief The AnnotationManager that generates a CFGStandard.

  \authors Arun Chauhan (2001 as part of Mint), Nathan Tallent, Michelle Strout
  \version $Id: ManagerCFGStandard.cpp,v 1.21 2005/10/17 21:21:41 garvin Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

// standard headers

#ifdef NO_STD_CHEADERS
# include <stdlib.h>
# include <string.h>
# include <assert.h>
#else
# include <cstdlib>
# include <cstring>
# include <cassert>
using namespace std; // For compatibility with non-std C headers
#endif

#include <iostream>
using std::ostream;
using std::endl;
using std::cout;
using std::cerr;
#include <list>
#include <set>
#include <map>

// Mint headers
#include "ManagerCFGStandard.hpp"


namespace OA {
  namespace CFG {

static bool debug = false;

//*****************************************************************************
// Declarations of auxilliary data structures to support construction of CFGs
// with delay slots
//*****************************************************************************

class Branch_Counter {
public:
  Branch_Counter(): branchInstruction(0 /* NULL */), count(-1) {}
  Branch_Counter(StmtHandle a, int c): branchInstruction(a), count(c) {}

public:
  StmtHandle branchInstruction;
  int count;
};



struct compareBranchCounters {

  bool operator() (const OA_ptr<Branch_Counter> b1, 
                   const OA_ptr<Branch_Counter> b2) const {
    return ((b1->branchInstruction != b2->branchInstruction) && 
            (b1->count == b2->count));
  }
};


typedef std::set<OA_ptr<Branch_Counter>,compareBranchCounters>::iterator 
    CounterlistIterator;


class Counterlist {
public:
  Counterlist();
  Counterlist(CounterlistIterator, CounterlistIterator);


  void decrementCounters();
  void addCounter(StmtHandle, int);
  void addCounters(OA_ptr<Counterlist> );

  CounterlistIterator getFirstNonZeroCounter();
  
  void addCounters(std::list<OA_ptr<Branch_Counter> >);
  
  int size();
  void reset();

  OA_ptr<Branch_Counter> getLeastCounter();
  
  CounterlistIterator begin();
  CounterlistIterator end();

private:
  std::set<OA_ptr<Branch_Counter>, compareBranchCounters> counters;
};


  
class Worklist {
public:
  void copyCountersToWorklist(OA_ptr<CFGStandard::Node>, OA_ptr<Counterlist>);
  
  void addCounterToWorklist(OA_ptr<CFGStandard::Node>, StmtHandle, int);
  
  OA_ptr<Counterlist> getCounterlist(OA_ptr<CFGStandard::Node>);
  
  void eraseCounterlist(OA_ptr<CFGStandard::Node>);
  bool isEmpty();

private:
  std::map<OA_ptr<CFGStandard::Node>, OA_ptr<Counterlist> > allElements;
};


//*****************************************************************************


//--------------------------------------------------------------------
    ManagerStandard::ManagerStandard (OA_ptr<CFGIRInterface> _ir,
				      bool _build_stmt_level_cfg /* = false */ )
  : mIR(_ir), mBuildStmtLevelCFG(_build_stmt_level_cfg)
/*!
 * \brief Constructs a ManagerStandard
 *
 * \param _ir    		CFGIRInteface implementation
 * \param _build_stmt_level_cfg	whether to build a statement-level CFG
 */
{
}
    
//! for debugging
void dumpCFG(OA_ptr<CFGStandard> cfg)
{
    std::cout << "dumpCFG( " << cfg << "): " << std::endl;
    for (CFGStandard::NodesIterator nodeIt(*cfg); nodeIt.isValid(); ++nodeIt)
    {
        OA_ptr<CFGStandard::Node> n = nodeIt.current();
        std::cout << n << " ";
    }
    std::cout << std::endl;
}

//! Create and return CFGStandard for given procedure
OA_ptr<CFG::CFGStandard> ManagerStandard::performAnalysis(ProcHandle proc)
{
  // from the ir interface get the function symbol handle,
  // and other information
  bool return_statements_allowed = mIR->returnStatementsAllowed();
  SymHandle procName = mIR->getProcSymHandle(proc);

  // create a CFG that just has an entry and an exit node
  mCFG = new CFGStandard(procName);

  // Create the unique entry node.
  OA_ptr<CFGStandard::Node> entry; entry = new CFGStandard::Node();
  mCFG->setEntry(entry);
  mCFG->addNode(entry);

  // Create the actual start node where the first statements wil be added.
  OA_ptr<CFGStandard::Node> r; r = new CFGStandard::Node();
  mCFG->addNode(r);
  mCFG->connect (mCFG->getEntry(), r, CFG::Interface::FALLTHROUGH_EDGE);

  CFGStandard::NodeLabelList exit_nodes;
  // some NodeLabelList's are OA_ptr's instead of passed around by reference
  // because some of the logic looks to see if the list is NULL
  OA_ptr<CFGStandard::NodeLabelList> return_nodes;
  return_nodes = new CFGStandard::NodeLabelList;
  OA_ptr<IRRegionStmtIterator> stmt_iterptr = mIR->procBody(proc);
  OA_ptr<CFGStandard::NodeLabelList> nullList; nullList = NULL;
  if (return_statements_allowed)
  {
    build_block(r, stmt_iterptr, exit_nodes, nullList, return_nodes, nullList);
  } else {
    build_block(r, stmt_iterptr, exit_nodes, nullList, nullList, nullList);
  }

  // create unique exit node
  OA_ptr<CFGStandard::Node> final; final = new CFGStandard::Node();  
  mCFG->addNode(final);
  mCFG->setExit(final);
  
  // connect the exit nodes to the final node
  mCFG->connect(exit_nodes, final); 
  // return nodes also lead to the exit node of the sub-program
  mCFG->connect(*return_nodes, final); 

  // finalize control flow for unstructured constructs
  HandleDelayedBranches();

  return mCFG;
}

ManagerStandard::~ManagerStandard()
/*!
 * \brief Destructor for standard CFGStandard AnnotationManager
 */
{
  mThe_worklist = NULL; 
  mFallThrough.clear();
}

//--------------------------------------------------------------------
// Private construction methods
//--------------------------------------------------------------------
IRStmtType
ManagerStandard::build_block (OA_ptr<CFGStandard::Node> prev_node, 
                              OA_ptr<IRRegionStmtIterator> si_ptr,
                              CFGStandard::NodeLabelList& exit_nodes,
                              OA_ptr<CFGStandard::NodeLabelList> break_nodes,
                              OA_ptr<CFGStandard::NodeLabelList> return_nodes,
                              OA_ptr<CFGStandard::NodeLabelList> continue_nodes)
{
    CFGStandard::NodeLabelList x_nodes;
    IRStmtType prev_statement = SIMPLE;
    while (si_ptr->isValid()) {
        StmtHandle stmt = si_ptr->current();
        if (debug) { std::cout << mIR->toString(stmt) << std::endl; }
        if (mIR->getLabel(stmt)) {
	    // FIXME: Unstructured handling (labels), partially tested.  If
	    // this statement has a label, it begins a new block (if prev_node
	    // is empty, we don't need to create another new block).  The new
	    // block will have already been created if the label was
            // referenced by a previously encountered statement.
	    StmtLabel lab = mIR->getLabel(stmt);
	    OA_ptr<CFGStandard::Node> new_node; 
	    if (mCFG->isLabelMappedToNode(lab)) {
                new_node = mCFG->node_from_label(lab);
            } else if (! prev_node->empty()) {
                new_node = new CFGStandard::Node();
                mCFG->addNode(new_node);
                mCFG->mapLabelToNode(lab,new_node);
            } else {
                mCFG->mapLabelToNode(lab,prev_node);
                new_node = prev_node;
            }
	    
            if (prev_statement != SIMPLE && !new_node.ptrEqual(prev_node)) {
                mCFG->connect(x_nodes, new_node);
            } else if (!prev_node.ptrEqual(NULL) 
                       && !new_node.ptrEqual(prev_node)) 
            {
                mCFG->connect(prev_node, new_node, Interface::FALLTHROUGH_EDGE);
            }
            x_nodes.clear();
            prev_node = new_node;
	} else if (mBuildStmtLevelCFG) {
	    if (!prev_node->empty()) {
		OA_ptr<CFGStandard::Node> new_node;
		new_node = new CFGStandard::Node();
		mCFG->addNode(new_node);
		if (prev_statement == SIMPLE) {
		    mCFG->connect(prev_node, new_node, Interface::FALLTHROUGH_EDGE);
		} else {
		    mCFG->connect(x_nodes, new_node);
		}
		prev_node = new_node;
	    }
	    x_nodes.clear();
	} else if (prev_statement != SIMPLE) {
	    // now we need to create a new CFG node
	    prev_node = new CFGStandard::Node();
	    mCFG->addNode(prev_node);
	    mCFG->connect(x_nodes, prev_node);
	    x_nodes.clear();
	}
        prev_statement = build_stmt(prev_node, stmt, x_nodes, break_nodes, 
                                    return_nodes, continue_nodes);
        ++(*si_ptr);
    }

  if (prev_statement == SIMPLE) {
    exit_nodes.push_back(CFGStandard::NodeLabel(prev_node, 
                         Interface::FALLTHROUGH_EDGE));
  } else {
    CFGStandard::NodeLabelListIterator x_nodes_iter(x_nodes);
    while (x_nodes_iter.isValid()) {
      exit_nodes.push_back(x_nodes_iter.current());
      ++x_nodes_iter;
    }
  }

  return NONE;
}


//--------------------------------------------------------------------
/*! 
   Build a CFGStandard for the IR rooted at the given node.  The CFG builder
   depends upon the IR providing a minimal interface.  In particular, the CFG
   needs for the IR nodes to be able to categorize themselves as simple
   statements, statement lists, loops, two-way branches, and multi-way
   branches.

   The routine uses the given node prev_node as the entry node and updates the
   exit_nodes for the CFG that it builds.  Notice that for every CFG component
   built by this call, there is exactly one entry node, but there may be
   multiple exit nodes.
*/
IRStmtType
ManagerStandard::build_stmt (OA_ptr<CFGStandard::Node> prev_node, 
    OA::StmtHandle stmt, 
    CFGStandard::NodeLabelList& exit_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> break_nodes,
    OA_ptr<CFGStandard::NodeLabelList> return_nodes, 
    OA_ptr<CFGStandard::NodeLabelList> continue_nodes)
  throw (ManagerStandard::Unexpected_Break, 
	 ManagerStandard::Unexpected_Return, 
	 ManagerStandard::Unexpected_Continue)
{
    // handle different types of nodes. 
    switch (mIR->getCFGStmtType(stmt)) {
    case SIMPLE:
      prev_node->add(stmt);
      if (debug) { std::cout << mIR->toString(stmt) << std::endl; }
      return SIMPLE;
    case COMPOUND:
      build_block(prev_node, mIR->getFirstInCompound(stmt), exit_nodes, 
                  break_nodes, return_nodes, continue_nodes);
      return COMPOUND;
    case BREAK:
      prev_node->add(stmt);
      if (break_nodes.ptrEqual(0)) {
        throw Unexpected_Break();
      } else {
        break_nodes->push_back(CFGStandard::NodeLabel(prev_node, 
                               Interface::BREAK_EDGE));
      }
      return BREAK;
    case RETURN:
      prev_node->add(stmt);
      if (return_nodes.ptrEqual(0)) {
        throw Unexpected_Return();
      } else {
        return_nodes->push_back(CFGStandard::NodeLabel(prev_node, 
                                Interface::RETURN_EDGE));
      }
      return RETURN;
    case LOOP:
      return build_CFG_loop (prev_node, stmt, exit_nodes, return_nodes);
    case STRUCT_TWOWAY_CONDITIONAL:
      return build_CFG_twoway_branch (prev_node, stmt, exit_nodes, break_nodes,
                                      return_nodes, continue_nodes);
    case STRUCT_MULTIWAY_CONDITIONAL:
      if (mIR->isBreakImplied(stmt)) {
        return build_CFG_multiway_branch (prev_node, stmt, exit_nodes, 
                                          break_nodes, return_nodes, 
                                          continue_nodes);
      } else {
        return build_CFG_multiway_branch_with_fallthrough (prev_node, stmt, 
                                          exit_nodes,
                                          return_nodes, continue_nodes);
      }
    case END_TESTED_LOOP:
      return build_CFG_end_tested_loop (prev_node, stmt, exit_nodes, return_nodes);
    case LOOP_CONTINUE:
      prev_node->add(stmt);
      if (continue_nodes.ptrEqual(0)) {
        throw Unexpected_Continue();
      } else {
        continue_nodes->push_back(CFGStandard::NodeLabel(prev_node, 
                                  Interface::CONTINUE_EDGE));
      }
      return LOOP_CONTINUE;
    //
    // These unstructured constructs have the possibility of being
    // delayed branches (depending on the underlying machine). Those
    // without delay slots are handled now.  Those with delay slots
    // are just added as simple statements.  A post-processing step
    // will finalize control flow for them.
    //
    case UNCONDITIONAL_JUMP:
      if (mIR->numberOfDelaySlots(stmt) == 0) {
        return build_CFG_unconditional_jump (prev_node, stmt);
      } else { 
        prev_node->add(stmt);
        return SIMPLE;
      }
    case USTRUCT_TWOWAY_CONDITIONAL_T:
    case USTRUCT_TWOWAY_CONDITIONAL_F:
      if (mIR->numberOfDelaySlots(stmt) == 0) {
        return build_CFG_ustruct_twoway_branch (prev_node, stmt, exit_nodes);
      } else {
        prev_node->add(stmt);
        return SIMPLE;
      }
    case UNCONDITIONAL_JUMP_I:
      if (mIR->numberOfDelaySlots(stmt) == 0) {
        return build_CFG_unconditional_jump_i (prev_node, stmt);
      } else {
        prev_node->add(stmt);
        return SIMPLE;
      }
    case USTRUCT_MULTIWAY_CONDITIONAL:
      // Currently assume multiways don't have delay slots.
      assert(mIR->numberOfDelaySlots(stmt) == 0);
      return build_CFG_ustruct_multiway_branch (prev_node, stmt);

    // FIXME: Unimplemented.
    case ALTERNATE_PROC_ENTRY:
      cout << "FIXME: ALTERNATE_PROC_ENTRY not yet implemented" << endl;
      assert (0);
      return ALTERNATE_PROC_ENTRY;
    default:
      assert (0);
    }
    assert (0); // eraxxon: eliminate compiler warnings about missing return 
    return (NONE);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! 
   The general structure of a top-tested loop CFG (e.g., FORTRAN DO statement,
   C for statement, while statements, etc.) is as below:
    <PRE>
              |
              V
          header (optional)
              |      _______
              |     /       \  
              V    V         |
           condition         | 
     ________/|              |
    /         |              |
    |         V              |
    |       body             |
    |         |              |
    |         |              |
    |         V              |
    |   increment (optional) |
    |         \_____________/
    |
     \________
              |
              V
    </PRE>
*/
IRStmtType
ManagerStandard::build_CFG_loop(OA_ptr<CFGStandard::Node> prev_node, 
                                StmtHandle th, 
                                CFGStandard::NodeLabelList& exit_nodes,
				OA_ptr<CFGStandard::NodeLabelList> return_nodes)
{
  // If there is a header, (as in a "for" loop), it is assimilated in the
  // previous (prev_node) CFG node.  The condition needs a separate CFG node.
  // So, unless prev_node is empty (in which case that can become the
  // condition node) a new CFG node must be allocated for the loop condition.
  OA_ptr<CFGStandard::Node> c;

  StmtHandle header = mIR->loopHeader(th);
  if (header) {
    prev_node->add(header);
  }

  // To handle C and Fortran loops in the simplest way we take the following
  // measures:
  //
  // If the number of loop iterations is defined
  // at loop entry (i.e. Fortran semantics), we add the loop statement
  // representative to the header node so that definitions from inside
  // the loop don't reach the condition and increment specifications
  // in the loop statement.
  //
  // On the other hand, if the number of iterations is not defined at
  // entry (i.e. C semantics), we add the loop statement to a node that
  // is inside the loop in the CFG so definitions inside the loop will 
  // reach uses in the conditional test. for C style semantics, the 
  // increment itself may be a separate statement. if so, it will appear
  // explicitly at the bottom of the loop. 
  bool definedAtEntry =  mIR->loopIterationsDefinedAtEntry(th);
  if (definedAtEntry) {
    prev_node->add(th); 
  }
  if (prev_node->empty()) {
    prev_node->add(th); 
    c = prev_node;
  }
  else {
    c = new CFGStandard::Node(th);
    mCFG->connect(prev_node, c, Interface::FALLTHROUGH_EDGE);
  }
  exit_nodes.push_back(CFGStandard::NodeLabel(c, Interface::FALSE_EDGE));

  // allocate a new CFG node for the loop body and recur
  CFGStandard::NodeLabelList x_nodes;
  OA_ptr<IRRegionStmtIterator> body = mIR->loopBody(th);
  if (body->isValid() && body->current()) {
    OA_ptr<CFGStandard::Node> b; b = new CFGStandard::Node();
    mCFG->addNode(b);
    OA_ptr<CFGStandard::NodeLabelList> break_statements, continue_statements;
    break_statements = new CFGStandard::NodeLabelList;
    continue_statements = new CFGStandard::NodeLabelList;
    if (build_block(b, body, x_nodes, break_statements, return_nodes, 
                    continue_statements) == SIMPLE)
    {
      x_nodes.push_back(CFGStandard::NodeLabel(b, Interface::FALLTHROUGH_EDGE));
    }

    mCFG->connect(c, b, Interface::TRUE_EDGE);
    CFGStandard::NodeLabelListIterator break_iter(*break_statements);
    while (break_iter.isValid()) {
      exit_nodes.push_back(break_iter.current());
      ++break_iter;
    }

    CFGStandard::NodeLabelListIterator continue_iter(*continue_statements);
    while (continue_iter.isValid()) {
      x_nodes.push_back(continue_iter.current());
      ++continue_iter;
    }
  } else {// empty body
    x_nodes.push_back(CFGStandard::NodeLabel(c, Interface::TRUE_EDGE));
  }

  // allocate a new CFG node for the increment part, if needed
  StmtHandle incr = mIR->getLoopIncrement(th);
  if (incr) {
    OA_ptr<CFGStandard::Node> li; li = new CFGStandard::Node(incr);
    mCFG->connect(x_nodes, li);
    mCFG->connect(li, c, Interface::FALLTHROUGH_EDGE);
  }
  else {
    mCFG->connect(x_nodes, c);
  }

  return LOOP;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! 
   The general structure of an end-tested loop (e.g., do-while in C)
   CFG is as below:
    <PRE>
              |
              |   ______
              |  /      \  
              V V        |
             body        | 
              |          |
              |          |
              V          |
          condition      |
              |\________/
              | 
              | 
              |
              V

    </PRE>
    The statement representing the do-while loop will be used to
    represent the condition block in the CFG.
*/
IRStmtType
ManagerStandard::build_CFG_end_tested_loop (OA_ptr<CFGStandard::Node> prev_node,
        StmtHandle th, CFGStandard::NodeLabelList& exit_nodes,
        OA_ptr<CFGStandard::NodeLabelList> return_nodes)
{
  // FIXME: New construct, partially tested.
  // New CFG nodes must be allocated for both the body and loop condition (we can re-use
  // prev_node for the body if it is empty).
  OA_ptr<CFGStandard::Node> b; b = NULL;
  OA_ptr<CFGStandard::Node> c; c = NULL;

  // Process the loop body.
  CFGStandard::NodeLabelList x_nodes;
  OA_ptr<IRRegionStmtIterator> body = mIR->loopBody(th);
  if (body->isValid() && body->current()) {
    if (prev_node->empty()) {
      b = prev_node;
    } else {
      b = new CFGStandard::Node();
      mCFG->addNode(b);
      mCFG->connect(prev_node, b, Interface::FALLTHROUGH_EDGE);
    }
    OA_ptr<CFGStandard::NodeLabelList> break_statements, continue_statements;
    break_statements = new CFGStandard::NodeLabelList;
    continue_statements = new CFGStandard::NodeLabelList;
    if (build_block(b, body, x_nodes, break_statements, return_nodes, continue_statements) == SIMPLE) {
      x_nodes.push_back(CFGStandard::NodeLabel(b, Interface::FALLTHROUGH_EDGE));
    }

    CFGStandard::NodeLabelListIterator break_iter(*break_statements);
    while (break_iter.isValid()) {
      exit_nodes.push_back(break_iter.current());
      ++break_iter;
    }

    CFGStandard::NodeLabelListIterator continue_iter(*continue_statements);
    while (continue_iter.isValid()) {
      x_nodes.push_back(continue_iter.current());
      ++continue_iter;
    }
  }
  else { // empty body
    // FIXME: c is NULL at this point...
    x_nodes.push_back(CFGStandard::NodeLabel(c, Interface::TRUE_EDGE));
  }

  // Allocate a node for the condition and make the proper connections.
  c = new CFGStandard::Node(th);
  exit_nodes.push_back(CFGStandard::NodeLabel(c, Interface::FALSE_EDGE));
  mCFG->connect(c, b, Interface::TRUE_EDGE);
  mCFG->connect(x_nodes, c);

  return END_TESTED_LOOP;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! 
   A two-way branch consists of a condition along with a true branch
   and a false branch.  Note that "elseif" statements should be
   translated to this form while parsing, since, they are semantically
   equivalent to a chain of nested "else if" statements.  Indeed,
   "elseif" is just a short form for "else if".  The general structure
   of the CFG for a two-way branch is as follows:

    <PRE>
        |
        V
    condition
      |      \ 
      V       V
   true_body false_body
      |          |
       \________/
           |
           V
    </PRE>
*/
IRStmtType
ManagerStandard::build_CFG_twoway_branch (OA_ptr<CFGStandard::Node> prev_node,
        StmtHandle th, CFGStandard::NodeLabelList& exit_nodes,
        OA_ptr<CFGStandard::NodeLabelList> break_nodes, 
        OA_ptr<CFGStandard::NodeLabelList> return_nodes, 
        OA_ptr<CFGStandard::NodeLabelList> continue_nodes)
{
  // add the if statement itself to the previous block. it represents 
  // the conditional branch terminating the block.
  prev_node->add(th); 

  // construct the CFG for the true block
  OA_ptr<CFGStandard::Node> b;
  OA_ptr<IRRegionStmtIterator> true_body = mIR->trueBody(th);
  if (true_body->isValid() && true_body->current()) {
    b = new CFGStandard::Node();
    mCFG->addNode(b);
    if (build_block(b, true_body, exit_nodes, break_nodes, return_nodes, continue_nodes) == SIMPLE)
      exit_nodes.push_back(CFGStandard::NodeLabel(b, Interface::FALLTHROUGH_EDGE));

    mCFG->connect(prev_node, b, Interface::TRUE_EDGE);
  }
  else {
    // the body is empty
    b = 0;
    exit_nodes.push_back(CFGStandard::NodeLabel(prev_node, Interface::TRUE_EDGE));
  }

  // handle the false block
  OA_ptr<IRRegionStmtIterator> false_body = mIR->elseBody(th);
  if (false_body->isValid() && false_body->current()) {
    OA_ptr<CFGStandard::Node> c_body; c_body = new CFGStandard::Node();
    mCFG->addNode(c_body);
    if (build_block(c_body, false_body, exit_nodes, break_nodes, return_nodes, continue_nodes) == SIMPLE)
      exit_nodes.push_back(CFGStandard::NodeLabel(c_body, Interface::FALLTHROUGH_EDGE));

    mCFG->connect(prev_node, c_body, Interface::FALSE_EDGE);
  }
  else if (!b.ptrEqual(0)) // if b is 0 then the "condition" node has already been added to exit_nodes
    exit_nodes.push_back(CFGStandard::NodeLabel(prev_node, Interface::FALSE_EDGE));

  return STRUCT_TWOWAY_CONDITIONAL;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! 
   A multi-way branch consists of a multiway condition expression that
   branches off into a number of cases, and an optional catchall
   (default) body.  The structure of a multi-way branch is as follows:

    <PRE>
                |
                |
                V
         ___condition______
        /  /  |  \ ...\    \ 
       /  /   |   \    \    \ 
      V  V    V    V    V    V
     B1  B2   B3  B4    Bk  catchall
      |  |    |    |    |    |
      \__|____|____|____|___/
                |
                |
                V
    </PRE>
*/
IRStmtType
ManagerStandard::build_CFG_multiway_branch (OA_ptr<CFGStandard::Node> prev_node,
        StmtHandle th, CFGStandard::NodeLabelList& exit_nodes,
        OA_ptr<CFGStandard::NodeLabelList> break_nodes, 
        OA_ptr<CFGStandard::NodeLabelList> return_nodes, 
        OA_ptr<CFGStandard::NodeLabelList> continue_nodes)
{
  // add the switch statement to the previous block. the switch statement
  // represents the multi-way conditional branch terminating the block.
  prev_node->add(th); 

  bool direct_drop = false;    // should there be a direct edge from the condition to exit?
  bool default_cond = false;   // is there a default case?

  // iterate over the multi-branches
  for (int bridx = 0; bridx < mIR->numMultiCases(th); bridx++) {
    OA_ptr<IRRegionStmtIterator> body = mIR->multiBody(th, bridx);

    // if there is an empty body then just make a direct drop, this is correct since
    // break is always implied
    if (body->isValid() && body->current()) {
      OA_ptr<CFGStandard::Node> c; c = new CFGStandard::Node();
      mCFG->addNode(c);
      if (build_block(c, body, exit_nodes, break_nodes, return_nodes, continue_nodes) == SIMPLE)
      {
        exit_nodes.push_back(CFGStandard::NodeLabel(c, Interface::FALLTHROUGH_EDGE));
      }

      // check if this is the catch all case
      if (mIR->isCatchAll(th,bridx)) {
        default_cond = true;
        mCFG->connect(prev_node, c, Interface::MULTIWAY_EDGE);
      } else {
        ExprHandle multiCond = mIR->getSMultiCondition(th, bridx);
        mCFG->connect(prev_node, c, Interface::MULTIWAY_EDGE, multiCond);
      }

    // there is an empty body with an implied break
    } else {
      direct_drop = true;
    }

  } /* for */

  // if we didn't find a catchall then there should be a direct drop
  if (default_cond == false) {
    direct_drop = true;
  }

  // direct drop is true iff there is not catchall or at least one multi-branch body is null
  if (direct_drop) {
    exit_nodes.push_back(CFGStandard::NodeLabel(prev_node, Interface::FALLTHROUGH_EDGE));
  }

  return STRUCT_MULTIWAY_CONDITIONAL;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! 
   This is similar to build_CFG_multiway_branch, but it handles switch
   statements that have fall-through semantics for each case (that is,
   there are no implied breaks).  Each case can fall-through to the
   following case if an explicit control flow statement (e.g., break
   in C) is not present.  Also, unlike build_CFG_multiway_branch, the
   catchall/default case is not necessarily the last.  The functions
   are different enough that is seems cleaner to keep them separate.

   FIXME: The default case is still assumed to be last. All of the
   underlying infrastructure of mint assumes this and has to be fixed.
   MMS 12/03: this function no longer assumes that the default case is
   last, not sure what other code might still make this assumption
*/
IRStmtType
ManagerStandard::build_CFG_multiway_branch_with_fallthrough (
        OA_ptr<CFGStandard::Node> prev_node, 
        StmtHandle th, CFGStandard::NodeLabelList& exit_nodes,
        OA_ptr<CFGStandard::NodeLabelList> return_nodes, 
        OA_ptr<CFGStandard::NodeLabelList> continue_nodes)
{
  // add the switch statement to the previous block. the switch statement
  // represents the multi-way conditional branch terminating the block.
  prev_node->add(th); 

  //bool direct_drop = true;   // should there be a direct edge from the condition to exit?
  bool empty_cond = false;     // is there an empty case?
  bool default_cond = false;   // is there a default case


  // We must create a new list of break statements since any breaks seen
  // in subtrees of this statement will exit this switch, not some other
  // enclosing construct.
  OA_ptr<CFGStandard::NodeLabelList> break_switch_statements;
  break_switch_statements = new CFGStandard::NodeLabelList;

  // Track the last case body processed so that fall-through
  // connections can be made This will be non-null whenever the
  // previous case falls through (i.e., did not end in a break
  // statement).  the case_exit_nodes list will contain all the exits
  // from the previous case block
  OA_ptr<CFGStandard::Node> prev_case_node; prev_case_node = 0;
  CFGStandard::NodeLabelList case_exit_nodes;

  // iterate over the multi-branches
  for (int bridx = 0; bridx < mIR->numMultiCases(th); bridx++) {

    OA_ptr<IRRegionStmtIterator> body = mIR->multiBody(th, bridx);
    //if (body->isValid() && body->current()) {
    OA_ptr<CFGStandard::Node> c; c = new CFGStandard::Node();
    mCFG->addNode(c);
    break_switch_statements->clear();
    // If a previous case falls-through, connect all of its exit nodes
    // to the current case node.
    if (!prev_case_node.ptrEqual(NULL)) {
      mCFG->connect(case_exit_nodes, c);  
      case_exit_nodes.clear();
      prev_case_node = 0;          
    }

    // if there are statements in the case body then look for break
    if (body->isValid() && body->current()) {
      // A placeholder for exit nodes.  We don't want the case body to
      // be an exit from the switch construct unless there is a break.
      // Either the body will have a break statement that collects
      // exits or the exits will all be connected the next case node
      IRStmtType prev_case_statement = build_block(c, body, case_exit_nodes, 
                                                   break_switch_statements, 
                                                   return_nodes, continue_nodes);

      // if there wasn't a break statement then update case_exit_nodes
      if ( break_switch_statements->size() == 0 ) {
        prev_case_node = c;
        if (prev_case_statement == SIMPLE) {
          case_exit_nodes.push_back(CFGStandard::NodeLabel(prev_case_node, Interface::FALLTHROUGH_EDGE));
        }

      // if we did see a break then this case node will not fall through to next case node
      } else {
        prev_case_node = 0; 

        // There should only be one break, but iterate anyway.
        CFGStandard::NodeLabelListIterator 
            break_stmt_iter(*break_switch_statements);
        while (break_stmt_iter.isValid()) {
          exit_nodes.push_back(break_stmt_iter.current());
          ++break_stmt_iter;
        }
        //IMPROVEME: if the body only has a break than we can simplify
        //the CFG by adding a direct_drop edge and removing this CFG
        //node, if figure out how to do this just need to set
        //empty_cond to true

      }

    // if have an empty block, still have a CFG node because break is not implied
    } else {
      prev_case_node = c;
      case_exit_nodes.push_back(CFGStandard::NodeLabel(prev_case_node, Interface::FALLTHROUGH_EDGE));
    }

    // check if this is the catch all case
    if (mIR->isCatchAll(th,bridx)) {
      default_cond = true;
      mCFG->connect(prev_node, c, Interface::MULTIWAY_EDGE);
    } else {
      ExprHandle multiCond = mIR->getSMultiCondition(th, bridx);
      mCFG->connect(prev_node, c, Interface::MULTIWAY_EDGE, multiCond);
    }

  } /* for */

  // if last condition didn't have a break then it won't be on
  // the exit_nodes stack and should be added
  if (!prev_case_node.ptrEqual(NULL)) {
    exit_nodes.push_back(CFGStandard::NodeLabel(prev_case_node, Interface::FALLTHROUGH_EDGE));
  }

  // direct drop is true iff there is not catchall or at least one multi-branch body is null
  if ( !default_cond || empty_cond ) {
    exit_nodes.push_back(CFGStandard::NodeLabel(prev_node, Interface::FALLTHROUGH_EDGE));
  }

  return STRUCT_MULTIWAY_CONDITIONAL;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! 
   Handle unconditional direct jumps.
    
    Create:
    goto statement ---> target block
    
*/
IRStmtType
ManagerStandard::build_CFG_unconditional_jump (
        OA_ptr<CFGStandard::Node> prev_node,
        StmtHandle stmt)
{
  // FIXME: New construct, partially tested.
  StmtLabel lab = mIR->getTargetLabel(stmt, 0);
  prev_node->add(stmt);
  mCFG->connect(prev_node, mCFG->node_from_label(lab), Interface::FALLTHROUGH_EDGE);
  return UNCONDITIONAL_JUMP;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! 
   Handle unconditional indirect jumps.

    Create:
    <PRE>
      goto statement
       |   ...   |   
       |         |
       V         V
    potential   potential
    target 0    target n
    </PRE> 

    Currently, any label in the program unit is considered to
    be a potential target of the indirect branch. This could be very
    conservative for some constructs, such as Fortran computed gotos
    (since we could more intelligently examine only those labels that
    are used in the Fortran ASSIGN statement -- i.e., labels that have
    their address taken and stored). It may be worthwhile to revisit 
    this if unacceptably large graphs result from a combination of many
    labels and many indirect branches.
*/
IRStmtType
ManagerStandard::build_CFG_unconditional_jump_i (
        OA_ptr<CFGStandard::Node> prev_node,
        StmtHandle stmt)
{
  // Every label must be seen before all the edges can be added.
  // Just add this to the list of indirect jumps, which will have their
  // edges added later.
  cout << "FIXME: UNCONDITIONAL_JUMP_I not yet implemented" << endl;
  assert(0);
  return UNCONDITIONAL_JUMP_I;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! 
    Handle unstructured twoway branches.
    The structure of a branch-on-true is as follows:

    <PRE>
        |
        V
    condition_t
      |        \ (true edge)
      V          \
    fall-through   \
     block          V
                  true block
    </PRE>

    A similar subgraph is created for branch-on-false, except that the
    fall-through is on true and the branching edge is a false edge.
*/
IRStmtType
ManagerStandard::build_CFG_ustruct_twoway_branch (OA_ptr<CFGStandard::Node>
        prev_node, StmtHandle stmt, CFGStandard::NodeLabelList& exit_nodes)
{
  // FIXME: New construct, partially untested.
  StmtLabel lab = mIR->getTargetLabel(stmt, 0);
  bool branch_on_true = (mIR->getCFGStmtType(stmt) == USTRUCT_TWOWAY_CONDITIONAL_T);
  prev_node->add(stmt);
  mCFG->connect(prev_node, mCFG->node_from_label(lab), (branch_on_true ? Interface::TRUE_EDGE : Interface::FALSE_EDGE));

  // This is so that the branch block will get connected to the fall-through block.
  exit_nodes.push_back(CFGStandard::NodeLabel(prev_node, Interface::FALLTHROUGH_EDGE));  /// ??? Use FALSE_EDGE?

  return mIR->getCFGStmtType(stmt);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*! Handle unstructured multiway branches.

    An unstructured multiway is essentially any non-structured branch
    with multiple targets. An optional default/catchall target is also
    allowed. Examples of this construct include Fortran's computed goto
    statement or a simple jump/dispatch table in a low-level intermediate
    representation. A vanilla jump table will not have a default target,
    but a Fortran computed goto (for example) may if it isn't known that
    one of the targets is always taken.
    <PRE>
                |
                |
                V
         ____branch________
        /  /  |  \ ...\    \ 
       /  /   |   \    \    \ 
      V  V    V    V    V    V
     B1  B2   B3  B4    Bk  catchall (optional)

    </PRE>

    Each edge will have an associated condition. For case-like statements,
    this is the case value for the particular target. For jump/dispatch
    tables or computed gotos, which have no conditions, the index of the
    target is used (i.e., 0..number_of_targets).
*/
IRStmtType
ManagerStandard::build_CFG_ustruct_multiway_branch (OA_ptr<CFGStandard::Node>
        prev_node, StmtHandle stmt)
{
  // FIXME: New construct, partially tested.
  // Add the multi-way statement to the previous block. It represents
  // the multi-way branch terminating the block (which holds the selector
  // expression).
  prev_node->add(stmt);

  // Connect the branch to each of its targets.
  for (int br = 0; br < mIR->numUMultiTargets(stmt); br++) {
    StmtLabel target_lab = mIR->getUMultiTargetLabel(stmt, br);
    ExprHandle cond_expr = mIR->getUMultiCondition(stmt, br);
    mCFG->connect(prev_node, mCFG->node_from_label(target_lab), 
                  Interface::MULTIWAY_EDGE, cond_expr );
  }

  // If there is a default target, make the proper connections.
  StmtLabel default_lab = mIR->getUMultiCatchallLabel(stmt);
  if (default_lab) {
    mCFG->connect(prev_node, mCFG->node_from_label(default_lab), 
                  Interface::MULTIWAY_EDGE /* , multiCond */);
  }
   
  return USTRUCT_MULTIWAY_CONDITIONAL;
}



//
// FIXME: The code for delay slot handling is untested.  This C++ implementation was translated directly
//        from a known-working C version in a different compilation system.
//
//
// Pre: h is a branch
// Post: Returns true if the label is in label_to_block_map
//
bool
ManagerStandard::isInternalBranch(StmtHandle h)
{
  StmtLabel lab = mIR->getTargetLabel(h, 0);

  return mCFG->isLabelMappedToNode(lab);
}


//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
// Definitions of auxilliary data structures to support construction of CFGs
// with delay slots
//*****************************************************************************

Counterlist::Counterlist ()
{
}


Counterlist::Counterlist (CounterlistIterator begin, CounterlistIterator end)
{
  counters.insert(begin, end);
}


//
// Subtracts 1 from each counter in the Worklist element
//
void
Counterlist::decrementCounters ()
{ 
  for (std::set<OA_ptr<Branch_Counter>, compareBranchCounters>::iterator 
       it = counters.begin();
       it != counters.end(); ++it) 
  {
    
    if ((*it)->count == 0) {
      cerr << "Trying to decrement counter beyond zero. Could be an uninitialized counter." << endl;
      exit(-1);
    }

    ((*it)->count)--;
  }
}


void
Counterlist::addCounter (StmtHandle stmt, int counterValue)
{
  OA_ptr<Branch_Counter> bc; bc = new Branch_Counter(stmt, counterValue);
  counters.insert(bc );
}


std::set<OA_ptr<Branch_Counter>, compareBranchCounters>::iterator 
    Counterlist::getFirstNonZeroCounter ()
{
  for (std::set<OA_ptr<Branch_Counter>, compareBranchCounters>::iterator 
       iter = counters.begin();
        iter != counters.end(); ++iter) 
  {

    if ((*iter)->count > 0) {
      return iter;
    }

  }
  
  return counters.end();
}


void
Counterlist::addCounters (OA_ptr<Counterlist> newCounters)
{
  // Called by copyCountersToWorklist
  
  for (CounterlistIterator it = newCounters->begin();
       it != newCounters->end();
       ++it) {
    
    counters.insert(*it);
  }

}



int
Counterlist::size ()
{
  return counters.size();
}


void
Counterlist::reset ()
{
  counters.clear();
}


OA_ptr<Branch_Counter>
Counterlist::getLeastCounter ()
{
  OA_ptr<Branch_Counter> retval; retval = NULL;
  if (size() > 0) {
    retval = *(counters.begin());
  }
  return retval;
}


CounterlistIterator
Counterlist::begin ()
{
  return counters.begin();
}


CounterlistIterator
Counterlist::end ()
{
  return counters.end();
}


void
Worklist::copyCountersToWorklist (OA_ptr<CFGStandard::Node> newBlock, 
                                  OA_ptr<Counterlist> blockCounterlist)
{
  //  Counterlist blockCounters = allElements[newBlock];
 
  allElements[newBlock]->addCounters(blockCounterlist);
}


void
Worklist::addCounterToWorklist (OA_ptr<CFGStandard::Node> block, 
                                StmtHandle stmt, int count)
{
  allElements[block]->addCounter(stmt, count);
}



void
Worklist::eraseCounterlist (OA_ptr<CFGStandard::Node> block)
{
  allElements.erase(block);
}


bool
Worklist::isEmpty ()
{
  return allElements.empty();
}


OA_ptr<Counterlist>
Worklist::getCounterlist (OA_ptr<CFGStandard::Node> block)
{
  return allElements[block];
}

//*****************************************************************************


//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
// Refine the CFG to split blocks as necessary to handle delay slots, along
// with branches and labels in delay slots
//*****************************************************************************
void
ManagerStandard::HandleDelayedBranches ()
{
  //
  // Preconditions:
  //
  // On input to this routine, all nodes and edges resulting from
  // structured control flow are already present.  In addition, all
  // nodes and edges induced by labels are present. To be finalized
  // here are the unstructured control flow contructs.
  //

  //
  // Create the "basic" CFG.  This adds control flow for the first
  // branch in a block (after its 'k' delay slots).
  //
  createBasicCFG();

#if 0 // FIXME: Disabled until its fixed. 
  //
  // Finally, perform Waterman's worklist algorithm to finalize
  // control flow for the cases where branches are contained in
  // the delay slots of other branches.
  // 

  // Place the start block of the procedure on the worklist
  Counterlist* emptyCounterList = new Counterlist;
  the_worklist->copyCountersToWorklist(Entry(), emptyCounterList);

  while (! the_worklist->isEmpty()) {
    for (NodesIterator nodes_iter(*this);
         (bool)nodes_iter && !the_worklist->isEmpty(); ++nodes_iter) {
      CFG::Node *node = dynamic_cast<CFG::Node*>((DGraph::Node*)nodes_iter);
      processBlock(node);
    }    
  }
#endif
} 


void
ManagerStandard::createBasicCFG()
{
  // Create and populate the block_list.
  std::set<OA_ptr<CFGStandard::Node> > block_list;
  for (CFGStandard::NodesIterator nodes_iter(*mCFG); nodes_iter.isValid(); 
       ++nodes_iter) 
  {
    OA_ptr<CFGStandard::Node> node = nodes_iter.current();
    block_list.insert(node);
  }

  // Process the blocks. 
  while (block_list.size() > 0) {
    bool branch_found = false;
    int countdown = 0;
    StmtHandle stmt = 0;
    OA_ptr<CFGStandard::Node> node = *block_list.begin();
    int numRemoved = block_list.erase(node);
    assert(numRemoved > 0);

    // Find first unstructured branch statement in this block.
    CFGStandard::NodeStatementsIterator si(*node);
    for ( ; si.isValid(); ++si) {
      stmt = si.current();
      IRStmtType ty = mIR->getCFGStmtType(stmt);
      assert(ty != UNCONDITIONAL_JUMP_I);  // FIXME: Add support.
      if (ty == USTRUCT_TWOWAY_CONDITIONAL_T
          || ty == USTRUCT_TWOWAY_CONDITIONAL_F 
          || ty == UNCONDITIONAL_JUMP) {
        branch_found = true;
        countdown = mIR->numberOfDelaySlots(stmt);
        break;
      } // if ty == branch
    } // For statements.

    // Only do this if countdown > 0, non-delayed branches are already
    // done in the main build routine.
    if (branch_found == true && countdown > 0) {
      // Move to end of this branch's delay slots.
      for ( ; si.isValid() && countdown > 0; ++si) {
        // FIXME: Need to check ParallelWithSuccessor, etc.
        --countdown;
      }

      if (countdown == 0) {
        //
        // Split the block just past its final delay slot.
        // Add the new block (the second piece of the original block) to
        // the block_list.
        // Add edges from the current block to the target(s) of the branch.
        //
        // FIXME: But before trying to do the split, check if the split point is
        // already at the end of the block.  This could happen if there
        // is a label just after the branch's final delay slot.
        //
        ++si;
        if (1 /* (bool)si == true */) {
          StmtHandle val = (si.isValid()) ? si.current() : (StmtHandle)0;
          OA_ptr<CFGStandard::Node> newnode = mCFG->splitBlock (node, val);
          mFallThrough[node] = newnode;
          block_list.insert(newnode);
          IRStmtType ty = mIR->getCFGStmtType(stmt);
          if (ty == USTRUCT_TWOWAY_CONDITIONAL_T
              || ty == USTRUCT_TWOWAY_CONDITIONAL_F) {
            mCFG->connect(node, mCFG->getLabelBlock(mIR->getTargetLabel(stmt, 0)),
                    ty == USTRUCT_TWOWAY_CONDITIONAL_T ? Interface::TRUE_EDGE : Interface::FALSE_EDGE);
            mCFG->connect(node, newnode, Interface::FALLTHROUGH_EDGE);
          } else if (ty == UNCONDITIONAL_JUMP) {
            StmtLabel label = mIR->getTargetLabel(stmt, 0);
            OA_ptr<CFGStandard::Node> nodeTmp = mCFG->getLabelBlock(label);
            mCFG->connect(node, nodeTmp, Interface::FALLTHROUGH_EDGE);
          } else {
            assert(0);
          }
        } // if (si)
      } // if (countdown == 0)
    } // if branch_found.

    if (branch_found == false || countdown > 0) {
      // ??? Add fall-thru edge from b (fixme, should already be there?).
      // Note, countdown > 0 can only happen if there was a label
      // in a delay slot.
    }
  } // While block_list.
 
}


//
// Add edges for all branches (including those in delay slots)
//
void
ManagerStandard::processBlock (OA_ptr<CFGStandard::Node> block)
{
  //
  // Mark block visited 
  //
  
  //  CFG::Node* block = (*currentWorklistElem)->block;

  OA_ptr<Counterlist> blockCounterlist = mThe_worklist->getCounterlist(block); 

  //CounterlistIterator blockCountersIter = blockCounterlist->begin();

  // Run loop until first element of counter_list hits zero or the end of 
  // the block is reached.  If a counter reaches zero divide the block
  // and add new edges as necessary 
  CFGStandard::NodeStatementsIterator statementsIter(*block);

  while (statementsIter.isValid()) {
    bool isTheSamePacket = true;
    // this loop emulates single_cycle()
    while (isTheSamePacket)  {
      //
      // Iterate through all instructions in a packet and  add an 
      // instruction to counter_list if it is a branch
      //
      if ((mIR->getCFGStmtType(statementsIter.current()) == USTRUCT_TWOWAY_CONDITIONAL_T
           || mIR->getCFGStmtType(statementsIter.current()) == USTRUCT_TWOWAY_CONDITIONAL_F)
          && isInternalBranch(statementsIter.current())) {
        
        mThe_worklist->addCounterToWorklist(block, statementsIter.current(), 
                                          mIR->numberOfDelaySlots(statementsIter.current()) );
      }


      ++statementsIter;

      // If the next instruction's parallel flag is clear, then a new packet
      // has begun
      isTheSamePacket = ! mIR->parallelWithSuccessor(statementsIter.current());

    }

    /* 
     * Don't decrement if end of block is reached and first instruction 
     * in the next block is in parallel 
     */ 
    
    // Get the first statement in the fallthrough node
    StmtHandle firstInstructionInNextBlock = 0 /* NULL */;
    for (CFGStandard::NodeStatementsIterator 
             nextBlockIter(*mFallThrough[block]);
         nextBlockIter.isValid();
         ++nextBlockIter) 
    {
      firstInstructionInNextBlock = nextBlockIter.current();
    }                   

    if (! (!statementsIter.isValid() && 
           mIR->parallelWithSuccessor(firstInstructionInNextBlock))) {
      
      blockCounterlist->decrementCounters();
    }

    //
    // If the first counter reaches zero, exit the loop
    //
    if (blockCounterlist->size() != 0 && 
        blockCounterlist->getLeastCounter().ptrEqual(0)) {
      break;
    }

  }


  //
  // If a counter reached zero before the block ended:
  //
  if (statementsIter.current())  {

    //
    // Splice off remainder of stmt_list and put in new block
    // connect these two blocks via a fallthrough
    //
    
    //
    // splitBlock adds an edge between block and the 
    // newly created fallthrough block
    //
    mFallThrough[block] = mCFG->splitBlock(block, statementsIter.current());

    // Add predecessor edge for new block
    mCFG->connect(block, mFallThrough[block], Interface::FALLTHROUGH_EDGE);
  }
    
  blockCounterlist = mThe_worklist->getCounterlist(block);

  CounterlistIterator firstNonZeroCounter = 
    blockCounterlist->getFirstNonZeroCounter();
  
  // newCounterlist consists of all counter lists which will be 
  // passed on to the successor blocks
  OA_ptr<Counterlist> newCounterList;
  newCounterList = new 
      Counterlist(blockCounterlist->begin(), firstNonZeroCounter);
  
  // Iterate through all the zero counters and add control flow
  for (CounterlistIterator counterListIter = blockCounterlist->begin();
       counterListIter != firstNonZeroCounter;
       ++counterListIter) {
   
    StmtHandle currentStatement = (*counterListIter)->branchInstruction;

    if (mIR->getCFGStmtType(currentStatement) == USTRUCT_TWOWAY_CONDITIONAL_T
        || mIR->getCFGStmtType(currentStatement) == USTRUCT_TWOWAY_CONDITIONAL_F) {
      
      mCFG->connect(block, mFallThrough[block], Interface::FALLTHROUGH_EDGE);      
      // Add the mFallThrough block to the worklist
      mThe_worklist->copyCountersToWorklist(mFallThrough[block], newCounterList);
      
    }

    if (isInternalBranch(currentStatement)) {
      OA_ptr<CFGStandard::Node> targetBlock;
      targetBlock = mCFG->getLabelBlock(mIR->getTargetLabel(currentStatement, 0));
      
      mCFG->connect(block, targetBlock, Interface::TRUE_EDGE );
      // Add the target block to the worklist
      mThe_worklist->copyCountersToWorklist( targetBlock, newCounterList);
    }
  } // for


  // If no counter reached zero, it implies that the blocks 
  // were split not due to a branch but due to something like a label    
  if ((firstNonZeroCounter == blockCounterlist->begin()) 
       && (mFallThrough.find(block) != mFallThrough.end())) {
    mThe_worklist->copyCountersToWorklist( mFallThrough[block], newCounterList);
  }

  // Erase the Counter list associated with the block
  mThe_worklist->eraseCounterlist(block);
}

  } // end of CFG namespace
} // end of OA namespace
