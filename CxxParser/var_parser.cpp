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

static  VariableList *           gs_vars = NULL;
static  std::vector<std::string> gs_names;
static  bool                     g_isUsedWithinFunc = false;
static  std::string              s_tmpString;
static  Variable                 curr_var;
static  std::string              s_templateInitList;
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
    5,    3,    3,    6,    6,    6,    7,    7,    7,   13,
    4,    4,    4,    4,    4,    4,    4,   11,   11,   11,
   11,   12,   12,   20,   20,   16,   16,   19,   19,   18,
   18,   18,   18,   17,   17,   17,   17,   17,   21,   21,
    9,    9,    8,    8,    8,    8,    8,   22,   22,   23,
   23,   10,   14,   14,   14,   14,   14,   15,   15,   15,
   24,   15,   15,
};
short cl_var_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    2,    2,    1,    1,    2,    2,    2,    1,    0,    2,
    0,    2,    1,    0,    1,    3,    4,    4,    7,    0,
    7,    6,    6,    6,    5,    5,    3,    0,    1,    2,
    2,    1,    3,    1,    1,    1,    4,    1,    1,    0,
    1,    1,    1,    1,    1,    1,    1,    1,    2,    5,
    0,    2,    0,    1,    2,    2,    3,    0,    1,    0,
    2,    2,    0,    1,    1,    1,    1,    2,    3,    6,
    0,    6,    4,
};
short cl_var_defred[] = {                                     19,
    0,   23,   20,    0,    0,    0,   75,   74,    0,   76,
   77,   22,    0,    0,    0,    0,    0,   70,   70,   70,
    0,   39,    0,    0,    0,    0,   66,    6,    0,   61,
    0,    2,    5,    0,    0,   10,    0,    9,   14,   13,
   18,   78,    0,    0,    0,    0,    0,   37,   40,   41,
   30,   67,   12,    0,   11,    3,    8,    7,   17,   15,
   16,    0,   62,    0,   71,   69,   72,    0,    0,    0,
    0,   59,    0,    0,   48,   49,   36,    0,   35,   46,
    0,   42,    0,    0,   81,    0,   25,   61,   52,   51,
   53,   34,   33,   70,   54,   57,   56,   55,   58,   32,
    0,   31,    0,   82,    0,    0,    0,    0,   43,    0,
   26,   60,    0,   70,   47,    0,   27,   28,    0,    0,
   29,
};
short cl_var_dgoto[] = {                                       1,
   41,   42,    3,   12,    4,   86,   87,   88,   43,   44,
   25,   83,   70,   13,   18,   81,  100,   92,   77,   14,
   63,   67,   45,  104,
};
short cl_var_sindex[] = {                                      0,
 -249,    0,    0,   55, -250, -261,    0,    0, -261,    0,
    0,    0, -261, -309,  -34,    6,  165,    0,    0,    0,
  -17,    0, -239, -236,  -67,   15,    0,    0, -182,    0,
 -170,    0,    0,  242,  242,    0,  242,    0,    0,    0,
    0,    0, -208, -261,    8, -261, -261,    0,    0,    0,
    0,    0,    0, -196,    0,    0,    0,    0,    0,    0,
    0,  -51,    0,  -38,    0,    0,    0,  -19, -190,  -34,
  -58,    0, -261,   14,    0,    0,    0,   14,    0,    0,
  103,    0,   42, -261,    0,   34,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -34,    0,   45,    0, -261, -209,  190, -178,    0, -209,
    0,    0,  -45,    0,    0, -261,    0,    0,   47, -209,
    0,
};
short cl_var_rindex[] = {                                      0,
  -40,    0,    0, -140,    0, -106,    0,    0,  113,    0,
    0,    0,  141,    0,   16,   25, -164,    0,    0,    0,
    0,    0,    0,    0,    0,   53,    0,    0,  -15,    0,
  -10,    0,    0,   20,   30,    0,   46,    0,    0,    0,
    0,    0,    0,  -11,  -27,  -11, -160,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   -2,    0,    0,    0,    0,    0,    0,    0,   58,
   35,    0,   86,    1,    0,    0,    0,    1,    0,    0,
    0,    0,    0,   86,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   58,    0,    0,    0,  218,   62,    0,    0,    0,    0,
    0,    0,   83,    0,    0,   86,    0,    0,    0,   83,
    0,
};
short cl_var_gindex[] = {                                      0,
  121,   43,    0,    0,    0,  -73,   44,    7,  -25,   18,
  -21,    0,    0,    0,  102,    0,    0,   73,   91,    0,
    0,    0,    0,    0,
};
#define YYTABLESIZE 592
short cl_var_table[] = {                                      21,
   50,   84,   76,   21,   54,   75,    2,   23,   73,   15,
  103,   16,   17,   68,  116,   17,   68,   21,   21,   17,
   21,   76,    1,   48,   75,    1,    1,    4,    1,   63,
    4,    4,   63,    4,   68,   79,   46,   47,   79,   79,
   50,   79,  119,   27,   50,   66,    1,   26,   82,   65,
   64,    4,   68,   69,   89,   51,   52,    3,   50,   50,
    3,    3,  107,    3,   85,   64,   49,    8,   64,   50,
    8,    8,   83,    8,   90,   83,   83,  105,   83,  109,
   53,    3,   21,    7,   21,  101,    7,    7,  105,    7,
  105,    8,   55,   65,    9,  106,   65,   62,    6,   80,
  102,   38,   80,   80,   91,   80,  110,    7,  120,   71,
   19,  108,   11,    8,   20,   80,   38,   73,   73,   73,
   70,  112,   73,   50,   70,   50,   70,  115,   73,   24,
  117,  118,   73,   73,   73,   73,   73,  121,   38,   73,
   73,   61,   96,   97,   70,   63,   94,   24,  111,  114,
   93,   63,   63,   63,   59,   60,   63,   61,   79,    0,
    0,   95,   63,   98,    0,   73,    0,   63,   63,   63,
   63,    0,    0,   63,   63,    0,    0,    7,    0,   10,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   99,    0,    0,    0,    0,    0,   63,
    0,    0,    0,    0,    0,    0,    0,    0,   73,   73,
    0,    0,    0,    0,    0,    0,    0,   21,   21,   21,
    0,    0,   21,   45,    0,    0,    0,   21,   21,    0,
    0,    0,   21,   21,   21,   21,   21,    0,    0,   21,
   21,    0,   63,   63,    0,   68,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   50,    1,   50,   50,
   50,    0,    4,   50,    0,   21,    0,   74,   50,   50,
   79,   22,   72,   50,   50,   50,   50,   50,   68,   72,
   50,   50,   64,   64,   64,   72,   78,   64,    0,    0,
    1,    0,    3,   64,   63,    4,    0,    0,   64,   64,
   64,   64,    8,   79,   64,   64,   50,   83,   21,   21,
   65,   65,   65,    0,    0,   65,   24,    0,    7,    0,
    0,   65,    5,    0,    0,    3,   65,   65,   65,   65,
   64,    0,   65,   65,   80,    8,    0,    0,    0,    0,
   83,    0,    0,   63,   63,    0,    0,    0,   63,   50,
   50,    7,    0,    0,   63,    0,    0,    0,   65,   63,
   63,   63,   63,    0,    0,   63,   63,   80,    0,    0,
   63,   63,   63,   64,   64,   63,    0,    0,    0,    0,
    0,   63,    0,    0,    0,    0,   63,   63,   63,   63,
    0,   63,   63,   63,    0,    0,    0,    0,   63,   63,
   63,   65,   65,   63,    0,    0,    0,    0,    0,   63,
    0,    0,    0,    0,   63,   63,   63,   63,   63,    0,
   63,   63,   28,   29,   30,    0,    0,   31,    0,    0,
    0,    0,    0,   32,   63,   63,    0,    0,   33,   34,
   35,   36,   44,    0,   37,   38,   63,   28,   29,    0,
    0,    0,   31,    0,    0,    0,    0,    0,   32,    0,
    0,   63,   63,   33,   34,   35,   36,    0,    0,   37,
   38,    0,    0,    0,    0,   63,   63,    0,    0,    0,
   63,    0,    0,    0,    0,    0,   63,    0,    0,   63,
   63,   63,   63,   63,   63,  113,    0,   63,   63,   28,
   29,    0,    0,    0,   31,    0,    0,    0,    0,    0,
   32,    0,    0,   39,   40,   33,   56,   57,   36,    0,
    0,   58,   38,   63,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   39,   40,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   63,   63,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   39,   40,
};
short cl_var_check[] = {                                      40,
    0,   60,   41,   44,   30,   44,  256,   42,   60,  260,
   84,  273,    6,   41,   60,    9,   44,   58,   59,   13,
  330,   41,   38,   41,   44,   41,   42,   38,   44,   41,
   41,   42,   44,   44,   62,   38,   19,   20,   41,   42,
   40,   44,  116,   38,   44,   38,   62,   42,   70,   42,
   44,   62,   46,   47,   41,  123,   42,   38,   58,   59,
   41,   42,   88,   44,  123,   41,  306,   38,   44,  306,
   41,   42,   38,   44,   61,   41,   42,   44,   44,  101,
  263,   62,  123,   38,  125,   44,   41,   42,   44,   44,
   44,   62,  263,   41,   40,   62,   44,  306,   44,   38,
   59,   44,   41,   42,   91,   44,   62,   62,   62,  306,
    9,   94,   58,   59,   13,  306,   59,  258,  259,  260,
   38,  331,  263,  123,   42,  125,   44,  306,  269,   44,
  113,  114,  273,  274,  275,  276,  277,  120,  123,  280,
  281,  306,   40,   41,   62,  306,   44,   62,  105,  107,
   78,  258,  259,  260,   34,   35,  263,   37,   68,   -1,
   -1,   59,  269,   61,   -1,  306,   -1,  274,  275,  276,
  277,   -1,   -1,  280,  281,   -1,   -1,  123,   -1,  125,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   91,   -1,   -1,   -1,   -1,   -1,  306,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  349,  350,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,  259,  260,
   -1,   -1,  263,  330,   -1,   -1,   -1,  268,  269,   -1,
   -1,   -1,  273,  274,  275,  276,  277,   -1,   -1,  280,
  281,   -1,  349,  350,   -1,  273,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  256,  273,  258,  259,
  260,   -1,  273,  263,   -1,  306,   -1,  306,  268,  269,
  273,  306,  331,  273,  274,  275,  276,  277,  306,  331,
  280,  281,  258,  259,  260,  331,  306,  263,   -1,   -1,
  306,   -1,  273,  269,  306,  306,   -1,   -1,  274,  275,
  276,  277,  273,  306,  280,  281,  306,  273,  349,  350,
  258,  259,  260,   -1,   -1,  263,  351,   -1,  273,   -1,
   -1,  269,  268,   -1,   -1,  306,  274,  275,  276,  277,
  306,   -1,  280,  281,  273,  306,   -1,   -1,   -1,   -1,
  306,   -1,   -1,  258,  259,   -1,   -1,   -1,  263,  349,
  350,  306,   -1,   -1,  269,   -1,   -1,   -1,  306,  274,
  275,  276,  277,   -1,   -1,  280,  281,  306,   -1,   -1,
  258,  259,  260,  349,  350,  263,   -1,   -1,   -1,   -1,
   -1,  269,   -1,   -1,   -1,   -1,  274,  275,  276,  277,
   -1,  306,  280,  281,   -1,   -1,   -1,   -1,  258,  259,
  260,  349,  350,  263,   -1,   -1,   -1,   -1,   -1,  269,
   -1,   -1,   -1,   -1,  274,  275,  276,  277,  306,   -1,
  280,  281,  258,  259,  260,   -1,   -1,  263,   -1,   -1,
   -1,   -1,   -1,  269,  349,  350,   -1,   -1,  274,  275,
  276,  277,  330,   -1,  280,  281,  306,  258,  259,   -1,
   -1,   -1,  263,   -1,   -1,   -1,   -1,   -1,  269,   -1,
   -1,  349,  350,  274,  275,  276,  277,   -1,   -1,  280,
  281,   -1,   -1,   -1,   -1,  258,  259,   -1,   -1,   -1,
  263,   -1,   -1,   -1,   -1,   -1,  269,   -1,   -1,  349,
  350,  274,  275,  276,  277,  306,   -1,  280,  281,  258,
  259,   -1,   -1,   -1,  263,   -1,   -1,   -1,   -1,   -1,
  269,   -1,   -1,  349,  350,  274,  275,  276,  277,   -1,
   -1,  280,  281,  306,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  349,  350,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  349,  350,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  349,  350,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 351
#if YYDEBUG
char *cl_var_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,"'&'",0,"'('","')'","'*'",0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,"':'","';'",
"'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,"LE_AUTO","LE_DOUBLE","LE_INT","LE_STRUCT","LE_BREAK",
"LE_ELSE","LE_LONG","LE_SWITCH","LE_CASE","LE_ENUM","LE_REGISTER","LE_TYPEDEF",
"LE_CHAR","LE_EXTERN","LE_RETURN","LE_UNION","LE_CONST","LE_FLOAT","LE_SHORT",
"LE_UNSIGNED","LE_BOOL","LE_CONTINUE","LE_FOR","LE_SIGNED","LE_VOID",
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
"basic_type_name_inter : LE_LONG LE_LONG",
"basic_type_name_inter : LE_INT LE_LONG",
"basic_type_name_inter : LE_TIME_T",
"basic_type_name_inter : LE_SIZE_T",
"basic_type_name : LE_UNSIGNED basic_type_name_inter",
"basic_type_name : LE_SIGNED basic_type_name_inter",
"basic_type_name : LE_SHORT basic_type_name_inter",
"basic_type_name : basic_type_name_inter",
"translation_unit :",
"translation_unit : translation_unit external_decl",
"$$1 :",
"external_decl : $$1 variables",
"external_decl : error",
"parameter_list :",
"parameter_list : template_parameter",
"parameter_list : parameter_list ',' template_parameter",
"template_parameter : const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp",
"template_parameter : const_spec nested_scope_specifier basic_type_name special_star_amp",
"template_parameter : const_spec nested_scope_specifier LE_IDENTIFIER '<' parameter_list '>' special_star_amp",
"$$2 :",
"variables : LE_TYPEDEF LE_STRUCT optional_struct_name '{' $$2 typedef_name_list ';'",
"variables : stmnt_starter variable_decl special_star_amp const_spec variable_name_list postfix",
"variables : '(' variable_decl special_star_amp const_spec LE_IDENTIFIER postfix2",
"variables : ',' variable_decl special_star_amp const_spec LE_IDENTIFIER postfix2",
"variables : '(' variable_decl special_star_amp const_spec postfix3",
"variables : ',' variable_decl special_star_amp const_spec postfix3",
"variables : ellipsis_prefix LE_ELLIPSIS ')'",
"optional_struct_name :",
"optional_struct_name : LE_IDENTIFIER",
"optional_struct_name : '*' LE_IDENTIFIER",
"optional_struct_name : \"**\" LE_IDENTIFIER",
"typedef_name_list : optional_struct_name",
"typedef_name_list : typedef_name_list ',' optional_struct_name",
"ellipsis_prefix : '('",
"ellipsis_prefix : ','",
"variable_name_list : LE_IDENTIFIER",
"variable_name_list : variable_name_list ',' special_star_amp LE_IDENTIFIER",
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
"const_spec :",
"const_spec : LE_CONST",
"const_spec : LE_CONST '*'",
"const_spec : LE_CONST '&'",
"const_spec : LE_CONST '*' '*'",
"amp_item :",
"amp_item : '&'",
"star_list :",
"star_list : star_list '*'",
"special_star_amp : star_list amp_item",
"stmnt_starter :",
"stmnt_starter : ';'",
"stmnt_starter : '{'",
"stmnt_starter : '}'",
"stmnt_starter : ':'",
"variable_decl : const_spec basic_type_name",
"variable_decl : const_spec nested_scope_specifier LE_IDENTIFIER",
"variable_decl : const_spec nested_scope_specifier LE_IDENTIFIER '<' parameter_list '>'",
"$$3 :",
"variable_decl : const_spec LE_STRUCT nested_scope_specifier LE_IDENTIFIER '{' $$3",
"variable_decl : const_spec LE_STRUCT nested_scope_specifier LE_IDENTIFIER",
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

void var_consumeDefaultValue(char c1, char c2)
{
	int depth = 0;
	bool cont(true);

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
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 12:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 13:
{ yyval = yyvsp[0]; }
break;
case 14:
{ yyval = yyvsp[0]; }
break;
case 15:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 16:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 17:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 18:
{ yyval = yyvsp[0]; }
break;
case 21:
{curr_var.Reset(); gs_names.clear(); s_tmpString.clear(); s_templateInitList.clear();}
break;
case 23:
{
                            yyclearin;    /*clear lookahead token*/
                            yyerrok;
/*                            printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_var_lval.c_str(), cl_scope_lineno);*/
                            }
break;
case 24:
{yyval = "";}
break;
case 25:
{yyval = yyvsp[0];}
break;
case 26:
{yyval = yyvsp[-2] + yyvsp[-1] + " " + yyvsp[0];}
break;
case 27:
{
                            yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
                        }
break;
case 28:
{
                            yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
                        }
break;
case 29:
{
                            yyval = yyvsp[-6] + yyvsp[-5] + yyvsp[-4] +yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0] + " " ;
                        }
break;
case 30:
{var_consumBracketsContent('{');}
break;
case 31:
{
					}
break;
case 32:
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
									if(var.m_completeType.empty())
										var.m_completeType = yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] ;
                                	var.m_name = gs_names.at(i);
                                	gs_vars->push_back(var);
                                }
                            	curr_var.Reset();
                            	gs_names.clear();
                            }
                        }
break;
case 33:
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
case 34:
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
case 35:
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
case 36:
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
case 37:
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
case 44:
{yyval = yyvsp[0];}
break;
case 45:
{yyval = yyvsp[0];}
break;
case 46:
{
                        	gs_names.push_back(yyvsp[0]);
                            yyval = yyvsp[0];
                        }
break;
case 47:
{
                            /*collect all the names*/
                        	gs_names.push_back(yyvsp[0]);
                            yyval = yyvsp[-3] + yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];
                        }
break;
case 50:
{yyval = "";}
break;
case 51:
{var_consumeDefaultValue(',', ')'); yyval = ""; }
break;
case 52:
{ yyval = ""; }
break;
case 53:
{ yyval = yyvsp[0] + var_consumBracketsContent('[');}
break;
case 57:
{ yyval = yyvsp[0] + var_consumBracketsContent('(');}
break;
case 58:
{ yyval = yyvsp[0] + var_consumBracketsContent('[');}
break;
case 59:
{
					yyval = yyvsp[-1]+ yyvsp[0]; 
				}
break;
case 60:
{
					yyval = yyvsp[-4] + yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0]; 
					s_templateInitList = yyvsp[-3] + yyvsp[-2] + yyvsp[-1];
				}
break;
case 61:
{yyval = "";}
break;
case 62:
{    yyval = yyvsp[-1] + yyvsp[0];}
break;
case 63:
{yyval = "";                          }
break;
case 64:
{ yyval = yyvsp[0];                         }
break;
case 65:
{ yyval = yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;     }
break;
case 66:
{ yyval = yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;     }
break;
case 67:
{ yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;}
break;
case 68:
{yyval = ""; }
break;
case 69:
{ yyval = yyvsp[0]; }
break;
case 70:
{yyval = ""; }
break;
case 71:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 72:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 73:
{yyval = "";}
break;
case 74:
{ yyval = ";";}
break;
case 75:
{ yyval = "{";}
break;
case 76:
{ yyval = "}";}
break;
case 77:
{ yyval = ":";}
break;
case 78:
{
                            yyval = yyvsp[-1] + " " + yyvsp[0];
                            yyvsp[0].erase(yyvsp[0].find_last_not_of(":")+1);
                        	curr_var.m_type = yyvsp[0];
							curr_var.m_isBasicType = true;
                        	curr_var.m_isConst = !yyvsp[-1].empty();
                        }
break;
case 79:
{
							yyval = yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];
                            yyvsp[-1].erase(yyvsp[-1].find_last_not_of(":")+1);
                        	curr_var.m_typeScope = yyvsp[-1];
                        	curr_var.m_type = yyvsp[0];
                        	curr_var.m_isConst = !yyvsp[-2].empty();
                        	s_tmpString.clear();
                        }
break;
case 80:
{
                            yyval = yyvsp[-5] + " " + yyvsp[-4] + yyvsp[-3] + " " + yyvsp[-2] + yyvsp[-1] + yyvsp[0];
                            yyvsp[-4].erase(yyvsp[-4].find_last_not_of(":")+1);
                        	curr_var.m_typeScope = yyvsp[-4];
                        	curr_var.m_type = yyvsp[-3];
                        	curr_var.m_isTemplate = true;
                        	curr_var.m_templateDecl = yyvsp[-2] +yyvsp[-1] +yyvsp[0];
                        	curr_var.m_isConst = !yyvsp[-5].empty();
                        	s_tmpString.clear();
                        }
break;
case 81:
{s_tmpString = var_consumBracketsContent('{');}
break;
case 82:
{
                            yyval = yyvsp[-5] + " " + yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] + yyvsp[-1] + yyvsp[0] + s_tmpString;
                            yyvsp[-3].erase(yyvsp[-3].find_last_not_of(":")+1);
                        	curr_var.m_typeScope = yyvsp[-3];
                        	curr_var.m_type = yyvsp[-2];
							curr_var.m_completeType = yyvsp[-3] + yyvsp[-2];
                        	curr_var.m_isConst = !yyvsp[-5].empty();
                        	s_tmpString.clear();
                        }
break;
case 83:
{
                            yyval = yyvsp[-4];
                            yyvsp[-1].erase(yyvsp[-1].find_last_not_of(":")+1);
                        	curr_var.m_typeScope = yyvsp[-1];
                        	curr_var.m_type = yyvsp[0];
                        	curr_var.m_isTemplate = !curr_var.m_templateDecl.empty();
							curr_var.m_completeType = yyvsp[-1] + yyvsp[0];
                        	curr_var.m_isConst = !yyvsp[-3].empty();
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
