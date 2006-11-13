
/*  A Bison parser, made from AccessLoc.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse accesslocparse
#define yylex accessloclex
#define yyerror accesslocerror
#define yylval accessloclval
#define yychar accesslocchar
#define yydebug accesslocdebug
#define yynerrs accesslocnerrs
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
#define	T_USE	269
#define	T_DEF	270
#define	T_NAMEDREF	271
#define	T_UNNAMEDREF	272
#define	T_UNKNOWNREF	273
#define	T_DEREF	274
#define	T_IDXACCESS	275
#define	T_ADDRESSOF	276
#define	T_FULLACCURACY	277
#define	T_MEMREFTYPE	278
#define	T_NUMDEREF	279
#define	T_IDX	280
#define	T_FULLOVERLAP	281
#define	T_PARTOVERLAP	282
#define	T_LOCAL	283
#define	T_FULL	284
#define	T_NAMEDLOC	285
#define	T_UNNAMEDLOC	286
#define	T_UNKNOWNLOC	287
#define	T_INVISIBLELOC	288
#define	T_LOCBASICSUBSET	289
#define	T_LOCIDXSUBSET	290
#define	T_MAPSTO	291
#define	T_FORMALPARAMS	292
#define	T_ACCESSIBLESYMS	293
#define	T_UNNAMEDDYNALLOCSTMTS	294
#define	T_POSSMEMREFEXPRS	295

#line 1 "AccessLoc.y"

extern int yylex ();
#define YYERROR_VERBOSE
#define YYDEBUG 1
void yyerror(char*);

// specific to this parser
#include "AccessLocIRInterfaceTest.hpp"
extern OA::OA_ptr<AccessLocIRInterfaceTest> gAccessLocIR;

static OA::ProcHandle gCurrProc;

#line 14 "AccessLoc.y"
typedef union {
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
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		169
#define	YYFLAG		-32768
#define	YYNTBASE	50

#define YYTRANSLATE(x) ((unsigned)(x) <= 295 ? yytranslate[x] : 67)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    48,
    49,     2,     2,    43,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    42,
    44,    47,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    45,     2,    46,     2,     2,     2,     2,     2,     2,     2,
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
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,    32,    35,    36,    40,    43,    44,    50,    57,
    64,    67,    68,    75,    78,    79,    86,    98,   100,   102,
   105,   106,   114,   122,   127,   137,   147,   149,   151,   173,
   183,   189,   191,   201
};

static const short yyrhs[] = {    -1,
    56,    51,    37,    42,    59,    43,    38,    44,    45,    60,
    46,    43,    39,    44,    45,    52,    46,    43,    40,    44,
    45,    58,    46,    43,    41,    44,    45,    54,    46,    47,
     0,    52,    53,     0,     0,    59,    37,    66,     0,    54,
    55,     0,     0,    59,    37,    45,    63,    46,     0,     7,
    48,     4,    43,     3,    49,     0,     8,    48,     4,    43,
     3,    49,     0,    58,    57,     0,     0,     9,    48,     4,
    43,     3,    49,     0,    60,    59,     0,     0,    10,    48,
     4,    43,     3,    49,     0,    22,    44,    65,    43,    23,
    44,    65,    43,    24,    44,    62,     0,    15,     0,    16,
     0,    63,    64,     0,     0,    17,    43,    42,    59,    43,
    61,    47,     0,    18,    43,    42,    57,    43,    61,    47,
     0,    19,    42,    62,    47,     0,    20,    43,    42,    64,
    43,    25,    44,     4,    47,     0,    21,    43,    42,    64,
    43,    26,    44,     4,    47,     0,     5,     0,     6,     0,
    31,    43,    42,    59,    43,    29,    44,    65,    43,    27,
    44,    45,    60,    46,    43,    28,    44,    45,    60,    46,
    47,     0,    32,    43,    42,    57,    43,    29,    44,    65,
    47,     0,    34,    43,    42,    64,    47,     0,    33,     0,
    35,    43,    42,    66,    43,    30,    44,    65,    47,     0,
    36,    43,    42,    66,    43,    26,    44,     4,    47,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    87,    89,   120,   122,   125,   134,   136,   139,   157,   165,
   173,   180,   187,   195,   202,   217,   252,   260,   265,   271,
   278,   284,   292,   299,   304,   310,   317,   322,   330,   355,
   360,   366,   371,   377
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_STR_LITERAL",
"T_INT_LITERAL","T_TRUE","T_FALSE","T_PROCHANDLE","T_STMTHANDLE","T_SYMHANDLE",
"T_MEMREFHANDLE","T_EXPRHANDLE","T_OPHANDLE","T_CONSTSYMHANDLE","T_CONSTVALHANDLE",
"T_USE","T_DEF","T_NAMEDREF","T_UNNAMEDREF","T_UNKNOWNREF","T_DEREF","T_IDXACCESS",
"T_ADDRESSOF","T_FULLACCURACY","T_MEMREFTYPE","T_NUMDEREF","T_IDX","T_FULLOVERLAP",
"T_PARTOVERLAP","T_LOCAL","T_FULL","T_NAMEDLOC","T_UNNAMEDLOC","T_UNKNOWNLOC",
"T_INVISIBLELOC","T_LOCBASICSUBSET","T_LOCIDXSUBSET","T_MAPSTO","T_FORMALPARAMS",
"T_ACCESSIBLESYMS","T_UNNAMEDDYNALLOCSTMTS","T_POSSMEMREFEXPRS","'<'","','",
"'='","'['","']'","'>'","'('","')'","ProcDef","@1","SymDefList","SymDef","SymToMemRefExprList",
"SymToMemRefExpr","Proc","Stmt","StmtList","Sym","SymList","MemRefExprBaseFields",
"MemRefType","MemRefExprList","MemRefExpr","BoolVal","Location", NULL
};
#endif

static const short yyr1[] = {     0,
    51,    50,    52,    52,    53,    54,    54,    55,    56,    57,
    58,    58,    59,    60,    60,    -1,    61,    62,    62,    63,
    63,    64,    64,    64,    64,    64,    65,    65,    66,    66,
    66,    66,    66,    66
};

static const short yyr2[] = {     0,
     0,    30,     2,     0,     3,     2,     0,     5,     6,     6,
     2,     0,     6,     2,     0,     6,    11,     1,     1,     2,
     0,     7,     7,     4,     9,     9,     1,     1,    21,     9,
     5,     1,     9,     9
};

static const short yydefact[] = {     0,
     0,     1,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     9,     0,     0,     0,     0,     0,     0,     0,    15,
    13,     0,     0,    14,     0,     0,     0,     4,     0,     0,
     3,     0,     0,     0,     0,     0,     0,    32,     0,     0,
     0,     5,     0,     0,     0,     0,     0,     0,    12,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    11,     0,     0,
     0,     0,     0,     0,     0,     0,    31,     0,     0,     0,
     0,     0,     0,     0,     0,    18,    19,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    24,     0,
     0,     0,     0,     0,    27,    28,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     7,     0,    10,    30,     0,
     0,     0,     0,     0,    33,    34,     0,     0,     0,    22,
    23,     0,     0,     0,     6,     0,     0,     0,     0,     0,
     2,     0,    15,     0,    25,    26,    21,     0,     0,     0,
     0,     0,     8,    20,     0,     0,     0,     0,     0,     0,
    15,     0,     0,    17,     0,    29,     0,     0,     0
};

static const short yydefgoto[] = {   167,
     4,    29,    31,   127,   135,     2,    58,    55,    24,    22,
   121,    88,   150,    64,   107,    42
};

static const short yypact[] = {    10,
   -42,-32768,    11,   -17,   -14,    -4,    34,    30,    -9,    12,
     6,-32768,    46,    20,    18,    19,    61,    22,    16,-32768,
-32768,    -5,    23,-32768,    29,    25,    26,-32768,    -2,    27,
-32768,    35,    33,    21,    31,    36,    37,-32768,    38,    39,
    40,-32768,    32,    42,    43,    44,    45,    47,-32768,    30,
    66,    14,    21,    21,    -3,    48,    49,    50,    51,    52,
    54,    55,    56,    41,    57,    58,    59,-32768,    63,    74,
    75,    64,    65,     3,    67,    68,-32768,    60,    77,    70,
    69,    62,    71,    30,    66,-32768,-32768,    73,    14,    14,
    72,    78,    79,     8,   105,     8,    81,    82,-32768,    83,
    84,     8,   108,    76,-32768,-32768,    85,    80,    86,    92,
    92,    93,    91,    87,    88,-32768,   103,-32768,-32768,    94,
    89,    90,    95,    96,-32768,-32768,    -1,    97,     8,-32768,
-32768,   127,   128,    98,-32768,   106,    99,   104,   101,   102,
-32768,   107,-32768,   119,-32768,-32768,-32768,     1,   109,     5,
   111,     8,-32768,-32768,   118,   112,   113,   126,   114,   116,
-32768,     3,     2,-32768,   115,-32768,   151,   156,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   -39,-32768,    -8,  -131,
    53,     4,-32768,   -88,   -93,   -26
};


#define	YYLAST		166


static const short yytable[] = {    11,
   100,   101,   109,    10,    57,     3,    10,    10,   114,    10,
    10,   148,   105,   106,     5,    68,     1,    86,    87,     6,
    32,    59,    60,    61,    62,    63,    65,    66,     7,   163,
    59,    60,    61,    62,    63,   138,     9,     8,    10,    12,
    23,    56,    67,    30,   134,    98,   151,   165,    14,    15,
   153,    36,    37,    38,    39,    40,    41,    16,   156,    13,
    17,   154,    18,    19,    21,    25,    20,    26,    27,    33,
    28,    34,    35,    57,    43,    97,    49,    82,    44,    45,
    46,    47,    48,    50,    51,    52,    53,    77,    54,    91,
    69,    81,    71,    72,    73,    74,    70,    75,    76,    78,
    79,    80,    92,    83,    95,    84,    85,   108,    89,    90,
    93,   115,    94,   120,    96,   102,   124,   123,   136,    99,
   116,   103,   104,   110,   111,   112,   113,   117,   118,   128,
   139,   140,   119,   125,   126,   130,   131,   129,   132,   133,
   137,   149,   142,   143,   141,   157,   144,   145,   146,   160,
   168,   147,   152,   155,   158,   169,   159,     0,   161,   162,
     0,   166,     0,   122,     0,   164
};

static const short yycheck[] = {     8,
    89,    90,    96,     9,     8,    48,     9,     9,   102,     9,
     9,   143,     5,     6,     4,    55,     7,    15,    16,    37,
    29,    17,    18,    19,    20,    21,    53,    54,    43,   161,
    17,    18,    19,    20,    21,   129,     3,    42,     9,    49,
    46,    50,    46,    46,    46,    85,    46,    46,    43,     4,
    46,    31,    32,    33,    34,    35,    36,    38,   152,    48,
    43,   150,    44,     3,    49,    43,    45,    39,    44,    43,
    45,    37,    40,     8,    44,    84,    45,     4,    43,    43,
    43,    43,    43,    42,    42,    42,    42,    47,    42,    30,
    43,    29,    43,    43,    43,    42,    48,    43,    43,    43,
    43,    43,    26,    29,    43,    42,    42,     3,    42,    42,
    41,     4,    44,    22,    44,    44,    26,    25,   127,    47,
    45,    44,    44,    43,    43,    43,    43,    43,    49,    27,
     4,     4,    47,    47,    47,    47,    47,    44,    44,    44,
    44,    23,    37,    45,    47,    28,    43,    47,    47,    24,
     0,    45,    44,    43,    43,     0,    44,    -1,    45,    44,
    -1,    47,    -1,   111,    -1,   162
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
#line 88 "AccessLoc.y"
{ gCurrProc = *yyvsp[0].procptr; ;
    break;}
case 2:
#line 95 "AccessLoc.y"
{
          // for getSymHandle(ProcHandle)
          gAccessLocIR->mapProcToSym(*yyvsp[-29].procptr,*yyvsp[-25].symptr);

          // for getFormalParamIterator(SymHandle)
          std::list<OA::SymHandle>::iterator listIter;
          for (listIter=yyvsp[-20].symlistptr->begin(); listIter!=yyvsp[-20].symlistptr->end(); listIter++) {
              gAccessLocIR->addFormalParam(*yyvsp[-25].symptr,*listIter);
          }
          delete yyvsp[-20].symlistptr;

          // for getUnnamedDynAllocStmtIterator(ProcHandle)
          std::list<OA::StmtHandle>::iterator stmtIter;
          for (stmtIter=yyvsp[-8].stmtlistptr->begin(); stmtIter!=yyvsp[-8].stmtlistptr->end(); stmtIter++) {
              gAccessLocIR->addUnnamedDynAllocStmt(*yyvsp[-29].procptr,*stmtIter);
          }
          delete yyvsp[-8].stmtlistptr;

          delete yyvsp[-29].procptr;
          delete yyvsp[-25].symptr;

          return 0;
        ;
    break;}
case 5:
#line 127 "AccessLoc.y"
{
            OA::OA_ptr<OA::Location::Location> loc; loc = yyvsp[0].locptr;
            gAccessLocIR->addSym(gCurrProc,*yyvsp[-2].symptr,loc);
            delete yyvsp[-2].symptr;
        ;
    break;}
case 8:
#line 141 "AccessLoc.y"
{
            // map symbol to each MemRefExpr 
            std::list<OA::MemRefExpr::MemRefExpr*>::iterator listIter;
            for (listIter=yyvsp[-1].mrelistptr->begin(); listIter!=yyvsp[-1].mrelistptr->end(); listIter++) {
                OA::OA_ptr<OA::MemRefExpr::MemRefExpr> memrefexpr;
                memrefexpr = *listIter;
                gAccessLocIR->addMemRefExpr(*yyvsp[-4].symptr,memrefexpr);
            }
            delete yyvsp[-4].symptr;
            delete yyvsp[-1].mrelistptr;
        ;
    break;}
case 9:
#line 159 "AccessLoc.y"
{
            yyval.procptr = new OA::ProcHandle(yyvsp[-3].ival);
            gAccessLocIR->addProcString(*yyval.procptr,yyvsp[-1].sval);
        ;
    break;}
case 10:
#line 167 "AccessLoc.y"
{
            yyval.stmtptr = new OA::StmtHandle(yyvsp[-3].ival);
            gAccessLocIR->addStmtString(*yyval.stmtptr,yyvsp[-1].sval);
        ;
    break;}
case 11:
#line 175 "AccessLoc.y"
{
            yyvsp[-1].stmtlistptr->push_back(*yyvsp[0].stmtptr);
            yyval.stmtlistptr = yyvsp[-1].stmtlistptr;
            delete yyvsp[0].stmtptr;
        ;
    break;}
case 12:
#line 181 "AccessLoc.y"
{
            yyval.stmtlistptr = new std::list<OA::StmtHandle>;
        ;
    break;}
case 13:
#line 189 "AccessLoc.y"
{
            yyval.symptr = new OA::SymHandle(yyvsp[-3].ival);
            gAccessLocIR->addSymString(*yyval.symptr,yyvsp[-1].sval);
        ;
    break;}
case 14:
#line 197 "AccessLoc.y"
{
            yyvsp[-1].symlistptr->push_back(*yyvsp[0].symptr);
            yyval.symlistptr = yyvsp[-1].symlistptr;
            delete yyvsp[0].symptr;
        ;
    break;}
case 15:
#line 203 "AccessLoc.y"
{
            yyval.symlistptr = new std::list<OA::SymHandle>;
        ;
    break;}
case 16:
#line 219 "AccessLoc.y"
{
            yyval.memrefptr = new OA::MemRefHandle(yyvsp[-3].ival);
            gAccessLocIR->addMemRefString(*yyval.memrefptr, yyvsp[-1].sval);
        ;
    break;}
case 17:
#line 255 "AccessLoc.y"
{
            yyval.mreptr = new OA::MemRefExpr::MemRefExpr(yyvsp[-8].bval,yyvsp[-4].bval,yyvsp[0].mrtval);
        ;
    break;}
case 18:
#line 262 "AccessLoc.y"
{
            yyval.mrtval = OA::MemRefExpr::USE;
        ;
    break;}
case 19:
#line 266 "AccessLoc.y"
{
            yyval.mrtval = OA::MemRefExpr::DEF;
        ;
    break;}
case 20:
#line 273 "AccessLoc.y"
{
            yyvsp[-1].mrelistptr->push_back(yyvsp[0].mreptr);
            yyval.mrelistptr = yyvsp[-1].mrelistptr;
        ;
    break;}
case 21:
#line 279 "AccessLoc.y"
{
            yyval.mrelistptr = new std::list<OA::MemRefExpr::MemRefExpr*>;
        ;
    break;}
case 22:
#line 286 "AccessLoc.y"
{
            yyval.mreptr = new OA::MemRefExpr::NamedRef(*yyvsp[-1].mreptr, *yyvsp[-3].symptr);
            delete yyvsp[-3].symptr;
            delete yyvsp[-1].mreptr;
        ;
    break;}
case 23:
#line 293 "AccessLoc.y"
{
            yyval.mreptr = new OA::MemRefExpr::UnnamedRef(*yyvsp[-1].mreptr,*yyvsp[-3].stmtptr);
            delete yyvsp[-3].stmtptr;
            delete yyvsp[-1].mreptr;
        ;
    break;}
case 24:
#line 300 "AccessLoc.y"
{
            yyval.mreptr = new OA::MemRefExpr::UnknownRef(yyvsp[-1].mrtval);
        ;
    break;}
case 25:
#line 305 "AccessLoc.y"
{
            OA::OA_ptr<OA::MemRefExpr::MemRefExpr> ref; ref = yyvsp[-5].mreptr;
            yyval.mreptr = new OA::MemRefExpr::Deref(ref,yyvsp[-1].ival);
        ;
    break;}
case 26:
#line 311 "AccessLoc.y"
{
            OA::OA_ptr<OA::MemRefExpr::MemRefExpr> ref; ref = yyvsp[-5].mreptr;
            yyval.mreptr = new OA::MemRefExpr::IdxAccess(ref,yyvsp[-1].ival);
        ;
    break;}
case 27:
#line 319 "AccessLoc.y"
{
            yyval.bval = true;
        ;
    break;}
case 28:
#line 323 "AccessLoc.y"
{
            yyval.bval = false;
        ;
    break;}
case 29:
#line 334 "AccessLoc.y"
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
case 30:
#line 356 "AccessLoc.y"
{
            yyval.locptr = new OA::Location::UnnamedLoc(*yyvsp[-5].stmtptr,yyvsp[-1].bval);
        ;
    break;}
case 31:
#line 361 "AccessLoc.y"
{
            OA::OA_ptr<OA::MemRefExpr::MemRefExpr> ref; ref = yyvsp[-1].mreptr;
            yyval.locptr = new OA::Location::InvisibleLoc(ref);
        ;
    break;}
case 32:
#line 367 "AccessLoc.y"
{
            yyval.locptr = new OA::Location::UnknownLoc();
        ;
    break;}
case 33:
#line 372 "AccessLoc.y"
{
            OA::OA_ptr<OA::Location::Location> loc; loc = yyvsp[-5].locptr;
            yyval.locptr = new OA::Location::LocBasicSubSet(loc,yyvsp[-1].bval);
        ;
    break;}
case 34:
#line 378 "AccessLoc.y"
{
            OA::OA_ptr<OA::Location::Location> loc; loc = yyvsp[-5].locptr;
            yyval.locptr = new OA::Location::LocIdxSubSet(loc,yyvsp[-1].ival);
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
#line 384 "AccessLoc.y"

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


