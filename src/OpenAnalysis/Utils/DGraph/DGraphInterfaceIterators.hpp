/*! \file
  
  \brief Helper functions for generating iterators over graph
         classes that satisfy the DGraph::Interface

  \authors Michelle Strout 
  \version $Id: DGraphInterfaceIterators.hpp,v 1.2 2005/06/10 02:32:05 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef DGraphInterfaceIterators_H
#define DGraphInterfaceIterators_H

#include "Interface.hpp"
#include <list>

namespace OA {
  namespace DGraph {

//--------------------------------------------------------------------

//! returns a list of DGraph::Interface::Nodes that are in
//! reverse post order starting from either all entry or all exit
//! nodes in the graph based on the given edge orientation
OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > >
create_reverse_post_order_list(DGraph::Interface& pDGraph,
                               DGraph::DGraphEdgeDirection pOrient);

//! returns a list of DGraph::Interface::Nodes that are in
//! reverse post order starting from either all entry or all exit
//! nodes in the graph based on original edge orientation
OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > >
create_reverse_post_order_list(DGraph::Interface& pDGraph);


//! returns a list of DGraph::Interface::Nodes that are in
//! reverse post order starting from the given startNode and
//! traversing the edges in the given orientation
OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > 
create_reverse_post_order_list(DGraph::Interface& pDGraph,
                               OA_ptr<DGraph::Interface::Node> startNode,
                               DGraph::DGraphEdgeDirection pOrient);


//! returns a list of DGraph::Interface::Nodes that do not have incoming
OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > >
create_entry_list(const DGraph::Interface& pDGraph);

//! returns a list of DGraph::Interface::Nodes that do not have outgoing
OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > >
create_exit_list(const DGraph::Interface& pDGraph);

//--------------------------------------------------------------------

  } // end of DGraph namespace
} // end of OA namespace

#endif
