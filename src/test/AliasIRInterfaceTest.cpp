#include "AliasIRInterfaceTest.hpp"

AliasIRInterfaceTest::AliasIRInterfaceTest()
{
    // will set up test IR
#if YYDEBUG != 0
//    yydebug = 1;
#endif
//    aliasparse();
}

void AliasIRInterfaceTest::addStmt(OA::ProcHandle proc, 
                                   OA::StmtHandle stmt,
                                   OA::Alias::IRStmtType stmtType)
{
    mProcBody[proc].push_back(stmt);
    mStmtAliasStmtType[stmt] = stmtType;
}

void AliasIRInterfaceTest::addMemRef(OA::StmtHandle stmt, OA::MemRefHandle memref)
{
    mStmtMemRefAll[stmt].push_back(memref);
}


void AliasIRInterfaceTest::addMemRefExpr(OA::MemRefHandle memref, 
        OA::OA_ptr<OA::MemRefExpr::MemRefExpr> mre)
{
    mMemRefToMRE[memref].push_back(mre);
}

void AliasIRInterfaceTest::addSym(OA::ProcHandle proc, OA::SymHandle sym, 
        OA::OA_ptr<OA::Location::Location> loc)
{
    mProcToSym[proc].push_back(sym);
    mSymToLoc[proc][sym] = loc;
}

void AliasIRInterfaceTest::dump(std::ostream& os)
{
    // loop through procedures
    std::map<OA::ProcHandle,std::string>::iterator procMapIter;
    for (procMapIter=mProcString.begin(); procMapIter!= mProcString.end();
         procMapIter++) 
    {
        OA::ProcHandle proc = procMapIter->first;
        os << "ProcHandle ( " << proc.hval();
        os << ", " << procMapIter->second << " ) => [" << std::endl;

        std::list<OA::StmtHandle>::iterator stmtIter;
        for (stmtIter=mProcBody[proc].begin(); stmtIter!=mProcBody[proc].end();
             stmtIter++)
        {
            OA::StmtHandle stmt = *stmtIter;
            os << "\tStmtHandle( " << stmt.hval();
            os << ", " << mStmtString[stmt] << " ) " << std::endl;
        }

        os << "]" << std::endl;
    }

}


