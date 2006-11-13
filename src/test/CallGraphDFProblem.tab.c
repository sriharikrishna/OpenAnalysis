
/*  A Bison parser, made from CallGraphDFProblem.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse callgraphdfproblemparse
#define yylex callgraphdfproblemlex
#define yyerror callgraphdfproblemerror
#define yylval callgraphdfproblemlval
#define yychar callgraphdfproblemchar
#define yydebug callgraphdfproblemdebug
#define yynerrs callgraphdfproblemnerrs
#define	T_STR_LITERAL	257
#define	T_INT_LITERAL	258
#define	T_TRUE	259
#define	T_FALSE	260
#define	T_PROCHANDLE	261
#define	T_STMTHANDLE	262
#define	T_SYMHANDLE	263
#define	T_MEMREFHANDLE	264
#define	T_EXPRHANDLE	265
#define	T_OPHANDLE	266
#define	T_CONSTSYMHANDLE	267
#define	T_CONSTVALHANDLE	268
#define	T_CHILDREN	269
#define	T_MEMREFNODE	270
#define	T_FULLOVERLAP	271
#define	T_PARTOVERLAP	272
#define	T_LOCAL	273
#define	T_FULL	274
#define	T_NAMEDLOC	275
#define	T_UNNAMEDLOC	276
#define	T_UNKNOWNLOC	277
#define	T_INVISIBLELOC	278
#define	T_LOCBASICSUBSET	279
#define	T_LOCIDXSUBSET	280
#define	T_MAPSTO	281
#define	T_SYMS	282
#define	T_PARAMS	283
#define	T_CALLSITES	284
#define	T_CALLSITEPARAMS	285
#define	T_CALLSITEPARAMINFO	286
#define	T_FORMAL	287
#define	T_EXPRTREE	288
#define	T_REFPARAM	289

#line 1 "CallGraphDFProblem.y"

extern int yylex ();
#define YYERROR_VERBOSE
#define YYDEBUG 1
void yyerror(char*);

// specific to this parser
#include "CallGraphDFProblemIRInterfaceTest.hpp"
extern OA::OA_ptr<CallGraphDFProblemIRInterfaceTest> gCallGraphDFProblemIR;

static OA::ProcHandle gCurrProc;
static OA::StmtHandle gCurrStmt;

#line 15 "CallGraphDFProblem.y"
typedef union {
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
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		124
#define	YYFLAG		-32768
#define	YYNTBASE	44

#define YYTRANSLATE(x) ((unsigned)(x) <= 289 ? yytranslate[x] : 63)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    42,
    43,     2,     2,    40,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    36,
    37,    41,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    38,     2,    39,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,    30,    33,    34,    42,    44,    46,    49,    50,
    60,    63,    64,    76,    79,    80,    84,    91,    98,   105,
   108,   109,   116,   119,   120,   127,   138
};

static const short yyrhs[] = {    -1,
    55,    45,    27,    36,    29,    37,    38,    46,    39,    40,
    30,    37,    38,    49,    39,    40,    32,    37,    38,    51,
    39,    40,    28,    37,    38,    53,    39,    41,     0,    46,
    47,     0,     0,    56,    27,    36,    35,    37,    48,    41,
     0,     5,     0,     6,     0,    49,    50,     0,     0,    58,
    27,    36,    31,    37,    38,    59,    39,    41,     0,    51,
    52,     0,     0,    58,    27,    36,    33,    37,    56,    40,
    34,    37,    61,    41,     0,    53,    54,     0,     0,    56,
    27,    62,     0,     7,    42,     4,    40,     3,    43,     0,
     8,    42,     4,    40,     3,    43,     0,     9,    42,     4,
    40,     3,    43,     0,    57,    56,     0,     0,    11,    42,
     4,    40,     3,    43,     0,    59,    58,     0,     0,    10,
    42,     4,    40,     3,    43,     0,    16,    40,    36,    60,
    40,    15,    37,    38,    39,    41,     0,    21,    40,    36,
    56,    40,    19,    37,    48,    40,    17,    37,    38,    57,
    39,    40,    18,    37,    38,    57,    39,    41,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    87,    89,    99,   101,   104,   112,   117,   123,   125,   128,
   141,   143,   146,   156,   158,   161,   173,   181,   189,   197,
   204,   210,   218,   225,   232,   276,   294
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_STR_LITERAL",
"T_INT_LITERAL","T_TRUE","T_FALSE","T_PROCHANDLE","T_STMTHANDLE","T_SYMHANDLE",
"T_MEMREFHANDLE","T_EXPRHANDLE","T_OPHANDLE","T_CONSTSYMHANDLE","T_CONSTVALHANDLE",
"T_CHILDREN","T_MEMREFNODE","T_FULLOVERLAP","T_PARTOVERLAP","T_LOCAL","T_FULL",
"T_NAMEDLOC","T_UNNAMEDLOC","T_UNKNOWNLOC","T_INVISIBLELOC","T_LOCBASICSUBSET",
"T_LOCIDXSUBSET","T_MAPSTO","T_SYMS","T_PARAMS","T_CALLSITES","T_CALLSITEPARAMS",
"T_CALLSITEPARAMINFO","T_FORMAL","T_EXPRTREE","T_REFPARAM","'<'","'='","'['",
"']'","','","'>'","'('","')'","ProcDef","@1","ParamDefList","ParamDef","BoolVal",
"CallsiteDefList","CallsiteDef","CallsiteParamDefList","CallsiteParamDef","SymDefList",
"SymDef","Proc","Sym","SymList","Expr","ExprList","MemRef","ExprTree","Location", NULL
};
#endif

static const short yyr1[] = {     0,
    45,    44,    46,    46,    47,    48,    48,    49,    49,    50,
    51,    51,    52,    53,    53,    54,    55,    -1,    56,    57,
    57,    58,    59,    59,    60,    61,    62
};

static const short yyr2[] = {     0,
     0,    28,     2,     0,     7,     1,     1,     2,     0,     9,
     2,     0,    11,     2,     0,     3,     6,     6,     6,     2,
     0,     6,     2,     0,     6,    10,    21
};

static const short yydefact[] = {     0,
     0,     1,     0,     0,     0,     0,     0,     0,     0,     0,
    17,     0,     4,     0,     0,     0,     3,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     9,     0,
    19,     0,     6,     7,     0,     0,     0,     8,     0,     5,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    12,     0,    22,     0,    24,     0,    11,     0,     0,     0,
     0,     0,    23,     0,     0,    10,     0,     0,    15,     0,
     0,     0,     0,    14,     0,     0,     2,     0,     0,     0,
    16,     0,     0,     0,     0,     0,     0,    13,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    25,     0,     0,    26,
    21,     0,     0,    20,     0,     0,     0,    21,     0,     0,
    27,     0,     0,     0
};

static const short yydefgoto[] = {   122,
     4,    14,    17,    35,    32,    38,    54,    57,    71,    74,
     2,   114,   112,    39,    59,    93,    85,    81
};

static const short yypact[] = {    -1,
   -29,-32768,    10,   -12,   -24,   -19,    15,   -10,   -23,   -16,
-32768,   -15,-32768,    -8,   -20,   -13,-32768,     1,    20,     0,
   -11,     2,     3,    -9,    26,     5,     4,     7,-32768,     6,
-32768,    -4,-32768,-32768,     8,     9,    12,-32768,    11,-32768,
    35,    13,    17,    14,    18,    16,    41,    21,    23,    19,
-32768,    25,-32768,    -3,-32768,    24,-32768,    31,    -2,    33,
    29,    27,-32768,    30,    36,-32768,    28,    34,-32768,    37,
    -7,    38,    32,-32768,    43,    40,-32768,    54,    39,    42,
-32768,    61,    44,    45,    46,    37,    47,-32768,    48,    69,
    62,    49,    50,    52,    80,    71,     6,    53,    55,    56,
    91,    57,    81,    58,    60,    63,-32768,    64,    59,-32768,
-32768,    -6,    66,-32768,    84,    67,    65,-32768,    -5,    68,
-32768,   107,   108,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,    22,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   -14,   -70,   -49,-32768,-32768,-32768,-32768
};


#define	YYLAST		119


static const short yytable[] = {    18,
    15,    15,    15,    15,    58,     1,    36,    36,    36,    63,
    33,    34,     3,     5,     6,     7,     8,     9,    10,    11,
    12,    19,    13,    22,    24,    27,    20,    21,    28,    23,
    16,    73,   113,   120,    37,    56,    62,    43,    44,    26,
    30,    25,    29,    50,    45,    15,    49,   119,    40,    31,
    41,    42,    46,    47,    48,    72,    75,    61,    51,    52,
    64,    53,    55,    60,    65,    69,    67,    66,    68,    78,
    70,    89,    77,    79,    80,    82,    84,    76,    92,    86,
    94,    83,    90,    98,    87,    99,    88,    91,    97,    96,
    95,   102,   101,   104,   105,   103,   111,   106,   108,   109,
   107,   116,   118,   117,   110,   115,   123,   124,   121,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   100
};

static const short yycheck[] = {    14,
     9,     9,     9,     9,    54,     7,    11,    11,    11,    59,
     5,     6,    42,     4,    27,    40,    36,     3,    29,    43,
    37,    42,    38,     4,    36,    35,    40,    27,     3,    30,
    39,    39,    39,    39,    39,    39,    39,    27,     4,    37,
    37,    40,    38,     3,    32,     9,    31,   118,    41,    43,
    42,    40,    36,    40,    37,    70,    71,    27,    38,    37,
    28,    43,    38,    40,    36,    38,    37,    41,    33,    27,
    37,    86,    41,    34,    21,    37,    16,    40,    10,    36,
    19,    40,    36,     4,    40,    15,    41,    40,    37,    40,
    42,    37,    40,     3,    38,    40,    38,    17,    39,    37,
    43,    18,    38,    37,    41,    40,     0,     0,    41,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    97
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 88 "CallGraphDFProblem.y"
{ gCurrProc = *yyvsp[0].procptr; ;
    break;}
case 2:
#line 94 "CallGraphDFProblem.y"
{
          return 0;
        ;
    break;}
case 5:
#line 106 "CallGraphDFProblem.y"
{
           gCallGraphDFProblemIR->addFormalParam(*yyvsp[-6].symptr, yyvsp[-1].bval); 
           delete yyvsp[-6].symptr;
        ;
    break;}
case 6:
#line 114 "CallGraphDFProblem.y"
{
            yyval.bval = true;
        ;
    break;}
case 7:
#line 118 "CallGraphDFProblem.y"
{
            yyval.bval = false;
        ;
    break;}
case 10:
#line 130 "CallGraphDFProblem.y"
{
            // get all parameter expressions for this callsite
            std::list<OA::ExprHandle>::iterator listIter;
            for (listIter=yyvsp[-2].exprlistptr->begin(); listIter!=yyvsp[-2].exprlistptr->end(); listIter++) {
                gCallGraphDFProblemIR->addCallsiteParam(*yyvsp[-8].exprptr,*listIter);
            }
            delete yyvsp[-2].exprlistptr;
            delete yyvsp[-8].exprptr;
        ;
    break;}
case 13:
#line 148 "CallGraphDFProblem.y"
{
            OA::OA_ptr<OA::ExprTree::ExprTree> etree; etree = yyvsp[-1].etreeptr;
            gCallGraphDFProblemIR->addCallsiteParamInfo(*yyvsp[-10].exprptr,*yyvsp[-5].symptr,etree);
            delete yyvsp[-10].exprptr;
            delete yyvsp[-5].symptr;
        ;
    break;}
case 16:
#line 163 "CallGraphDFProblem.y"
{
            OA::OA_ptr<OA::Location::Location> loc; loc = yyvsp[0].locptr;
            gCallGraphDFProblemIR->addSym(gCurrProc,*yyvsp[-2].symptr,loc);
            delete yyvsp[-2].symptr;
        ;
    break;}
case 17:
#line 175 "CallGraphDFProblem.y"
{
            yyval.procptr = new OA::ProcHandle(yyvsp[-3].ival);
            gCallGraphDFProblemIR->addProcString(*yyval.procptr,yyvsp[-1].sval);
        ;
    break;}
case 18:
#line 183 "CallGraphDFProblem.y"
{
            yyval.stmtptr = new OA::StmtHandle(yyvsp[-3].ival);
            gCallGraphDFProblemIR->addStmtString(*yyval.stmtptr,yyvsp[-1].sval);
        ;
    break;}
case 19:
#line 191 "CallGraphDFProblem.y"
{
            yyval.symptr = new OA::SymHandle(yyvsp[-3].ival);
            gCallGraphDFProblemIR->addSymString(*yyval.symptr,yyvsp[-1].sval);
        ;
    break;}
case 20:
#line 199 "CallGraphDFProblem.y"
{
            yyvsp[-1].symlistptr->push_back(*yyvsp[0].symptr);
            yyval.symlistptr = yyvsp[-1].symlistptr;
            delete yyvsp[0].symptr;
        ;
    break;}
case 21:
#line 205 "CallGraphDFProblem.y"
{
            yyval.symlistptr = new std::list<OA::SymHandle>;
        ;
    break;}
case 22:
#line 212 "CallGraphDFProblem.y"
{
            yyval.exprptr = new OA::ExprHandle(yyvsp[-3].ival);
            gCallGraphDFProblemIR->addExprString(*yyval.exprptr, yyvsp[-1].sval);
        ;
    break;}
case 23:
#line 220 "CallGraphDFProblem.y"
{
            yyvsp[-1].exprlistptr->push_back(*yyvsp[0].exprptr);
            yyval.exprlistptr = yyvsp[-1].exprlistptr;
            delete yyvsp[0].exprptr;
        ;
    break;}
case 24:
#line 226 "CallGraphDFProblem.y"
{
            yyval.exprlistptr = new std::list<OA::ExprHandle>;
        ;
    break;}
case 25:
#line 234 "CallGraphDFProblem.y"
{
            yyval.memrefptr = new OA::MemRefHandle(yyvsp[-3].ival);
            gCallGraphDFProblemIR->addMemRefString(*yyval.memrefptr, yyvsp[-1].sval);
        ;
    break;}
case 26:
#line 278 "CallGraphDFProblem.y"
{
            // make an expression tree if we are a leaf node
            //if ($9->empty()) {
                yyval.etreeptr = new OA::ExprTree::ExprTree;
            //}

            // for now we will just be the root node
            OA::OA_ptr<OA::ExprTree::ExprTree::Node> node;
            node = new OA::ExprTree::ExprTree::MemRefNode(*yyvsp[-6].memrefptr);
            yyval.etreeptr->addNode( node );
            yyval.etreeptr->dump(std::cout,gCallGraphDFProblemIR);
        ;
    break;}
case 27:
#line 298 "CallGraphDFProblem.y"
{
            OA::Location::NamedLoc* loc = new OA::Location::NamedLoc(*yyvsp[-17].symptr,yyvsp[-13].bval);
            delete yyvsp[-17].symptr;
            
            // get all fully overlapping locations from first SymList
            std::list<OA::SymHandle>::iterator listIter;
            for (listIter=yyvsp[-8].symlistptr->begin(); listIter!=yyvsp[-8].symlistptr->end(); listIter++) {
                loc->addFullOverlap(*listIter);
            }
            delete yyvsp[-8].symlistptr;

            // get all fully overlapping locations from first SymList
            for (listIter=yyvsp[-2].symlistptr->begin(); listIter!=yyvsp[-2].symlistptr->end(); listIter++) {
                loc->addPartOverlap(*listIter);
            }
            delete yyvsp[-2].symlistptr;

            yyval.locptr = loc;

         ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 349 "CallGraphDFProblem.y"

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


