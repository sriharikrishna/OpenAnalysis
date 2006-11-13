%{
extern int yylex ();
#define YYERROR_VERBOSE
#define YYDEBUG 1
void yyerror(char*);

// specific to this parser
#include "CallGraphDFProblemIRInterfaceTest.hpp"
extern OA::OA_ptr<CallGraphDFProblemIRInterfaceTest> gCallGraphDFProblemIR;

static OA::ProcHandle gCurrProc;
static OA::StmtHandle gCurrStmt;
%}

%union {
    char*   sval;
    int     ival;
    bool    bval;
    OA::ProcHandle* procptr;
    OA::StmtHandle* stmtptr;
    OA::SymHandle* symptr;
    std::list<OA::SymHandle>* symlistptr;
    OA::ExprHandle* exprptr;
    std::list<OA::ExprHandle>* exprlistptr;
    OA::MemRefHandle* memrefptr;
    OA::OpHandle* opptr;
    OA::ConstSymHandle* constsymptr;
    OA::ConstValHandle* constvalptr;

    OA::Location::Location* locptr;

    OA::ExprTree::ExprTree* etreeptr;
};

/* IRHandles tokens */
%token <sval> T_STR_LITERAL 
%token <ival> T_INT_LITERAL
%token <bval> T_TRUE T_FALSE 
%token T_PROCHANDLE T_STMTHANDLE T_SYMHANDLE T_MEMREFHANDLE
%token T_EXPRHANDLE T_OPHANDLE T_CONSTSYMHANDLE T_CONSTVALHANDLE
/* ExprTree tokens */
%token T_CHILDREN
%token T_MEMREFNODE
// FIXME: will eventually need all the other nodes

/* Location tokens */
%token T_FULLOVERLAP T_PARTOVERLAP T_LOCAL T_FULL
%token T_NAMEDLOC T_UNNAMEDLOC T_UNKNOWNLOC T_INVISIBLELOC
%token T_LOCBASICSUBSET T_LOCIDXSUBSET
/* CallGraphDFProblem tokens */
%token T_MAPSTO 
%token T_SYMS
%token T_PARAMS
%token T_CALLSITES
%token T_CALLSITEPARAMS
%token T_CALLSITEPARAMINFO
%token T_FORMAL T_EXPRTREE T_REFPARAM



/* IRHandles types */
%type <procptr> Proc
%type <stmtptr> Stmt
%type <symptr> Sym
%type <symlistptr> SymList
%type <exprptr> Expr
%type <exprlistptr> ExprList
%type <memrefptr> MemRef
//%type <opptr> Op
//%type <constsymptr> ConstSym
//%type <constvalptr> ConstVal
%type <bval> BoolVal

/* Location types */
%type <locptr> Location

/* ExprTree types */
%type <etreeptr> ExprTree

%start ProcDef

%% /* Grammer rules and actions follow */


/* CallGraphDFProblem grammer */

ProcDef:
        Proc { gCurrProc = *$1; } 
             T_MAPSTO '<' T_PARAMS            '=' '[' ParamDefList ']' ','
                          T_CALLSITES         '=' '[' CallsiteDefList ']' ','
                          T_CALLSITEPARAMINFO '=' '[' CallsiteParamDefList ']' ','
                          T_SYMS     '=' '[' SymDefList ']' 
                      '>' 
        {
          return 0;
        }
    ;

ParamDefList:
        ParamDefList ParamDef
    |   /* empty */
    ;

ParamDef:
        Sym T_MAPSTO '<' T_REFPARAM '=' BoolVal '>'
        {
           gCallGraphDFProblemIR->addFormalParam(*$1, $6); 
           delete $1;
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

CallsiteDefList:
        CallsiteDefList CallsiteDef
    |   /* empty */
    ;

CallsiteDef:
        Expr T_MAPSTO '<' T_CALLSITEPARAMS '=' '[' ExprList ']' '>'
        {
            // get all parameter expressions for this callsite
            std::list<OA::ExprHandle>::iterator listIter;
            for (listIter=$7->begin(); listIter!=$7->end(); listIter++) {
                gCallGraphDFProblemIR->addCallsiteParam(*$1,*listIter);
            }
            delete $7;
            delete $1;
        }
    ;

CallsiteParamDefList:
        CallsiteParamDefList CallsiteParamDef
    |   /* empty */
    ;

CallsiteParamDef:
        Expr T_MAPSTO '<' T_FORMAL '=' Sym ',' T_EXPRTREE '=' ExprTree '>'
        {
            OA::OA_ptr<OA::ExprTree::ExprTree> etree; etree = $10;
            gCallGraphDFProblemIR->addCallsiteParamInfo(*$1,*$6,etree);
            delete $1;
            delete $6;
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
            gCallGraphDFProblemIR->addSym(gCurrProc,*$1,loc);
            delete $1;
        }
    ;


/***************************************************** IRHandles-grammer */

Proc:  
        T_PROCHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::ProcHandle($3);
            gCallGraphDFProblemIR->addProcString(*$$,$5);
        }
    ;

Stmt:
        T_STMTHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::StmtHandle($3);
            gCallGraphDFProblemIR->addStmtString(*$$,$5);
        }
    ;

Sym:
        T_SYMHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::SymHandle($3);
            gCallGraphDFProblemIR->addSymString(*$$,$5);
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

Expr:
        T_EXPRHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::ExprHandle($3);
            gCallGraphDFProblemIR->addExprString(*$$, $5);
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


MemRef:
        T_MEMREFHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
        {
            $$ = new OA::MemRefHandle($3);
            gCallGraphDFProblemIR->addMemRefString(*$$, $5);
        }
    ;


//Op:
//        T_OPHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::OpHandle($3);
//            gCallGraphDFProblemIR->addOpString(*$$, $5);
//        }
//    ;

//ConstSym:
//        T_CONSTSYMHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::ConstSymHandle($3);
//            gCallGraphDFProblemIR->addConstSymString(*$$, $5);
//        }
//    ;

//ConstVal:
//        T_CONSTVALHANDLE '(' T_INT_LITERAL ',' T_STR_LITERAL ')'
//        {
//            $$ = new OA::ConstValHandle($3);
//            gCallGraphDFProblemIR->addConstValString(*$$, $5);
//        }
//    ;

/************************************************* ExprTree-grammer */
//ExprTreeList:
//        ExprTreeList ExprTree
//    |   /* empty */
//        { $$ = new std::list<ExprTree::ExprTree*>; }
//    ;

// for now just handling a memory reference node 
// When doing full ExprTree need to add some functionality to ExprTree's
// that allow merging them together, right now can't have nodes in multiple
// trees in Utils/Tree implementation
ExprTree:
        T_MEMREFNODE ',' '<' MemRef ',' T_CHILDREN '=' '[' ']' '>'
        {
            // make an expression tree if we are a leaf node
            //if ($9->empty()) {
                $$ = new OA::ExprTree::ExprTree;
            //}

            // for now we will just be the root node
            OA::OA_ptr<OA::ExprTree::ExprTree::Node> node;
            node = new OA::ExprTree::ExprTree::MemRefNode(*$4);
            $$->addNode( node );
            $$->dump(std::cout,gCallGraphDFProblemIR);
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
      ;

//    |   T_UNNAMEDLOC ',' '<' Stmt ',' T_LOCAL '=' BoolVal '>' 
//        {
//            $$ = new OA::Location::UnnamedLoc(*$4,$8);
//        }

//    |   T_INVISIBLELOC ',' '<' MemRefExpr '>' 
//        {
//            OA::OA_ptr<OA::MemRefExpr::MemRefExpr> ref; ref = $4;
//            $$ = new OA::Location::InvisibleLoc(ref);
//        }

//    |   T_UNKNOWNLOC 
//        {
//            $$ = new OA::Location::UnknownLoc();
//        }

//    |   T_LOCBASICSUBSET ',' '<' Location ',' T_FULL '=' BoolVal '>' 
//        {
//            OA::OA_ptr<OA::Location::Location> loc; loc = $4;
//            $$ = new OA::Location::LocBasicSubSet(loc,$8);
//        }

//    |   T_LOCIDXSUBSET ',' '<' Location ',' T_IDX '=' T_INT_LITERAL '>' 
//        {
//            OA::OA_ptr<OA::Location::Location> loc; loc = $4;
//            $$ = new OA::Location::LocIdxSubSet(loc,$8);
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


