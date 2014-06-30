//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : scope_parser.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define yyparse cl_scope_parse
#define yylex cl_scope_lex
#define yyerror cl_scope_error
#define yychar cl_scope_char
#define yyval cl_scope_val
#define yylval cl_scope_lval
#define yydebug cl_scope_debug
#define yynerrs cl_scope_nerrs
#define yyerrflag cl_scope_errflag
#define yyss cl_scope_ss
#define yyssp cl_scope_ssp
#define yyvs cl_scope_vs
#define yyvsp cl_scope_vsp
#define yylhs cl_scope_lhs
#define yylen cl_scope_len
#define yydefred cl_scope_defred
#define yydgoto cl_scope_dgoto
#define yysindex cl_scope_sindex
#define yyrindex cl_scope_rindex
#define yygindex cl_scope_gindex
#define yytable cl_scope_table
#define yycheck cl_scope_check
#define yyname cl_scope_name
#define yyrule cl_scope_rule
#define YYPREFIX "cl_scope_"
/* Copyright Eran Ifrah(c)*/
/*************** Includes and Defines *****************************/
#include <string>
#include <vector>
#include <stdio.h>
#include <map>
#include <string.h>

#define YYDEBUG_LEXER_TEXT (cl_scope_lval)
#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/
static std::string readInitializer(const char* delim);
static void readClassName();

static std::string className;

static std::string templateInitList;
int cl_scope_parse();
void cl_scope_error(char *string);
void syncParser();

static std::vector<std::string> gs_additionlNS;

/*---------------------------------------------*/
/* externs defined in the lexer*/
/*---------------------------------------------*/
extern char *cl_scope_text;
extern int cl_scope_lex();
extern bool setLexerInput(const std::string &in, const std::map<std::string, std::string> &ignoreTokens);
extern int cl_scope_lineno;
extern std::vector<std::string> currentScope;
extern void printScopeName();	/*print the current scope name*/
extern void increaseScope();	/*increase scope with anonymouse value*/
extern std::string getCurrentScope();
extern void cl_scope_lex_clean();
extern void cl_scope_less(int count);

/*************** Standard ytab.c continues here *********************/
#define LE_AUTO 257
#define LE_DOUBLE 258
#define LE_INT 259
#define LE_STRUCT 260
#define LE_BREAK 261
#define LE_ELSE 262
#define LE_LONG 263
#define LE_SWITCH 264
#define LE_CASE 265
#define LE_ENUM 266
#define LE_REGISTER 267
#define LE_TYPEDEF 268
#define LE_CHAR 269
#define LE_EXTERN 270
#define LE_RETURN 271
#define LE_UNION 272
#define LE_CONST 273
#define LE_FLOAT 274
#define LE_SHORT 275
#define LE_UNSIGNED 276
#define LE_BOOL 277
#define LE_CONTINUE 278
#define LE_FOR 279
#define LE_SIGNED 280
#define LE_VOID 281
#define LE_DEFAULT 282
#define LE_GOTO 283
#define LE_SIZEOF 284
#define LE_VOLATILE 285
#define LE_DO 286
#define LE_IF 287
#define LE_STATIC 288
#define LE_WHILE 289
#define LE_NEW 290
#define LE_DELETE 291
#define LE_THIS 292
#define LE_OPERATOR 293
#define LE_CLASS 294
#define LE_PUBLIC 295
#define LE_PROTECTED 296
#define LE_PRIVATE 297
#define LE_VIRTUAL 298
#define LE_FRIEND 299
#define LE_INLINE 300
#define LE_OVERLOAD 301
#define LE_TEMPLATE 302
#define LE_TYPENAME 303
#define LE_THROW 304
#define LE_CATCH 305
#define LE_IDENTIFIER 306
#define LE_STRINGliteral 307
#define LE_FLOATINGconstant 308
#define LE_INTEGERconstant 309
#define LE_CHARACTERconstant 310
#define LE_OCTALconstant 311
#define LE_HEXconstant 312
#define LE_POUNDPOUND 313
#define LE_CComment 314
#define LE_CPPComment 315
#define LE_NAMESPACE 316
#define LE_USING 317
#define LE_TYPEDEFname 318
#define LE_ARROW 319
#define LE_ICR 320
#define LE_DECR 321
#define LE_LS 322
#define LE_RS 323
#define LE_LE 324
#define LE_GE 325
#define LE_EQ 326
#define LE_NE 327
#define LE_ANDAND 328
#define LE_OROR 329
#define LE_ELLIPSIS 330
#define LE_CLCL 331
#define LE_DOTstar 332
#define LE_ARROWstar 333
#define LE_MULTassign 334
#define LE_DIVassign 335
#define LE_MODassign 336
#define LE_PLUSassign 337
#define LE_MINUSassign 338
#define LE_LSassign 339
#define LE_RSassign 340
#define LE_ANDassign 341
#define LE_ERassign 342
#define LE_ORassign 343
#define LE_MACRO 344
#define LE_DYNAMIC_CAST 345
#define LE_STATIC_CAST 346
#define LE_CONST_CAST 347
#define LE_REINTERPRET_CAST 348
#define LE_SIZE_T 349
#define LE_TIME_T 350
#define LE_DECLSPEC 351
#define LE_DLLIMPORT 352
#define LE_DLLIEXPORT 353
#define YYERRCODE 256
short cl_scope_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    2,    2,    2,    2,    2,    0,    0,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    3,   14,
   14,   14,   16,   16,   15,   15,   17,   17,   18,   18,
   18,   19,   19,   19,   10,    9,    9,    4,   11,   12,
   13,   24,   24,   25,   25,   26,   26,   26,   26,   26,
   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,
   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,
   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,
   26,   26,   26,   26,   26,   26,   27,   27,   28,   28,
    5,   31,    8,   32,    8,   33,    8,   21,   21,   34,
   35,   34,   29,   29,   20,   20,   36,   36,   37,   37,
   22,   23,   23,   23,   30,   30,   30,   39,    6,   40,
   40,   40,   40,   41,   38,   38,   38,    7,
};
short cl_scope_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    2,    2,    2,    2,    1,    0,    2,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    2,    3,    1,    3,    1,    1,    0,    4,    0,    1,
    3,    4,    4,    7,    5,    4,    3,    3,    1,    1,
    1,    1,    1,    1,    2,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    2,    2,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    0,    4,
    2,    0,   12,    0,    9,    0,   11,    0,    2,    2,
    0,    4,    0,    1,    0,    1,    0,    1,    0,    2,
    2,    0,    1,    1,    3,    3,    6,    0,    7,    0,
    2,    2,    2,    2,    0,    1,    3,    4,
};
short cl_scope_defred[] = {                                     17,
    0,   29,    0,    0,  123,   50,   49,   51,  124,   18,
   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,
    0,  101,  108,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   47,   53,   52,  114,   48,    0,    0,
  109,  128,  138,   35,   36,   33,    0,    0,   46,  116,
  108,  110,  111,   45,    0,    0,    0,   38,    0,  108,
    0,    0,    0,  136,   32,   34,    6,    1,    0,    2,
    5,    3,    0,   10,    0,    9,    0,    0,   11,    0,
   16,  119,    0,    0,  112,    0,  134,    0,  129,   15,
   14,    4,    8,    7,   12,   13,   84,   85,   72,   76,
   77,   68,   69,   78,   79,   80,   81,   70,   71,   75,
   73,   87,   88,   89,   90,   91,   92,   93,   94,   95,
   96,   57,   86,   66,   67,   56,   58,   59,   60,   61,
   62,   63,   64,   65,   74,    0,    0,   55,    0,  126,
    0,   54,    0,  125,  104,    0,    0,  133,  132,  131,
  137,   82,   83,    0,   40,  108,  120,  118,  121,  106,
    0,  102,    0,  119,    0,    0,   97,   98,  105,    0,
   41,  127,    0,  119,    0,    0,    0,   42,   43,  107,
    0,    0,    0,    0,  103,  119,    0,   44,  100,
};
short cl_scope_dgoto[] = {                                       1,
   81,   82,   10,   11,   12,   13,   14,   15,   16,   17,
   18,   19,   20,   46,   47,   48,   28,  154,  155,  156,
   29,  140,   21,   38,   83,  138,  169,  182,   39,   60,
  170,  161,  166,   41,   61,  159,  141,   63,   55,   87,
   64,
};
short cl_scope_sindex[] = {                                      0,
   16,    0, -281, -260,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -224,    0,    0, -239, -219,   39, -105, -217, -198,  -35,
   -2, -223,    2,    0,    0,    0,    0,    0, -146,  -58,
    0,    0,    0,    0,    0,    0, -176,   -9,    0,    0,
    0,    0,    0,    0, -172,   75, -223,    0,  115,    0,
 -194,   77,  -20,    0,    0,    0,    0,    0, -218,    0,
    0,    0,  169,    0,  169,    0,  -33,  -52,    0, -249,
    0,    0,  100, -246,    0, -200,    0, -172,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  101,   50,    0, -146,    0,
   44,    0,  104,    0,    0,  -48,  105,    0,    0,    0,
    0,    0,    0,    7,    0,    0,    0,    0,    0,    0,
  -19,    0, -146,    0, -174, -146,    0,    0,    0, -146,
    0,    0,  -34,    0,   23, -157, -146,    0,    0,    0,
  113,   33,   28, -146,    0,    0,   85,    0,    0,
};
short cl_scope_rindex[] = {                                      0,
 -109,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   82,    0,    0,    0,    0,    0,    0,  -71,    0,    0,
    0,   52,    0,    0,    0,    0,    0,    0,  124,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   -7,   53,   52,    0,    0,    0,
    0,   -6,    0,    0,    0,    0,    0,    0,  -22,    0,
    0,    0,   -8,    0,   21,    0,    0,   26,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   54,    0,
   32,    0,    0,    0,    0,  118,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  160,    0,    0,   36,    0,    0,    0, -108,
    0,    0,    8,    0,    0,   38,   54,    0,    0,    0,
    0,    0,    0,  -38,    0,    0,    0,    0,    0,
};
short cl_scope_gindex[] = {                                      0,
   58,  -10,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  112,    0,    0,    0,  -67,   -1,  -18,
  -28,  -51,    0,    0,   40,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   86,
};
#define YYTABLESIZE 518
short cl_scope_table[] = {                                     134,
   54,   53,   39,  129,  131,   39,  136,  139,  127,  126,
  123,   53,  135,  128,  115,    4,  122,   34,    4,    4,
   51,    4,   59,   88,   22,  177,  124,  122,  125,    8,
  144,   84,    8,    8,   57,    8,  135,  130,  168,    4,
   90,   24,   35,   77,   91,  119,   77,   25,  119,  119,
  163,  119,   58,    8,  113,   23,  142,  137,    7,  146,
  130,    7,    7,  119,    7,   54,   30,  119,  164,  119,
   44,  163,  117,    9,    5,  117,   36,   26,    8,   45,
   37,  158,    7,   67,   68,  157,   31,   42,   69,  186,
  132,   27,  133,  117,   70,   30,   31,   39,   32,   71,
   72,   73,   74,  167,   89,   75,   76,   40,  148,  183,
  149,  150,  172,   30,   31,   39,  187,  135,  130,  143,
   43,  178,  179,  147,   49,  189,   50,  165,  163,   56,
   95,  173,   96,   62,  188,   65,   85,   86,    6,  145,
    7,  152,  153,  160,  162,  180,  181,  175,  122,  122,
  122,  176,  184,  122,  174,  185,  122,   54,  115,  122,
   99,  171,  122,  122,  122,  122,  122,  122,   66,    0,
  122,  122,    0,  151,   79,    0,    0,    0,    0,    0,
    0,    0,    0,  122,  122,    0,  113,  113,  122,    0,
    0,  113,  122,    0,    0,  115,  122,  113,    0,    0,
   33,  113,  113,  113,  113,  113,  122,   37,  113,  113,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  115,
  115,  113,    0,    0,  115,    0,    0,    0,    0,    0,
  115,    0,    0,    0,  113,  115,  115,  115,  115,  122,
   80,  115,  115,    0,    0,    0,    0,    0,    0,  115,
    0,    0,    0,    0,    0,    0,   97,   98,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  115,    0,    0,
    4,    2,   52,    0,    0,    0,    0,  113,   52,    0,
    3,    0,   52,    4,    8,   99,  100,  101,  102,  103,
  104,  105,  106,  107,  108,  109,   52,    8,  110,  111,
  112,  113,  114,  115,  116,  117,  118,  119,  120,  121,
  115,  115,  115,    7,    0,    0,  115,    0,  119,    0,
    0,    0,  115,    0,  117,    0,    7,  115,  115,  115,
  115,  119,    4,  115,  115,    0,    0,  117,    0,   37,
   37,   37,    0,    0,   37,    0,    0,    0,    0,    0,
   37,    0,    0,    0,   37,   37,   37,   37,   37,  115,
    0,   37,   37,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   67,   68,   37,   37,    0,   69,    0,   37,
    0,  115,  115,   70,  111,    0,  115,   37,   71,   72,
   73,   74,  115,    0,   75,   76,    0,  115,  115,  115,
  115,    0,  115,  115,  115,    0,    0,   77,    0,    0,
    0,    0,    0,    0,    0,    0,  115,  115,  115,    0,
   78,    0,  115,    0,    0,    0,   67,   68,  115,  115,
   37,   92,    0,  115,  115,  115,  115,   70,    0,  115,
  115,    0,   71,   72,   93,   74,    0,    0,   94,   76,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   79,    0,  115,    0,    0,    0,    0,
    0,    0,  115,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  115,    0,
    0,    0,    0,    0,    0,    0,    0,   79,
};
short cl_scope_check[] = {                                      33,
   59,   60,   41,   37,   38,   44,   40,   60,   42,   43,
   44,   60,   46,   47,  123,   38,  126,  123,   41,   42,
   39,   44,   51,   44,  306,   60,   60,   61,   62,   38,
   82,   60,   41,   42,   44,   44,   44,   44,   58,   62,
  259,  266,  260,  293,  263,   38,  293,  272,   41,   42,
   44,   44,   62,   62,  126,  316,  306,   91,   38,  306,
   94,   41,   42,   38,   44,   40,  306,   42,   62,   62,
  294,   44,   41,   58,   59,   44,  294,  302,   63,  303,
  298,   38,   62,  258,  259,   42,  306,  123,  263,   62,
  124,  316,  126,   62,  269,   44,   44,   44,   60,  274,
  275,  276,  277,  123,  125,  280,  281,  306,  309,  177,
  311,  312,  164,   62,   62,   62,  184,  125,  125,   80,
  123,  173,  174,   84,  123,   41,  273,  156,   44,  306,
   73,  306,   75,  306,  186,   61,  331,   61,  123,   40,
  125,   41,   93,   40,   40,  123,  304,  166,  258,  259,
  260,  170,   40,  263,  165,  123,  266,   40,  123,  269,
  123,  163,  272,  273,  274,  275,  276,  277,   57,   -1,
  280,  281,   -1,   88,  349,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  293,  294,   -1,  258,  259,  298,   -1,
   -1,  263,  302,   -1,   -1,  304,  306,  269,   -1,   -1,
  306,  273,  274,  275,  276,  277,  316,  126,  280,  281,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,
  259,  293,   -1,   -1,  263,   -1,   -1,   -1,   -1,   -1,
  269,   -1,   -1,   -1,  306,  274,  275,  276,  277,  349,
  126,  280,  281,   -1,   -1,   -1,   -1,   -1,   -1,  126,
   -1,   -1,   -1,   -1,   -1,   -1,  290,  291,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  306,   -1,   -1,
  293,  256,  331,   -1,   -1,   -1,   -1,  349,  331,   -1,
  265,   -1,  331,  306,  293,  319,  320,  321,  322,  323,
  324,  325,  326,  327,  328,  329,  331,  306,  332,  333,
  334,  335,  336,  337,  338,  339,  340,  341,  342,  343,
  349,  258,  259,  293,   -1,   -1,  263,   -1,  293,   -1,
   -1,   -1,  269,   -1,  293,   -1,  306,  274,  275,  276,
  277,  306,  317,  280,  281,   -1,   -1,  306,   -1,  258,
  259,  260,   -1,   -1,  263,   -1,   -1,   -1,   -1,   -1,
  269,   -1,   -1,   -1,  273,  274,  275,  276,  277,  306,
   -1,  280,  281,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  258,  259,  293,  294,   -1,  263,   -1,  298,
   -1,  258,  259,  269,  331,   -1,  263,  306,  274,  275,
  276,  277,  269,   -1,  280,  281,   -1,  274,  275,  276,
  277,   -1,  349,  280,  281,   -1,   -1,  293,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  293,  258,  259,   -1,
  306,   -1,  263,   -1,   -1,   -1,  258,  259,  269,  306,
  349,  263,   -1,  274,  275,  276,  277,  269,   -1,  280,
  281,   -1,  274,  275,  276,  277,   -1,   -1,  280,  281,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  349,   -1,  306,   -1,   -1,   -1,   -1,
   -1,   -1,  349,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  349,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  349,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 353
#if YYDEBUG
char *cl_scope_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,0,0,"'%'","'&'",0,"'('","')'","'*'","'+'","','",0,"'.'","'/'",0,0,0,0,0,
0,0,0,0,0,"':'","';'","'<'","'='","'>'","'?'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,"'['",0,"']'","'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,"'{'","'|'","'}'","'~'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"LE_AUTO",
"LE_DOUBLE","LE_INT","LE_STRUCT","LE_BREAK","LE_ELSE","LE_LONG","LE_SWITCH",
"LE_CASE","LE_ENUM","LE_REGISTER","LE_TYPEDEF","LE_CHAR","LE_EXTERN",
"LE_RETURN","LE_UNION","LE_CONST","LE_FLOAT","LE_SHORT","LE_UNSIGNED","LE_BOOL",
"LE_CONTINUE","LE_FOR","LE_SIGNED","LE_VOID","LE_DEFAULT","LE_GOTO","LE_SIZEOF",
"LE_VOLATILE","LE_DO","LE_IF","LE_STATIC","LE_WHILE","LE_NEW","LE_DELETE",
"LE_THIS","LE_OPERATOR","LE_CLASS","LE_PUBLIC","LE_PROTECTED","LE_PRIVATE",
"LE_VIRTUAL","LE_FRIEND","LE_INLINE","LE_OVERLOAD","LE_TEMPLATE","LE_TYPENAME",
"LE_THROW","LE_CATCH","LE_IDENTIFIER","LE_STRINGliteral","LE_FLOATINGconstant",
"LE_INTEGERconstant","LE_CHARACTERconstant","LE_OCTALconstant","LE_HEXconstant",
"LE_POUNDPOUND","LE_CComment","LE_CPPComment","LE_NAMESPACE","LE_USING",
"LE_TYPEDEFname","LE_ARROW","LE_ICR","LE_DECR","LE_LS","LE_RS","LE_LE","LE_GE",
"LE_EQ","LE_NE","LE_ANDAND","LE_OROR","LE_ELLIPSIS","LE_CLCL","LE_DOTstar",
"LE_ARROWstar","LE_MULTassign","LE_DIVassign","LE_MODassign","LE_PLUSassign",
"LE_MINUSassign","LE_LSassign","LE_RSassign","LE_ANDassign","LE_ERassign",
"LE_ORassign","LE_MACRO","LE_DYNAMIC_CAST","LE_STATIC_CAST","LE_CONST_CAST",
"LE_REINTERPRET_CAST","LE_SIZE_T","LE_TIME_T","LE_DECLSPEC","LE_DLLIMPORT",
"LE_DLLIEXPORT",
};
char *cl_scope_rule[] = {
"$accept : translation_unit",
"basic_type_name_inter : LE_INT",
"basic_type_name_inter : LE_CHAR",
"basic_type_name_inter : LE_SHORT",
"basic_type_name_inter : LE_LONG",
"basic_type_name_inter : LE_FLOAT",
"basic_type_name_inter : LE_DOUBLE",
"basic_type_name_inter : LE_SIGNED",
"basic_type_name_inter : LE_UNSIGNED",
"basic_type_name_inter : LE_VOID",
"basic_type_name_inter : LE_BOOL",
"basic_type_name_inter : LE_SIZE_T",
"basic_type_name : LE_UNSIGNED basic_type_name_inter",
"basic_type_name : LE_SIGNED basic_type_name_inter",
"basic_type_name : LE_LONG LE_LONG",
"basic_type_name : LE_LONG LE_INT",
"basic_type_name : basic_type_name_inter",
"translation_unit :",
"translation_unit : translation_unit external_decl",
"external_decl : class_decl",
"external_decl : dummy_case",
"external_decl : enum_decl",
"external_decl : union_decl",
"external_decl : function_decl",
"external_decl : namespace_decl",
"external_decl : using_namespace",
"external_decl : scope_reducer",
"external_decl : scope_increaer",
"external_decl : question_expression",
"external_decl : error",
"template_arg :",
"template_arg : template_specifiter LE_IDENTIFIER",
"template_arg : template_specifiter LE_IDENTIFIER '='",
"template_arg_list : template_arg",
"template_arg_list : template_arg_list ',' template_arg",
"template_specifiter : LE_CLASS",
"template_specifiter : LE_TYPENAME",
"opt_template_qualifier :",
"opt_template_qualifier : LE_TEMPLATE '<' template_arg_list '>'",
"template_parameter_list :",
"template_parameter_list : template_parameter",
"template_parameter_list : template_parameter_list ',' template_parameter",
"template_parameter : const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp",
"template_parameter : const_spec nested_scope_specifier basic_type_name special_star_amp",
"template_parameter : const_spec nested_scope_specifier LE_IDENTIFIER '<' template_parameter_list '>' special_star_amp",
"using_namespace : LE_USING LE_NAMESPACE nested_scope_specifier LE_IDENTIFIER ';'",
"namespace_decl : stmnt_starter LE_NAMESPACE LE_IDENTIFIER '{'",
"namespace_decl : stmnt_starter LE_NAMESPACE '{'",
"class_decl : stmnt_starter opt_template_qualifier class_keyword",
"scope_reducer : '}'",
"scope_increaer : '{'",
"question_expression : '?'",
"class_keyword : LE_CLASS",
"class_keyword : LE_STRUCT",
"func_name : LE_IDENTIFIER",
"func_name : LE_OPERATOR any_operator",
"any_operator : '+'",
"any_operator : '='",
"any_operator : '*'",
"any_operator : '/'",
"any_operator : '%'",
"any_operator : '^'",
"any_operator : '&'",
"any_operator : '|'",
"any_operator : '~'",
"any_operator : '!'",
"any_operator : '<'",
"any_operator : '>'",
"any_operator : LE_LS",
"any_operator : LE_RS",
"any_operator : LE_ANDAND",
"any_operator : LE_OROR",
"any_operator : LE_ARROW",
"any_operator : LE_ARROWstar",
"any_operator : '.'",
"any_operator : LE_DOTstar",
"any_operator : LE_ICR",
"any_operator : LE_DECR",
"any_operator : LE_LE",
"any_operator : LE_GE",
"any_operator : LE_EQ",
"any_operator : LE_NE",
"any_operator : '(' ')'",
"any_operator : '[' ']'",
"any_operator : LE_NEW",
"any_operator : LE_DELETE",
"any_operator : ','",
"any_operator : LE_MULTassign",
"any_operator : LE_DIVassign",
"any_operator : LE_MODassign",
"any_operator : LE_PLUSassign",
"any_operator : LE_MINUSassign",
"any_operator : LE_LSassign",
"any_operator : LE_RSassign",
"any_operator : LE_ANDassign",
"any_operator : LE_ERassign",
"any_operator : LE_ORassign",
"optional_initialization_list : '{'",
"optional_initialization_list : ':'",
"declare_throw :",
"declare_throw : LE_THROW '(' template_parameter_list ')'",
"dummy_case : LE_CASE LE_IDENTIFIER",
"$$1 :",
"function_decl : stmnt_starter opt_template_qualifier virtual_spec const_spec variable_decl nested_scope_specifier func_name '(' $$1 const_spec declare_throw '{'",
"$$2 :",
"function_decl : stmnt_starter opt_template_qualifier virtual_spec const_spec nested_scope_specifier func_name '(' $$2 optional_initialization_list",
"$$3 :",
"function_decl : stmnt_starter opt_template_qualifier virtual_spec const_spec nested_scope_specifier '~' func_name '(' $$3 const_spec '{'",
"nested_scope_specifier :",
"nested_scope_specifier : nested_scope_specifier scope_specifier",
"scope_specifier : LE_IDENTIFIER LE_CLCL",
"$$4 :",
"scope_specifier : LE_IDENTIFIER '<' $$4 LE_CLCL",
"virtual_spec :",
"virtual_spec : LE_VIRTUAL",
"const_spec :",
"const_spec : LE_CONST",
"amp_item :",
"amp_item : '&'",
"star_list :",
"star_list : star_list '*'",
"special_star_amp : star_list amp_item",
"stmnt_starter :",
"stmnt_starter : ';'",
"stmnt_starter : ':'",
"variable_decl : nested_scope_specifier basic_type_name special_star_amp",
"variable_decl : nested_scope_specifier LE_IDENTIFIER special_star_amp",
"variable_decl : nested_scope_specifier LE_IDENTIFIER '<' template_parameter_list '>' special_star_amp",
"$$5 :",
"enum_decl : stmnt_starter LE_ENUM LE_IDENTIFIER '{' $$5 enum_arg_list '}'",
"enum_optional_assign :",
"enum_optional_assign : '=' LE_HEXconstant",
"enum_optional_assign : '=' LE_OCTALconstant",
"enum_optional_assign : '=' LE_INTEGERconstant",
"enum_argument : LE_IDENTIFIER enum_optional_assign",
"enum_arg_list :",
"enum_arg_list : enum_argument",
"enum_arg_list : enum_arg_list ',' enum_argument",
"union_decl : stmnt_starter LE_UNION LE_IDENTIFIER '{'",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
void yyerror(char *s) {}

void syncParser(){
	//move lexer to the next ';' line or scope opening '{'
	//int ch = cl_scope_lex();
}

//swallow all tokens up to the first '{'
void consumeInitializationList(){
	while( true ){
		int ch = cl_scope_lex();
		if(ch == 0){
			break;
		}

		if(ch == '{'){
			break;
		}
	}
}

void consumeFuncArgList(){
	int depth = 1;
	while(depth > 0){
		int ch = cl_scope_lex();
		if(ch == 0){
			break;
		}

		if(ch == ')'){
			depth--;
			continue;
		}
		else if(ch == '('){
			depth ++ ;
			continue;
		}
	}
}

void readClassName()
{
#define NEXT_TOK()          c = cl_scope_lex(); if(c == 0) {className.clear(); return;}
#define BREAK_IF_NOT(x)     if(c != (int)x) {className.clear(); break;}
#define BREAK_IF_NOT2(x, y) if(c != (int)x && c != (int)y) break;
#define BREAK_IF(x)         if(c == (int)x) break;

	className.clear();
	
	// look ahead and see if we can see another 
	while( true ){
		int c = cl_scope_lex();
		if(c == 0){ // EOF?
			className.clear();
			break;
		}
		
		if(c == LE_MACRO) {
			continue;
			
		} else if(c == LE_IDENTIFIER) {
			className = cl_scope_text;
		
		} else if(c == LE_DECLSPEC && className.empty()) {
			// found decl sepc
			
			// Next token is '('
			NEXT_TOK();
			BREAK_IF_NOT('(');
			
			// Next token is LE_DLLIMPORT or LE_DLLEXPORT
			NEXT_TOK();
			BREAK_IF_NOT2(LE_DLLIEXPORT, LE_DLLIMPORT);
			
			// Next token should be closing brace
			NEXT_TOK();
			BREAK_IF_NOT(')');
			
		} else if( (c == '{') && (!className.empty()) ){
			// The following is the class content
			break;
			
		} else if( c == ':' && !className.empty() ) {
			// we got the class name, and we found ':'
			// read all tokens up until the first open brace
			while (true) {
				NEXT_TOK();
				if( c == (int)'{') {
					return;
				}
			}
		} else {
			className.clear();
			break;
			
		}
	}
}

/**
 * consume all token until matching closing brace is found
 */
void consumeDecl()
{
	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_scope_lex();
		if(ch ==0)
		{
			break;
		}
		if(ch == '}')
		{
			depth--;
			if(depth == 0) currentScope.pop_back();//reduce the scope
			continue;
		}
		else if(ch == '{')
		{
			depth ++ ;
			continue;
		}
	}

}

void consumeTemplateDecl()
{
	templateInitList.clear();
	int dep = 0;
	while( true ){
		int c = cl_scope_lex();
		if(c == 0){ // EOF?
			break;
		}

		if(c == '>' && dep == 0){
			templateInitList += cl_scope_text;
			break;
			
		} else {
			templateInitList += cl_scope_text;
			templateInitList += " ";
		}
		
		switch(c) {
		case (int)'<':
			dep++;
			break;
		case (int)'>':
			dep--;
			break;
		default:
			break;
		}
	}
	
	if(templateInitList.empty() == false)
		templateInitList.insert(0, "<");
}

std::string readInitializer(const char* delim)
{
	std::string intializer;
	int dep = 0;
	while( true ){
		int c = cl_scope_lex();
		if(c == 0){ // EOF?
			break;
		}
		
		if(strchr(delim, (char)c) && dep == 0){
			cl_scope_less(0);
			break;
			
		} else {
			intializer += cl_scope_text;
			intializer += " ";
		}
		
		switch(c) {
		case (int)'<':
			dep++;
			break;
		case (int)'>':
			dep--;
			break;
		default:
			break;
		}
	}
	return intializer;
}
//swallow all tokens up to the first '{'
void consumeNotIncluding(int ch){
	while( true ){
		int c = cl_scope_lex();
		if(c == 0){ // EOF?
			break;
		}

		//keep the function signature
		if(c == ch){
			cl_scope_less(0);
			break;
		}
	}
}

// return the scope name at the end of the input string
std::string get_scope_name(	const std::string &in,
							std::vector<std::string> &additionalNS,
							const std::map<std::string, std::string> &ignoreTokens)
{
	if( !setLexerInput(in, ignoreTokens) ){
		return "";
	}

	//call tghe main parsing routine
	cl_scope_parse();
	std::string scope = getCurrentScope();
	//do the lexer cleanup
	cl_scope_lex_clean();

	for(size_t i=0; i<gs_additionlNS.size(); i++){
		additionalNS.push_back(gs_additionlNS.at(i));
	}
	gs_additionlNS.clear();
	return scope;
}
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
{ yyval = yyvsp[0]; }
break;
case 2:
{ yyval = yyvsp[0]; }
break;
case 3:
{ yyval = yyvsp[0]; }
break;
case 4:
{ yyval = yyvsp[0]; }
break;
case 5:
{ yyval = yyvsp[0]; }
break;
case 6:
{ yyval = yyvsp[0]; }
break;
case 7:
{ yyval = yyvsp[0]; }
break;
case 8:
{ yyval = yyvsp[0]; }
break;
case 9:
{ yyval = yyvsp[0]; }
break;
case 10:
{ yyval = yyvsp[0]; }
break;
case 11:
{ yyval = yyvsp[0]; }
break;
case 12:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 13:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 14:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 15:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 16:
{ yyval = yyvsp[0]; }
break;
case 29:
{
								/*printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_scope_text, cl_scope_lineno);*/
								/*syncParser();*/
							}
break;
case 30:
{ yyval = "";}
break;
case 31:
{yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 32:
{yyval = yyvsp[-2] + " " + yyvsp[-1] + "=" + readInitializer(",>");}
break;
case 33:
{ yyval = yyvsp[0]; }
break;
case 34:
{ yyval = yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0]; }
break;
case 35:
{ yyval = yyvsp[0]; }
break;
case 36:
{ yyval = yyvsp[0]; }
break;
case 38:
{ 
								yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0];
							}
break;
case 39:
{yyval = "";}
break;
case 40:
{yyval = yyvsp[0];}
break;
case 41:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 42:
{
							yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
						}
break;
case 43:
{
							yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
						}
break;
case 44:
{
							yyval = yyvsp[-6] + yyvsp[-5] + yyvsp[-4] +yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0] + " " ;
						}
break;
case 45:
{
						gs_additionlNS.push_back(yyvsp[-2]+yyvsp[-1]);
					}
break;
case 46:
{
							currentScope.push_back(yyvsp[-1]);
							
						}
break;
case 47:
{
							/*anonymouse namespace*/
							increaseScope();
							
						}
break;
case 48:
{
					readClassName();
					/*increase the scope level*/
					if(className.empty() == false) {
						currentScope.push_back( className );
						printScopeName();
					}
				}
break;
case 49:
{
								if(currentScope.empty())
								{
									/*fatal error!*/
									/*printf("CodeLite: fatal error - cant go beyond global scope!\n");*/
								}
								else
								{
									currentScope.pop_back();
									printScopeName();
								}
							}
break;
case 50:
{
								/*increase random scope*/
								increaseScope();
								printScopeName();
							 }
break;
case 51:
{
							consumeNotIncluding(';');
						}
break;
case 52:
{yyval = yyvsp[0];}
break;
case 53:
{yyval = yyvsp[0];}
break;
case 54:
{yyval = yyvsp[0];}
break;
case 55:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 82:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 83:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 97:
{yyval = '{';}
break;
case 98:
{consumeInitializationList() /*eat everything including the open brace*/;}
break;
case 99:
{yyval = "";}
break;
case 100:
{yyval = yyvsp[-1];}
break;
case 101:
{
	/* eat up all tokens not including the ':'*/
	consumeNotIncluding(':');
}
break;
case 102:
{consumeFuncArgList();}
break;
case 103:
{
                        /*printf("found function %s\n", $7.c_str());*/
						/*trim down trailing '::' from scope name*/
						if(yyvsp[-6].find_last_not_of(":") != std::string::npos){
							yyvsp[-6].erase(yyvsp[-6].find_last_not_of(":")+1);
						}
						currentScope.push_back(yyvsp[-6]);
						printScopeName();
					}
break;
case 104:
{consumeFuncArgList();}
break;
case 105:
{

						/*trim down trailing '::' from scope name*/
						if(yyvsp[-4].find_last_not_of(":") != std::string::npos){
							yyvsp[-4].erase(yyvsp[-4].find_last_not_of(":")+1);
						}
						currentScope.push_back(yyvsp[-4]);
						printScopeName();
					}
break;
case 106:
{consumeFuncArgList();}
break;
case 107:
{

						/*trim down trailing '::' from scope name*/
						if(yyvsp[-6].find_last_not_of(":") != std::string::npos){
							yyvsp[-6].erase(yyvsp[-6].find_last_not_of(":")+1);
						}
						currentScope.push_back(yyvsp[-6]);
						printScopeName();
					}
break;
case 108:
{yyval = "";}
break;
case 109:
{	yyval = yyvsp[-1] + yyvsp[0];}
break;
case 110:
{yyval = yyvsp[-1]+ yyvsp[0];}
break;
case 111:
{consumeTemplateDecl();}
break;
case 112:
{yyval = yyvsp[-3] + yyvsp[0];}
break;
case 113:
{yyval = ""; }
break;
case 114:
{ yyval = yyvsp[0]; }
break;
case 115:
{yyval = ""; }
break;
case 116:
{ yyval = yyvsp[0]; }
break;
case 117:
{yyval = ""; }
break;
case 118:
{ yyval = yyvsp[0]; }
break;
case 119:
{yyval = ""; }
break;
case 120:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 121:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 122:
{yyval = "";}
break;
case 123:
{ yyval = ";";}
break;
case 124:
{ yyval = ":";}
break;
case 125:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;}
break;
case 126:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;}
break;
case 127:
{yyval = yyvsp[-5] + yyvsp[-4] + yyvsp[-3]  + yyvsp[-2] + yyvsp[-1] + yyvsp[0] ;}
break;
case 128:
{currentScope.push_back(yyvsp[-1]); printScopeName();}
break;
case 129:
{
							currentScope.pop_back();/*reduce the scope*/
							printScopeName();
							/*printf("found enum: %s, args are: %s\n", $2.c_str(), $5.c_str());*/
						}
break;
case 130:
{yyval = "";}
break;
case 131:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 132:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 133:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 134:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 135:
{yyval = "";}
break;
case 136:
{yyval = yyvsp[0];}
break;
case 137:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 138:
{
								currentScope.push_back(yyvsp[-1]);
								printScopeName();
								consumeDecl();
								printScopeName();
							}
break;
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
