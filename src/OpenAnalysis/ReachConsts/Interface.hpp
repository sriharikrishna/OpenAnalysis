/*! \file
  
  \brief Abstract interface that all ReachConsts analysis results must satisfy.

  \authors Michelle Strout, Barbara Kreaseck
  \version $Id: Interface.hpp,v 1.4 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#ifndef REACHCONSTSINTERFACE_H
#define REACHCONSTSINTERFACE_H

#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/IRInterface/ConstValBasicInterface.hpp>

namespace OA {
  namespace ReachConsts {

class Interface {
public:
  Interface(){}
  virtual ~Interface(){}

  //! return a ConstValBasicInterface* for a given mem ref handle
  //! do not delete contents of ConstValBasicInterface*
  //! Returns NULL if no constant value exists for this MemRefHandle
  virtual OA_ptr<ConstValBasicInterface> getReachConst(MemRefHandle h) = 0;

  virtual void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> ir) = 0;
};
  } // end of ReachConsts namespace
} // end of OA namespace

#endif
