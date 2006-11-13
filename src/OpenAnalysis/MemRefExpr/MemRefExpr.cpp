/*! \file
  
  \brief Definition for MemRefExpr class hierarchy.  

  \authors Michelle Strout
  \version $Id: MemRefExpr.cpp,v 1.14 2005/06/10 02:32:04 mstrout Exp $

  Copyright (c) 2002-2004, Rice University <br>
  Copyright (c) 2004, University of Chicago <br>  
  All rights reserved. <br>
  See ../../../Copyright.txt for details. <br>
*/

#include "MemRefExpr.hpp"

// included here instead of in hpp file to remove circular reference
#include <OpenAnalysis/MemRefExpr/MemRefExprVisitor.hpp>

namespace OA {


//*****************************************************************
// MemRefExpr methods
//*****************************************************************

/*!
   Need consistent ordering.  Will order by adressTaken (true < false)
   and then FullAccuracy (true < false).  
*/
bool MemRefExpr::operator<(MemRefExpr& other) 
{
    bool retval = false;

    // MemRefType doesn't affect ordering
    if (hasAddressTaken()  == other.hasAddressTaken()) {
        if (hasFullAccuracy()  == other.hasFullAccuracy()) {
            retval = false;
        } else if (hasFullAccuracy() && !other.hasFullAccuracy()) {
            retval = true;
        } else if (!hasFullAccuracy() && other.hasFullAccuracy()) {
            retval = false;
        }
    } else if (hasAddressTaken() && !other.hasAddressTaken()) {
        retval = true;
    } else if (!hasAddressTaken() && other.hasAddressTaken()) {
        retval = false;
    }

    return retval;
}

bool MemRefExpr::operator==(MemRefExpr& other) 
{
    // must have same status for addressOf and accuracy
    // but don't have to have the same MemRefType
    bool retval = true;
    if (hasAddressTaken() != other.hasAddressTaken()) {
        retval = false;
    }
    if (hasFullAccuracy() != other.hasFullAccuracy()) {
        retval = false;
    }

    return retval;
}

void MemRefExpr::output(OA_ptr<IRHandlesIRInterface> pIR)
{
    sOB->field("mAddressOf", bool2string(mAddressOf) );
    sOB->field("mFullAccuracy", bool2string(mFullAccuracy) );
    sOB->field("mMemRefType", toString(mMemRefType) );
}

void MemRefExpr::dump(std::ostream& os, IRHandlesIRInterface& pIR)
{
    MemRefExpr::dump(os);
}


void MemRefExpr::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
    MemRefExpr::dump(os);
}

void MemRefExpr::dump(std::ostream& os)
{
    // string for mSymHandle
    os << "MemRefExpr(this=" << this << ", mAddressOf=" << mAddressOf
                   <<  ", mFullAccuracy=" << mFullAccuracy 
                   << ", type = " << toString(mMemRefType) << ")\t";
}

std::string MemRefExpr::toString(MemRefType type) 
{
    std::ostringstream oss;
    switch (mMemRefType) {
      case USE: oss << "USE"; break;
      case DEF: oss << "DEF"; break;
      case USEDEF: oss << "USEDEF"; break;
      case DEFUSE: oss << "DEFUSE"; break;
    }
    return oss.str();
}

//*****************************************************************
// NamedRef methods
//*****************************************************************

void NamedRef::acceptVisitor(MemRefExprVisitor& pVisitor)
{
    pVisitor.visitNamedRef(*this);
}

class NamedRefLessThanVisitor : public virtual MemRefExprVisitor {
  private:
    NamedRef& mThisRef;
  public:
    bool mLessThan;
    NamedRefLessThanVisitor(NamedRef& thisRef) 
        : mThisRef(thisRef), mLessThan(false) {}
    ~NamedRefLessThanVisitor() {}
    void visitNamedRef(NamedRef& ref) 
      { if (mThisRef.getSymHandle() < ref.getSymHandle()) 
        { mLessThan = true; } else { mLessThan = false; }
      }
    void visitUnnamedRef(UnnamedRef& ref) { mLessThan = true; }
    void visitUnknownRef(UnknownRef& ref) { mLessThan = true; }
    void visitDeref(Deref& ref) { mLessThan = true; }
    void visitSubSetRef(SubSetRef& ref) { mLessThan = true; }
};

/*! 
   References are ordered first between MemRefExpr subclasses in an
   specific order: NamedRef < UnnamedRef < Deref
   < SubSetRefs( IdxAccess < ... ) < UnknownRef.
   Then there is a consistent ordering within each subclass.

   For NamedRef's ordering is done by SymHandle.
*/
bool NamedRef::operator<(MemRefExpr& other)
{
    // apply the visitor to the other reference and return result
    NamedRefLessThanVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);
    return ltVisitor.mLessThan;
} 

class NamedRefEqualVisitor : public virtual MemRefExprVisitor {
  private:
    NamedRef& mThisRef;
  public:
    bool mEqual;
    NamedRefEqualVisitor(NamedRef& thisRef) 
        : mThisRef(thisRef), mEqual(false) {}
    ~NamedRefEqualVisitor() {}
    void visitNamedRef(NamedRef& ref) 
      { if (mThisRef.getSymHandle() == ref.getSymHandle()) 
        { mEqual = true; } else { mEqual = false; }
      }
    void visitUnnamedRef(UnnamedRef& ref) { mEqual = false; }
    void visitUnknownRef(UnknownRef& ref) { mEqual = false; }
    void visitDeref(Deref& ref) { mEqual = false; }
    void visitSubSetRef(SubSetRef& ref) { mEqual = false; }
};

bool NamedRef::operator==(MemRefExpr& other)
{
    // apply the visitor to the other reference and return result
    NamedRefEqualVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);
    return ltVisitor.mEqual;
} 

void NamedRef::output(OA_ptr<IRHandlesIRInterface> pIR)
{
    sOB->objStart("NamedRef");

    MemRefExpr::output(pIR);

    sOB->fieldStart("mSymHandle");
    sOB->outputIRHandle(mSymHandle,pIR);
    sOB->fieldEnd("mSymHandle");

    sOB->objEnd("NamedRef");
}

void NamedRef::dump(std::ostream& os)
{
    MemRefExpr::dump(os);
    os << "NamedRef(this=" << this << ", mSymHandle.hval()=" 
       << mSymHandle.hval() << ")";
    os << std::endl;
}

void NamedRef::dump(std::ostream& os, IRHandlesIRInterface& pIR)
{
    // string for mSymHandle
    MemRefExpr::dump(os,pIR);
    os << "NamedRef(this=" << this << ", mSymHandle.hval()=" 
       << mSymHandle.hval() << "): "
       << pIR.toString(mSymHandle);
    os << std::endl;
}


void NamedRef::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
    dump(os,*pIR);
}

//*****************************************************************
// UnnamedRef methods
//*****************************************************************

void UnnamedRef::acceptVisitor(MemRefExprVisitor& pVisitor)
{
    pVisitor.visitUnnamedRef(*this);
}

class UnnamedRefLessThanVisitor : public virtual MemRefExprVisitor {
  private:
    UnnamedRef& mThisRef;
  public:
    bool mLessThan;
    UnnamedRefLessThanVisitor(UnnamedRef& thisRef) 
        : mThisRef(thisRef), mLessThan(false) {}
    ~UnnamedRefLessThanVisitor() {}
    void visitNamedRef(NamedRef& ref)  { mLessThan = false; }
    void visitUnnamedRef(UnnamedRef& ref) 
      { if (mThisRef.getStmtHandle() < ref.getStmtHandle()) 
        { mLessThan = true; } else { mLessThan = false; }
      }
    void visitUnknownRef(UnknownRef& ref) { mLessThan = true; }
    void visitDeref(Deref& ref) { mLessThan = true; }
    void visitSubSetRef(SubSetRef& ref) { mLessThan = true; }
};

/*! 
   References are ordered first between MemRefExpr subclasses in an
   specific order: NamedRef < UnnamedRef < Deref
   < SubSetRefs( IdxAccess < ... ) < UnknownRef.
   Then there is a consistent ordering within each subclass.

   For UnnamedRef's ordering is done by StmtHandle.
*/
bool UnnamedRef::operator<(MemRefExpr& other)
{
    // apply the visitor to the other reference and return result
    UnnamedRefLessThanVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);
    return ltVisitor.mLessThan;
}  

class UnnamedRefEqualVisitor : public virtual MemRefExprVisitor {
  private:
    UnnamedRef& mThisRef;
  public:
    bool mEqual;
    UnnamedRefEqualVisitor(UnnamedRef& thisRef) 
        : mThisRef(thisRef), mEqual(false) {}
    ~UnnamedRefEqualVisitor() {}
    void visitNamedRef(NamedRef& ref) { mEqual = false; }
    void visitUnnamedRef(UnnamedRef& ref) 
      { if (mThisRef.getStmtHandle() == ref.getStmtHandle()) 
        { mEqual = true; } else { mEqual = false; }
      }
    void visitUnknownRef(UnknownRef& ref) { mEqual = false; }
    void visitDeref(Deref& ref) { mEqual = false; }
    void visitSubSetRef(SubSetRef& ref) { mEqual = false; }
};

bool UnnamedRef::operator==(MemRefExpr& other)
{
    // apply the visitor to the other reference and return result
    UnnamedRefEqualVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);
    return ltVisitor.mEqual;
}   

void UnnamedRef::output(OA_ptr<IRHandlesIRInterface> pIR)
{
    sOB->objStart("UnnamedRef");

    MemRefExpr::output(pIR);

    sOB->fieldStart("mStmtHandle");
    sOB->outputIRHandle(mStmtHandle,pIR);
    sOB->fieldEnd("mStmtHandle");

    sOB->objEnd("UnnamedRef");
}


void UnnamedRef::dump(std::ostream& os)
{
    MemRefExpr::dump(os);
    os << "UnnamedRef(this=" << this << ", mStmtHandle.hval()="
       << mStmtHandle.hval() << ")";
    os << std::endl;
}

void UnnamedRef::dump(std::ostream& os, IRHandlesIRInterface& pIR)
{
    MemRefExpr::dump(os,pIR);
    os << "UnnamedRef(this=" << this << ", mStmtHandle.hval()="
       << mStmtHandle.hval() << "): "
       << pIR.toString(mStmtHandle);
    os << std::endl;
}

void UnnamedRef::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
    dump(os,*pIR);
}

//*****************************************************************
// UnknownRef methods
//*****************************************************************

void UnknownRef::acceptVisitor(MemRefExprVisitor& pVisitor)
{
    pVisitor.visitUnknownRef(*this);
}

bool UnknownRef::operator<(MemRefExpr& other) 
{
    return false;
}
    
bool UnknownRef::operator==(MemRefExpr& other)
{
    bool retval = false;  

    // we can only be equivalent to another unknown memory 
    if (other.isaUnnamed()) {
        retval = MemRefExpr::operator==(other);
    }

    return retval;
}

void UnknownRef::output(OA_ptr<IRHandlesIRInterface> pIR)
{
    sOB->objStart("UnknownRef");

    MemRefExpr::output(pIR);

    sOB->objEnd("UnknownRef");
}


void UnknownRef::dump(std::ostream& os)
{
    MemRefExpr::dump(os);
    os << "UnknownRef";
    os << std::endl;
}

void UnknownRef::dump(std::ostream& os, IRHandlesIRInterface& pIR)
{
    MemRefExpr::dump(os,pIR);
    os << "UnknownRef";
    os << std::endl;
}
void UnknownRef::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
    dump(os,*pIR);
}

//*****************************************************************
// RefOp methods
//*****************************************************************
SymHandle RefOp::getBaseSym()
{
    OA_ptr<MemRefExpr> m = mMRE; // Stupid Sun CC 5.4

    // if our mre is another RefOp then call recursively
    if (mMRE->isaRefOp()) {
        OA_ptr<RefOp> refOp = m.convert<RefOp>();
        return refOp->getBaseSym();
    }

    // otherwise should be a named ref
    assert(mMRE->isaNamed());
    OA_ptr<NamedRef> namedRef = m.convert<NamedRef>();
    return namedRef->getSymHandle();
}

void RefOp::output(OA_ptr<IRHandlesIRInterface> ir)
{
    MemRefExpr::output(ir);

    sOB->fieldStart("mMRE");
    mMRE->output(ir);
    sOB->fieldEnd("mMRE");
}

//*****************************************************************
// Deref methods
//*****************************************************************

void Deref::acceptVisitor(MemRefExprVisitor& pVisitor)
{
    pVisitor.visitDeref(*this);
}

class DerefLessThanVisitor : public virtual MemRefExprVisitor {
  private:
    Deref& mThisRef;
  public:
    bool mLessThan;
    DerefLessThanVisitor(Deref& thisRef) 
        : mThisRef(thisRef), mLessThan(false) {}
    ~DerefLessThanVisitor() {}
    void visitNamedRef(NamedRef& ref)  { mLessThan = false; }
    void visitUnnamedRef(UnnamedRef& ref) { mLessThan = false; }
    void visitUnknownRef(UnknownRef& ref) { mLessThan = true; }
    // have to compare the MemRefExpr fields in the Deref instance
    // because things like fullAccuracy can be different at each level
    void visitDeref(Deref& ref)
      { if ( mThisRef.MemRefExpr::operator<(ref) ||
             (mThisRef.MemRefExpr::operator==(ref) &&
              (mThisRef.getMemRefExpr() < ref.getMemRefExpr() ||
               (mThisRef.getMemRefExpr() == ref.getMemRefExpr()
                && mThisRef.getNumDerefs() < ref.getNumDerefs()) ) ) )
        { mLessThan = true; } else { mLessThan = false; }
      }
    void visitSubSetRef(SubSetRef& ref) { mLessThan = true; }
};

/*! 
   References are ordered first between MemRefExpr subclasses in an
   specific order: NamedRef < UnnamedRef < Deref
   < SubSetRefs( IdxAccess < ... ) < UnknownRef.
   Then there is a consistent ordering within each subclass.

   For Deref's ordering is done by underlying reference and then
   number of derefs.
*/
bool Deref::operator<(MemRefExpr& other)
{
    // apply the visitor to the other reference and return result
    DerefLessThanVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);
    return ltVisitor.mLessThan;
}   

class DerefEqualVisitor : public virtual MemRefExprVisitor {
  private:
    Deref& mThisRef;
  public:
    bool mEqual;
    DerefEqualVisitor(Deref& thisRef) 
        : mThisRef(thisRef), mEqual(false) {}
    ~DerefEqualVisitor() {}
    void visitNamedRef(NamedRef& ref) { mEqual = false; }
    void visitUnnamedRef(UnnamedRef& ref) { mEqual = false; }
    void visitUnknownRef(UnknownRef& ref) { mEqual = false; }
    void visitDeref(Deref& ref) 
      { // make sure these guys have valid memory references that
        // they operate on
        assert(!mThisRef.getMemRefExpr().ptrEqual(NULL));
        assert(!ref.getMemRefExpr().ptrEqual(NULL));
        // check if deref count is equal and underlying memref is equal
        // have to check that our MemRefExpr fields like fullAccuracy are
        // equal because accuracy can change at any level
        if (mThisRef.MemRefExpr::operator==(ref)
            && mThisRef.getNumDerefs() == ref.getNumDerefs()
            && mThisRef.getMemRefExpr() == ref.getMemRefExpr() )
        { mEqual = true; } else { mEqual = false; }
      }
    void visitSubSetRef(SubSetRef& ref) { mEqual = false; }
};

bool Deref::operator==(MemRefExpr& other)
{
    // apply the visitor to the other reference and return result
    DerefEqualVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);
    return ltVisitor.mEqual;
}

void Deref::output(OA_ptr<IRHandlesIRInterface> pIR)
{
    sOB->objStart("Deref");

    RefOp::output(pIR);

    sOB->field("mNumDeref", int2string(mNumDeref));

    sOB->objEnd("Deref");
}


void Deref::dump(std::ostream& os)
{
    MemRefExpr::dump(os);
    os << "Deref(this=" << this << ", mMemRefExpr=";
    getMemRefExpr()->dump(os);
    os << "\tmNumDerefs = " << getNumDerefs() << " )"; 
    os << std::endl;
}

void Deref::dump(std::ostream& os, IRHandlesIRInterface& pIR)
{
    MemRefExpr::dump(os,pIR);
    os << "Deref(this=" << this << ", mMemRefExpr=";
    getMemRefExpr()->dump(os,pIR);
    os << "\tmNumDerefs = " << getNumDerefs() << " )"; 
    os << std::endl;
}

void Deref::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
    dump(os,*pIR);
}

//*****************************************************************
// SubSetRef methods
//*****************************************************************

// default visitor routine for all subclasses
void SubSetRef::acceptVisitor(MemRefExprVisitor& pVisitor)
{
    pVisitor.visitSubSetRef(*this);
}

void SubSetRef::output(OA_ptr<IRHandlesIRInterface> ir)
{
    RefOp::output(ir);
}

//*****************************************************************
// IdxAccess methods
//*****************************************************************

void IdxAccess::acceptVisitor(MemRefExprVisitor& pVisitor)
{
    pVisitor.visitIdxAccess(*this);
}

class IdxAccessLessThanVisitor : public virtual MemRefExprVisitor {
  private:
    IdxAccess& mThisRef;
  public:
    bool mLessThan;
    IdxAccessLessThanVisitor(IdxAccess& thisRef) 
        : mThisRef(thisRef), mLessThan(false) {}
    ~IdxAccessLessThanVisitor() {}
    void visitNamedRef(NamedRef& ref)  { mLessThan = false; }
    void visitUnnamedRef(UnnamedRef& ref) { mLessThan = false; }
    void visitUnknownRef(UnknownRef& ref) { mLessThan = true; }
    void visitDeref(Deref& ref) { mLessThan = false; }
    void visitSubSetRef(SubSetRef& ref) { mLessThan = true; }
    void visitIdxAccess(IdxAccess& ref) 
      { if ( mThisRef.MemRefExpr::operator<(ref) ||
             (mThisRef.MemRefExpr::operator==(ref) &&
              (mThisRef.getMemRefExpr() < ref.getMemRefExpr() ||
               (mThisRef.getMemRefExpr() == ref.getMemRefExpr()
                && mThisRef.getIdx() < ref.getIdx()) ) ) )
        { mLessThan = true; } else { mLessThan = false; }
      }
};

/*! 
   References are ordered first between MemRefExpr subclasses in an
   specific order: NamedRef < UnnamedRef < Deref
   < SubSetRefs( IdxAccess < ... ) < UnknownRef.
   Then there is a consistent ordering within each subclass.

   For IdxAccess ordering is done by underlying reference and then
   then idx.
   
   Any subclasses to SubSetRef that are added later must handle 
   IdxAccess specifically,
   otherwise there will be inconsistent ordering.
*/
bool IdxAccess::operator<(MemRefExpr& other)
{
    // apply the visitor to the other ref and return result
    IdxAccessLessThanVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);
    return ltVisitor.mLessThan;
}  

class IdxAccessEqualVisitor : public virtual MemRefExprVisitor {
  private:
    IdxAccess& mThisRef;
  public:
    bool mEqual;
    IdxAccessEqualVisitor(IdxAccess& thisRef) 
        : mThisRef(thisRef), mEqual(false) {}
    ~IdxAccessEqualVisitor() {}
    void visitNamedRef(NamedRef& ref) { mEqual = false; }
    void visitUnnamedRef(UnnamedRef& ref) { mEqual = false; }
    void visitUnknownRef(UnknownRef& ref) { mEqual = false; }
    void visitDeref(Deref& ref) { mEqual = false; }
    void visitSubSetRef(SubSetRef& ref) { mEqual = false; }
    void visitIdxAccess(IdxAccess& ref) 
      { if ( mThisRef.MemRefExpr::operator==(ref) 
             && mThisRef.getMemRefExpr() == ref.getMemRefExpr()
             && mThisRef.getIdx() == ref.getIdx() )
        { mEqual = true; } else { mEqual = false; }
      }
};

bool IdxAccess::operator==(MemRefExpr& other)
{
    // apply the visitor to the other reference and return result
    IdxAccessEqualVisitor ltVisitor(*this);
    other.acceptVisitor(ltVisitor);

    return ltVisitor.mEqual;
}

void IdxAccess::output(OA_ptr<IRHandlesIRInterface> pIR)
{
    sOB->objStart("IdxAccess");

    RefOp::output(pIR);

    sOB->field("mIdx", int2string(mIdx));

    sOB->objEnd("IdxAccess");
}


void IdxAccess::dump(std::ostream& os)
{
    MemRefExpr::dump(os);
    os << "IdxAccess(this=" << this << ", mMemRefExpr=";
    getMemRefExpr()->dump(os);
    os << "\tmIdx=" << getIdx() << " )"; 
    os << std::endl;
}

void IdxAccess::dump(std::ostream& os, IRHandlesIRInterface& pIR)
{
    MemRefExpr::dump(os,pIR);
    os << "IdxAccess(this=" << this << ", mMemRefExpr=";
    getMemRefExpr()->dump(os,pIR);
    os << "\tmIdx=" << getIdx() << " )"; 
    os << std::endl;
}

void IdxAccess::dump(std::ostream& os, OA_ptr<IRHandlesIRInterface> pIR)
{
    dump(os,*pIR);
}



} // end of OA namespace

