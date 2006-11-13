/*! \file
  
  \brief Definition of helper functions for generating iterators over
         graph classes that satisfy the DGraph::Interface

  \authors Michelle Strout
  \version $Id: DGraphInterfaceIterators.cpp,v 1.2 2005/06/10 02:32:05 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "DGraphInterfaceIterators.hpp"


namespace OA {
  namespace DGraph {

//--------------------------------------------------------------------

void reverse_postorder_recurse(DGraph::Interface& pDGraph, 
                 OA_ptr<DGraph::Interface::Node> pNode,
                 DGraph::DGraphEdgeDirection pOrient,
                 OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > pList )
{
    OA_ptr<DGraph::Interface::NodesIterator> neighIter;
    
    // mark as visited so that we don't get in an infinite
    // loop on cycles in the graph
    pNode->setVisitFlag();

    // loop over the successors or predecessors based on orientation
    if (pOrient==DGraph::DEdgeOrg) {
      OA_ptr<DGraph::Interface::SinkNodesIterator> it = 
        pNode->getSinkNodesIterator(); // Stupid Sun CC 5.4
      neighIter = it.convert<DGraph::Interface::NodesIterator>();
    } else {
      OA_ptr<DGraph::Interface::SourceNodesIterator> it = 
        pNode->getSourceNodesIterator(); // Stupid Sun CC 5.4
      neighIter = it.convert<DGraph::Interface::NodesIterator>();
    }
    for (; neighIter->isValid(); ++(*neighIter)) {
        OA_ptr<DGraph::Interface::Node> n; 
        n = neighIter->current();

        // if the node hasn't been visited then call recursively 
        if (!n->getVisitFlag()) {
            reverse_postorder_recurse(pDGraph, n, pOrient, pList);
        }
    }

    // add ourselves to the beginning of the list
    pList->push_front(pNode);
}



/*! Creates reverse post order list of nodes starting from the
    specified root.
*/
OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > >
create_reverse_post_order_list(DGraph::Interface& pDGraph,
                               OA_ptr<DGraph::Interface::Node> root,
                               DGraph::DGraphEdgeDirection pOrient)
{
    // loop over all nodes and set their visit field to false
    OA_ptr<DGraph::Interface::NodesIterator> nodeIter
        = pDGraph.getNodesIterator();
    for ( ; nodeIter->isValid(); (*nodeIter)++ ) {
        OA_ptr<DGraph::Interface::Node> node = nodeIter->current();
        node->clearVisitFlag();
    } 
        
    // generate a list of nodes in the requested ordering
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > retval;
    retval = new std::list<OA_ptr<DGraph::Interface::Node> >;

    reverse_postorder_recurse(pDGraph, root, pOrient, retval);

    return retval;

}

OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > >
create_reverse_post_order_list(DGraph::Interface& pDGraph)
{
    return create_reverse_post_order_list(pDGraph, DGraph::DEdgeOrg); 
}

/*! Creates reverse post order list of all nodes in the graph by
    starting at all entry nodes or all exit nodes depending on the 
    orientation provided.
*/
OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > >
create_reverse_post_order_list(DGraph::Interface& pDGraph,
                               DGraph::DGraphEdgeDirection pOrient)
{
    // loop over all nodes and set their visit field to false
    OA_ptr<DGraph::Interface::NodesIterator> nodeIter
        = pDGraph.getNodesIterator();
    for ( ; nodeIter->isValid(); (*nodeIter)++ ) {
        OA_ptr<DGraph::Interface::Node> node = nodeIter->current();
        node->clearVisitFlag();
    } 
        
    // generate a list of nodes in the requested ordering
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > retval;
    retval = new std::list<OA_ptr<DGraph::Interface::Node> >;

    // if original edge direction then start ordering from entry nodes
    if (pOrient==DGraph::DEdgeOrg) {
        nodeIter = pDGraph.getEntryNodesIterator();

    // reverse edge direction start at exit nodes
    } else {
        nodeIter = pDGraph.getExitNodesIterator();
    }

    for ( ; nodeIter->isValid(); (*nodeIter)++ ) {
        reverse_postorder_recurse(pDGraph, nodeIter->current(), 
                                  pOrient, retval);
    }

    return retval;

}

//! returns a list of DGraph::Interface::Nodes that do not have incoming
OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > >
create_entry_list(const DGraph::Interface& pDGraph) 
{
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > retval;
    retval = new std::list<OA_ptr<DGraph::Interface::Node> >;

    OA_ptr<DGraph::Interface::NodesIterator> nodeIter 
        = pDGraph.getNodesIterator();
    for ( ; nodeIter->isValid(); (*nodeIter)++ ) {
        OA_ptr<DGraph::Interface::Node> node = nodeIter->current();
        if (node->isAnEntry()) {
            retval->push_back(node);
        }
    }

    return retval;
}

//! returns a list of DGraph::Interface::Nodes that do not have outgoing
OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > >
create_exit_list(const DGraph::Interface& pDGraph) 
{
    OA_ptr<std::list<OA_ptr<DGraph::Interface::Node> > > retval;
    retval = new std::list<OA_ptr<DGraph::Interface::Node> >;

    OA_ptr<DGraph::Interface::NodesIterator> nodeIter 
        = pDGraph.getNodesIterator();
    for ( ; nodeIter->isValid(); (*nodeIter)++ ) {
        OA_ptr<DGraph::Interface::Node> node = nodeIter->current();
        if (node->isAnExit()) {
            retval->push_back(node);
        }
    }

    return retval;
}

  } // end of DGraph namespace
} // end of OA namespace
