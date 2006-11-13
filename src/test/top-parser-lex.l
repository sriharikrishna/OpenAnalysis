/* scanner for OpenAnalysis/src/test/top-parser */

%{
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "top-parser.token.h"

// for passing ID string to top-parser.c, don't really need this
//char* retstr;

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

[\{\}]               { return yytext[0]; }

PROCEDURE            { return T_PROCEDURE; }

AliasIRInterface     { return T_ALIASIRINTERFACE; }

AccessLocIRInterface { return T_ACCESSLOCIRINTERFACE; }

CallGraphIRInterface { return T_CALLGRAPHIRINTERFACE; }

CallGraphDFProblemIRInterface { return T_CALLGRAPHDFPROBLEMIRINTERFACE; }

SideEffectIRInterface { return T_SIDEEFFECTIRINTERFACE; }

DepIRInterface       { return T_DEPIRINTERFACE; }

END                  { return T_END; }

{ID}+                { char *retstr = (char*)malloc(strlen(yytext)+1);
                       strcpy(retstr,yytext);
                       //yylval.sval = retstr; 
                       return T_ID;
                     }

"\/\/"[^\n]*"\n"     /* eat up one-line comments */

[ \t\n]+             /* eat up whitespace */



%%

int yywrap() { return 1; }
