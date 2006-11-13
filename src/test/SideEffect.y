%{
extern int yylex ();
#define YYERROR_VERBOSE
#define YYDEBUG 1
void yyerror(char*);

// specific to this parser
#include "SideEffectIRInterfaceTest.hpp"
extern OA::OA_ptr<SideEffectIRInterfaceTest> gSideEffectIR;

static OA::ProcHandle gCurrProc;
static OA::StmtHandle gCurrStmt;
%}

%union {
    char*   sval;
    int     ival;
    OA::ProcHandle* procptr;
    OA::StmtHandle* stmtptr;
    std::list<OA::StmtHandle>* stmtlistptr;
    OA::ExprHandle* exprptr;
    std::list<OA::ExprHandle>* exprlistptr;
    OA::MemRefHandle* memrefptr;
    std::list<OA::MemRefHandle>* memreflistptr;
};

/* IRHandles tokens */
%token <sval> T_STR_LITERAL 
%token <ival> T_INT_LITERAL
%token <bval> T_TRUE T_FALSE 
%token T_PROCHANDLE T_STMTHANDLE T_SYMHANDLE T_MEMREFHANDLE
%token T_EXPRHANDLE T_OPHANDLE T_CONSTSYMHANDLE T_CONSTVALHANDLE
/* SideEffect tokens */
%token T_MAPSTO 
%token T_STMTS
%token T_USES
%token T_DEFS
%token T_CALLSITES


/* IRHandles types */
%type <procptr> Proc
%type <stmtptr> Stmt
%type <stmtlistptr> StmtList
%type <exprptr> Expr
%type <exprlistptr> ExprList
%type <memrefptr> MemRef
%type <memreflistptr> MemRefList


%start ProcDef

%% /* Grammer rules and actions follow */


/* SideEffect grammer */

ProcDef:
        Proc { gCurrProc = *$1; } T_MAPSTO 
            '<' T_STMTS '=' '[' StmtDefList ']' '>'
        {
          delete $1;

          return 0;
        }
    ;    

StmtDefList: 
        StmtDefList StmtDef
    |   /* empty */
    ;

StmtDef:
        Stmt { gCurrStmt = *$1; } T_MAPSTO 
            '<' T_USES         '=' '[' MemRefList ']' ',' 
                T_DEFS         '=' '[' MemRefList ']' ','
                T_CALLSITES    '=' '[' ExprList ']' 
            '>' 
        {
          // add the statement to the procedure
          gSideEffectIR->addStmt(gCurrProc,*$1);
          std::cout << "In sideeffectparse: stmt = " 
                    << gSideEffectIR->toString(*$1) << std::endl;

          // for getUseMemRefs
          std::list<OA::MemRefHandle>::iterator listIter;
          for (listIter=$8->begin(); listIter!=$8->end(); listIter++) {
              gSideEffectIR->addUseMemRef(*$1,*listIter);
          }
          delete $8;

          // for getDefMemRefs
          for (listIter=$14->begin(); listIter!=$14->end(); listIter++) {
              gSideEffectIR->addDefMemRef(*$1,*listIter);
          }
          delete $14;

          // for getCallsites
          std::list<OA::ExprHandle>::iterator exprIter;
          for (exprIter=$20->begin(); exprIter!=$20->end(); exprIter++) {
              gSideEffectIR->addCallsite(*$1,*exprIter);
          }
          delete $20;

          delete $1;
        }
    ;



/***************************************************** IRHandles-grammer */

Proc:  
        T_PROCHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::ProcHandle($3);
            gSideEffectIR->addProcString(*$$,$5);
        }
    ;

Stmt:
        T_STMTHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::StmtHandle($3);
            gSideEffectIR->addStmtString(*$$,$5);
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


//Sym:
//        T_SYMHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::SymHandle($3);
//            gSideEffectIR->addSymString(*$$,$5);
//        }
//    ;

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


MemRefList:
        MemRefList MemRef
        {
            $1->push_back(*$2);
            $$ = $1;
            delete $2;
        }
    |   /* empty */
        {
            $$ = new std::list<OA::MemRefHandle>;
        }
    ;

Expr:
        T_EXPRHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::ExprHandle($3);
            gSideEffectIR->addExprString(*$$, $5);
        }
    ;


MemRef:
        T_MEMREFHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::MemRefHandle($3);
            gSideEffectIR->addMemRefString(*$$, $5);
        }
    ;


//Op:
//        T_OPHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::OpHandle($3);
//            gSideEffectIR->addOpString(*$$, $5);
//        }
//    ;

//ConstSym:
//        T_CONSTSYMHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::ConstSymHandle($3);
//            gSideEffectIR->addConstSymString(*$$, $5);
//        }
//    ;

//ConstVal:
//        T_CONSTVALHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::ConstValHandle($3);
//            gSideEffectIR->addConstValString(*$$, $5);
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


