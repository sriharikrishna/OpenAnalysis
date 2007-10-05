/*! \file

    \brief Generate a notation file from an interface.

    \authors Andy Stone
*/

#include "NotationGenerator.hpp"

int gIndentLevel = 0;
std::ostream &indt(std::ostream &os)
{
    os << '\n';
    os.flush();
    for(int i = 0; i < gIndentLevel; i++)
        os << "    ";
    return os;
}

std::ostream &pushIndt(std::ostream &os)
{
    gIndentLevel++;
    return os;
}

std::ostream &popIndt(std::ostream &os)
{
    gIndentLevel--;
    return os;
}

NotationGenerator::NotationGenerator(OA::OA_ptr<OA::Alias::AliasIRInterface>
    ir, std::ostream &codeStream)
    :
    mIR(ir),
    mCodeStream(codeStream)
{
}

void NotationGenerator::generate(OA::ProcHandle proc)
{
    outputProcedure(proc);
    outputFormals(proc);

    // iterate over all statements in the procedure and output them
    OA::OA_ptr<OA::IRStmtIterator> i = mIR->getStmtIterator(proc);
    for(; i->isValid(); (*i)++) {
        outputStmt(i->current());
    }

    outputLocations(proc);

    mCodeStream << popIndt;
    mCodeStream << std::endl;
}

void NotationGenerator::outputProcedure(OA::ProcHandle proc)
{
    // example procedure:
    //     PROCEDURE = { < ProcHandle("main"), SymHandle("main") > }

    mCodeStream << indt << "PROCEDURE = { < ";
    outputHandle(proc);
    mCodeStream << ", ";
    outputHandle(mIR->getSymHandle(proc));
    mCodeStream << " > }";
    mCodeStream << pushIndt;
}

void NotationGenerator::outputFormals(OA::ProcHandle proc)
{
    // example formals:
    //     FORMALS = { 
    //         [
    //             < 1, SymHandle("p1") >
    //             < 2, SymHandle("p2") >
    //             < 3, SymHandle("p3") >
    //         ] }

    // a formals clause only needs to be outputted if atleast 1 formal exists.
    // to determine whether one exists, try to grab the symhandle for the 
    // first formal. The IR will return SymHandle(0) if the formal does not
    // exist.
    int formalId = 0;
    OA::SymHandle sym = mIR->getFormalSym(proc, formalId);
    if(sym == OA::SymHandle(0)) { return; }

    mCodeStream << indt << "FORMALS = {" << pushIndt;
    mCodeStream << indt << "[" << pushIndt;
    while ( sym != OA::SymHandle(0) ) {
        mCodeStream << indt << "< " << formalId << ", ";
        outputHandle(sym);
        mCodeStream << " >";
	++formalId;
        sym = mIR->getFormalSym(proc, formalId);
    }
    mCodeStream << popIndt << indt << "] }" << popIndt;
}

void NotationGenerator::outputStmt(OA::StmtHandle stmt)
{
    // statements can consist of the following clauses:
    //     MEMREFEXPRS, PTRASSIGNPAIRS, CALLSITES, PARAMBINDPTRASSIGNPAIRS 

    outputMemRefExprs(stmt);
    outputPtrAssignPairs(stmt);
    outputCallSites(stmt);
    outputParamBindPtrAssignPairs(stmt);
}

void NotationGenerator::outputMemRefExprs(OA::StmtHandle stmt)
{
   // example clause:
   //     MEMREFEXPRS = { StmtHandle("cptr1 = &c;") =>
   //         [
   //             MemRefHandle("cptr1_1") =>
   //                 NamedRef( DEF, SymHandle("cptr1") )
   //             MemRefHandle("&c_1") =>
   //                 NamedRef( USE, SymHandle("c"), T, full )
   //         ] }

    mCodeStream << indt << "MEMREFEXPRS = { ";
      outputHandle(stmt);
      mCodeStream << " =>" << pushIndt;
    mCodeStream << indt << "[" << pushIndt;

    OA::OA_ptr<OA::MemRefHandleIterator> i = mIR->getAllMemRefs(stmt);
    for(; i->isValid(); (*i)++) {
        OA::OA_ptr<OA::MemRefExprIterator> j =
            mIR->getMemRefExprIterator(i->current());
        for(; j->isValid(); (*j)++) {
            mCodeStream << indt;
              outputHandle(i->current());
              mCodeStream << " => " << pushIndt;
            mCodeStream << indt;
              outputMemRefExpr(j->current());
            mCodeStream << popIndt;
        }
    }

    mCodeStream << popIndt << indt << "] }" << popIndt;
}

void NotationGenerator::outputPtrAssignPairs(OA::StmtHandle stmt)
{
    // example clause:
    //    PTRASSIGNPAIRS = { StmtHandle("cptr1 = &c;") =>
    //        [
    //            < NamedRef( DEF, SymHandle("cptr1") ),
    //              NamedRef( USE, SymHandle("c"), T, full ) >
    //        ] }

    // if a statement is not of type PTR_ASSIGN_STMT then it can not
    // contain a PTRASSIGNPAIRS clause.
    
    OA::Alias::IRStmtType type = mIR->getAliasStmtType(stmt);
    if(type != OA::Alias::PTR_ASSIGN_STMT) { 
        return; 
    }

    mCodeStream << indt << "PTRASSIGNPAIRS = { ";
      outputHandle(stmt);
      mCodeStream << " =>" << pushIndt;
    mCodeStream << indt << "[" << pushIndt;

    OA::OA_ptr<OA::Alias::PtrAssignPairStmtIterator> i;
    i = mIR->getPtrAssignStmtPairIterator(stmt);
    for(; i->isValid(); (*i)++) {
        mCodeStream << indt << "< ";
          outputMemRefExpr(i->currentTarget());
        mCodeStream << indt << ", ";
          outputMemRefExpr(i->currentSource());
          mCodeStream << " >";
    }

    mCodeStream << popIndt << indt << "] }" << popIndt;
    
}
    
void NotationGenerator::outputCallSites(OA::StmtHandle stmt)
{
    // example clause:
    //    CALLSITES = { StmtHandle("foo( S.lf, B, c);") =>
    //        [
    //            CallHandle("foo( S.lf, B, c);") =>
    //                NamedRef( USE, SymHandle("foo"), F, full )
    //        ] }

    OA::OA_ptr<OA::IRCallsiteIterator> i;
    i = mIR->getCallsites(stmt);
    if(!i->isValid()) { return; }

    mCodeStream << indt << "CALLSITES = { ";
      outputHandle(stmt);
      mCodeStream << " =>" << pushIndt;
    mCodeStream << indt << "[" << pushIndt;

    for(; i->isValid(); (*i)++) {
        mCodeStream << indt;
          outputHandle(i->current());
          mCodeStream << " => " << pushIndt;
        mCodeStream << indt;
          outputMemRefExpr(mIR->getCallMemRefExpr(i->current()));
          mCodeStream << popIndt;
    }

    mCodeStream << popIndt << indt << "] }" << popIndt;
}

void NotationGenerator::outputParamBindPtrAssignPairs(OA::StmtHandle stmt)
{
    // iterate over all callsites generating PARAMBINDPTRASSIGNPAIRS
    // for them
    OA::OA_ptr<OA::IRCallsiteIterator> i;
    i = mIR->getCallsites(stmt);
    for(; i->isValid(); (*i)++) {
        outputParamBindPtrAssignPairs(stmt, (*i).current());
    }
}

void NotationGenerator::outputParamBindPtrAssignPairs(OA::StmtHandle stmt,
    OA::CallHandle call)
{
    // example clause:
    //    PARAMBINDPTRASSIGNPAIRS = { CallHandle("foo( S.lf, B, c);") => 
    //        [
    //            < 2, NamedRef( USE, SymHandle("B"), T, full ) >
    //            < 3, NamedRef( USE, SymHandle("c"), T, full ) >
    //        ] }

    OA::OA_ptr<OA::Alias::ParamBindPtrAssignIterator> i;
    i = mIR->getParamBindPtrAssignIterator(call);
    if(!i->isValid()) { return; }

    mCodeStream << indt << "PARAMBINDPTRASSIGNPAIRS = { ";
      outputHandle(call);
      mCodeStream << " =>" << pushIndt;
    mCodeStream << indt << "[" << pushIndt;

    for(; i->isValid(); (*i)++) {
        int formalId = (*i).currentFormalId();
        OA::OA_ptr<OA::MemRefExpr> actual = (*i).currentActual();

        mCodeStream << indt << "< "
                    << (formalId) << ", ";
          outputMemRefExpr(actual);
          mCodeStream << " >";
    }

    mCodeStream << popIndt << indt << "] }" << popIndt;
}

void NotationGenerator::outputMemRefExpr(OA::OA_ptr<OA::MemRefExpr> expr)
{
    // determine what type of MemRefExpr this is and call the proper
    // function to output it
    NGOutputVisitor visitor(*this);
    expr->acceptVisitor(visitor);
}

void NotationGenerator::outputMemRefExpr(OA::NamedRef& expr)
{
    // example: NamedRef( USE, SymHandle("S"), T, part )

    mCodeStream << "NamedRef( ";
    outputMemRefType(expr);
    mCodeStream << ", ";
    outputHandle(expr.getSymHandle());
    mCodeStream << ")";
}

void NotationGenerator::outputMemRefExpr(OA::UnnamedRef& expr)
{
    mCodeStream << "UnnamedRef( ";
    outputMemRefType(expr);
    mCodeStream << ", ";
    outputHandle(expr.getExprHandle());
    mCodeStream << ", ";
    mCodeStream << expr.isLocal();
    if(expr.isLocal() == true) {
       mCodeStream << ", ";
       outputHandle(expr.getProcHandle());
    }
    mCodeStream << ")";
}

void NotationGenerator::outputMemRefExpr(OA::UnknownRef& expr)
{
    mCodeStream << "UnknownRef( ";
    outputMemRefType(expr);
    mCodeStream << ")";
}

void NotationGenerator::outputMemRefExpr(OA::AddressOf& expr)
{
    mCodeStream << "AddressOf( ";
    outputMemRefType(expr);
    mCodeStream << ", ";
    outputMemRefExpr(expr.getMemRefExpr());
    mCodeStream << ")";
}

void NotationGenerator::outputMemRefExpr(OA::Deref& expr)
{
    //example: Deref( USE, NamedRef ( USE, SymHandle("cptr2") ) )

    mCodeStream << "Deref( ";
    outputMemRefType(expr);
    mCodeStream << ", ";
    outputMemRefExpr(expr.getMemRefExpr());
    mCodeStream << ", ";
    mCodeStream << expr.getNumDerefs();
    mCodeStream << ")";
}

void NotationGenerator::outputMemRefExpr(OA::FieldAccess& expr)
{
    //example: Deref( USE, NamedRef ( USE, SymHandle("cptr2") ) )

    mCodeStream << "FieldAccess( ";
    outputMemRefType(expr);
    mCodeStream << ", ";
    outputMemRefExpr(expr.getMemRefExpr());
    mCodeStream << ", ";
    mCodeStream << expr.getFieldName();
    mCodeStream << ")";
}

void NotationGenerator::outputMemRefExpr(OA::IdxExprAccess& expr)
{
    mCodeStream << "IdxExprAccess( ";
    outputMemRefType(expr);
    mCodeStream << ", ";
    outputMemRefExpr(expr.getMemRefExpr());
    mCodeStream << ", ";
    outputHandle(expr.getExpr());
    mCodeStream << ")";
}

void NotationGenerator::outputMemRefExpr(OA::IdxAccess& expr)
{
    mCodeStream << "IdxAccess( ";
    outputMemRefType(expr);
    mCodeStream << ", ";
    outputMemRefExpr(expr.getMemRefExpr());
    mCodeStream << ", ";
    mCodeStream << expr.getIdx() << ")";
}


// Added PLM 1/23/07
void NotationGenerator::outputMemRefExpr(OA::SubSetRef& expr)
{
    mCodeStream << "SubSetRef( ";
    outputMemRefType(expr);
    mCodeStream << ", ";
    outputMemRefExpr(expr.getMemRefExpr());
    mCodeStream << ")";
}


void NotationGenerator::outputHandle(OA::StmtHandle stmt)
{
    mCodeStream << "StmtHandle(\"";
    mCodeStream << mIR->toString(stmt);
    mCodeStream << "\")";
}


void NotationGenerator::outputHandle(OA::ExprHandle expr)
{
    mCodeStream << "ExprHandle(\"";
    mCodeStream << mIR->toString(expr);
    mCodeStream << "\")";
}


void NotationGenerator::outputHandle(OA::MemRefHandle h)
{
    mCodeStream << "MemRefHandle(\"";
    mCodeStream << mIR->toString(h);
    mCodeStream << "\")";
}

void NotationGenerator::outputHandle(OA::ProcHandle h)
{
    mCodeStream << "ProcHandle(\"";
    mCodeStream << mIR->toString(h);
    mCodeStream << "\")";
}

void NotationGenerator::outputMemRefType(OA::MemRefExpr &expr)
{
    if(expr.isDefUse()) { mCodeStream << "DEFUSE"; }
    else if(expr.isUseDef()) { mCodeStream << "USEDEF"; }
    else if(expr.isDef())    { mCodeStream << "DEF"; }
    else if(expr.isUse())    { mCodeStream << "USE"; }
}

void NotationGenerator::outputHandle(OA::SymHandle h)
{
    mCodeStream << "SymHandle(\"";
    mCodeStream << mIR->toString(h);
    mCodeStream << "\")";
    mSymHandles.insert(h);
}

void NotationGenerator::outputHandle(OA::CallHandle h)
{
    mCodeStream << "CallHandle(\"";
    mCodeStream << mIR->toString(h);
    mCodeStream << "\")";
}

void NotationGenerator::outputTF(bool hasAddressTaken)
{
    if(hasAddressTaken) { mCodeStream << "T"; }
    else { mCodeStream << "F"; }
}

void NotationGenerator::outputAccuracy(bool hasFullAccuracy)
{
    if(hasFullAccuracy) { mCodeStream << "full"; }
    else { mCodeStream << "part"; }
}

void NotationGenerator::outputLocations(OA::ProcHandle proc)
{
    // iterate over all SymHandles that were found while the notation
    // generator was outputting statements
    std::set<OA::SymHandle>::iterator i = mSymHandles.begin();
    for(; i != mSymHandles.end(); i++)
    {
        outputLocation(proc, *i);
    }
}

void NotationGenerator::outputLocation(OA::ProcHandle proc, OA::SymHandle sym)
{
    // example clause:
    //     LOCATION = { < SymHandle("p1"), local > }

    OA::OA_ptr<OA::Location> loc = mIR->getLocation(proc, sym);
    if(loc.ptrEqual(0)) { return; }
    bool isLocal = loc->isLocal();


    mCodeStream << indt << "LOCATION = { < ";
      outputHandle(sym);
      mCodeStream << ", ";
      mCodeStream << (isLocal ? "local" : "not local");
      mCodeStream << " > }";
}

