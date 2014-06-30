//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : var_parser.cpp
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
#define yyparse cl_var_parse
#define yylex cl_var_lex
#define yyerror cl_var_error
#define yychar cl_var_char
#define yyval cl_var_val
#define yylval cl_var_lval
#define yydebug cl_var_debug
#define yynerrs cl_var_nerrs
#define yyerrflag cl_var_errflag
#define yyss cl_var_ss
#define yyssp cl_var_ssp
#define yyvs cl_var_vs
#define yyvsp cl_var_vsp
#define yylhs cl_var_lhs
#define yylen cl_var_len
#define yydefred cl_var_defred
#define yydgoto cl_var_dgoto
#define yysindex cl_var_sindex
#define yyrindex cl_var_rindex
#define yygindex cl_var_gindex
#define yytable cl_var_table
#define yycheck cl_var_check
#define yyname cl_var_name
#define yyrule cl_var_rule
#define YYPREFIX "cl_var_"
/* Copyright Eran Ifrah(c)*/
/*************** Includes and Defines *****************************/
#include "string"
#include "vector" 
#include "stdio.h"
#include "map"
#include "variable.h"
#include "cl_typedef.h"

#ifdef yylex
#undef yylex
#define yylex cl_scope_lex
#endif

#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

void cl_scope_error(char *string);
int  cl_var_parse();
void syncParser();
void var_consumeDefaultValue(char c1, char c2);
void var_consumeDefaultValueIfNeeded();

static  VariableList *        gs_vars = NULL;
static  std::vector<Variable> gs_names;
static  bool                  g_isUsedWithinFunc = false;
static  std::string           s_tmpString;
static  Variable              curr_var;
static  std::string           s_templateInitList;
static  bool                  isBasicType = false;

/*---------------------------------------------*/
/* externs defined in the lexer*/
/*---------------------------------------------*/
extern char *cl_scope_text;
extern int cl_scope_lex();
extern void cl_scope_less(int count);

extern int cl_scope_lineno;
extern std::vector<std::string> currentScope;
extern bool setLexerInput(const std::string &in, const std::map<std::string, std::string> &ignoreMap);
extern void setUseIgnoreMacros(bool ignore);
extern void cl_scope_lex_clean();

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
#define YYERRCODE 256
short cl_var_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    2,    2,    2,    2,    0,    0,
    5,    3,    3,    3,    6,    6,    6,    7,    7,    7,
   12,   13,   16,    4,   19,    4,    4,    4,    4,    4,
    4,    4,   14,   14,   14,   14,   24,   24,   15,   15,
   23,   23,   26,   18,   27,   18,   22,   22,   21,   21,
   21,   21,   20,   20,   20,   20,   20,   25,   25,    9,
    9,   28,   28,    8,    8,    8,    8,    8,    8,    8,
   29,   29,   30,   30,   31,   31,   10,   11,   11,   11,
   11,   11,   17,   17,   17,   32,   17,   17,
};
short cl_var_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    2,    2,    2,    2,    1,    2,    0,    2,
    0,    2,    3,    1,    0,    1,    3,    4,    4,    7,
    0,    0,    0,   13,    0,    8,    6,    6,    6,    5,
    5,    3,    0,    1,    2,    2,    1,    2,    1,    3,
    1,    1,    0,    2,    0,    5,    1,    1,    0,    1,
    1,    1,    1,    1,    1,    1,    1,    2,    5,    0,
    2,    0,    1,    0,    2,    2,    2,    3,    1,    2,
    0,    1,    0,    1,    0,    2,    3,    0,    1,    1,
    1,    1,    2,    3,    6,    0,    6,    4,
};
short cl_var_defred[] = {                                     19,
    0,   24,    0,   20,    0,    6,    1,    0,    2,    5,
    0,    0,   10,    0,    9,   12,   11,   17,    0,   89,
    0,   90,    0,   91,   92,   22,    0,    0,   14,   13,
   18,    3,    8,    7,   15,   16,   23,    0,    0,   79,
    0,   85,   85,    0,   85,    0,   80,   73,    0,   77,
   75,   70,   93,    0,    0,    0,    0,    0,    0,   42,
   78,    0,    0,   71,    0,   86,   82,    0,    0,   31,
    0,    0,   68,    0,    0,   57,   58,   41,   84,   87,
    0,   40,   32,    0,    0,   53,    0,    0,   96,    0,
   26,   70,   60,   61,   62,   39,   38,    0,   63,   85,
   64,   35,   66,   65,   67,   37,   54,   48,    0,   97,
    0,    0,    0,    0,    0,    0,    0,   27,   69,    0,
   85,    0,   55,   36,    0,   28,   29,   44,    0,    0,
    0,   56,    0,   45,   46,   33,    0,    0,   30,   49,
    0,   34,    0,   50,
};
short cl_var_dgoto[] = {                                       1,
   18,   19,    4,   26,    5,   90,   91,   92,   54,   55,
   27,   83,   98,  131,  141,  138,   42,   85,  116,  106,
   96,   78,   28,   86,   64,  107,  132,   51,   68,   80,
   56,  110,
};
short cl_var_sindex[] = {                                      0,
  -25,    0,  304,    0,   -4,    0,    0, -151,    0,    0,
  304,  331,    0,  331,    0,    0,    0,    0,  -45,    0,
 -148,    0, -148,    0,    0,    0, -186, -310,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -148,    8,    0,
  243,    0,    0, -284,    0,   -2,    0,    0,    9,    0,
    0,    0,    0, -254, -148,  103, -148,   15, -148,    0,
    0, -253,  -52,    0,  -15,    0,    0,   17,  -10,    0,
 -222,  -58,    0, -148,    7,    0,    0,    0,    0,    0,
    7,    0,    0,  -51,  555,    0, -220, -148,    0,   44,
    0,    0,    0,    0,    0,    0,    0, -167,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   67,    0,
 -148, -228,  252, -144, -222,   50, -228,    0,    0,  -44,
    0,  -37,    0,    0, -148,    0,    0,    0, -189, -183,
   20,    0,   70,    0,    0,    0, -228,  -37,    0,    0,
    5,    0,  -37,    0,
};
short cl_var_rindex[] = {                                      0,
  -40,    0,    0,    0,  192,    0,    0,   51,    0,    0,
   98,  123,    0,  132,    0,    0,    0,    0,    0,    0,
 -202,    0, -125,    0,    0,    0,  216,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   25,   53,    0,
 -175,    0,    0,    0,    0,    0,    0,    0,   86,    0,
    0,    0,    0,    0,   -9,  -34,   -9,    0, -160,    0,
    0,    0,  -17,    0,    0,    0,    0,  122,    0,    0,
    0,  148,    0,  110,    1,    0,    0,    0,    0,    0,
    1,    0,    0,  569,    0,    0,    0,  110,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  276,  157,    0,    0,    0,    0,    0,    0,    0,   58,
    0,   35,    0,    0,  110,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   58,   18,    0,    0,
    0,    0,   18,    0,
};
short cl_var_gindex[] = {                                      0,
   30,   -8,    0,    0,    0,  -82,   42,  388,  -29,  -30,
    0,    0,    0, -126,    0,    0,  152,    0,    0,    0,
   78,   99,    0,   54,  -33,    0,    0,    0,    0,    0,
    0,    0,
};
#define YYTABLESIZE 692
short cl_var_table[] = {                                      21,
   59,   88,   31,   21,  129,  109,   81,   74,   88,   81,
    3,  140,   57,   37,   59,  125,  144,   21,   21,   46,
   94,   58,   62,   94,   94,   77,   94,   81,   76,   81,
   77,   74,   53,   76,   74,   23,   59,   87,   60,   21,
   59,   35,  133,   36,   59,   50,   94,   94,  143,   49,
   61,   63,   72,   25,   20,   74,   74,   74,   59,   59,
   74,   43,  113,  142,   89,   74,   74,   93,   74,  115,
   44,   74,   74,   74,   74,   70,   43,   74,   74,   38,
   79,   87,   21,   84,   21,  108,   39,  111,    4,  126,
  127,    4,    4,   72,    4,   85,   72,   95,   40,   85,
  114,   85,  119,   74,  121,  112,  139,   29,  124,    4,
  111,   30,    4,  111,    4,  122,  134,   38,   22,   85,
   24,   85,  135,   59,   39,   59,   76,   52,  117,   76,
   70,  137,   74,   74,   74,    3,   40,   74,    3,    3,
   67,    3,  136,   74,   66,   74,   74,   74,   74,   74,
   74,   74,  118,   25,   74,   74,    3,   43,   97,    3,
    8,    3,   83,    8,    8,   83,    8,   82,  123,    7,
    0,   25,    7,    7,   43,    7,    0,    0,   45,    0,
   74,    8,    0,   83,    8,   98,    8,    0,   98,   98,
    7,   98,    0,    7,   95,    7,    0,   95,   95,    0,
   95,    0,    0,    0,   51,    0,    0,    0,    0,    0,
    0,   98,    0,    0,    0,    0,   21,   21,   21,   21,
   95,    0,   21,   74,   74,   21,    0,    0,   21,    0,
    2,   81,   21,   21,   21,   21,   21,    0,   81,   21,
   21,    0,    0,    0,   21,    0,    0,    0,   94,    0,
   81,    0,    0,    0,    0,   94,   59,   59,   59,   59,
   59,    0,    0,   59,    0,   21,   59,   94,  128,   59,
    0,   81,   73,   59,   59,   59,   59,   59,   73,   73,
   59,   59,   74,   74,   74,   59,   73,   74,   94,    0,
   75,    0,   48,   74,    0,   81,   74,    0,   74,   74,
   74,   74,    0,    0,   74,   74,   59,    0,   21,   21,
   72,   72,   72,  130,    0,   72,    4,    0,    0,    0,
    0,   72,    0,    4,    0,    0,   72,   72,   72,   72,
   74,    0,   72,   72,    0,    4,    0,    0,    0,    0,
    0,    0,    0,   76,   76,   76,    0,    0,   76,   59,
   59,    0,    0,    0,   76,    0,    4,    0,   72,   76,
   76,   76,   76,    3,    0,   76,   76,   74,   74,    0,
    3,    0,   74,   74,   74,    0,    0,    0,   74,    0,
    0,    0,    3,   74,   74,   74,   74,   83,    8,   74,
   74,   76,    0,    0,   83,    8,    0,    7,    0,    0,
    0,   72,   72,    3,    7,    0,   83,    8,   41,    0,
   41,    0,    0,   98,   41,   74,    7,    0,    0,    0,
   98,    0,   95,    0,    0,   47,    0,   83,    8,   95,
    0,    0,   98,    0,   76,   76,    0,    7,    0,    0,
    0,   95,   65,    0,   69,    0,   71,    0,   88,   88,
   88,   88,    0,   98,   88,    0,    0,   88,   74,   74,
   88,    0,   95,    0,   88,   88,   88,   88,   88,    0,
    0,   88,   88,   74,   74,   74,   88,    0,   74,    0,
    0,    0,    0,    0,   74,    0,    0,    0,    0,   74,
   74,   74,   74,    0,    0,   74,   74,   88,    0,    0,
    6,    7,   52,    0,    0,    8,    0,    0,    0,    6,
    7,    9,    0,    0,    8,    0,   10,   11,   12,   13,
    9,   74,   14,   15,    0,   10,   11,   12,   13,    0,
    0,   14,   15,   74,   74,    0,    0,    0,   74,    0,
   88,   88,    0,    0,   74,    0,    0,    0,    0,   74,
   74,   74,   74,    0,    0,   74,   74,  120,    0,    0,
    0,    6,    7,    0,   74,   74,    8,    0,    0,    0,
    0,    0,    9,    0,    0,    0,    0,   10,   11,   12,
   13,   74,    0,   14,   15,    0,    0,    0,    6,    7,
    0,   16,   17,    8,  103,  104,    0,    0,  100,    9,
   16,   17,    0,    0,   10,   32,   33,   13,   47,   47,
   34,   15,   47,   99,    0,  101,    0,    0,    0,    0,
    0,    0,    0,    0,   74,   74,    0,   47,    0,   47,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  105,    0,    0,    0,    0,
    0,    0,   16,   17,    0,    0,    0,    0,    0,   47,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  102,    0,   16,
   17,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   47,
};
short cl_var_check[] = {                                      40,
    0,   60,   11,   44,   42,   88,   41,   60,   60,   44,
   36,  138,   43,   59,   45,   60,  143,   58,   59,  330,
   38,  306,   52,   41,   42,   41,   44,   62,   44,   64,
   41,   41,   41,   44,   44,   40,   36,   71,   41,   44,
   40,   12,  125,   14,   44,   38,   64,   41,   44,   42,
   42,  306,  306,   58,   59,  258,  259,  260,   58,   59,
  263,   44,   92,   59,  123,   41,  269,   61,   44,  100,
  257,  274,  275,  276,  277,   61,   59,  280,  281,  266,
   64,  115,  123,  306,  125,  306,  273,   44,   38,  120,
  121,   41,   42,   41,   44,   38,   44,   91,  285,   42,
  268,   44,  331,  306,  113,   62,  137,  259,   59,   59,
   44,  263,   62,   44,   64,  260,  306,  266,  123,   62,
  125,   64,  306,  123,  273,  125,   41,  330,   62,   44,
  306,   62,  258,  259,  260,   38,  285,  263,   41,   42,
   38,   44,  123,  269,   42,  306,  349,  350,  274,  275,
  276,  277,  111,   44,  280,  281,   59,  123,   81,   62,
   38,   64,   41,   41,   42,   44,   44,   69,  115,   38,
   -1,   62,   41,   42,   23,   44,   -1,   -1,   27,   -1,
  306,   59,   -1,   62,   62,   38,   64,   -1,   41,   42,
   59,   44,   -1,   62,   38,   64,   -1,   41,   42,   -1,
   44,   -1,   -1,   -1,  330,   -1,   -1,   -1,   -1,   -1,
   -1,   64,   -1,   -1,   -1,   -1,  257,  258,  259,  260,
   64,   -1,  263,  349,  350,  266,   -1,   -1,  269,   -1,
  256,  266,  273,  274,  275,  276,  277,   -1,  273,  280,
  281,   -1,   -1,   -1,  285,   -1,   -1,   -1,  266,   -1,
  285,   -1,   -1,   -1,   -1,  273,  256,  257,  258,  259,
  260,   -1,   -1,  263,   -1,  306,  266,  285,  306,  269,
   -1,  306,  331,  273,  274,  275,  276,  277,  331,  331,
  280,  281,  258,  259,  260,  285,  331,  263,  306,   -1,
  306,   -1,  285,  269,   -1,  306,  306,   -1,  274,  275,
  276,  277,   -1,   -1,  280,  281,  306,   -1,  349,  350,
  258,  259,  260,  351,   -1,  263,  266,   -1,   -1,   -1,
   -1,  269,   -1,  273,   -1,   -1,  274,  275,  276,  277,
  306,   -1,  280,  281,   -1,  285,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  258,  259,  260,   -1,   -1,  263,  349,
  350,   -1,   -1,   -1,  269,   -1,  306,   -1,  306,  274,
  275,  276,  277,  266,   -1,  280,  281,  258,  259,   -1,
  273,   -1,  263,  349,  350,   -1,   -1,   -1,  269,   -1,
   -1,   -1,  285,  274,  275,  276,  277,  266,  266,  280,
  281,  306,   -1,   -1,  273,  273,   -1,  266,   -1,   -1,
   -1,  349,  350,  306,  273,   -1,  285,  285,   21,   -1,
   23,   -1,   -1,  266,   27,  306,  285,   -1,   -1,   -1,
  273,   -1,  266,   -1,   -1,   38,   -1,  306,  306,  273,
   -1,   -1,  285,   -1,  349,  350,   -1,  306,   -1,   -1,
   -1,  285,   55,   -1,   57,   -1,   59,   -1,  257,  258,
  259,  260,   -1,  306,  263,   -1,   -1,  266,  349,  350,
  269,   -1,  306,   -1,  273,  274,  275,  276,  277,   -1,
   -1,  280,  281,  258,  259,  260,  285,   -1,  263,   -1,
   -1,   -1,   -1,   -1,  269,   -1,   -1,   -1,   -1,  274,
  275,  276,  277,   -1,   -1,  280,  281,  306,   -1,   -1,
  258,  259,  260,   -1,   -1,  263,   -1,   -1,   -1,  258,
  259,  269,   -1,   -1,  263,   -1,  274,  275,  276,  277,
  269,  306,  280,  281,   -1,  274,  275,  276,  277,   -1,
   -1,  280,  281,  258,  259,   -1,   -1,   -1,  263,   -1,
  349,  350,   -1,   -1,  269,   -1,   -1,   -1,   -1,  274,
  275,  276,  277,   -1,   -1,  280,  281,  306,   -1,   -1,
   -1,  258,  259,   -1,  349,  350,  263,   -1,   -1,   -1,
   -1,   -1,  269,   -1,   -1,   -1,   -1,  274,  275,  276,
  277,  306,   -1,  280,  281,   -1,   -1,   -1,  258,  259,
   -1,  349,  350,  263,   40,   41,   -1,   -1,   44,  269,
  349,  350,   -1,   -1,  274,  275,  276,  277,   40,   41,
  280,  281,   44,   59,   -1,   61,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  349,  350,   -1,   59,   -1,   61,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   91,   -1,   -1,   -1,   -1,
   -1,   -1,  349,  350,   -1,   -1,   -1,   -1,   -1,   91,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  123,   -1,  349,
  350,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  123,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 351
#if YYDEBUG
char *cl_var_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,"'$'",0,"'&'",0,"'('","')'","'*'",0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,"':'",
"';'","'<'","'='","'>'",0,"'@'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,"'['",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,
"'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,"LE_AUTO","LE_DOUBLE","LE_INT","LE_STRUCT",
"LE_BREAK","LE_ELSE","LE_LONG","LE_SWITCH","LE_CASE","LE_ENUM","LE_REGISTER",
"LE_TYPEDEF","LE_CHAR","LE_EXTERN","LE_RETURN","LE_UNION","LE_CONST","LE_FLOAT",
"LE_SHORT","LE_UNSIGNED","LE_BOOL","LE_CONTINUE","LE_FOR","LE_SIGNED","LE_VOID",
"LE_DEFAULT","LE_GOTO","LE_SIZEOF","LE_VOLATILE","LE_DO","LE_IF","LE_STATIC",
"LE_WHILE","LE_NEW","LE_DELETE","LE_THIS","LE_OPERATOR","LE_CLASS","LE_PUBLIC",
"LE_PROTECTED","LE_PRIVATE","LE_VIRTUAL","LE_FRIEND","LE_INLINE","LE_OVERLOAD",
"LE_TEMPLATE","LE_TYPENAME","LE_THROW","LE_CATCH","LE_IDENTIFIER",
"LE_STRINGliteral","LE_FLOATINGconstant","LE_INTEGERconstant",
"LE_CHARACTERconstant","LE_OCTALconstant","LE_HEXconstant","LE_POUNDPOUND",
"LE_CComment","LE_CPPComment","LE_NAMESPACE","LE_USING","LE_TYPEDEFname",
"LE_ARROW","LE_ICR","LE_DECR","LE_LS","LE_RS","LE_LE","LE_GE","LE_EQ","LE_NE",
"LE_ANDAND","LE_OROR","LE_ELLIPSIS","LE_CLCL","LE_DOTstar","LE_ARROWstar",
"LE_MULTassign","LE_DIVassign","LE_MODassign","LE_PLUSassign","LE_MINUSassign",
"LE_LSassign","LE_RSassign","LE_ANDassign","LE_ERassign","LE_ORassign",
"LE_MACRO","LE_DYNAMIC_CAST","LE_STATIC_CAST","LE_CONST_CAST",
"LE_REINTERPRET_CAST","LE_SIZE_T","LE_TIME_T","\"**\"",
};
char *cl_var_rule[] = {
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
"basic_type_name_inter : LE_TIME_T",
"basic_type_name_inter : LE_SIZE_T",
"basic_type_name_inter : LE_LONG LE_LONG",
"basic_type_name_inter : LE_LONG LE_INT",
"basic_type_name : LE_UNSIGNED basic_type_name_inter",
"basic_type_name : LE_SIGNED basic_type_name_inter",
"basic_type_name : basic_type_name_inter",
"basic_type_name : LE_SHORT basic_type_name",
"translation_unit :",
"translation_unit : translation_unit external_decl",
"$$1 :",
"external_decl : $$1 variables",
"external_decl : '$' basic_type_name ';'",
"external_decl : error",
"parameter_list :",
"parameter_list : template_parameter",
"parameter_list : parameter_list ',' template_parameter",
"template_parameter : const_or_volatile_spec nested_scope_specifier LE_IDENTIFIER special_star_amp",
"template_parameter : const_or_volatile_spec nested_scope_specifier basic_type_name special_star_amp",
"template_parameter : const_or_volatile_spec nested_scope_specifier LE_IDENTIFIER '<' parameter_list '>' special_star_amp",
"$$2 :",
"$$3 :",
"$$4 :",
"variables : stmnt_starter LE_AUTO LE_IDENTIFIER '=' $$2 $$3 LE_TYPEDEF LE_STRUCT optional_struct_name '{' $$4 typedef_name_list ';'",
"$$5 :",
"variables : stmnt_starter variable_decl special_star_amp const_or_volatile_spec variable_name_list '{' $$5 ';'",
"variables : stmnt_starter variable_decl special_star_amp const_or_volatile_spec variable_name_list postfix",
"variables : '(' variable_decl special_star_amp const_or_volatile_spec LE_IDENTIFIER postfix2",
"variables : ',' variable_decl special_star_amp const_or_volatile_spec LE_IDENTIFIER postfix2",
"variables : '(' variable_decl special_star_amp const_or_volatile_spec postfix3",
"variables : ',' variable_decl special_star_amp const_or_volatile_spec postfix3",
"variables : ellipsis_prefix LE_ELLIPSIS ')'",
"optional_struct_name :",
"optional_struct_name : LE_IDENTIFIER",
"optional_struct_name : '*' LE_IDENTIFIER",
"optional_struct_name : \"**\" LE_IDENTIFIER",
"fully_qualified_identifier_name : LE_IDENTIFIER",
"fully_qualified_identifier_name : scope_specifier LE_IDENTIFIER",
"typedef_name_list : optional_struct_name",
"typedef_name_list : typedef_name_list ',' optional_struct_name",
"ellipsis_prefix : '('",
"ellipsis_prefix : ','",
"$$6 :",
"variable_name_list : fully_qualified_identifier_name $$6",
"$$7 :",
"variable_name_list : variable_name_list ',' special_star_amp fully_qualified_identifier_name $$7",
"postfix3 : ','",
"postfix3 : ')'",
"postfix2 :",
"postfix2 : '='",
"postfix2 : ')'",
"postfix2 : '['",
"postfix : ';'",
"postfix : '='",
"postfix : ')'",
"postfix : '('",
"postfix : '['",
"scope_specifier : LE_IDENTIFIER LE_CLCL",
"scope_specifier : LE_IDENTIFIER '<' parameter_list '>' LE_CLCL",
"nested_scope_specifier :",
"nested_scope_specifier : nested_scope_specifier scope_specifier",
"volatile_qualifier :",
"volatile_qualifier : LE_VOLATILE",
"const_or_volatile_spec :",
"const_or_volatile_spec : LE_CONST volatile_qualifier",
"const_or_volatile_spec : LE_CONST '*'",
"const_or_volatile_spec : LE_CONST '&'",
"const_or_volatile_spec : LE_CONST '*' '*'",
"const_or_volatile_spec : LE_VOLATILE",
"const_or_volatile_spec : LE_ENUM const_or_volatile_spec",
"amp_item :",
"amp_item : '&'",
"at_list :",
"at_list : '@'",
"star_list :",
"star_list : star_list '*'",
"special_star_amp : star_list amp_item at_list",
"stmnt_starter :",
"stmnt_starter : ';'",
"stmnt_starter : '{'",
"stmnt_starter : '}'",
"stmnt_starter : ':'",
"variable_decl : const_or_volatile_spec basic_type_name",
"variable_decl : const_or_volatile_spec nested_scope_specifier LE_IDENTIFIER",
"variable_decl : const_or_volatile_spec nested_scope_specifier LE_IDENTIFIER '<' parameter_list '>'",
"$$8 :",
"variable_decl : const_or_volatile_spec LE_STRUCT nested_scope_specifier LE_IDENTIFIER '{' $$8",
"variable_decl : const_or_volatile_spec LE_STRUCT nested_scope_specifier LE_IDENTIFIER",
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

void var_consumeAutoAssignment(const std::string& varname)
{
    // Collect everything until we encounter the first ';'
    std::string expression;
    while ( true ) {
        int ch = cl_scope_lex();
        if(ch == 0){
            break;
        }
        
        if ( ch == ';' ) {
            // add an auto variable
            Variable var;
            var.m_name = varname;
            var.m_isAuto = true;
            var.m_completeType.swap(expression);
            curr_var.m_lineno = cl_scope_lineno;
            
            s_templateInitList.clear();
            gs_vars->push_back(var);
            
            curr_var.Reset();
            gs_names.clear();
            
            break;
        } else {
            expression.append( cl_scope_text ).append(" ");
        }
    }
}

std::string var_consumBracketsContent(char openBrace)
{
    char closeBrace;

    switch(openBrace) {
    case '(': closeBrace = ')'; break;
    case '[': closeBrace = ']'; break;
    case '<': closeBrace = '>'; break;
    case '{': closeBrace = '}'; break;
    default:
        openBrace = '(';
        closeBrace = ')';
        break;
    }

    std::string consumedData;
    int depth = 1;
    while(depth > 0)
    {
        int ch = cl_scope_lex();
        if(ch == 0){
            break;
        }


        if(ch == closeBrace)
        {
            consumedData.erase(0, consumedData.find_first_not_of(" "));
            consumedData.erase(consumedData.find_last_not_of(" ")+1);
            consumedData += cl_scope_text;

            depth--;
            continue;
        }
        else if(ch == openBrace)
        {
            consumedData.erase(0, consumedData.find_first_not_of(" "));
            consumedData.erase(consumedData.find_last_not_of(" ")+1);
            consumedData += cl_scope_text;

            depth ++ ;
            continue;
        }

        consumedData += cl_scope_text;
        consumedData += " ";
    }

    return consumedData;
}

void var_consumeDefaultValueIfNeeded()
{
    int ch = cl_scope_lex();
    if(ch != '=') {
        cl_scope_less(0);
        return;
    }
    var_consumeDefaultValue(';', ',');
}
 
void var_consumeDefaultValue(char c1, char c2)
{
    int depth = 0;
    bool cont(true);

    curr_var.m_defaultValue.clear();
    while (depth >= 0) {
        int ch = cl_scope_lex();
        if(ch == 0) { break;}

        if(ch == c1 && depth == 0) {
            cl_scope_less(0);
            break;
        }

        if(ch == c2 && depth == 0) {
            cl_scope_less(0);
            break;
        }

        curr_var.m_defaultValue += cl_scope_text;
        if(ch == ')' || ch == '}'){
            depth--;
            continue;
        } else if(ch == '(' || ch == '{') {
            depth ++ ;
            continue;
        }
    }
}

void clean_up()
{
    gs_vars = NULL;

    // restore settings
    setUseIgnoreMacros(true);
    g_isUsedWithinFunc = false;
    isBasicType = false;
    
    //do the lexer cleanup
    cl_scope_lex_clean();
}

// return the scope name at the end of the input string
void get_variables(const std::string &in, VariableList &li, const std::map<std::string, std::string> &ignoreMap, bool isUsedWithinFunc)
{
    //provide the lexer with new input
    if( !setLexerInput(in, ignoreMap) ){
        return;
    }

    //set the parser local output to our variable list
    gs_vars = &li;
    setUseIgnoreMacros(false);

    // the 'g_isUsedWithinFunc' allows us to parse variabels without name
    // this is typical when used as function declaration (e.g. void setValue(bool);)
    g_isUsedWithinFunc = isUsedWithinFunc;

    //call tghe main parsing routine
    cl_var_parse();
    clean_up();
}

bool is_primitive_type(const std::string &in)
{
    std::string input = "@"; // Hack the input string...
    input += in;
    input += ";";
    
    const std::map<std::string, std::string> ignoreMap;
    if(!setLexerInput(input, ignoreMap)) {
        return false;
    }

    isBasicType = false;
    cl_var_parse();
    bool res = isBasicType;
    clean_up();
    return res;
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
{ yyval = yyvsp[0]; }
break;
case 13:
{ yyval = "long long";}
break;
case 14:
{ yyval = "long int";}
break;
case 15:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 16:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 17:
{ yyval = yyvsp[0]; }
break;
case 18:
{ yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 21:
{curr_var.Reset(); gs_names.clear(); s_tmpString.clear(); s_templateInitList.clear();}
break;
case 23:
{
                            isBasicType = true;
                        }
break;
case 24:
{
                            yyclearin;    /*clear lookahead token*/
                            yyerrok;
/*                            printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_var_lval.c_str(), cl_scope_lineno);*/
                            }
break;
case 25:
{yyval = "";}
break;
case 26:
{yyval = yyvsp[0];}
break;
case 27:
{yyval = yyvsp[-2] + yyvsp[-1] + " " + yyvsp[0];}
break;
case 28:
{
                            yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
                        }
break;
case 29:
{
                            yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
                        }
break;
case 30:
{
                            yyval = yyvsp[-6] + yyvsp[-5] + yyvsp[-4] +yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0] + " " ;
                        }
break;
case 31:
{var_consumeAutoAssignment(yyvsp[-1]);}
break;
case 32:
{}
break;
case 33:
{var_consumBracketsContent('{');}
break;
case 34:
{
                    }
break;
case 35:
{var_consumBracketsContent('{');}
break;
case 36:
{
                        if(gs_vars)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_isPtr = (yyvsp[-5].find("*") != (size_t)-1);
                                curr_var.m_starAmp = yyvsp[-5];
                                curr_var.m_lineno = cl_scope_lineno;
                                curr_var.m_rightSideConst = yyvsp[-4];
                                if(curr_var.m_templateDecl.empty())
                                    curr_var.m_templateDecl = s_templateInitList;
                                s_templateInitList.clear();

                                for(size_t i=0; i< gs_names.size(); i++)
                                {
                                    /*create new variable for every variable name found*/
                                    var = curr_var;
                                    var.m_pattern      = yyvsp[-6] + " " + yyvsp[-5] + " " + yyvsp[-4] ;
                                    if(var.m_completeType.empty()) {
                                        var.m_completeType = yyvsp[-6] + " " + yyvsp[-5] + " " + yyvsp[-4] ;
                                    }
                                    var.m_name         = gs_names.at(i).m_name;
                                    var.m_defaultValue = gs_names.at(i).m_defaultValue;

                                    if(i != 0) {
                                        var.m_isPtr        = gs_names.at(i).m_isPtr;
                                        var.m_starAmp      = gs_names.at(i).m_starAmp;
                                    }
                                    gs_vars->push_back(var);
                                }
                                curr_var.Reset();
                                gs_names.clear();
                            }
                    }
break;
case 37:
{
                            if(gs_vars)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_isPtr = (yyvsp[-3].find("*") != (size_t)-1);
                                curr_var.m_starAmp = yyvsp[-3];
                                curr_var.m_lineno = cl_scope_lineno;
                                curr_var.m_rightSideConst = yyvsp[-2];
                                if(curr_var.m_templateDecl.empty())
                                    curr_var.m_templateDecl = s_templateInitList;
                                s_templateInitList.clear();

                                for(size_t i=0; i< gs_names.size(); i++)
                                {
                                    /*create new variable for every variable name found*/
                                    var = curr_var;
                                    var.m_pattern      = yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] ;
                                    if(var.m_completeType.empty()) {
                                        var.m_completeType = yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] ;
                                    }
                                    var.m_name         = gs_names.at(i).m_name;
                                    var.m_defaultValue = gs_names.at(i).m_defaultValue;

                                    if(i != 0) {
                                        var.m_isPtr        = gs_names.at(i).m_isPtr;
                                        var.m_starAmp      = gs_names.at(i).m_starAmp;
                                    }
                                    gs_vars->push_back(var);
                                }
                                curr_var.Reset();
                                gs_names.clear();
                            }
                        }
break;
case 38:
{
                            if(gs_vars)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_pattern       = yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1];
                                curr_var.m_completeType  = yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] ;
                                curr_var.m_isPtr         = (yyvsp[-3].find("*") != (size_t)-1);
                                curr_var.m_starAmp       = yyvsp[-3];
                                curr_var.m_arrayBrackets = yyvsp[0];
                                curr_var.m_rightSideConst= yyvsp[-2];
                                curr_var.m_lineno        = cl_scope_lineno;
                                if(curr_var.m_templateDecl.empty())
                                    curr_var.m_templateDecl = s_templateInitList;
                                s_templateInitList.clear();    
                                
                                /*create new variable for every variable name found*/
                                var = curr_var;
                                var.m_name               = yyvsp[-1];
                                gs_vars->push_back(var);
                                curr_var.Reset();
                                gs_names.clear();
                            }
                        }
break;
case 39:
{
                            if(gs_vars && g_isUsedWithinFunc)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_pattern         = yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1];
                                curr_var.m_completeType    = yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] ;
                                curr_var.m_isPtr           = (yyvsp[-3].find("*") != (size_t)-1);
                                curr_var.m_starAmp         = yyvsp[-3];
                                curr_var.m_arrayBrackets   = yyvsp[0];
                                curr_var.m_rightSideConst  = yyvsp[-2];
                                curr_var.m_lineno          = cl_scope_lineno;
                                
                                if(curr_var.m_templateDecl.empty())
                                    curr_var.m_templateDecl = s_templateInitList;
                                s_templateInitList.clear();    
                                
                                /*create new variable for every variable name found*/
                                var = curr_var;
                                var.m_name = yyvsp[-1];
                                gs_vars->push_back(var);

                                curr_var.Reset();
                                gs_names.clear();
                            }
                        }
break;
case 40:
{
                            if(gs_vars && g_isUsedWithinFunc)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_pattern       = yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1];
                                curr_var.m_completeType  = yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1];
                                curr_var.m_isPtr         = (yyvsp[-2].find("*") != (size_t)-1);
                                curr_var.m_starAmp       = yyvsp[-2];
                                curr_var.m_rightSideConst= yyvsp[-1];
                                curr_var.m_lineno        = cl_scope_lineno;
                                if(curr_var.m_templateDecl.empty())
                                    curr_var.m_templateDecl = s_templateInitList;
                                s_templateInitList.clear();    
                                
                                /*create new variable for every variable name found*/
                                var = curr_var;
                                var.m_name = "";
                                gs_vars->push_back(var);

                                curr_var.Reset();
                                gs_names.clear();
                            }
                            if(yyvsp[0] == ",") {
                                cl_scope_less(0);
                            }
                        }
break;
case 41:
{
                            if(gs_vars && g_isUsedWithinFunc)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_pattern       = yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1];
                                curr_var.m_completeType  = yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1];
                                curr_var.m_isPtr         = (yyvsp[-2].find("*") != (size_t)-1);
                                curr_var.m_starAmp       = yyvsp[-2];
                                curr_var.m_lineno        = cl_scope_lineno;
                                curr_var.m_rightSideConst= yyvsp[-1];
                                if(curr_var.m_templateDecl.empty())
                                    curr_var.m_templateDecl = s_templateInitList;
                                s_templateInitList.clear();    
                                
                                /*create new variable for every variable name found*/
                                var = curr_var;
                                var.m_name = "";
                                gs_vars->push_back(var);

                                curr_var.Reset();
                                gs_names.clear();
                            }
                            if(yyvsp[0] == ",") {
                                cl_scope_less(0);
                            }
                        }
break;
case 42:
{
                            /* special type of argument: Ellipsis, can only be at the end of function argument */
                            if(gs_vars && g_isUsedWithinFunc)
                            {
                                Variable var;
                                std::string pattern;
                                curr_var.m_pattern      = yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0];
                                curr_var.m_completeType = yyvsp[-1];
                                curr_var.m_isPtr        = false;
                                curr_var.m_starAmp      = "";
                                curr_var.m_lineno       = cl_scope_lineno;
                                curr_var.m_isEllipsis   = true;

                                var = curr_var;
                                gs_vars->push_back(var);

                                curr_var.Reset();
                                gs_names.clear();
                            }
                        }
break;
case 47:
{yyval = yyvsp[0];}
break;
case 48:
{yyval = yyvsp[0];}
break;
case 51:
{yyval = yyvsp[0];}
break;
case 52:
{yyval = yyvsp[0];}
break;
case 53:
{var_consumeDefaultValueIfNeeded();}
break;
case 54:
{
                        curr_var.m_name = yyvsp[-1];
                        gs_names.push_back(curr_var);
                    }
break;
case 55:
{var_consumeDefaultValueIfNeeded();}
break;
case 56:
{
                        /*collect all the names*/
                        curr_var.m_name = yyvsp[-1];
                           curr_var.m_isPtr = (yyvsp[-2].find("*") != (size_t)-1);
                        curr_var.m_starAmp = yyvsp[-2];
                        gs_names.push_back(curr_var);
                        yyval = yyvsp[-4] + yyvsp[-3] + " " + yyvsp[-2] + yyvsp[-1];
                    }
break;
case 59:
{yyval = "";}
break;
case 60:
{var_consumeDefaultValue(',', ')'); yyval = ""; }
break;
case 61:
{ yyval = ""; }
break;
case 62:
{ yyval = yyvsp[0] + var_consumBracketsContent('[');}
break;
case 66:
{ yyval = yyvsp[0] + var_consumBracketsContent('(');}
break;
case 67:
{ yyval = yyvsp[0] + var_consumBracketsContent('[');}
break;
case 68:
{
                    yyval = yyvsp[-1]+ yyvsp[0]; 
                }
break;
case 69:
{
                    yyval = yyvsp[-4] + yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0]; 
                    s_templateInitList = yyvsp[-3] + yyvsp[-2] + yyvsp[-1];
                }
break;
case 70:
{yyval = "";}
break;
case 71:
{    yyval = yyvsp[-1] + yyvsp[0];}
break;
case 72:
{ yyval = "";           }
break;
case 73:
{ yyval = yyvsp[0]; curr_var.m_isVolatile = true; }
break;
case 74:
{ yyval = "";           }
break;
case 75:
{ yyval = yyvsp[-1];           }
break;
case 76:
{ yyval = yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;     }
break;
case 77:
{ yyval = yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;     }
break;
case 78:
{ yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;}
break;
case 79:
{ yyval = yyvsp[0]; curr_var.m_isVolatile = true; }
break;
case 80:
{ curr_var.m_enumInTypeDecl = true; }
break;
case 81:
{yyval = ""; }
break;
case 82:
{ yyval = yyvsp[0]; }
break;
case 83:
{yyval = "";}
break;
case 84:
{yyval = "*";}
break;
case 85:
{yyval = ""; }
break;
case 86:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 87:
{ yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]; }
break;
case 88:
{yyval = "";}
break;
case 89:
{ yyval = ";";}
break;
case 90:
{ yyval = "{";}
break;
case 91:
{ yyval = "}";}
break;
case 92:
{ yyval = ":";}
break;
case 93:
{
                            yyval = yyvsp[-1] + " " + yyvsp[0];
                            yyvsp[0].erase(yyvsp[0].find_last_not_of(":")+1);
                            curr_var.m_type = yyvsp[0];
                            curr_var.m_isBasicType = true;
                            curr_var.m_isConst = (yyvsp[-1].find("const") != std::string::npos);
                        }
break;
case 94:
{
                            yyval = yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];
                            yyvsp[-1].erase(yyvsp[-1].find_last_not_of(":")+1);
                            curr_var.m_typeScope = yyvsp[-1];
                            curr_var.m_type = yyvsp[0];
                            curr_var.m_isConst = (yyvsp[-2].find("const") != std::string::npos);
                            s_tmpString.clear();
                        }
break;
case 95:
{
                            yyval = yyvsp[-5] + " " + yyvsp[-4] + yyvsp[-3] + " " + yyvsp[-2] + yyvsp[-1] + yyvsp[0];
                            yyvsp[-4].erase(yyvsp[-4].find_last_not_of(":")+1);
                            curr_var.m_typeScope = yyvsp[-4];
                            curr_var.m_type = yyvsp[-3];
                            curr_var.m_isTemplate = true;
                            curr_var.m_templateDecl = yyvsp[-2] +yyvsp[-1] +yyvsp[0];
                            curr_var.m_isConst = (yyvsp[-5].find("const") != std::string::npos);
                            s_tmpString.clear();
                        }
break;
case 96:
{s_tmpString = var_consumBracketsContent('{');}
break;
case 97:
{
                            yyval = yyvsp[-5] + " " + yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] + yyvsp[-1] + yyvsp[0] + s_tmpString;
                            yyvsp[-3].erase(yyvsp[-3].find_last_not_of(":")+1);
                            curr_var.m_typeScope = yyvsp[-3];
                            curr_var.m_type = yyvsp[-2];
                            curr_var.m_completeType = yyvsp[-3] + yyvsp[-2];
                            curr_var.m_isConst = (yyvsp[-5].find("const") != std::string::npos);
                            s_tmpString.clear();
                        }
break;
case 98:
{
                            yyval = yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0];
                            yyvsp[-1].erase(yyvsp[-1].find_last_not_of(":")+1);
                            curr_var.m_typeScope = yyvsp[-1];
                            curr_var.m_type = yyvsp[0];
                            curr_var.m_isTemplate = !curr_var.m_templateDecl.empty();
                            curr_var.m_completeType = yyvsp[-1] + yyvsp[0];
                            curr_var.m_isConst = (yyvsp[-3].find("const") != std::string::npos);
                            s_tmpString.clear();
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
