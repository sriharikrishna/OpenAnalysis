/*! \file
  
  \brief Testdriver for MemRefExpr hierarchy

  \authors Michelle Strout
  \version $Id: mretest.cpp,v 1.3 2005/01/18 21:13:16 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>

*/

#include "MemRefExpr.hpp"
#include <OpenAnalysis/IRInterface/IRHandles.hpp>

class TestIRInterface : public virtual OA::IRHandlesIRInterface {

public:
  TestIRInterface() {}
  virtual ~TestIRInterface() {}

  //--------------------------------------------------------
  // create a string for the given handle, should be succinct
  // and there should be no newlines
  //--------------------------------------------------------  
  std::string toString(const OA::ProcHandle h) { return ""; }
  std::string toString(const OA::StmtHandle h) 
    { std::ostringstream os; os << "StmtHandle(" << h.hval() << ")"; return os.str(); } 
  std::string toString(const OA::ExprHandle h) { return ""; }
  std::string toString(const OA::OpHandle h) { return ""; }
  std::string toString(const OA::MemRefHandle h) { return ""; }
  std::string toString(const OA::SymHandle h)
    { std::ostringstream os; os << "SymHandle(" << h.hval() << ")"; return os.str(); } 
  std::string toString(const OA::ConstSymHandle h) { return ""; }
  std::string toString(const OA::ConstValHandle h) { return ""; }

  //--------------------------------------------------------
  // dumps that do pretty prints
  //--------------------------------------------------------  

  //! Given a memory reference, pretty-print it to the output stream os.
  void dump(OA::MemRefHandle h, std::ostream& os) {}

  //! Given a statement, pretty-print it to the output stream os.
  void dump(OA::StmtHandle stmt, std::ostream& os) {}

  
  //--------------------------------------------------------
  // Other
  //--------------------------------------------------------  
  // FIXME: should eventually get rid of this
  void currentProc(OA::ProcHandle p) {}
};


int main() {
    OA::OA_ptr<TestIRInterface> ir;
    ir = new TestIRInterface;

    OA::StmtHandle stmt1(1);
    OA::StmtHandle stmt2(2);
    OA::SymHandle sym1(1);
    OA::SymHandle sym2(2);

    // build a ref of each kind and dump it
    std::cout << "===== NamedRef =====" << std::endl;
    OA::OA_ptr<OA::MemRefExpr::MemRefExpr> namedRef1;
    namedRef1 = new OA::MemRefExpr::NamedRef(OA::MemRefExpr::USE, sym1);
    std::cout << "== dump ==" << std::endl;
    namedRef1->dump(std::cout,ir);
    std::cout << "==========" << std::endl;
    std::cout << "== output ==" << std::endl;
    namedRef1->output(ir);
    std::cout << "==========" << std::endl;
    std::cout << "isaNamed() = " << namedRef1->isaNamed() << std::endl;
    std::cout << "isaUnknown() = " << namedRef1->isaUnknown() << std::endl;
    std::cout << "====================" << std::endl << std::endl;

    std::cout << "===== NamedRef =====" << std::endl;
    OA::OA_ptr<OA::MemRefExpr::MemRefExpr> namedRef2;
    namedRef2 = new OA::MemRefExpr::NamedRef(OA::MemRefExpr::USE, sym2);
    std::cout << "== dump ==" << std::endl;
    namedRef2->dump(std::cout,ir);
    std::cout << "==========" << std::endl;
    std::cout << "isaNamed() = " << namedRef2->isaNamed() << std::endl;
    std::cout << "isaUnknown() = " << namedRef2->isaUnknown() << std::endl;
    std::cout << "====================" << std::endl << std::endl;

    std::cout << "===== NamedRef =====" << std::endl;
    OA::OA_ptr<OA::MemRefExpr::MemRefExpr> namedRef3;
    namedRef3 = new OA::MemRefExpr::NamedRef(OA::MemRefExpr::USE, sym1);
    std::cout << "== dump ==" << std::endl;
    namedRef3->dump(std::cout,ir);
    std::cout << "==========" << std::endl;
    std::cout << "isaNamed() = " << namedRef3->isaNamed() << std::endl;
    std::cout << "isaRefOp() = " << namedRef3->isaRefOp() << std::endl;
    std::cout << "====================" << std::endl << std::endl;

    std::cout << "===== UnnamedRef =====" << std::endl;
    OA::OA_ptr<OA::MemRefExpr::MemRefExpr> unnamedRef;
    unnamedRef = new OA::MemRefExpr::UnnamedRef(OA::MemRefExpr::DEF, stmt1);
    std::cout << "== dump ==" << std::endl;
    unnamedRef->dump(std::cout,ir);
    std::cout << "==========" << std::endl;
    std::cout << "isaNamed() = " << unnamedRef->isaNamed() << std::endl;
    std::cout << "isaUnnamed() = " << unnamedRef->isaUnnamed() << std::endl;
    std::cout << "====================" << std::endl << std::endl;

    std::cout << "===== UnKnownRef =====" << std::endl;
    OA::OA_ptr<OA::MemRefExpr::MemRefExpr> unknownRef;
    unknownRef = new OA::MemRefExpr::UnknownRef(OA::MemRefExpr::USE);
    std::cout << "== dump ==" << std::endl;
    unknownRef->dump(std::cout,ir);
    std::cout << "==========" << std::endl;
    std::cout << "isaNamed() = " << unknownRef->isaNamed() << std::endl;
    std::cout << "isaUnknown() = " << unknownRef->isaUnknown() << std::endl;
    std::cout << "isaRefOp() = " << unknownRef->isaRefOp() << std::endl;
    std::cout << "====================" << std::endl << std::endl;


    // testing equality operator
    std::cout << "(namedRef1 == namedRef2) = " << namedRef1->operator==(*namedRef2)
              << std::endl;
    std::cout << "(namedRef1 == namedRef3) = " << (*namedRef1==*namedRef3)
              << std::endl;
    std::cout << "(namedRef1 == unnamedRef) = " << (*namedRef1==*unnamedRef)
              << std::endl;
    std::cout << "(unnamedRef == unknownRef) = " << (*unnamedRef==*unknownRef)
              << std::endl;
    std::cout << "(unnamedRef == unnamedRef) = " << (*unnamedRef==*unnamedRef)
              << std::endl << std::endl;
    
    //*****************************************************************
    // RefOp decorator
    //*****************************************************************
    OA::OA_ptr<OA::MemRefExpr::Deref> derefMRE3;
    derefMRE3 = new OA::MemRefExpr::Deref(namedRef3, 2);
    OA::OA_ptr<OA::MemRefExpr::Deref> derefMRE1;
    derefMRE1 = new OA::MemRefExpr::Deref(namedRef1, 2);

    std::cout << "== derefMRE3 dump ==" << std::endl;
    derefMRE3->dump(std::cout,ir);
    std::cout << "====================" << std::endl;
    std::cout << "== derefMRE1 dump ==" << std::endl;
    derefMRE1->dump(std::cout,ir);
    std::cout << "====================" << std::endl << std::endl;
    std::cout << "(derefMRE3 == derefMRE1) = " << (*derefMRE3==*derefMRE1)
              << std::endl;

    derefMRE3->setPartialAccuracy();
    std::cout << "Now should be false:" << std::endl;
    derefMRE3->dump(std::cout,ir);
    std::cout << "(derefMRE3 == derefMRE1) = " << (*derefMRE3==*derefMRE1)
              << std::endl << std::endl;

    derefMRE1->setPartialAccuracy();
    OA::OA_ptr<OA::MemRefExpr::IdxAccess> idxAccess3;
    idxAccess3 = new OA::MemRefExpr::IdxAccess(derefMRE3,5);
    OA::OA_ptr<OA::MemRefExpr::IdxAccess> idxAccess1;
    idxAccess1 = new OA::MemRefExpr::IdxAccess(derefMRE1,5);
    OA::OA_ptr<OA::MemRefExpr::IdxAccess> idxAccessX;
    idxAccessX = new OA::MemRefExpr::IdxAccess(derefMRE1,6);
    std::cout << "== idxAccess3 dump ==" << std::endl;
    idxAccess3->dump(std::cout,ir);
    std::cout << "=====================" << std::endl;
    std::cout << "== idxAccess1 dump ==" << std::endl;
    idxAccess1->dump(std::cout,ir);
    std::cout << "=====================" << std::endl;
    std::cout << "== idxAccessX dump ==" << std::endl;
    idxAccessX->dump(std::cout,ir);
    std::cout << "=====================" << std::endl;
    std::cout << "== idxAccessX output ==" << std::endl;
    idxAccessX->output(ir);
    std::cout << "==========" << std::endl;
    std::cout << "(derefMRE3 == idxAccess3) = " << (*derefMRE3==*idxAccess3)
              << std::endl;
    std::cout << "(idxAccess1 == idxAccess3) = " << (*idxAccess1==*idxAccess3)
              << std::endl;
    std::cout << "(idxAccess1 == idxAccessX) = " << (*idxAccess1==*idxAccessX)
              << std::endl;

    return 0;
}


