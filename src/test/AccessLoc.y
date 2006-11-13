%{
extern int yylex ();
#define YYERROR_VERBOSE
#define YYDEBUG 1
void yyerror(char*);

// specific to this parser
#include "AccessLocIRInterfaceTest.hpp"
extern OA::OA_ptr<AccessLocIRInterfaceTest> gAccessLocIR;

static OA::ProcHandle gCurrProc;
%}

%union {
    char*   sval;
    int     ival;
    bool    bval;
    OA::ProcHandle* procptr;
    OA::StmtHandle* stmtptr;
    std::list<OA::StmtHandle>* stmtlistptr;
    OA::SymHandle* symptr;
    std::list<OA::SymHandle>* symlistptr;
    OA::ExprHandle* exprptr;
    OA::MemRefHandle* memrefptr;
    OA::OpHandle* opptr;
    OA::ConstSymHandle* constsymptr;
    OA::ConstValHandle* constvalptr;

    OA::MemRefExpr::MemRefType mrtval;
    OA::MemRefExpr::MemRefExpr* mreptr;
    std::list<OA::MemRefExpr::MemRefExpr*>* mrelistptr;

    OA::Location::Location* locptr;
};

/* IRHandles tokens */
%token <sval> T_STR_LITERAL 
%token <ival> T_INT_LITERAL
%token <bval> T_TRUE T_FALSE 
%token T_PROCHANDLE T_STMTHANDLE T_SYMHANDLE T_MEMREFHANDLE
%token T_EXPRHANDLE T_OPHANDLE T_CONSTSYMHANDLE T_CONSTVALHANDLE
/* MemRefExpr tokens */
%token <mrtval> T_USE T_DEF
%token T_NAMEDREF T_UNNAMEDREF T_UNKNOWNREF T_DEREF T_IDXACCESS
%token T_ADDRESSOF T_FULLACCURACY T_MEMREFTYPE T_NUMDEREF T_IDX
/* Location tokens */
%token T_FULLOVERLAP T_PARTOVERLAP T_LOCAL T_FULL
%token T_NAMEDLOC T_UNNAMEDLOC T_UNKNOWNLOC T_INVISIBLELOC
%token T_LOCBASICSUBSET T_LOCIDXSUBSET
/* Alias tokens */
%token T_MAPSTO 
%token T_FORMALPARAMS
%token T_ACCESSIBLESYMS
%token T_UNNAMEDDYNALLOCSTMTS
%token T_POSSMEMREFEXPRS


/* IRHandles types */
%type <procptr> Proc
%type <stmtptr> Stmt
%type <stmtlistptr> StmtList
%type <symptr> Sym
%type <symlistptr> SymList
//%type <exprptr> Expr
%type <memrefptr> MemRef
//%type <opptr> Op
//%type <constsymptr> ConstSym
//%type <constvalptr> ConstVal
%type <bval> BoolVal
/* MemRefExpr types */
%type <mrelistptr> MemRefExprList
%type <mreptr> MemRefExprBaseFields MemRefExpr
%type <bval> BoolVal
%type <mrtval> MemRefType
/* Location types */
%type <locptr> Location



%start ProcDef

%% /* Grammer rules and actions follow */


/* AccessLoc grammer */

ProcDef:
        Proc { gCurrProc = *$1; } 
             T_MAPSTO '<' Sym','
                          T_FORMALPARAMS         '=' '[' SymList ']' ',' 
                          T_ACCESSIBLESYMS       '=' '[' SymDefList ']' ','
                          T_UNNAMEDDYNALLOCSTMTS '=' '[' StmtList ']' ','
                          T_POSSMEMREFEXPRS '=' '[' SymToMemRefExprList ']'
                      '>' 
        {
          // for getSymHandle(ProcHandle)
          gAccessLocIR->mapProcToSym(*$1,*$5);

          // for getFormalParamIterator(SymHandle)
          std::list<OA::SymHandle>::iterator listIter;
          for (listIter=$10->begin(); listIter!=$10->end(); listIter++) {
              gAccessLocIR->addFormalParam(*$5,*listIter);
          }
          delete $10;

          // for getUnnamedDynAllocStmtIterator(ProcHandle)
          std::list<OA::StmtHandle>::iterator stmtIter;
          for (stmtIter=$22->begin(); stmtIter!=$22->end(); stmtIter++) {
              gAccessLocIR->addUnnamedDynAllocStmt(*$1,*stmtIter);
          }
          delete $22;

          delete $1;
          delete $5;

          return 0;
        }
    ;

SymDefList:   
        SymDefList SymDef
    |   /* empty */
    ;

SymDef:
        Sym T_MAPSTO Location 
        {
            OA::OA_ptr<OA::Location::Location> loc; loc = $3;
            gAccessLocIR->addSym(gCurrProc,*$1,loc);
            delete $1;
        }
    ;

SymToMemRefExprList:   
        SymToMemRefExprList SymToMemRefExpr
    |   /* empty */
    ;

SymToMemRefExpr:
        Sym T_MAPSTO '[' MemRefExprList ']' 
        {
            // map symbol to each MemRefExpr 
            std::list<OA::MemRefExpr::MemRefExpr*>::iterator listIter;
            for (listIter=$4->begin(); listIter!=$4->end(); listIter++) {
                OA::OA_ptr<OA::MemRefExpr::MemRefExpr> memrefexpr;
                memrefexpr = *listIter;
                gAccessLocIR->addMemRefExpr(*$1,memrefexpr);
            }
            delete $1;
            delete $4;
        }
    ;


/***************************************************** IRHandles-grammer */

Proc:  
        T_PROCHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::ProcHandle($3);
            gAccessLocIR->addProcString(*$$,$5);
        }
    ;

Stmt:
        T_STMTHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::StmtHandle($3);
            gAccessLocIR->addStmtString(*$$,$5);
        }
    ;

StmtList:
        StmtList Stmt
        {
            $1->push_back(*$2);
            $$ = $1;
            delete $2;
        }
    |   /* empty */
        {
            $$ = new std::list<OA::StmtHandle>;
        }
    ;


Sym:
        T_SYMHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::SymHandle($3);
            gAccessLocIR->addSymString(*$$,$5);
        }
    ;

SymList:
        SymList Sym
        {
            $1->push_back(*$2);
            $$ = $1;
            delete $2;
        }
    |   /* empty */
        {
            $$ = new std::list<OA::SymHandle>;
        }
    ;

//Expr:
//        T_EXPRHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::ExprHandle($3);
//            gAccessLocIR->addExprString(*$$, $5);
//        }
//    ;


MemRef:
        T_MEMREFHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::MemRefHandle($3);
            gAccessLocIR->addMemRefString(*$$, $5);
        }
    ;


//Op:
//        T_OPHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::OpHandle($3);
//            gAccessLocIR->addOpString(*$$, $5);
//        }
//    ;

//ConstSym:
//        T_CONSTSYMHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::ConstSymHandle($3);
//            gAccessLocIR->addConstSymString(*$$, $5);
//        }
//    ;

//ConstVal:
//        T_CONSTVALHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::ConstValHandle($3);
//            gAccessLocIR->addConstValString(*$$, $5);
//        }
//    ;

/************************************************* MemRefExpr-grammer */

MemRefExprBaseFields:
        T_ADDRESSOF '=' BoolVal ',' T_FULLACCURACY '=' BoolVal ','
        T_MEMREFTYPE '=' MemRefType  
        {
            $$ = new OA::MemRefExpr::MemRefExpr($3,$7,$11);
        }
    ;

MemRefType:
        T_USE
        {
            $$ = OA::MemRefExpr::USE;
        }
    |   T_DEF
        {
            $$ = OA::MemRefExpr::DEF;
        }
    ;

MemRefExprList:
        MemRefExprList MemRefExpr
        {
            $1->push_back($2);
            $$ = $1;
        }

    |   /* empty */
        {
            $$ = new std::list<OA::MemRefExpr::MemRefExpr*>;
        }
    ;

MemRefExpr:
        T_NAMEDREF ',' '<' Sym ',' MemRefExprBaseFields '>'
        {
            $$ = new OA::MemRefExpr::NamedRef(*$6, *$4);
            delete $4;
            delete $6;
        }

    |   T_UNNAMEDREF ',' '<' Stmt ',' MemRefExprBaseFields '>'
        {
            $$ = new OA::MemRefExpr::UnnamedRef(*$6,*$4);
            delete $4;
            delete $6;
        }

    |   T_UNKNOWNREF '<' MemRefType '>' 
        {
            $$ = new OA::MemRefExpr::UnknownRef($3);
        }

    |   T_DEREF ',' '<' MemRefExpr ',' T_NUMDEREF '=' T_INT_LITERAL '>'
        {
            OA::OA_ptr<OA::MemRefExpr::MemRefExpr> ref; ref = $4;
            $$ = new OA::MemRefExpr::Deref(ref,$8);
        }

    |   T_IDXACCESS ',' '<' MemRefExpr ',' T_IDX '=' T_INT_LITERAL '>' 
        {
            OA::OA_ptr<OA::MemRefExpr::MemRefExpr> ref; ref = $4;
            $$ = new OA::MemRefExpr::IdxAccess(ref,$8);
        }
    ;

BoolVal:
        T_TRUE
        {
            $$ = true;
        }
    |   T_FALSE
        {
            $$ = false;
        }
    ;

/*************************************************** Location-grammer */

Location:
        T_NAMEDLOC ',' '<' Sym ',' T_LOCAL '=' BoolVal ',' 
                              T_FULLOVERLAP '=' '[' SymList ']' ',' 
                              T_PARTOVERLAP '=' '[' SymList ']' '>' 
        {
            OA::Location::NamedLoc* loc = new OA::Location::NamedLoc(*$4,$8);
            delete $4;
            
            // get all fully overlapping locations from first SymList
            std::list<OA::SymHandle>::iterator listIter;
            for (listIter=$13->begin(); listIter!=$13->end(); listIter++) {
                loc->addFullOverlap(*listIter);
            }
            delete $13;

            // get all fully overlapping locations from first SymList
            for (listIter=$19->begin(); listIter!=$19->end(); listIter++) {
                loc->addPartOverlap(*listIter);
            }
            delete $19;

            $$ = loc;

         }

    |   T_UNNAMEDLOC ',' '<' Stmt ',' T_LOCAL '=' BoolVal '>' 
        {
            $$ = new OA::Location::UnnamedLoc(*$4,$8);
        }

    |   T_INVISIBLELOC ',' '<' MemRefExpr '>' 
        {
            OA::OA_ptr<OA::MemRefExpr::MemRefExpr> ref; ref = $4;
            $$ = new OA::Location::InvisibleLoc(ref);
        }

    |   T_UNKNOWNLOC 
        {
            $$ = new OA::Location::UnknownLoc();
        }

    |   T_LOCBASICSUBSET ',' '<' Location ',' T_FULL '=' BoolVal '>' 
        {
            OA::OA_ptr<OA::Location::Location> loc; loc = $4;
            $$ = new OA::Location::LocBasicSubSet(loc,$8);
        }

    |   T_LOCIDXSUBSET ',' '<' Location ',' T_IDX '=' T_INT_LITERAL '>' 
        {
            OA::OA_ptr<OA::Location::Location> loc; loc = $4;
            $$ = new OA::Location::LocIdxSubSet(loc,$8);
        }
    ;

%%
#include <stdio.h>
void yyerror (char *s)  /* Called by yyparse on error */
{
    printf ("%s\n", s);
}

/*
main ()     
{
#ifdef YYDEBUG
  yydebug = 1;
#endif
  yyparse ();
}
*/


