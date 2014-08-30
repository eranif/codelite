
/*  A Bison parser, made from php_expr.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse php_expr_parse
#define yylex php_expr_lex
#define yyerror php_expr_error
#define yylval php_expr_lval
#define yychar php_expr_char
#define yydebug php_expr_debug
#define yynerrs php_expr_nerrs
#define	END	257
#define	T_INCLUDE	258
#define	T_INCLUDE_ONCE	259
#define	T_EVAL	260
#define	T_REQUIRE	261
#define	T_REQUIRE_ONCE	262
#define	T_LOGICAL_OR	263
#define	T_LOGICAL_XOR	264
#define	T_LOGICAL_AND	265
#define	T_PRINT	266
#define	T_YIELD	267
#define	T_PLUS_EQUAL	268
#define	T_MINUS_EQUAL	269
#define	T_MUL_EQUAL	270
#define	T_DIV_EQUAL	271
#define	T_CONCAT_EQUAL	272
#define	T_MOD_EQUAL	273
#define	T_AND_EQUAL	274
#define	T_OR_EQUAL	275
#define	T_XOR_EQUAL	276
#define	T_SL_EQUAL	277
#define	T_SR_EQUAL	278
#define	T_BOOLEAN_OR	279
#define	T_BOOLEAN_AND	280
#define	T_IS_EQUAL	281
#define	T_IS_NOT_EQUAL	282
#define	T_IS_IDENTICAL	283
#define	T_IS_NOT_IDENTICAL	284
#define	T_IS_SMALLER_OR_EQUAL	285
#define	T_IS_GREATER_OR_EQUAL	286
#define	T_SL	287
#define	T_SR	288
#define	T_INSTANCEOF	289
#define	T_INC	290
#define	T_DEC	291
#define	T_INT_CAST	292
#define	T_DOUBLE_CAST	293
#define	T_STRING_CAST	294
#define	T_ARRAY_CAST	295
#define	T_OBJECT_CAST	296
#define	T_BOOL_CAST	297
#define	T_UNSET_CAST	298
#define	T_NEW	299
#define	T_CLONE	300
#define	T_EXIT	301
#define	T_IF	302
#define	T_ELSEIF	303
#define	T_ELSE	304
#define	T_ENDIF	305
#define	T_LNUMBER	306
#define	T_DNUMBER	307
#define	T_STRING	308
#define	T_STRING_VARNAME	309
#define	T_VARIABLE	310
#define	T_NUM_STRING	311
#define	T_INLINE_HTML	312
#define	T_CHARACTER	313
#define	T_BAD_CHARACTER	314
#define	T_ENCAPSED_AND_WHITESPACE	315
#define	T_CONSTANT_ENCAPSED_STRING	316
#define	T_ECHO	317
#define	T_DO	318
#define	T_WHILE	319
#define	T_ENDWHILE	320
#define	T_FOR	321
#define	T_ENDFOR	322
#define	T_FOREACH	323
#define	T_ENDFOREACH	324
#define	T_DECLARE	325
#define	T_ENDDECLARE	326
#define	T_AS	327
#define	T_SWITCH	328
#define	T_ENDSWITCH	329
#define	T_CASE	330
#define	T_DEFAULT	331
#define	T_BREAK	332
#define	T_CONTINUE	333
#define	T_GOTO	334
#define	T_FUNCTION	335
#define	T_CONST	336
#define	T_RETURN	337
#define	T_TRY	338
#define	T_CATCH	339
#define	T_THROW	340
#define	T_USE	341
#define	T_INSTEADOF	342
#define	T_GLOBAL	343
#define	T_STATIC	344
#define	T_ABSTRACT	345
#define	T_FINAL	346
#define	T_PRIVATE	347
#define	T_PROTECTED	348
#define	T_PUBLIC	349
#define	T_VAR	350
#define	T_UNSET	351
#define	T_ISSET	352
#define	T_EMPTY	353
#define	T_HALT_COMPILER	354
#define	T_CLASS	355
#define	T_TRAIT	356
#define	T_INTERFACE	357
#define	T_EXTENDS	358
#define	T_IMPLEMENTS	359
#define	T_OBJECT_OPERATOR	360
#define	T_DOUBLE_ARROW	361
#define	T_LIST	362
#define	T_ARRAY	363
#define	T_CALLABLE	364
#define	T_CLASS_C	365
#define	T_TRAIT_C	366
#define	T_METHOD_C	367
#define	T_FUNC_C	368
#define	T_LINE	369
#define	T_FILE	370
#define	T_COMMENT	371
#define	T_DOC_COMMENT	372
#define	T_OPEN_TAG	373
#define	T_OPEN_TAG_WITH_ECHO	374
#define	T_CLOSE_TAG	375
#define	T_WHITESPACE	376
#define	T_START_HEREDOC	377
#define	T_END_HEREDOC	378
#define	T_DOLLAR_OPEN_CURLY_BRACES	379
#define	T_CURLY_OPEN	380
#define	T_PAAMAYIM_NEKUDOTAYIM	381
#define	T_NAMESPACE	382
#define	T_NS_C	383
#define	T_DIR	384
#define	T_DEFINE	385
#define	T_NS_SEPARATOR	386

#line 1 "php_expr.y"

#define YYTOKENTYPE
#include "php_parser_api.h"
#define YYSIZE_T size_t

#ifdef YYSTYPE
#undef YYSTYPE
#endif

#define YYSTYPE wxString
extern void php_expr_error(char*);
extern int php_expr_lex();
extern void php_expr_less(int count);
extern int php_expr_lineno;
extern int php_expr_type;

void consume_method_call_args();

#define RESET_PARSER() PHPParser.resetChain();
#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		30
#define	YYFLAG		-32768
#define	YYNTBASE	155

#define YYTRANSLATE(x) ((unsigned)(x) <= 386 ? yytranslate[x] : 165)

static const short yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    50,     2,     2,     2,    49,    33,     2,   153,
   154,    47,    44,     9,    45,    46,    48,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    28,   152,    38,
    15,    40,    27,    62,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    63,     2,     2,    32,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    31,     2,    52,     2,     2,     2,     2,
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
     7,     8,    10,    11,    12,    13,    14,    16,    17,    18,
    19,    20,    21,    22,    23,    24,    25,    26,    29,    30,
    34,    35,    36,    37,    39,    41,    42,    43,    51,    53,
    54,    55,    56,    57,    58,    59,    60,    61,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
    86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
    96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
   116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
   126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     6,    12,    14,    16,    18,    20,    22,
    23,    25,    26,    28,    30,    33,    35,    39,    40,    42,
    44,    45,    47,    51
};

static const short yyrhs[] = {   156,
     0,   157,   156,     0,     0,   158,   159,   164,   160,   152,
     0,     1,     0,     7,     0,     8,     0,     4,     0,     5,
     0,     0,   153,     0,     0,   154,     0,    81,     0,    73,
   162,     0,   149,     0,   153,   163,   154,     0,     0,   135,
     0,   149,     0,     0,   161,     0,   164,    46,   161,     0,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   184,   187,   188,   191,   195,   207,   208,   209,   210,   213,
   214,   217,   218,   221,   231,   245,   251,   252,   255,   258,
   262,   265,   266,   267
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","\"end of file\"",
"\"include (T_INCLUDE)\"","\"include_once (T_INCLUDE_ONCE)\"","\"eval (T_EVAL)\"",
"\"require (T_REQUIRE)\"","\"require_once (T_REQUIRE_ONCE)\"","','","\"or (T_LOGICAL_OR)\"",
"\"xor (T_LOGICAL_XOR)\"","\"and (T_LOGICAL_AND)\"","\"print (T_PRINT)\"","\"yield (T_YIELD)\"",
"'='","\"+= (T_PLUS_EQUAL)\"","\"-= (T_MINUS_EQUAL)\"","\"*= (T_MUL_EQUAL)\"",
"\"/= (T_DIV_EQUAL)\"","\".= (T_CONCAT_EQUAL)\"","\"%= (T_MOD_EQUAL)\"","\"&= (T_AND_EQUAL)\"",
"\"|= (T_OR_EQUAL)\"","\"^= (T_XOR_EQUAL)\"","\"<<= (T_SL_EQUAL)\"","\">>= (T_SR_EQUAL)\"",
"'?'","':'","\"|| (T_BOOLEAN_OR)\"","\"&& (T_BOOLEAN_AND)\"","'|'","'^'","'&'",
"\"== (T_IS_EQUAL)\"","\"!= (T_IS_NOT_EQUAL)\"","\"=== (T_IS_IDENTICAL)\"","\"!== (T_IS_NOT_IDENTICAL)\"",
"'<'","\"<= (T_IS_SMALLER_OR_EQUAL)\"","'>'","\">= (T_IS_GREATER_OR_EQUAL)\"",
"\"<< (T_SL)\"","\">> (T_SR)\"","'+'","'-'","'.'","'*'","'/'","'%'","'!'","\"instanceof (T_INSTANCEOF)\"",
"'~'","\"++ (T_INC)\"","\"-- (T_DEC)\"","\"(int) (T_INT_CAST)\"","\"(double) (T_DOUBLE_CAST)\"",
"\"(string) (T_STRING_CAST)\"","\"(array) (T_ARRAY_CAST)\"","\"(object) (T_OBJECT_CAST)\"",
"\"(bool) (T_BOOL_CAST)\"","\"(unset) (T_UNSET_CAST)\"","'@'","'['","\"new (T_NEW)\"",
"\"clone (T_CLONE)\"","\"exit (T_EXIT)\"","\"if (T_IF)\"","\"elseif (T_ELSEIF)\"",
"\"else (T_ELSE)\"","\"endif (T_ENDIF)\"","\"integer number (T_LNUMBER)\"","\"floating-point number (T_DNUMBER)\"",
"\"identifier (T_STRING)\"","\"variable name (T_STRING_VARNAME)\"","\"variable (T_VARIABLE)\"",
"\"number (T_NUM_STRING)\"","T_INLINE_HTML","T_CHARACTER","T_BAD_CHARACTER",
"\"quoted-string and whitespace (T_ENCAPSED_AND_WHITESPACE)\"","\"quoted-string (T_CONSTANT_ENCAPSED_STRING)\"",
"\"echo (T_ECHO)\"","\"do (T_DO)\"","\"while (T_WHILE)\"","\"endwhile (T_ENDWHILE)\"",
"\"for (T_FOR)\"","\"endfor (T_ENDFOR)\"","\"foreach (T_FOREACH)\"","\"endforeach (T_ENDFOREACH)\"",
"\"declare (T_DECLARE)\"","\"enddeclare (T_ENDDECLARE)\"","\"as (T_AS)\"","\"switch (T_SWITCH)\"",
"\"endswitch (T_ENDSWITCH)\"","\"case (T_CASE)\"","\"default (T_DEFAULT)\"",
"\"break (T_BREAK)\"","\"continue (T_CONTINUE)\"","\"goto (T_GOTO)\"","\"function (T_FUNCTION)\"",
"\"const (T_CONST)\"","\"return (T_RETURN)\"","\"try (T_TRY)\"","\"catch (T_CATCH)\"",
"\"throw (T_THROW)\"","\"use (T_USE)\"","\"insteadof (T_INSTEADOF)\"","\"global (T_GLOBAL)\"",
"\"static (T_STATIC)\"","\"abstract (T_ABSTRACT)\"","\"final (T_FINAL)\"","\"private (T_PRIVATE)\"",
"\"protected (T_PROTECTED)\"","\"public (T_PUBLIC)\"","\"var (T_VAR)\"","\"unset (T_UNSET)\"",
"\"isset (T_ISSET)\"","\"empty (T_EMPTY)\"","\"__halt_compiler (T_HALT_COMPILER)\"",
"\"class (T_CLASS)\"","\"trait (T_TRAIT)\"","\"interface (T_INTERFACE)\"","\"extends (T_EXTENDS)\"",
"\"implements (T_IMPLEMENTS)\"","\"-> (T_OBJECT_OPERATOR)\"","\"=> (T_DOUBLE_ARROW)\"",
"\"list (T_LIST)\"","\"array (T_ARRAY)\"","\"callable (T_CALLABLE)\"","\"__CLASS__ (T_CLASS_C)\"",
"\"__TRAIT__ (T_TRAIT_C)\"","\"__METHOD__ (T_METHOD_C)\"","\"__FUNCTION__ (T_FUNC_C)\"",
"\"__LINE__ (T_LINE)\"","\"__FILE__ (T_FILE)\"","\"comment (T_COMMENT)\"","\"doc comment (T_DOC_COMMENT)\"",
"\"open tag (T_OPEN_TAG)\"","\"open tag with echo (T_OPEN_TAG_WITH_ECHO)\"",
"\"close tag (T_CLOSE_TAG)\"","\"whitespace (T_WHITESPACE)\"","\"heredoc start (T_START_HEREDOC)\"",
"\"heredoc end (T_END_HEREDOC)\"","\"${ (T_DOLLAR_OPEN_CURLY_BRACES)\"","\"{$ (T_CURLY_OPEN)\"",
"\":: (T_PAAMAYIM_NEKUDOTAYIM)\"","\"namespace (T_NAMESPACE)\"","\"__NAMESPACE__ (T_NS_C)\"",
"\"__DIR__ (T_DIR)\"","\"define (T_DEFINE)\"","\"\\\\ (T_NS_SEPARATOR)\"","';'",
"'('","')'","start","include_statement_list","include_statement","include_k",
"open_brace","close_brace","expr_basic","function_call_or_not","file_constants",
"expr", NULL
};
#endif

static const short yyr1[] = {     0,
   155,   156,   156,   157,   157,   158,   158,   158,   158,   159,
   159,   160,   160,   161,   161,   161,   162,   162,   163,   163,
   163,   164,   164,   164
};

static const short yyr2[] = {     0,
     1,     2,     0,     5,     1,     1,     1,     1,     1,     0,
     1,     0,     1,     1,     2,     1,     3,     0,     1,     1,
     0,     1,     3,     0
};

static const short yydefact[] = {     0,
     5,     8,     9,     6,     7,     1,     0,    10,     2,    11,
    24,    18,    14,    16,    22,    12,    21,    15,     0,    13,
     0,    19,    20,     0,    23,     4,    17,     0,     0,     0
};

static const short yydefgoto[] = {    28,
     6,     7,     8,    11,    21,    15,    18,    24,    16
};

static const short yypact[] = {     3,
-32768,-32768,-32768,-32768,-32768,-32768,     3,  -148,-32768,-32768,
   -72,  -147,-32768,-32768,-32768,   -46,  -133,-32768,   -72,-32768,
  -140,-32768,-32768,  -141,-32768,-32768,-32768,    14,    15,-32768
};

static const short yypgoto[] = {-32768,
    10,-32768,-32768,-32768,-32768,    -1,-32768,-32768,-32768
};


#define	YYLAST		108


static const short yytable[] = {    19,
    12,    22,    -3,     1,    10,    17,     2,     3,    13,     4,
     5,    26,    27,    29,    30,    23,     9,    25,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    14,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    20
};

static const short yycheck[] = {    46,
    73,   135,     0,     1,   153,   153,     4,     5,    81,     7,
     8,   152,   154,     0,     0,   149,     7,    19,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   149,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   154
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"
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

#line 217 "/usr/local/share/bison.simple"

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

case 4:
#line 192 "php_expr.y"
{
						PHPParser.setIncludeFile(yyvsp[-2]);
					;
    break;}
case 5:
#line 196 "php_expr.y"
{
						yyclearin;
						yyerrok;
//						wxPrintf(wxT("error: unexpected '%s' at line %d type=%d\n"),
//								 php_expr_lval.c_str(),
//								 php_expr_lineno,
//								 php_expr_type
//								 );
					;
    break;}
case 14:
#line 221 "php_expr.y"
{ 
				wxString strippedString(yyvsp[0]);
				if(strippedString.StartsWith(wxT("\"")) || strippedString.StartsWith(wxT("'")))
					strippedString.Remove(0, 1);
				
				if(strippedString.EndsWith(wxT("\"")) || strippedString.EndsWith(wxT("'")))
					strippedString.RemoveLast();
				
				yyval = strippedString;
		  ;
    break;}
case 15:
#line 231 "php_expr.y"
{
			  
			  if(yyvsp[-1] == wxT("getcwd")) {
				yyval = wxGetCwd();
				  
			  } else if(yyvsp[-1] == wxT("dirname")) {
				wxFileName fn(yyvsp[0]);
				yyval = fn.GetPath();
				  
			  } else if(yyvsp[-1] == wxT("DIRECTORY_SEPARATOR")) {
				yyval = wxFileName::GetPathSeparator();
				
			  }
		  ;
    break;}
case 16:
#line 245 "php_expr.y"
{
			   wxFileName fn(PHPParser.getCurrentFileName());
				yyval = fn.GetPath();
		  ;
    break;}
case 17:
#line 251 "php_expr.y"
{ yyval = yyvsp[-1];    ;
    break;}
case 18:
#line 252 "php_expr.y"
{ yyval.Clear(); ;
    break;}
case 19:
#line 255 "php_expr.y"
{
					yyval = PHPParser.getCurrentFileName();
				;
    break;}
case 20:
#line 258 "php_expr.y"
{ 
					wxFileName fn(PHPParser.getCurrentFileName());
					yyval = fn.GetPath();
				;
    break;}
case 22:
#line 265 "php_expr.y"
{ yyval = yyvsp[0];  ;
    break;}
case 23:
#line 266 "php_expr.y"
{ yyval << yyvsp[0]; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/local/share/bison.simple"

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
#line 270 "php_expr.y"



