/* scanner for Alias */

%{
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "AliasIRInterfaceTest.hpp"
#include "Alias.tab.h"

/* for reading from a string */
#undef YY_INPUT
extern int my_yyinput_getchar();
#define YY_INPUT(buf,result,max_size) \
    { \
        int c = my_yyinput_getchar(); \
        result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
    }


%}

DIGIT   [0-9]
ID      [a-zA-Z][a-zA-Z0-9]*

%%

[\(,\)\[\]\<\>\=] { return yytext[0]; }

"\""[^\"]*"\"" { char *retstr = (char*)malloc(strlen(yytext)+1);
                  strcpy(retstr,yytext);
                  aliaslval.sval = retstr; 
                  return T_STR_LITERAL; 
                }

{DIGIT}+        { aliaslval.ival = atoi(yytext); 
                  return T_INT_LITERAL; }

=>              { return T_MAPSTO; }

false           { return T_FALSE; }

true            { return T_TRUE; }

ProcHandle      { return T_PROCHANDLE; }

StmtHandle      { return T_STMTHANDLE; }

SymHandle       { return T_SYMHANDLE; }

MemRefHandle    { return T_MEMREFHANDLE; }

ExprHandle      { return T_EXPRHANDLE; }

OpHandle        { return T_OPHANDLE; }

ConstSymHandle  { return T_CONSTSYMHANDLE; }

ConstValHandle  { return T_CONSTVALHANDLE; }

USE             { return T_USE; }

DEF             { return T_DEF; }

NamedRef        { return T_NAMEDREF; }

UnnamedRef      { return T_UNNAMEDREF; }

UnknownRef      { return T_UNKNOWNREF; }

Deref           { return T_DEREF; }

IdxAccess       { return T_IDXACCESS; }

addressOf       { return T_ADDRESSOF; }

fullAccuracy    { return T_FULLACCURACY; }

memRefType      { return T_MEMREFTYPE; }

numDeref        { return T_NUMDEREF; }

idx             { return T_IDX; }

NamedLoc        { return T_NAMEDLOC; }

UnnamedLoc      { return T_UNNAMEDLOC; }

UnknownLoc      { return T_UNKNOWNLOC; }

LocBasicSubSet  { return T_LOCBASICSUBSET; }

LocIdxSubSet    { return T_LOCIDXSUBSET; }

InvisibleLoc    { return T_INVISIBLELOC; }

fullOverlap     { return T_FULLOVERLAP; }

partOverlap     { return T_PARTOVERLAP; }

local           { return T_LOCAL; }

full            { return T_FULL; }

Syms            { return T_SYMS; }

Stmts           { return T_STMTS; }

MemRefs         { return T_MEMREFS; }

ANY_STMT        { return T_ANY; }

PTR_ASSIGN_STMT { return T_PTRASSIGN; }

"\/\/"[^\n]*"\n"  /* eat up one-line comments */

[ \t\n]+        /* eat up whitespace */



%%

int yywrap() { return 1; }
