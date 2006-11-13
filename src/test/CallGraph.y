%{
extern int yylex ();
#define YYERROR_VERBOSE
#define YYDEBUG 1
void yyerror(char*);

// specific to this parser
#include "CallGraphIRInterfaceTest.hpp"
extern OA::OA_ptr<CallGraphIRInterfaceTest> gCallGraphIR;

static OA::ProcHandle gCurrProc;
static OA::StmtHandle gCurrStmt;
%}

%union {
    char*   sval;
    int     ival;
    OA::ProcHandle* procptr;
    OA::StmtHandle* stmtptr;
    OA::SymHandle* symptr;
    OA::ExprHandle* exprptr;
    std::list<OA::ExprHandle>* exprlistptr;
};

/* IRHandles tokens */
%token <sval> T_STR_LITERAL 
%token <ival> T_INT_LITERAL
%token T_PROCHANDLE T_STMTHANDLE T_SYMHANDLE T_MEMREFHANDLE
%token T_EXPRHANDLE T_OPHANDLE T_CONSTSYMHANDLE T_CONSTVALHANDLE
/* CallGraphtokens */
%token T_MAPSTO 
%token T_CALLSITES
%token T_STMTS


/* IRHandles types */
%type <procptr> Proc
%type <stmtptr> Stmt
%type <symptr> Sym
//%type <symlistptr> SymList
%type <exprptr> Expr
%type <exprlistptr> ExprList
//%type <memrefptr> MemRef
//%type <opptr> Op
//%type <constsymptr> ConstSym
//%type <constvalptr> ConstVal
//%type <bval> BoolVal

%start ProcDef

%% /* Grammer rules and actions follow */


/* CallGraph grammer */

ProcDef:
        Proc { gCurrProc = *$1; } 
             T_MAPSTO '<' Sym ','
                          T_STMTS '=' '[' StmtDefList ']' ','
                          T_CALLSITES         '=' '[' CallsiteDefList ']' 
                      '>' 
        {
          gCallGraphIR->mapProcToSym(*$1, *$5);

          delete $1;
          delete $5;
          return 0;
        }
    ;

StmtDefList:
        StmtDefList StmtDef
    |   /* empty */
    ;

StmtDef:
        Stmt T_MAPSTO '<' T_CALLSITES '=' '[' ExprList ']' '>'
        {
            // get all callsites for this statement
            std::list<OA::ExprHandle>::iterator listIter;
            for (listIter=$7->begin(); listIter!=$7->end(); listIter++) {
                gCallGraphIR->addStmtCallsite(gCurrProc,*$1,*listIter);
            }
            delete $7;
            delete $1;
        }
    ;

CallsiteDefList:
        CallsiteDefList CallsiteDef
    |   /* empty */
    ;

CallsiteDef:
        Expr T_MAPSTO '<' Sym '>'
        {
            gCallGraphIR->addCallSym(*$1, *$4);
            delete $1;
            delete $4;
        }
    ;


/***************************************************** IRHandles-grammer */

Proc:  
        T_PROCHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::ProcHandle($3);
            gCallGraphIR->addProcString(*$$,$5);
        }
    ;

Stmt:
        T_STMTHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::StmtHandle($3);
            gCallGraphIR->addStmtString(*$$,$5);
        }
    ;

Sym:
        T_SYMHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::SymHandle($3);
            gCallGraphIR->addSymString(*$$,$5);
        }
    ;

//SymList:
//        SymList Sym
//        {
//            $1->push_back(*$2);
//            $$ = $1;
//            delete $2;
//        }
//    |   /* empty */
//        {
//            $$ = new std::list<OA::SymHandle>;
//        }
//    ;

Expr:
        T_EXPRHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::ExprHandle($3);
            gCallGraphIR->addExprString(*$$, $5);
        }
    ;

ExprList:
        ExprList Expr
        {
            $1->push_back(*$2);
            $$ = $1;
            delete $2;
        }
    |   /* empty */
        {
            $$ = new std::list<OA::ExprHandle>;
        }
    ;


//MemRef:
//        T_MEMREFHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::MemRefHandle($3);
//            gCallGraphIR->addMemRefString(*$$, $5);
//        }
//    ;


//Op:
//        T_OPHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::OpHandle($3);
//            gCallGraphIR->addOpString(*$$, $5);
//        }
//    ;

//ConstSym:
//        T_CONSTSYMHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::ConstSymHandle($3);
//            gCallGraphIR->addConstSymString(*$$, $5);
//        }
//    ;

//ConstVal:
//        T_CONSTVALHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::ConstValHandle($3);
//            gCallGraphIR->addConstValString(*$$, $5);
//        }
//    ;

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


