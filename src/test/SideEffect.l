/* scanner for SideEffect analysis in OpenAnalysis */

%{
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "SideEffectIRInterfaceTest.hpp"
#include "SideEffect.tab.h"

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
                  sideeffectlval.sval = retstr; 
                  return T_STR_LITERAL; 
                }

{DIGIT}+        { sideeffectlval.ival = atoi(yytext); 
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

Uses            { return T_USES; }

Defs            { return T_DEFS; }

Callsites       { return T_CALLSITES; }

Stmts           { return T_STMTS; }

"\/\/"[^\n]*"\n"  /* eat up one-line comments */

[ \t\n]+        /* eat up whitespace */



%%

int yywrap() { return 1; }
