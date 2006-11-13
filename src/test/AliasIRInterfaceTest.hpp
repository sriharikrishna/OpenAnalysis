/*! \file
    
    \brief Test IR interface for Alias analysis.  
        
    \authors Michelle Strout
    \version $Id: AliasIRInterfaceTest.hpp,v 1.3 2004/11/28 21:33:28 mstrout Exp $
 
     Copyright (c) 2002-2004, Rice University
     Copyright (c) 2004, University of Chicago  
     All rights reserved.
     See ../../../Copyright.txt for details.
 
*/

//extern int aliasparse();
//extern int aliasdebug;

#include <string>
#include <list>
#include <map>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/Location/Location.hpp>
#include <OpenAnalysis/MemRefExpr/MemRefExpr.hpp>

#include <OpenAnalysis/IRInterface/AliasIRInterfaceDefault.hpp>
#include "IRHandlesIRInterfaceTest.hpp"

#ifndef ALIASIRInterfaceTest_HPP
#define ALIASIRInterfaceTest_HPP


//! Not implemented yet
class TestIRPtrAssignPairStmtIterator 
    : public OA::Alias::PtrAssignPairStmtIterator 
{
  public:
    TestIRPtrAssignPairStmtIterator() {}
    ~TestIRPtrAssignPairStmtIterator() {}

    //! right hand side
    OA::MemRefHandle currentSource() const { return OA::MemRefHandle(0); }
    //! left hand side
    OA::MemRefHandle currentTarget() const { return OA::MemRefHandle(0); }

    bool isValid() const  { return false; }
                    
    void operator++() {}
};



/*! methods needed to build the TestIR and implement
    required IRInterfaces 
*/
class AliasIRInterfaceTest 
    : public virtual OA::Alias::AliasIRInterfaceDefault,
      public IRHandlesIRInterfaceTest
{ 
  private:
    std::map<OA::ProcHandle,std::list<OA::StmtHandle> > mProcBody;
    std::map<OA::ProcHandle,std::list<OA::SymHandle> > mProcToSym;

    std::map<OA::StmtHandle,std::list<OA::MemRefHandle> > mStmtMemRefAll;
    std::map<OA::StmtHandle,OA::Alias::IRStmtType > mStmtAliasStmtType;

    std::map<OA::MemRefHandle,
             std::list<OA::OA_ptr<OA::MemRefExpr::MemRefExpr> > > mMemRefToMRE;

    std::map<OA::ProcHandle,
             std::map<OA::SymHandle, 
                      OA::OA_ptr<OA::Location::Location> > > mSymToLoc;

  public:
    AliasIRInterfaceTest();
    ~AliasIRInterfaceTest() {}

    //*****************************************************************
    // Implementation of IRHandlesIRInterface
    //*****************************************************************
    virtual std::string toString(OA::ProcHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::StmtHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::MemRefHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::SymHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::ExprHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::ConstSymHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::ConstValHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual std::string toString(OA::OpHandle h) 
    { return IRHandlesIRInterfaceTest::toString(h); }

    virtual void dump(OA::MemRefHandle h, std::ostream& os)
    { IRHandlesIRInterfaceTest::dump(h,os); }

    virtual void dump(OA::StmtHandle h, std::ostream& os)
    { IRHandlesIRInterfaceTest::dump(h,os); }

    //*****************************************************************
    // Implementation of AliasIRInterface
    //*****************************************************************
    
    //! Given a subprogram return an IRStmtIterator for the entire
    //! subprogram
    OA::OA_ptr<OA::IRStmtIterator> getStmtIterator(OA::ProcHandle h) 
    { OA::OA_ptr<TestIRStmtIterator> retval;
      OA::OA_ptr<std::list<OA::StmtHandle> > listptr;
      listptr = new std::list<OA::StmtHandle>(mProcBody[h]) ; 
      retval = new TestIRStmtIterator( listptr );
      return retval;
    }
  
    //! Return a list of all the memory reference handles that appear
    //! in the given statement.
    OA::OA_ptr<OA::MemRefHandleIterator> getAllMemRefs(OA::StmtHandle stmt)
    { OA::OA_ptr<TestIRMemRefIterator> retval;
      OA::OA_ptr<std::list<OA::MemRefHandle> > listptr;
      listptr = new std::list<OA::MemRefHandle>(mStmtMemRefAll[stmt]); 
      retval = new TestIRMemRefIterator( listptr );
      return retval;
    }
  
    //! Given a statement return an IRTopMemRefIterator 
    //! this is an iterator over all the top memory references in a statement
    //! for example, in *p = q[i][j], *p and q[i][j] are the top memory 
    //! references which contain the sub memory references p and q,i,j 
    //! respectively
    //! The user must free the iterator's memory via delete.
    //! FIXME: Would like to deprecate this, is it only used for 
    //! IAliasMapsXAIF?
    OA::OA_ptr<OA::IRTopMemRefIterator> getTopMemRefIterator(OA::StmtHandle h)
    { // returning an iterator over empty list for now
        OA::OA_ptr<TestIRMemRefIterator> retval;
        OA::OA_ptr<std::list<OA::MemRefHandle> > listptr;
        listptr = new std::list<OA::MemRefHandle>();
        retval = new TestIRMemRefIterator( listptr );
        return retval;
    }
  
    //! Given a statement, return its Alias::IRStmtType
    OA::Alias::IRStmtType getAliasStmtType(OA::StmtHandle stmt)
    { return mStmtAliasStmtType[stmt]; }

    //! If this is a PTR_ASSIGN_STMT then return an iterator over MemRefHandle
    //! pairs where there is a source and target such that target
    OA::OA_ptr<OA::Alias::PtrAssignPairStmtIterator> 
        getPtrAssignStmtPairIterator(OA::StmtHandle stmt)
      { OA::OA_ptr<OA::Alias::PtrAssignPairStmtIterator> retval;
        retval = new TestIRPtrAssignPairStmtIterator;
        return retval;
      }


    //! Given a procedure return associated SymHandle
    OA::SymHandle getSymHandle(OA::ProcHandle h)
    { return OA::SymHandle(0); }
  
    //! For the given symbol create a Location that indicates statically
    //! overlapping locations and information about whether the location
    //! is local or not
    OA::OA_ptr<OA::Location::Location> getLocation(OA::ProcHandle proc,
                                                   OA::SymHandle s)
    { return mSymToLoc[proc][s]; }

    //! Given a MemRefHandle return a list of all 
    //! MemRefExprs that describe this memory reference
    OA::OA_ptr<OA::MemRefExpr::MemRefExprIterator>
        getMemRefExprIterator(OA::MemRefHandle h)
    { OA::OA_ptr<TestIRMemRefExprIterator> retval;
      OA::OA_ptr<std::list<OA::OA_ptr<OA::MemRefExpr::MemRefExpr> > > listptr;
      listptr 
        = new std::list<OA::OA_ptr<OA::MemRefExpr::MemRefExpr> >(mMemRefToMRE[h]);
       retval = new TestIRMemRefExprIterator( listptr );
       return retval;
    }

    //*****************************************************************
    // Construction methods 
    //*****************************************************************
    void addSym(OA::ProcHandle, OA::SymHandle,OA::OA_ptr<OA::Location::Location>);
    void addStmt(OA::ProcHandle,OA::StmtHandle,OA::Alias::IRStmtType stmtType);
    void addMemRef(OA::StmtHandle,OA::MemRefHandle);
    void addMemRefExpr(OA::MemRefHandle,
                       OA::OA_ptr<OA::MemRefExpr::MemRefExpr>);

    void dump(std::ostream& os);

};

#endif
