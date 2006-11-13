
/*  A Bison parser, made from Alias.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse aliasparse
#define yylex aliaslex
#define yyerror aliaserror
#define yylval aliaslval
#define yychar aliaschar
#define yydebug aliasdebug
#define yynerrs aliasnerrs
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
#define	T_SYMS	292
#define	T_STMTS	293
#define	T_MEMREFS	294
#define	T_ANY	295
#define	T_PTRASSIGN	296

#line 1 "Alias.y"

extern int yylex ();
#define YYERROR_VERBOSE
#define YYDEBUG 1
void yyerror(char*);

// specific to this parser
#include "AliasIRInterfaceTest.hpp"
extern OA::OA_ptr<AliasIRInterfaceTest> gAliasIR;

static OA::ProcHandle gCurrProc;
static OA::StmtHandle gCurrStmt;

#line 15 "Alias.y"
typedef union {
    OA::Alias::IRStmtType astval;

    char*   sval;
    int     ival;
    bool    bval;
    OA::ProcHandle* procptr;
    OA::StmtHandle* stmtptr;
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



#define	YYFINAL		180
#define	YYFLAG		-32768
#define	YYNTBASE	51

#define YYTRANSLATE(x) ((unsigned)(x) <= 296 ? yytranslate[x] : 73)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    49,
    50,     2,     2,    47,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    43,
    44,    48,     2,     2,     2,     2,     2,     2,     2,     2,
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
    37,    38,    39,    40,    41,    42
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,    24,    27,    28,    32,    35,    36,    37,    48,
    50,    52,    55,    56,    59,    60,    66,    73,    80,    87,
    90,    91,    98,   110,   112,   114,   117,   118,   126,   134,
   139,   149,   159,   161,   163,   185,   195,   201,   203,   213
};

static const short yyrhs[] = {    -1,
    62,    52,    37,    43,    38,    44,    45,    53,    46,    47,
    39,    44,    45,    55,    46,    47,    40,    44,    45,    60,
    46,    48,     0,    53,    54,     0,     0,    64,    37,    72,
     0,    55,    56,     0,     0,     0,    63,    57,    37,    43,
    58,    47,    45,    59,    46,    48,     0,    41,     0,    42,
     0,    59,    66,     0,     0,    60,    61,     0,     0,    66,
    37,    45,    69,    46,     0,     7,    49,     4,    47,     3,
    50,     0,     8,    49,     4,    47,     3,    50,     0,     9,
    49,     4,    47,     3,    50,     0,    65,    64,     0,     0,
    10,    49,     4,    47,     3,    50,     0,    22,    44,    71,
    47,    23,    44,    71,    47,    24,    44,    68,     0,    15,
     0,    16,     0,    69,    70,     0,     0,    17,    47,    43,
    64,    47,    67,    48,     0,    18,    47,    43,    63,    47,
    67,    48,     0,    19,    43,    68,    48,     0,    20,    47,
    43,    70,    47,    25,    44,     4,    48,     0,    21,    47,
    43,    70,    47,    26,    44,     4,    48,     0,     5,     0,
     6,     0,    31,    47,    43,    64,    47,    29,    44,    71,
    47,    27,    44,    45,    65,    46,    47,    28,    44,    45,
    65,    46,    48,     0,    32,    47,    43,    63,    47,    29,
    44,    71,    48,     0,    34,    47,    43,    70,    48,     0,
    33,     0,    35,    47,    43,    72,    47,    30,    44,    71,
    48,     0,    36,    47,    43,    72,    47,    26,    44,     4,
    48,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    87,    89,    98,   100,   103,   112,   114,   117,   119,   126,
   131,   137,   139,   142,   144,   147,   165,   173,   181,   189,
   196,   211,   246,   254,   259,   265,   272,   278,   286,   293,
   298,   304,   311,   316,   324,   349,   354,   360,   365,   371
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_STR_LITERAL",
"T_INT_LITERAL","T_TRUE","T_FALSE","T_PROCHANDLE","T_STMTHANDLE","T_SYMHANDLE",
"T_MEMREFHANDLE","T_EXPRHANDLE","T_OPHANDLE","T_CONSTSYMHANDLE","T_CONSTVALHANDLE",
"T_USE","T_DEF","T_NAMEDREF","T_UNNAMEDREF","T_UNKNOWNREF","T_DEREF","T_IDXACCESS",
"T_ADDRESSOF","T_FULLACCURACY","T_MEMREFTYPE","T_NUMDEREF","T_IDX","T_FULLOVERLAP",
"T_PARTOVERLAP","T_LOCAL","T_FULL","T_NAMEDLOC","T_UNNAMEDLOC","T_UNKNOWNLOC",
"T_INVISIBLELOC","T_LOCBASICSUBSET","T_LOCIDXSUBSET","T_MAPSTO","T_SYMS","T_STMTS",
"T_MEMREFS","T_ANY","T_PTRASSIGN","'<'","'='","'['","']'","','","'>'","'('",
"')'","ProcDef","@1","SymDefList","SymDef","StmtDefList","StmtDef","@2","StmtType",
"MemRefList","MemRefDefList","MemRefDef","Proc","Stmt","Sym","SymList","MemRef",
"MemRefExprBaseFields","MemRefType","MemRefExprList","MemRefExpr","BoolVal",
"Location", NULL
};
#endif

static const short yyr1[] = {     0,
    52,    51,    53,    53,    54,    55,    55,    57,    56,    58,
    58,    59,    59,    60,    60,    61,    62,    63,    64,    65,
    65,    66,    67,    68,    68,    69,    69,    70,    70,    70,
    70,    70,    71,    71,    72,    72,    72,    72,    72,    72
};

static const short yyr2[] = {     0,
     0,    22,     2,     0,     3,     2,     0,     0,    10,     1,
     1,     2,     0,     2,     0,     5,     6,     6,     6,     2,
     0,     6,    11,     1,     1,     2,     0,     7,     7,     4,
     9,     9,     1,     1,    21,     9,     5,     1,     9,     9
};

static const short yydefact[] = {     0,
     0,     1,     0,     0,     0,     0,     0,     0,     0,     0,
    17,     0,     4,     0,     0,     0,     3,     0,     0,     0,
     0,     0,     0,     0,     0,    38,     0,     0,     0,     5,
     0,     0,     0,     0,     0,     0,     0,     0,     7,     0,
     0,     0,     0,     0,    19,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     6,     8,
     0,     0,     0,     0,     0,     0,     0,     0,    37,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    24,    25,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    30,     0,     0,     0,     0,     0,     0,    33,
    34,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    15,    10,    11,     0,     0,    18,    36,     0,     0,     0,
     0,     0,    39,    40,     0,     0,     0,     0,    28,    29,
     0,     0,     0,     0,    14,     0,    13,     0,     0,     0,
     0,     0,     2,     0,     0,    21,     0,    31,    32,     0,
    27,     0,    12,     0,     0,     0,     0,     9,     0,    20,
     0,     0,    16,    26,     0,     0,    22,     0,     0,     0,
     0,    21,     0,     0,    23,     0,    35,     0,     0,     0
};

static const short yydefgoto[] = {   178,
     4,    14,    17,    46,    59,    73,   114,   145,   125,   135,
     2,    49,   160,   154,   136,   119,    81,   157,    55,   102,
    30
};

static const short yypact[] = {     3,
   -37,-32768,    14,    15,    -9,     8,    36,    16,     5,    12,
-32768,    13,-32768,    -3,     4,    10,-32768,    22,    56,    23,
    -1,    17,    19,    18,    20,-32768,    24,    25,    27,-32768,
    58,    31,    26,    34,    35,    37,    38,    29,-32768,    59,
    62,     7,    -1,    -1,-32768,    -4,    39,    33,    40,    41,
    42,    47,    44,    45,    46,    48,    49,    50,-32768,-32768,
    54,    80,    64,    55,    57,    21,    60,    61,-32768,    69,
    75,    65,    70,    66,    67,    68,    59,    62,-32768,-32768,
    63,     7,     7,    71,    72,    73,    76,     9,    82,     9,
    74,    77,-32768,    78,    79,     9,    98,    83,   -25,-32768,
-32768,    84,    85,    81,    86,    86,    88,    92,    89,    90,
-32768,-32768,-32768,    87,    93,-32768,-32768,    95,    94,    96,
    97,    99,-32768,-32768,    -5,    91,   101,     9,-32768,-32768,
   102,   105,   100,   103,-32768,   109,-32768,   107,   106,   108,
   110,   118,-32768,   112,     1,-32768,   104,-32768,-32768,   113,
-32768,   111,-32768,    -2,   117,   120,     2,-32768,   115,-32768,
     9,   114,-32768,-32768,   119,   116,-32768,   121,   124,   122,
   125,-32768,    21,     0,-32768,   123,-32768,   130,   132,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   -33,   -11,   -39,    28,   126,   -23,-32768,   -82,   -88,
     6
};


#define	YYLAST		232


static const short yytable[] = {    94,
    95,   104,    18,    48,   133,    15,    15,   109,    15,     1,
   133,     3,    60,   100,   101,   112,   113,     5,    50,    51,
    52,    53,    54,    50,    51,    52,    53,    54,    47,    24,
    25,    26,    27,    28,    29,    79,    80,     7,     9,   139,
   134,    58,    16,   159,    92,   176,   152,   163,    56,    57,
     8,     6,    19,    10,    11,    12,    20,    13,    21,    22,
    38,    23,    32,    31,    33,    91,    34,    15,    40,    48,
    35,    36,   166,    37,   164,    39,    41,    42,    45,    43,
    44,    62,    74,    75,   103,    61,    63,    64,    65,    66,
    67,    68,    76,    69,    70,    71,    72,    77,    84,    78,
    85,   110,    82,    83,    86,   140,    87,   118,   141,    88,
    93,    90,   121,    89,    96,    97,    98,   122,    99,   127,
   105,   150,   162,   106,   107,   108,   155,   111,   117,   179,
   115,   180,   174,   126,   116,   137,   123,   124,   128,     0,
   131,   129,   132,   130,   138,   144,   168,   171,   142,   175,
   143,   146,   147,     0,     0,   148,   151,   149,   158,   156,
   161,   165,   169,   167,   170,     0,   172,     0,   173,     0,
   177,     0,   153,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   120
};

static const short yycheck[] = {    82,
    83,    90,    14,     8,    10,     9,     9,    96,     9,     7,
    10,    49,    46,     5,     6,    41,    42,     4,    17,    18,
    19,    20,    21,    17,    18,    19,    20,    21,    40,    31,
    32,    33,    34,    35,    36,    15,    16,    47,     3,   128,
    46,    46,    46,    46,    78,    46,    46,    46,    43,    44,
    43,    37,    49,    38,    50,    44,    47,    45,    37,     4,
     3,    39,    44,    47,    47,    77,    47,     9,    43,     8,
    47,    47,   161,    47,   157,    45,    43,    43,    50,    43,
    43,    49,    29,     4,     3,    47,    47,    47,    47,    43,
    47,    47,    29,    48,    47,    47,    47,    43,    30,    43,
    26,     4,    43,    43,    40,     4,    37,    22,     4,    44,
    48,    44,    25,    47,    44,    44,    44,    26,    43,    27,
    47,     4,     3,    47,    47,    47,    23,    45,    48,     0,
    47,     0,   172,    47,    50,    45,    48,    48,    44,    -1,
    44,    48,    44,    48,    44,    37,    28,    24,    49,   173,
    48,    45,    47,    -1,    -1,    48,    45,    48,    48,    47,
    44,    47,    47,    50,    44,    -1,    45,    -1,    44,    -1,
    48,    -1,   145,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   106
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
#line 88 "Alias.y"
{ gCurrProc = *yyvsp[0].procptr; ;
    break;}
case 2:
#line 93 "Alias.y"
{
          return 0;
        ;
    break;}
case 5:
#line 105 "Alias.y"
{
            OA::OA_ptr<OA::Location::Location> loc; loc = yyvsp[0].locptr;
            gAliasIR->addSym(gCurrProc,*yyvsp[-2].symptr,loc);
            delete yyvsp[-2].symptr;
        ;
    break;}
case 8:
#line 118 "Alias.y"
{ gCurrStmt = *yyvsp[0].stmtptr; ;
    break;}
case 9:
#line 120 "Alias.y"
{
            gAliasIR->addStmt(gCurrProc,*yyvsp[-9].stmtptr,yyvsp[-5].astval);
            delete yyvsp[-9].stmtptr;
        ;
    break;}
case 10:
#line 128 "Alias.y"
{
            yyval.astval = OA::Alias::ANY_STMT;
        ;
    break;}
case 11:
#line 132 "Alias.y"
{
            yyval.astval = OA::Alias::PTR_ASSIGN_STMT;
        ;
    break;}
case 16:
#line 149 "Alias.y"
{
            gAliasIR->addMemRef(gCurrStmt, *yyvsp[-4].memrefptr);
            // map memref to each MemRefExpr 
            std::list<OA::MemRefExpr::MemRefExpr*>::iterator listIter;
            for (listIter=yyvsp[-1].mrelistptr->begin(); listIter!=yyvsp[-1].mrelistptr->end(); listIter++) {
                OA::OA_ptr<OA::MemRefExpr::MemRefExpr> memrefexpr;
                memrefexpr = *listIter;
                gAliasIR->addMemRefExpr(*yyvsp[-4].memrefptr,memrefexpr);
            }
            delete yyvsp[-1].mrelistptr;
            delete yyvsp[-4].memrefptr;
        ;
    break;}
case 17:
#line 167 "Alias.y"
{
            yyval.procptr = new OA::ProcHandle(yyvsp[-3].ival);
            gAliasIR->addProcString(*yyval.procptr,yyvsp[-1].sval);
        ;
    break;}
case 18:
#line 175 "Alias.y"
{
            yyval.stmtptr = new OA::StmtHandle(yyvsp[-3].ival);
            gAliasIR->addStmtString(*yyval.stmtptr,yyvsp[-1].sval);
        ;
    break;}
case 19:
#line 183 "Alias.y"
{
            yyval.symptr = new OA::SymHandle(yyvsp[-3].ival);
            gAliasIR->addSymString(*yyval.symptr,yyvsp[-1].sval);
        ;
    break;}
case 20:
#line 191 "Alias.y"
{
            yyvsp[-1].symlistptr->push_back(*yyvsp[0].symptr);
            yyval.symlistptr = yyvsp[-1].symlistptr;
            delete yyvsp[0].symptr;
        ;
    break;}
case 21:
#line 197 "Alias.y"
{
            yyval.symlistptr = new std::list<OA::SymHandle>;
        ;
    break;}
case 22:
#line 213 "Alias.y"
{
            yyval.memrefptr = new OA::MemRefHandle(yyvsp[-3].ival);
            gAliasIR->addMemRefString(*yyval.memrefptr, yyvsp[-1].sval);
        ;
    break;}
case 23:
#line 249 "Alias.y"
{
            yyval.mreptr = new OA::MemRefExpr::MemRefExpr(yyvsp[-8].bval,yyvsp[-4].bval,yyvsp[0].mrtval);
        ;
    break;}
case 24:
#line 256 "Alias.y"
{
            yyval.mrtval = OA::MemRefExpr::USE;
        ;
    break;}
case 25:
#line 260 "Alias.y"
{
            yyval.mrtval = OA::MemRefExpr::DEF;
        ;
    break;}
case 26:
#line 267 "Alias.y"
{
            yyvsp[-1].mrelistptr->push_back(yyvsp[0].mreptr);
            yyval.mrelistptr = yyvsp[-1].mrelistptr;
        ;
    break;}
case 27:
#line 273 "Alias.y"
{
            yyval.mrelistptr = new std::list<OA::MemRefExpr::MemRefExpr*>;
        ;
    break;}
case 28:
#line 280 "Alias.y"
{
            yyval.mreptr = new OA::MemRefExpr::NamedRef(*yyvsp[-1].mreptr, *yyvsp[-3].symptr);
            delete yyvsp[-3].symptr;
            delete yyvsp[-1].mreptr;
        ;
    break;}
case 29:
#line 287 "Alias.y"
{
            yyval.mreptr = new OA::MemRefExpr::UnnamedRef(*yyvsp[-1].mreptr,*yyvsp[-3].stmtptr);
            delete yyvsp[-3].stmtptr;
            delete yyvsp[-1].mreptr;
        ;
    break;}
case 30:
#line 294 "Alias.y"
{
            yyval.mreptr = new OA::MemRefExpr::UnknownRef(yyvsp[-1].mrtval);
        ;
    break;}
case 31:
#line 299 "Alias.y"
{
            OA::OA_ptr<OA::MemRefExpr::MemRefExpr> ref; ref = yyvsp[-5].mreptr;
            yyval.mreptr = new OA::MemRefExpr::Deref(ref,yyvsp[-1].ival);
        ;
    break;}
case 32:
#line 305 "Alias.y"
{
            OA::OA_ptr<OA::MemRefExpr::MemRefExpr> ref; ref = yyvsp[-5].mreptr;
            yyval.mreptr = new OA::MemRefExpr::IdxAccess(ref,yyvsp[-1].ival);
        ;
    break;}
case 33:
#line 313 "Alias.y"
{
            yyval.bval = true;
        ;
    break;}
case 34:
#line 317 "Alias.y"
{
            yyval.bval = false;
        ;
    break;}
case 35:
#line 328 "Alias.y"
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
case 36:
#line 350 "Alias.y"
{
            yyval.locptr = new OA::Location::UnnamedLoc(*yyvsp[-5].stmtptr,yyvsp[-1].bval);
        ;
    break;}
case 37:
#line 355 "Alias.y"
{
            OA::OA_ptr<OA::MemRefExpr::MemRefExpr> ref; ref = yyvsp[-1].mreptr;
            yyval.locptr = new OA::Location::InvisibleLoc(ref);
        ;
    break;}
case 38:
#line 361 "Alias.y"
{
            yyval.locptr = new OA::Location::UnknownLoc();
        ;
    break;}
case 39:
#line 366 "Alias.y"
{
            OA::OA_ptr<OA::Location::Location> loc; loc = yyvsp[-5].locptr;
            yyval.locptr = new OA::Location::LocBasicSubSet(loc,yyvsp[-1].bval);
        ;
    break;}
case 40:
#line 372 "Alias.y"
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
#line 378 "Alias.y"

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


