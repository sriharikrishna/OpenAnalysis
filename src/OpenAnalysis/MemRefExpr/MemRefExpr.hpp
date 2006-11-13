/*! \file
  
  \brief Declarations for the MemRefExpr abstraction.

  \authors Michelle Strout
  \version $Id: MemRefExpr.hpp,v 1.17 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#ifndef MemRefExpr_H
#define MemRefExpr_H

#include <iostream>
#include <cassert>
#include <OpenAnalysis/Utils/OA_ptr.hpp>
#include <OpenAnalysis/IRInterface/IRHandles.hpp>
#include <OpenAnalysis/OABase/Annotation.hpp>

namespace OA {

class MemRefExpr;

class MemRefExprVisitor;

class MemRefExprIterator {
public:
  MemRefExprIterator() { }
  virtual ~MemRefExprIterator() { }

  virtual OA_ptr<MemRefExpr> current() const = 0;  // Returns the current item.
  virtual bool isValid() const = 0;     // False when all items are exhausted.
        
  virtual void operator++() = 0;
  void operator++(int) { ++*this; } ;

  virtual void reset() = 0;
};



/*! abstract base class that has default implementations for the
    methods that all MemRefExpr's must provide
*/
class MemRefExpr : public Annotation {
  public:
    typedef enum {
      USE,     // mem ref specifies where to find source location for use
      DEF,     // mem ref specifies where to put result of assign
      USEDEF,  // mem ref that specifies a location that will be used first
               // and then defined (eg. i++)
      DEFUSE   // mem ref that specifies a location that will be defined first
               // and then used (eg. i++)
    } MemRefType;

    MemRefExpr(MemRefExpr::MemRefType mrType) 
        : mAddressOf(false), mFullAccuracy(true), mMemRefType(mrType) {}
    MemRefExpr(bool addressOf, bool fullAccuracy, MemRefType mrType) 
        : mAddressOf(addressOf), mFullAccuracy(fullAccuracy), 
          mMemRefType(mrType) {}

    //! copy constructor
    MemRefExpr(MemRefExpr &mre) : mAddressOf(mre.mAddressOf),
        mFullAccuracy(mre.mFullAccuracy), mMemRefType(mre.mMemRefType) {}

    virtual ~MemRefExpr() {}

    virtual void acceptVisitor(MemRefExprVisitor& pVisitor) = 0;

    //*****************************************************************
    // Subclass type methods 
    //*****************************************************************
    virtual bool isaNamed() { return false; }
    virtual bool isaUnnamed() { return false; }
    virtual bool isaUnknown() { return false; }
    virtual bool isaRefOp() { return false; }

    //*****************************************************************
    // Info methods 
    //*****************************************************************
        
    //! does the mem ref expr have its address taken
    bool hasAddressTaken() { return mAddressOf; }

    //! whether the memory reference expression full describes the ref
    bool hasFullAccuracy() { return mFullAccuracy; }
    
    //! is this a def mem ref
    bool isDef() { return (mMemRefType==DEF 
                           || mMemRefType == USEDEF || mMemRefType == DEFUSE); }

    //! is this a use mem ref
    bool isUse() { return (mMemRefType==USE 
                          || mMemRefType == USEDEF || mMemRefType == DEFUSE); }

    //*****************************************************************
    // Construction methods 
    //*****************************************************************
        
    //! specify that this MemRefExpr involves an address being taken
    void setAddressTaken() { mAddressOf = true; }

    //! specify that this MemRefExpr does not approximate the memory
    //! reference with full accuracy
    void setPartialAccuracy() { mFullAccuracy = false; }

    //*****************************************************************
    // Relationship methods 
    //*****************************************************************

    //! an ordering for locations, needed for use within STL containers
    virtual bool operator<(MemRefExpr & other);

    //! check if two memory references are equal at the level of
    //! accuracy provided by the MemRefExpr approximation
    virtual bool operator==(MemRefExpr& other);
    
    //! check if two memory references are not equal at the level of
    //! accuracy provided by the MemRefExpr approximation
    bool operator!=(MemRefExpr& other) { return ! ((*this)==other); }
    
    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir); 

    // helper functions for output
    std::string toString(MemRefType);

    //*****************************************************************
    // Debugging methods 
    //*****************************************************************
    virtual void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);
    virtual void dump(std::ostream& os, IRHandlesIRInterface& pIR);
    virtual void dump(std::ostream& os);

private:
    bool mAddressOf;
    bool mFullAccuracy;
    MemRefType mMemRefType;
};

/*!
   A named memory reference has a SymHandle base. 
   Examples include references involving local and global variables.
*/
class NamedRef: public MemRefExpr {
  public:
    NamedRef(MemRefType mrType, SymHandle sh) 
        : MemRefExpr(mrType), mSymHandle(sh) { }
    NamedRef(bool addressTaken, bool fullAccuracy, MemRefType mrType, 
             SymHandle sh) 
        : MemRefExpr(addressTaken,fullAccuracy,mrType), mSymHandle(sh) { }

    //! copy constructor
    NamedRef(NamedRef &mre) : MemRefExpr(mre), mSymHandle(mre.mSymHandle) {}
    NamedRef(MemRefExpr &mre, SymHandle sh) : MemRefExpr(mre), mSymHandle(sh) {}

    ~NamedRef() { }

    void acceptVisitor(MemRefExprVisitor& pVisitor);

    //*****************************************************************
    // Subclass type methods 
    //*****************************************************************
    bool isaNamed() { return true; }
    
    //*****************************************************************
    // Info methods 
    //*****************************************************************
    SymHandle getSymHandle() { return mSymHandle; }
    
    //*****************************************************************
    // Relationship methods 
    //*****************************************************************

    bool operator<(MemRefExpr & other);

    //! check if two memory references are equal at the level of
    //! accuracy provided by the MemRefExpr approximation
    bool operator==(MemRefExpr& other);

    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir); 

    //*****************************************************************
    // Debugging methods 
    //*****************************************************************
    virtual void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);
    virtual void dump(std::ostream& os, IRHandlesIRInterface& pIR);
    virtual void dump(std::ostream& os);

  private:
    SymHandle mSymHandle;
};

/*!
   An unnamed memory reference has a StmtHandle base. 
   Examples include references involving dynamically allocated locations.
*/
class UnnamedRef: public MemRefExpr {
  public:
    UnnamedRef(MemRefType mrType, StmtHandle sh) 
        : MemRefExpr(mrType), mStmtHandle(sh) { }
    UnnamedRef(bool addressTaken, bool fullAccuracy, MemRefType mrType, 
               StmtHandle sh) 
        : MemRefExpr(addressTaken,fullAccuracy,mrType), mStmtHandle(sh) { }

    //! copy constructor
    UnnamedRef(UnnamedRef &mre) : MemRefExpr(mre), 
                                  mStmtHandle(mre.mStmtHandle) {}
    UnnamedRef(MemRefExpr &mre, StmtHandle s) : MemRefExpr(mre), mStmtHandle(s) {}

    ~UnnamedRef() { }

    void acceptVisitor(MemRefExprVisitor& pVisitor);

    //*****************************************************************
    // Subclass type methods 
    //*****************************************************************
    bool isaUnnamed() { return true; }
    
    //*****************************************************************
    // Info methods 
    //*****************************************************************
    StmtHandle getStmtHandle() { return mStmtHandle; }
    
    //*****************************************************************
    // Relationship methods 
    //*****************************************************************

    bool operator<(MemRefExpr & other);

    //! check if two memory references are equal at the level of
    //! accuracy provided by the MemRefExpr approximation
    bool operator==(MemRefExpr& other);
    
    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir); 

    //*****************************************************************
    // Debugging methods 
    //*****************************************************************
    virtual void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);
    virtual void dump(std::ostream& os, IRHandlesIRInterface& pIR);
    virtual void dump(std::ostream& os);

  private:
    StmtHandle mStmtHandle;
};

/*!
   An unknown memory reference has an unknown base.
   Examples include references involving involving arbitrary function
   calls, eg. *hello()
*/
class UnknownRef: public MemRefExpr {
  public:
    //! addressTaken is always true and accuracy is always partial
    UnknownRef(MemRefType mrType) 
        : MemRefExpr(true, false, mrType) { }

    //! copy constructor
    UnknownRef(UnknownRef &mre) : MemRefExpr(mre) {}

    ~UnknownRef() { }

    void acceptVisitor(MemRefExprVisitor& pVisitor);

    //*****************************************************************
    // Subclass type methods 
    //*****************************************************************
    bool isaUnknown() { return true; }
    
    //*****************************************************************
    // Relationship methods 
    //*****************************************************************

    bool operator<(MemRefExpr & other);

    //! check if two memory references are equal at the level of
    //! accuracy provided by the MemRefExpr approximation
    bool operator==(MemRefExpr& other);
    
    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir); 

    //*****************************************************************
    // Debugging methods 
    //*****************************************************************
    virtual void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);
    virtual void dump(std::ostream& os, IRHandlesIRInterface& pIR);
    virtual void dump(std::ostream& os);

};


/*!
   The RefOp implements a decorator pattern for memory references.
   If we decorate a memory reference with a RefOp it represents either
   a dereference or referencing a subset of a location (eg. field access
   or array access).
*/
class RefOp: public MemRefExpr {
  public:
    RefOp(OA_ptr<MemRefExpr> mre) : MemRefExpr(*mre), mMRE(mre) { }

    //! copy constructor
    RefOp(RefOp & mre) : MemRefExpr(mre), mMRE(mre.mMRE) { }

    virtual ~RefOp() { }

    //*****************************************************************
    // MemRefExpr subclass type methods 
    //*****************************************************************
    bool isaRefOp() { return true; }

    //*****************************************************************
    // RefOp subclass type methods 
    //*****************************************************************
    virtual bool isaDeref() { return false; }
    virtual bool isaSubSetRef() { return false; }
    SymHandle getBaseSym();

    //*****************************************************************
    // Info methods 
    //*****************************************************************
    OA_ptr<MemRefExpr> getMemRefExpr() { return mMRE; }

    //*****************************************************************
    // Relationship methods, will be defined in subclasses 
    //*****************************************************************

    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir); 


private:
    OA_ptr<MemRefExpr> mMRE;

};

/*!
   The Deref class indicates how many times a memory reference is 
   being dereferenced.
*/
class Deref: public RefOp {
  public:
    Deref(OA_ptr<MemRefExpr> mre, int numDeref) : RefOp(mre), mNumDeref(numDeref) { }

    //! copy constructor
    Deref(Deref &mre) : RefOp(mre), mNumDeref(mre.mNumDeref) {}

    ~Deref() { }

    void acceptVisitor(MemRefExprVisitor& pVisitor);

    //*****************************************************************
    // RefOp subclass type methods 
    //*****************************************************************
    bool isaDeref() { return true; }
    
    //*****************************************************************
    // Info methods 
    //*****************************************************************
    int getNumDerefs() { return mNumDeref; }

    //*****************************************************************
    // Relationship methods
    //*****************************************************************
    
    bool operator<(MemRefExpr & other);

    //! check if two memory references are equal at the level of
    //! accuracy provided by the MemRefExpr approximation
    bool operator==(MemRefExpr& other);
    
    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir); 

    //*****************************************************************
    // Debugging methods 
    //*****************************************************************
    virtual void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);
    virtual void dump(std::ostream& os, IRHandlesIRInterface& pIR);
    virtual void dump(std::ostream& os);

private:
    int mNumDeref;
};

/*!
   The SubSetRef implements a decorator pattern for memory references.
   If we decorate a memory reference with a subclass
   of SubSetRef it represents some operation such as a field 
   access or array access occuring.
   Many sub set reference operations are possible abstractions are possible.
   Will never have a direct instance of this class because if don't 
   use a more precise subclass will just use something like NamedRef
   and specify partial accuracy.
*/
class SubSetRef: public RefOp {
  public:
    SubSetRef(OA_ptr<MemRefExpr> mre) : RefOp(mre) { }

    //! copy constructor
    SubSetRef(SubSetRef & mre) : RefOp(mre) { }

    virtual ~SubSetRef() { }

    virtual void acceptVisitor(MemRefExprVisitor& pVisitor);

    //*****************************************************************
    // RefOp subclass type methods 
    //*****************************************************************
    virtual bool isaSubSetRef() { return true; }

    //*****************************************************************
    // SubSetRef subclass type methods 
    //*****************************************************************

    //! This needs to be logical OR of all isa methods below it
    //! If a direct instance of this class then will return false
    //bool isSubClassOfSubSetRef() { return isaIdxAccess(); }

    virtual bool isaIdxAccess() { return false; }
    //...

    //*****************************************************************
    // Relationship methods, will be defined in subclasses
    //*****************************************************************
    
    virtual bool operator<(MemRefExpr & other) = 0;

    //! check if two memory references are equal at the level of
    //! accuracy provided by the MemRefExpr approximation
    virtual bool operator==(MemRefExpr& other) = 0;
    
 
    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir); 

};


/*!
   The IdxAccess class indicates a constant array index.
*/
class IdxAccess: public SubSetRef {
  public:
    IdxAccess(OA_ptr<MemRefExpr> mre, int idx) : SubSetRef(mre), mIdx(idx) { }
    
    //! copy constructor
    IdxAccess(IdxAccess &mre) : SubSetRef(mre), mIdx(mre.mIdx) {}

    ~IdxAccess() { }

    virtual void acceptVisitor(MemRefExprVisitor& pVisitor);

    //*****************************************************************
    // SubSetRef subclass type methods 
    //*****************************************************************
    bool isaIdxAccess() { return true; }
    
    //*****************************************************************
    // Info methods 
    //*****************************************************************
    int getIdx() { return mIdx; }

    //*****************************************************************
    // Relationship methods
    //*****************************************************************
    
    bool operator<(MemRefExpr & other);

    //! check if two memory references are equal at the level of
    //! accuracy provided by the MemRefExpr approximation
    bool operator==(MemRefExpr& other);
    
    //*****************************************************************
    // Annotation Interface
    //*****************************************************************
    void output(OA_ptr<IRHandlesIRInterface> ir); 

    //*****************************************************************
    // Debugging methods 
    //*****************************************************************
    virtual void dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR);
    virtual void dump(std::ostream& os, IRHandlesIRInterface& pIR);
    virtual void dump(std::ostream& os);

private:
    int mIdx;
};


} // end of OA namespace

#endif

