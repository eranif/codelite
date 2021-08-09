#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define yyparse cl_func_parse
#define yylex cl_func_lex
#define yyerror cl_func_error
#define yychar cl_func_char
#define yyval cl_func_val
#define yylval cl_func_lval
#define yydebug cl_func_debug
#define yynerrs cl_func_nerrs
#define yyerrflag cl_func_errflag
#define yyss cl_func_ss
#define yyssp cl_func_ssp
#define yyvs cl_func_vs
#define yyvsp cl_func_vsp
#define yylhs cl_func_lhs
#define yylen cl_func_len
#define yydefred cl_func_defred
#define yydgoto cl_func_dgoto
#define yysindex cl_func_sindex
#define yyrindex cl_func_rindex
#define yygindex cl_func_gindex
#define yytable cl_func_table
#define yycheck cl_func_check
#define yyname cl_func_name
#define yyrule cl_func_rule
#define YYPREFIX "cl_func_"
/* Copyright Eran Ifrah(c)*/
/*************** Includes and Defines *****************************/
#include "string"
#include "vector"
#include "stdio.h"
#include "map"
#include "function.h"

#define YYDEBUG_LEXER_TEXT (cl_func_lval)
#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

#ifdef yylex
#undef yylex
#define yylex cl_scope_lex
#endif

int cl_func_parse();
void cl_func_error(char *string);

static FunctionList *g_funcs = NULL;
static clFunction curr_func;

/*---------------------------------------------*/
/* externs defined in the lexer*/
/*---------------------------------------------*/
extern char *cl_func_text;
extern int cl_scope_lex();
extern bool setLexerInput(const std::string &in, const std::map<std::string, std::string> &ignoreTokens);
extern int cl_scope_lineno;
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
#define LE_OVERRIDE 302
#define LE_FINAL 303
#define LE_TEMPLATE 304
#define LE_TYPENAME 305
#define LE_THROW 306
#define LE_CATCH 307
#define LE_IDENTIFIER 308
#define LE_STRINGliteral 309
#define LE_FLOATINGconstant 310
#define LE_INTEGERconstant 311
#define LE_CHARACTERconstant 312
#define LE_OCTALconstant 313
#define LE_HEXconstant 314
#define LE_POUNDPOUND 315
#define LE_CComment 316
#define LE_CPPComment 317
#define LE_NAMESPACE 318
#define LE_USING 319
#define LE_TYPEDEFname 320
#define LE_ARROW 321
#define LE_ICR 322
#define LE_DECR 323
#define LE_LS 324
#define LE_RS 325
#define LE_LE 326
#define LE_GE 327
#define LE_EQ 328
#define LE_NE 329
#define LE_ANDAND 330
#define LE_OROR 331
#define LE_ELLIPSIS 332
#define LE_CLCL 333
#define LE_DOTstar 334
#define LE_ARROWstar 335
#define LE_MULTassign 336
#define LE_DIVassign 337
#define LE_MODassign 338
#define LE_PLUSassign 339
#define LE_MINUSassign 340
#define LE_LSassign 341
#define LE_RSassign 342
#define LE_ANDassign 343
#define LE_ERassign 344
#define LE_ORassign 345
#define LE_MACRO 346
#define LE_DYNAMIC_CAST 347
#define LE_STATIC_CAST 348
#define LE_CONST_CAST 349
#define LE_REINTERPRET_CAST 350
#define LE_SIZE_T 351
#define LE_TIME_T 352
#define YYERRCODE 256
short cl_func_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    2,    2,    2,    2,    2,    0,    0,    5,
    3,    3,    6,    6,    8,    8,    7,    7,    9,    9,
   10,   10,   10,   11,   11,   11,   15,   15,   15,   16,
   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,
   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,
   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,
   20,    4,   21,   21,   23,   23,   23,   24,   24,   13,
   13,   22,   22,   25,   26,   25,   18,   18,   12,   12,
   27,   27,   28,   28,   14,   17,   17,   17,   19,   19,
   19,
};
short cl_func_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    2,    2,    2,    2,    1,    0,    2,    0,
    2,    1,    0,    2,    1,    3,    1,    1,    0,    4,
    0,    1,    3,    4,    4,    7,    1,    2,    2,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    2,    2,    1,    1,    1,
    0,   15,    0,    4,    0,    1,    1,    1,    1,    0,
    2,    0,    2,    2,    0,    4,    0,    1,    0,    1,
    0,    1,    0,    2,    2,    0,    1,    1,    3,    3,
    6,
};
short cl_func_defred[] = {                                     18,
    0,   22,   19,    0,   97,   98,   21,    0,    0,    0,
    0,   88,    0,   27,   28,   25,    0,    0,   90,   80,
   24,    0,   30,    0,    0,   26,    6,    1,    0,    2,
    5,    3,    0,   10,    0,    9,    0,   12,   11,   17,
   93,   81,   80,   16,   15,    4,    8,    7,   13,   14,
   84,    0,  100,    0,   99,    0,    0,   32,   80,    0,
   94,   92,   95,    0,    0,    0,    0,    0,   93,    0,
   86,   68,   69,   56,   60,   61,   52,   53,   62,   63,
   64,   65,   54,   55,   59,   57,   70,   50,   51,   48,
   40,   41,   42,   43,   44,   45,   46,   47,   49,   58,
    0,    0,   39,   85,   38,   71,   33,  101,    0,   93,
   66,   67,    0,    0,   34,   35,    0,    0,    0,    0,
   93,    0,    0,    0,   36,    0,   83,   76,   77,    0,
   74,   78,   79,   72,
};
short cl_func_dgoto[] = {                                       1,
   40,   41,    3,    7,    4,   16,   17,   18,   10,   57,
   58,   59,   24,   53,   67,  103,    8,   13,   25,  113,
  120,  124,  130,  134,   42,   60,   63,   54,
};
short cl_func_sindex[] = {                                      0,
 -247,    0,    0,   22,    0,    0,    0, -267,    7, -223,
 -229,    0, -185,    0,    0,    0, -212,  -26,    0,    0,
    0, -229,    0,   84, -185,    0,    0,    0, -181,    0,
    0,    0,  142,    0,  142,    0,  -59,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0, -185,    0,   51,    0, -121,  -11,    0,    0, -235,
    0,    0,    0,    1,  -58, -203,   67, -185,    0,  113,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   68,   17,    0,    0,    0,    0,    0,    0,  -56,    0,
    0,    0, -185, -185,    0,    0, -191,    8,   81,   62,
    0, -185, -189, -200,    0,   53,    0,    0,    0,  -52,
    0,    0,    0,    0,
};
short cl_func_rindex[] = {                                      0,
   75,    0,    0, -190,    0,    0,    0, -128,    0,  -67,
   12,    0,  122,    0,    0,    0,    0,    0,    0,    0,
    0,   12,    0,    0, -120,    0,    0,    0,  -27,    0,
    0,    0,  -22,    0,  -13,    0,  -15,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  -32,    0,  -20,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   88,    0,    0,  122,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   78,    0,
    0,    0,   -6,  -32,    0,    0,    3,    0,    0,  -46,
    0,  -41,    0,  -51,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,
};
short cl_func_gindex[] = {                                      0,
   24,   59,    0,    0,    0,  110,    0,    0,    0,  -64,
   69,   41,  -33,   -9,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 494
short cl_func_table[] = {                                      31,
   52,  104,   31,  114,   66,   89,  133,   75,    2,   56,
    4,   31,   82,    4,    4,    8,    4,   22,    8,    8,
   91,    8,   93,   91,    7,   70,   93,    7,    7,   31,
    7,   55,   68,   99,    4,   23,    9,   95,   97,    8,
  101,   91,   93,   91,   87,   92,  100,   94,    7,  118,
   69,   68,   89,   20,   89,   23,   49,  126,   50,  108,
   88,   73,   89,   73,   14,   43,   11,   96,   96,  121,
  132,   75,   96,   23,   12,   15,   82,   44,   96,    6,
    5,   45,   96,   96,   96,   96,   96,   19,   62,   96,
   96,  102,   61,  131,   96,   21,   68,   71,    4,  115,
  116,  128,  129,    8,  105,   91,  106,   96,  111,  112,
   93,  125,    7,   96,  119,   93,   89,   96,   93,   93,
  122,   93,  123,  127,   98,   73,   90,   37,  110,   29,
   29,   26,   20,   20,   29,    0,  107,    0,    0,   93,
   29,    0,    0,    0,   29,   29,   29,   29,   29,    0,
    0,   29,   29,  117,    0,    0,    0,    0,    0,    0,
   96,   96,    0,    0,    0,    0,    0,    0,    0,   29,
    0,   64,   89,    0,    0,    0,    0,    0,    0,   29,
    0,    0,    0,    0,    0,    0,   65,   89,    0,    0,
   87,   87,    0,    0,    0,   87,    0,    0,    0,    0,
    0,   87,    0,    0,    0,   87,   87,   87,   87,   87,
    0,    0,   87,   87,    0,    0,   89,   89,    0,    0,
    0,   89,   29,   29,    0,   89,   89,   89,    0,    0,
   89,    0,   89,   89,   89,   89,   89,    0,   89,   89,
   87,   89,   89,   89,   89,    4,    0,   89,   89,    0,
    8,    0,   91,    0,    0,   82,   82,   93,    0,    7,
    0,    0,    0,    0,    0,    4,   89,    0,    0,    0,
    8,    0,   91,   51,   51,   89,   51,   93,    0,    7,
    4,    0,    0,   87,   87,    8,    0,   91,    0,    0,
   72,   73,   93,    0,    7,   89,   89,    0,    0,   89,
   85,    0,    0,    0,   73,   73,    0,    0,    0,   89,
   89,    0,    0,    0,    0,    0,    0,    0,   89,   89,
    0,   74,   75,   76,   77,   78,   79,   80,   81,   82,
   83,   84,   20,   20,   85,   86,    0,   20,    0,    0,
    0,   27,   28,   20,    0,    0,   29,   20,   20,   20,
   20,   20,   30,    0,   20,   20,    0,   31,   32,   33,
   34,    0,    0,   35,   36,    0,    0,    0,    0,    0,
   27,   28,   20,    0,    0,   29,    0,    0,   20,   89,
   89,   30,   20,    0,   89,    0,   31,   32,   33,   34,
   89,   37,   35,   36,    0,   89,   89,   89,   89,   27,
   28,   89,   89,    0,   46,    0,    0,    0,    0,    0,
   30,    0,    0,    0,    0,   31,   32,   47,   34,    0,
  109,   48,   36,    0,    0,   20,   20,    0,    0,   89,
    0,    0,    0,    0,   38,   39,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   38,   39,    0,    0,    0,    0,    0,
    0,    0,   89,   89,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   38,   39,
};
short cl_func_check[] = {                                      41,
   60,   60,   44,   60,  126,  126,   59,   59,  256,   43,
   38,   44,   59,   41,   42,   38,   44,   44,   41,   42,
   41,   44,   38,   44,   38,   59,   42,   41,   42,   62,
   44,   41,   44,   33,   62,   62,  304,   37,   38,   62,
   40,   62,   42,   43,   44,   45,   46,   47,   62,  114,
   62,   44,   59,   13,   61,   44,   33,  122,   35,   69,
   60,   59,   62,   61,  294,   25,   60,  258,  259,   62,
  123,  123,  263,   62,  298,  305,  123,  259,  269,   58,
   59,  263,  273,  274,  275,  276,  277,  273,   38,  280,
  281,   91,   42,   41,   94,  308,   44,  333,  126,  109,
  110,  302,  303,  126,  308,  126,   40,  298,   41,   93,
  126,  121,  126,  304,  306,   38,  123,  308,   41,   42,
   40,   44,   61,  313,  124,  123,  126,   40,   70,  258,
  259,   22,   58,   59,  263,   -1,   68,   -1,   -1,   62,
  269,   -1,   -1,   -1,  273,  274,  275,  276,  277,   -1,
   -1,  280,  281,  113,   -1,   -1,   -1,   -1,   -1,   -1,
  351,  352,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  298,
   -1,  293,  293,   -1,   -1,   -1,   -1,   -1,   -1,  308,
   -1,   -1,   -1,   -1,   -1,   -1,  308,  308,   -1,   -1,
  258,  259,   -1,   -1,   -1,  263,   -1,   -1,   -1,   -1,
   -1,  269,   -1,   -1,   -1,  273,  274,  275,  276,  277,
   -1,   -1,  280,  281,   -1,   -1,  258,  259,   -1,   -1,
   -1,  263,  351,  352,   -1,  258,  259,  269,   -1,   -1,
  263,   -1,  274,  275,  276,  277,  269,   -1,  280,  281,
  308,  274,  275,  276,  277,  273,   -1,  280,  281,   -1,
  273,   -1,  273,   -1,   -1,  302,  303,  273,   -1,  273,
   -1,   -1,   -1,   -1,   -1,  293,  308,   -1,   -1,   -1,
  293,   -1,  293,  333,  333,  308,  333,  293,   -1,  293,
  308,   -1,   -1,  351,  352,  308,   -1,  308,   -1,   -1,
  290,  291,  308,   -1,  308,  302,  303,   -1,   -1,  306,
  333,   -1,   -1,   -1,  302,  303,   -1,   -1,   -1,  351,
  352,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  351,  352,
   -1,  321,  322,  323,  324,  325,  326,  327,  328,  329,
  330,  331,  258,  259,  334,  335,   -1,  263,   -1,   -1,
   -1,  258,  259,  269,   -1,   -1,  263,  273,  274,  275,
  276,  277,  269,   -1,  280,  281,   -1,  274,  275,  276,
  277,   -1,   -1,  280,  281,   -1,   -1,   -1,   -1,   -1,
  258,  259,  298,   -1,   -1,  263,   -1,   -1,  304,  258,
  259,  269,  308,   -1,  263,   -1,  274,  275,  276,  277,
  269,  308,  280,  281,   -1,  274,  275,  276,  277,  258,
  259,  280,  281,   -1,  263,   -1,   -1,   -1,   -1,   -1,
  269,   -1,   -1,   -1,   -1,  274,  275,  276,  277,   -1,
  308,  280,  281,   -1,   -1,  351,  352,   -1,   -1,  308,
   -1,   -1,   -1,   -1,  351,  352,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  351,  352,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  351,  352,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  351,  352,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 352
#if YYDEBUG
char *cl_func_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,0,0,"'%'","'&'",0,"'('","')'","'*'","'+'","','","'-'","'.'","'/'",0,0,0,
0,0,0,0,0,0,0,"':'","';'","'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,"'['",0,"']'","'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,"'{'","'|'",0,"'~'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"LE_AUTO","LE_DOUBLE",
"LE_INT","LE_STRUCT","LE_BREAK","LE_ELSE","LE_LONG","LE_SWITCH","LE_CASE",
"LE_ENUM","LE_REGISTER","LE_TYPEDEF","LE_CHAR","LE_EXTERN","LE_RETURN",
"LE_UNION","LE_CONST","LE_FLOAT","LE_SHORT","LE_UNSIGNED","LE_BOOL",
"LE_CONTINUE","LE_FOR","LE_SIGNED","LE_VOID","LE_DEFAULT","LE_GOTO","LE_SIZEOF",
"LE_VOLATILE","LE_DO","LE_IF","LE_STATIC","LE_WHILE","LE_NEW","LE_DELETE",
"LE_THIS","LE_OPERATOR","LE_CLASS","LE_PUBLIC","LE_PROTECTED","LE_PRIVATE",
"LE_VIRTUAL","LE_FRIEND","LE_INLINE","LE_OVERLOAD","LE_OVERRIDE","LE_FINAL",
"LE_TEMPLATE","LE_TYPENAME","LE_THROW","LE_CATCH","LE_IDENTIFIER",
"LE_STRINGliteral","LE_FLOATINGconstant","LE_INTEGERconstant",
"LE_CHARACTERconstant","LE_OCTALconstant","LE_HEXconstant","LE_POUNDPOUND",
"LE_CComment","LE_CPPComment","LE_NAMESPACE","LE_USING","LE_TYPEDEFname",
"LE_ARROW","LE_ICR","LE_DECR","LE_LS","LE_RS","LE_LE","LE_GE","LE_EQ","LE_NE",
"LE_ANDAND","LE_OROR","LE_ELLIPSIS","LE_CLCL","LE_DOTstar","LE_ARROWstar",
"LE_MULTassign","LE_DIVassign","LE_MODassign","LE_PLUSassign","LE_MINUSassign",
"LE_LSassign","LE_RSassign","LE_ANDassign","LE_ERassign","LE_ORassign",
"LE_MACRO","LE_DYNAMIC_CAST","LE_STATIC_CAST","LE_CONST_CAST",
"LE_REINTERPRET_CAST","LE_SIZE_T","LE_TIME_T",
};
char *cl_func_rule[] = {
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
"basic_type_name : LE_UNSIGNED basic_type_name_inter",
"basic_type_name : LE_SIGNED basic_type_name_inter",
"basic_type_name : LE_LONG LE_LONG",
"basic_type_name : LE_LONG LE_INT",
"basic_type_name : basic_type_name_inter",
"translation_unit :",
"translation_unit : translation_unit external_decl",
"$$1 :",
"external_decl : $$1 function_decl",
"external_decl : error",
"template_arg :",
"template_arg : template_specifiter LE_IDENTIFIER",
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
"func_name : LE_IDENTIFIER",
"func_name : '~' LE_IDENTIFIER",
"func_name : LE_OPERATOR any_operator",
"any_operator : '+'",
"any_operator : '-'",
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
"$$2 :",
"function_decl : stmnt_starter opt_template_qualifier virtual_spec const_spec variable_decl const_spec nested_scope_specifier func_name '(' $$2 const_spec declare_throw opt_pure_virtual override_final_specifier func_postfix",
"declare_throw :",
"declare_throw : LE_THROW '(' template_parameter_list ')'",
"override_final_specifier :",
"override_final_specifier : LE_OVERRIDE",
"override_final_specifier : LE_FINAL",
"func_postfix : '{'",
"func_postfix : ';'",
"nested_scope_specifier :",
"nested_scope_specifier : nested_scope_specifier scope_specifier",
"opt_pure_virtual :",
"opt_pure_virtual : '=' LE_OCTALconstant",
"scope_specifier : LE_IDENTIFIER LE_CLCL",
"$$3 :",
"scope_specifier : LE_IDENTIFIER '<' $$3 LE_CLCL",
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

void func_consumeFuncArgList()
{
	curr_func.m_signature = "(";

	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_scope_lex();
		if(ch == 0)
		{
			break;
		}

		curr_func.m_signature += cl_func_lval;
		curr_func.m_signature += " ";
		if(ch == ')')
		{
			depth--;
			continue;
		}
		else if(ch == '(')
		{
			depth ++ ;
			continue;
		}
	}
}

/**
 * consume all token until matching closing brace is found
 */
void func_consumeDecl()
{
	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_scope_lex();
		//printf("ch=%d\n", ch);
		//fflush(stdout);
		if(ch ==0)
		{
			break;
		}
		if(ch == '}')
		{
			depth--;
			continue;
		}
		else if(ch == '{')
		{
			depth ++ ;
			continue;
		}
	}

}

void func_consumeTemplateDecl()
{
	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_scope_lex();
		//printf("ch=%d\n", ch);
		//fflush(stdout);
		if(ch ==0){
			break;
		}

		if(ch == '>')
		{
			depth--;
			continue;
		}
		else if(ch == '<')
		{
			depth ++ ;
			continue;
		}
	}
}

// return the scope name at the end of the input string
void get_functions(const std::string &in, FunctionList &li, const std::map<std::string, std::string> &ignoreTokens)
{
	if( !setLexerInput(in, ignoreTokens) )
	{
		return;
	}

	g_funcs = &li;

	//call tghe main parsing routine
	cl_func_parse();
	g_funcs = NULL;

	//do the lexer cleanup
	cl_scope_lex_clean();
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
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 14:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 15:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 16:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 17:
{ yyval = yyvsp[0];            }
break;
case 20:
{curr_func.Reset();}
break;
case 22:
{
							/*printf("CodeLite: syntax error, unexpected token '%s' found\n", cl_func_lval.c_str());*/
						}
break;
case 23:
{ yyval = "";}
break;
case 24:
{yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 25:
{ yyval = yyvsp[0]; }
break;
case 26:
{ yyval = yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0]; }
break;
case 27:
{ yyval = yyvsp[0]; }
break;
case 28:
{ yyval = yyvsp[0]; }
break;
case 30:
{ yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 31:
{yyval = "";}
break;
case 32:
{yyval = yyvsp[0];}
break;
case 33:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 34:
{
							yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
						}
break;
case 35:
{
							yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
						}
break;
case 36:
{
							yyval = yyvsp[-6] + yyvsp[-5] + yyvsp[-4] +yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0] + " " ;
						}
break;
case 37:
{yyval = yyvsp[0];}
break;
case 38:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 39:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 71:
{func_consumeFuncArgList();}
break;
case 72:
{
						/*trim down trailing '::' from scope name*/
						yyvsp[-9].erase(yyvsp[-9].find_last_not_of(":")+1);
						curr_func.m_isVirtual = yyvsp[-12].find("virtual") != std::string::npos;
						curr_func.m_isPureVirtual = yyvsp[-2].find("=") != std::string::npos;
						curr_func.m_isConst = yyvsp[-4].find("const") != std::string::npos;
						curr_func.m_isFinal = yyvsp[-1].find("final") != std::string::npos;
						curr_func.m_name = yyvsp[-7];
						curr_func.m_scope = yyvsp[-8];
						curr_func.m_retrunValusConst = yyvsp[-11];
						curr_func.m_lineno = cl_scope_lineno;
						curr_func.m_throws = yyvsp[-3];
                        curr_func.m_returnValue.m_rightSideConst = yyvsp[-9];
                        curr_func.m_returnValue.m_isConst = !yyvsp[-11].empty();
						if(g_funcs)
						{
							g_funcs->push_back(curr_func);
						}
						curr_func.Reset();
					}
break;
case 73:
{yyval = "";}
break;
case 74:
{yyval = yyvsp[-1];}
break;
case 75:
{yyval = "";}
break;
case 80:
{yyval = "";}
break;
case 81:
{	yyval = yyvsp[-1] + yyvsp[0];}
break;
case 82:
{yyval = "";}
break;
case 83:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 84:
{yyval = yyvsp[-1]+ yyvsp[0];}
break;
case 85:
{func_consumeTemplateDecl();}
break;
case 86:
{yyval = yyvsp[-3] + yyvsp[0];}
break;
case 87:
{yyval = ""; }
break;
case 88:
{ yyval = yyvsp[0]; }
break;
case 89:
{yyval = ""; }
break;
case 90:
{ yyval = yyvsp[0]; }
break;
case 91:
{yyval = ""; }
break;
case 92:
{ yyval = yyvsp[0]; }
break;
case 93:
{yyval = ""; }
break;
case 94:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 95:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 96:
{yyval = "";}
break;
case 97:
{ yyval = ";";}
break;
case 98:
{ yyval = ":";}
break;
case 99:
{
								yyvsp[-2].erase(yyvsp[-2].find_last_not_of(":")+1);
								curr_func.m_returnValue.m_type = yyvsp[-1];
								curr_func.m_returnValue.m_typeScope = yyvsp[-2];
								curr_func.m_returnValue.m_starAmp = yyvsp[0];
								curr_func.m_returnValue.m_isPtr = (yyvsp[0].find("*") != (size_t)-1);
								yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];
							}
break;
case 100:
{
								yyvsp[-2].erase(yyvsp[-2].find_last_not_of(":")+1);
								curr_func.m_returnValue.m_type = yyvsp[-1];
								curr_func.m_returnValue.m_typeScope = yyvsp[-2];
								curr_func.m_returnValue.m_starAmp = yyvsp[0];
								curr_func.m_returnValue.m_isPtr = (yyvsp[0].find("*") != (size_t)-1);
								yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;
							}
break;
case 101:
{
								yyvsp[-5].erase(yyvsp[-5].find_last_not_of(":")+1);
								curr_func.m_returnValue.m_type = yyvsp[-4];
								curr_func.m_returnValue.m_typeScope = yyvsp[-5];
								curr_func.m_returnValue.m_starAmp = yyvsp[0];
								curr_func.m_returnValue.m_isPtr = (yyvsp[0].find("*") != (size_t)-1);
								curr_func.m_returnValue.m_isTemplate = true;
								curr_func.m_returnValue.m_templateDecl = yyvsp[-2];
								yyval = yyvsp[-5] + yyvsp[-4] + yyvsp[-3]  + yyvsp[-2] + yyvsp[-1] + yyvsp[0] ;
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
