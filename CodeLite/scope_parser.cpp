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
#include "string"
#include "vector"
#include "stdio.h"
#include "map"

#define YYDEBUG_LEXER_TEXT (cl_scope_lval)
#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

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
#define YYERRCODE 256
short cl_scope_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    2,    2,    2,    2,    2,    0,    0,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,   13,   13,   15,
   15,   14,   14,   16,   16,   17,   17,   17,   18,   20,
   20,   21,   21,   19,   19,   19,   19,   22,   22,   22,
   23,   23,   23,    9,    8,    8,   28,   28,    4,    4,
   10,   11,   12,   29,   29,   30,   30,   31,   31,   31,
   31,   31,   31,   31,   31,   31,   31,   31,   31,   31,
   31,   31,   31,   31,   31,   31,   31,   31,   31,   31,
   31,   31,   31,   31,   31,   31,   31,   31,   31,   31,
   31,   31,   31,   31,   31,   31,   31,   31,   32,   32,
   33,   33,   36,    7,   37,    7,   38,    7,   25,   25,
   39,   40,   39,   34,   34,   24,   24,   41,   41,   42,
   42,   26,   27,   27,   27,   35,   35,   35,   44,    5,
   45,   45,   45,   45,   46,   43,   43,   43,    6,
};
short cl_scope_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    2,    2,    2,    2,    1,    0,    2,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    0,    2,    1,
    3,    1,    1,    0,    4,    0,    1,    3,    3,    1,
    3,    0,    3,    0,    1,    1,    1,    0,    1,    3,
    4,    4,    7,    5,    4,    3,    0,    1,    6,    8,
    1,    1,    1,    1,    1,    1,    2,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    2,    2,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    0,    4,    0,   12,    0,    9,    0,   11,    0,    2,
    2,    0,    4,    0,    1,    0,    1,    0,    1,    0,
    2,    2,    0,    1,    1,    3,    3,    6,    0,    7,
    0,    2,    2,    2,    2,    0,    1,    3,    4,
};
short cl_scope_defred[] = {                                     16,
    0,   27,    0,  134,   62,  135,   61,   63,   17,   18,
   19,   20,   21,   22,   23,   24,   25,   26,    0,  119,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   56,   65,   64,  125,    0,    0,    0,  120,  139,  149,
   32,   33,   30,    0,    0,   55,   58,    0,  127,  119,
  121,  122,   54,    0,   29,    0,   35,    0,    0,  119,
    0,    0,    0,  147,   31,   59,    0,    6,    1,    0,
    2,    5,    3,    0,   10,    0,    9,    0,    0,    0,
   15,  130,    0,    0,  123,    0,  145,    0,  140,   45,
   47,   46,    0,   37,    0,   14,   13,    4,    8,    7,
   11,   12,   96,   97,   84,   88,   89,   80,   81,   90,
   91,   92,   93,   82,   83,   87,   85,   99,  100,  101,
  102,  103,  104,  105,  106,  107,  108,   98,   78,   79,
   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,
   86,    0,    0,   67,    0,  137,    0,   66,    0,  136,
  115,    0,    0,  144,  143,  142,  148,    0,   60,   40,
    0,   94,   95,    0,   49,  119,  131,  129,  132,  117,
    0,  113,   38,    0,    0,   39,    0,  130,    0,    0,
  109,  110,  116,    0,   41,    0,   50,  138,    0,  130,
    0,    0,   43,    0,   51,   52,  118,    0,    0,    0,
    0,  114,  130,    0,   53,  112,
};
short cl_scope_dgoto[] = {                                       1,
   81,   82,    9,   10,   11,   12,   13,   14,   15,   16,
   17,   18,   43,   44,   45,   25,   93,   94,   95,  161,
  176,  164,  165,  166,   26,  146,   19,   48,   35,   83,
  144,  183,  199,   36,   60,  184,  171,  180,   38,   61,
  169,  147,   63,   54,   87,   64,
};
short cl_scope_sindex[] = {                                      0,
  -37,    0, -238,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -201,    0,
 -216, -209,   44, -105, -221, -189,    1,   10, -180,   11,
    0,    0,    0,    0, -212, -138,  -58,    0,    0,    0,
    0,    0,    0, -170,    2,    0,    0, -169,    0,    0,
    0,    0,    0, -168,    0, -180,    0,  -27,  102,    0,
 -190,   81,  -20,    0,    0,    0, -166,    0,    0, -214,
    0,    0,    0,  182,    0,  182,    0,  -33,  -57, -249,
    0,    0,  103, -193,    0, -200,    0, -168,    0,    0,
    0,    0,   -3,    0, -161,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  105,   57,    0, -138,    0,    9,    0,  111,    0,
    0,  -54,  114,    0,    0,    0,    0, -166,    0,    0,
  -52,    0,    0,   32,    0,    0,    0,    0,    0,    0,
  -15,    0,    0, -149, -138,    0, -138,    0,  142, -138,
    0,    0,    0, -138,    0,   45,    0,    0,  -48,    0,
   36, -144,    0, -138,    0,    0,    0,  128,   49,   48,
 -138,    0,    0,   78,    0,    0,
};
short cl_scope_rindex[] = {                                      0,
 -111,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  -51,    0,
    0,    0,    0,    0,  -42,    0,    0,    0,   54,    0,
    0,    0,    0,    0, -133,  122,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -19,    0,   54,    0,    0,    0,    0,
    0,   -7,    0,    0,    0,    0,  -28,    0,    0,   -8,
    0,    0,    0,   18,    0,   25,    0,    0,   30,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   -9,    0,  -24,    0,    0,    0,
    0,  134,    0,    0,    0,    0,    0, -131,    0,    0,
   -2,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   82,    0,  162,    0,    0,   53,
    0,    0,    0, -104,    0,    0,    0,    0,   41,    0,
    0,   55,    0,   -9,    0,    0,    0,    0,    0,    0,
   58,    0,    0,    0,    0,    0,
};
short cl_scope_gindex[] = {                                      0,
   51,    5,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  121,    0,    0,    0,    0,   21,    0,    0,
    0, -120,    3,  -13,  -10,  -50,    0,    0,    0,  -32,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   93,
};
#define YYTABLESIZE 468
short cl_scope_table[] = {                                     140,
   53,   52,  145,  135,  137,   52,  142,  175,  133,  131,
  128,  194,  141,  134,  133,   36,  128,   31,  126,  128,
    6,    4,   50,   88,  146,    8,  129,  132,  130,    4,
   67,  150,    4,    4,   48,    4,  141,  128,   32,   59,
  158,   42,  182,   78,   96,   56,  168,  149,   97,   84,
  167,  153,   48,    4,  186,    8,  148,  143,    8,    8,
  136,    8,    7,   57,   21,    7,    7,  130,    7,   66,
   22,  130,   33,  200,   34,  177,   34,   20,  130,    8,
  204,  130,  130,  124,  130,    5,    7,    7,  177,   27,
  138,  177,  139,  178,   36,   66,   28,   28,   48,   78,
   23,   48,  130,   29,   89,  146,  193,  181,  154,  203,
  155,  156,  152,   41,   24,   28,   37,  141,  206,  159,
   42,  177,   42,   39,  101,   48,  102,  188,   90,   91,
   92,   47,   40,   46,   49,   55,   58,   62,  195,  196,
   85,   86,  151,   48,  160,  162,  133,  133,  133,  163,
  170,  133,  205,  172,  133,  179,  185,  133,  197,  198,
  133,  133,  133,  133,  133,  133,  191,  201,  133,  133,
  192,  202,   57,   66,   44,  126,   65,  111,  173,  187,
  157,  133,  133,  190,    0,    0,  133,    0,    0,    0,
  133,    0,    0,    0,  133,    0,    0,    0,    0,  126,
   30,    0,    0,    0,  133,    0,   34,   34,   34,    0,
    0,   34,    0,    0,    0,  124,  124,   34,    2,    0,
  124,   34,   34,   34,   34,   34,  124,   80,   34,   34,
  124,  124,  124,  124,  124,    0,    0,  124,  124,    0,
    0,   34,   34,    0,    0,    0,   34,  126,  126,  126,
  124,    0,    0,  126,   34,    0,  103,  104,    0,  126,
    0,    0,    0,  124,  126,  126,  126,  126,  128,    0,
  126,  126,   51,   51,    0,    0,   51,   44,  174,    3,
    0,  128,   51,    0,    4,  105,  106,  107,  108,  109,
  110,  111,  112,  113,  114,  115,  126,    4,  116,  117,
  118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
    8,    0,    0,    0,    0,  126,  126,    7,    0,    0,
  126,  122,  130,    8,    0,    0,  126,    0,    0,    0,
    7,  126,  126,  126,  126,  130,    0,  126,  126,  126,
  126,    0,    0,    0,  126,    0,    0,    0,    0,    0,
  126,    0,    0,    0,    0,  126,  126,  126,  126,   68,
   69,  126,  126,  126,   70,    0,    0,    0,    0,    0,
   71,    0,    0,    0,    0,   72,   73,   74,   75,  126,
  126,   76,   77,    0,  126,    0,    0,  126,    0,    0,
  126,    0,    0,    0,   78,  126,  126,  126,  126,   68,
   69,  126,  126,    0,   70,    0,    0,   79,    0,    0,
   71,    0,    0,    0,  126,   72,   73,   74,   75,  126,
  126,   76,   77,    0,  126,    0,    0,  126,    0,    0,
  126,    0,    0,    0,    0,  126,  126,  126,  126,   68,
   69,  126,  126,    0,   98,    0,    0,  189,    0,    0,
   71,    0,    0,    0,    0,   72,   73,   99,   75,    0,
    0,  100,   77,    0,    0,    0,    0,  126,
};
short cl_scope_check[] = {                                      33,
   59,   60,   60,   37,   38,   60,   40,   60,   42,   43,
   44,   60,   46,   47,  126,   44,   41,  123,  123,   44,
   58,   59,   36,   44,   44,   63,   60,   61,   62,   38,
   58,   82,   41,   42,   44,   44,   44,   62,  260,   50,
   44,   44,   58,  293,  259,   44,   38,   80,  263,   60,
   42,   84,   62,   62,  175,   38,  306,   91,   41,   42,
   94,   44,   38,   62,  266,   41,   42,   38,   44,   40,
  272,   42,  294,  194,  126,   44,  298,  316,   38,   62,
  201,   41,   42,  126,   44,  123,   62,  125,   44,  306,
  124,   44,  126,   62,  123,  123,  306,   44,   41,  293,
  302,   44,   62,   60,  125,  125,   62,  123,  309,   62,
  311,  312,  306,  294,  316,   62,  306,  125,   41,  123,
  123,   44,  303,  123,   74,   44,   76,  178,  295,  296,
  297,  344,  123,  123,  273,  306,  306,  306,  189,  190,
  331,   61,   40,   62,  306,   41,  258,  259,  260,   93,
   40,  263,  203,   40,  266,  166,  306,  269,  123,  304,
  272,  273,  274,  275,  276,  277,  180,   40,  280,  281,
  184,  123,  306,   40,  306,  123,   56,  123,  158,  177,
   88,  293,  294,  179,   -1,   -1,  298,   -1,   -1,   -1,
  302,   -1,   -1,   -1,  306,   -1,   -1,   -1,   -1,  304,
  306,   -1,   -1,   -1,  316,   -1,  258,  259,  260,   -1,
   -1,  263,   -1,   -1,   -1,  258,  259,  269,  256,   -1,
  263,  273,  274,  275,  276,  277,  269,  126,  280,  281,
  273,  274,  275,  276,  277,   -1,   -1,  280,  281,   -1,
   -1,  293,  294,   -1,   -1,   -1,  298,  126,  258,  259,
  293,   -1,   -1,  263,  306,   -1,  290,  291,   -1,  269,
   -1,   -1,   -1,  306,  274,  275,  276,  277,  293,   -1,
  280,  281,  331,  331,   -1,   -1,  331,  306,  331,  317,
   -1,  306,  331,   -1,  293,  319,  320,  321,  322,  323,
  324,  325,  326,  327,  328,  329,  306,  306,  332,  333,
  334,  335,  336,  337,  338,  339,  340,  341,  342,  343,
  293,   -1,   -1,   -1,   -1,  258,  259,  293,   -1,   -1,
  263,  331,  293,  306,   -1,   -1,  269,   -1,   -1,   -1,
  306,  274,  275,  276,  277,  306,   -1,  280,  281,  258,
  259,   -1,   -1,   -1,  263,   -1,   -1,   -1,   -1,   -1,
  269,   -1,   -1,   -1,   -1,  274,  275,  276,  277,  258,
  259,  280,  281,  306,  263,   -1,   -1,   -1,   -1,   -1,
  269,   -1,   -1,   -1,   -1,  274,  275,  276,  277,  258,
  259,  280,  281,   -1,  263,   -1,   -1,  306,   -1,   -1,
  269,   -1,   -1,   -1,  293,  274,  275,  276,  277,  258,
  259,  280,  281,   -1,  263,   -1,   -1,  306,   -1,   -1,
  269,   -1,   -1,   -1,  293,  274,  275,  276,  277,  258,
  259,  280,  281,   -1,  263,   -1,   -1,  306,   -1,   -1,
  269,   -1,   -1,   -1,   -1,  274,  275,  276,  277,  258,
  259,  280,  281,   -1,  263,   -1,   -1,  306,   -1,   -1,
  269,   -1,   -1,   -1,   -1,  274,  275,  276,  277,   -1,
   -1,  280,  281,   -1,   -1,   -1,   -1,  306,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 348
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
"LE_REINTERPRET_CAST",
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
"basic_type_name : LE_UNSIGNED basic_type_name_inter",
"basic_type_name : LE_SIGNED basic_type_name_inter",
"basic_type_name : LE_LONG LE_LONG",
"basic_type_name : LE_LONG LE_INT",
"basic_type_name : basic_type_name_inter",
"translation_unit :",
"translation_unit : translation_unit external_decl",
"external_decl : class_decl",
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
"template_arg_list : template_arg",
"template_arg_list : template_arg_list ',' template_arg",
"template_specifiter : LE_CLASS",
"template_specifiter : LE_TYPENAME",
"opt_template_qualifier :",
"opt_template_qualifier : LE_TEMPLATE '<' template_arg_list '>'",
"derivation_list :",
"derivation_list : parent_class",
"derivation_list : derivation_list ',' parent_class",
"parent_class : access_specifier class_name opt_template_specifier",
"class_name : LE_IDENTIFIER",
"class_name : class_name LE_CLCL LE_IDENTIFIER",
"opt_template_specifier :",
"opt_template_specifier : '<' template_parameter_list '>'",
"access_specifier :",
"access_specifier : LE_PUBLIC",
"access_specifier : LE_PRIVATE",
"access_specifier : LE_PROTECTED",
"template_parameter_list :",
"template_parameter_list : template_parameter",
"template_parameter_list : template_parameter_list ',' template_parameter",
"template_parameter : const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp",
"template_parameter : const_spec nested_scope_specifier basic_type_name special_star_amp",
"template_parameter : const_spec nested_scope_specifier LE_IDENTIFIER '<' template_parameter_list '>' special_star_amp",
"using_namespace : LE_USING LE_NAMESPACE nested_scope_specifier LE_IDENTIFIER ';'",
"namespace_decl : stmnt_starter LE_NAMESPACE LE_IDENTIFIER '{'",
"namespace_decl : stmnt_starter LE_NAMESPACE '{'",
"opt_class_qualifier :",
"opt_class_qualifier : LE_MACRO",
"class_decl : stmnt_starter opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER '{'",
"class_decl : stmnt_starter opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER ':' derivation_list '{'",
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

//swallow all tokens up to the first '{'
void consumeBody (){
	std::string cs = "{";
	int depth = 1;
	while( true ) {
		int ch = cl_scope_lex();
		if(ch == 0){
			break;
		}

		cs += cl_scope_text;
		cs += " ";

		if(ch == '{'){
			depth++;
		}else if(ch == '}'){
			depth--;
			if(depth == 0){
				cl_scope_less(0);
				break;
			}
		}
	}
	printf("Consumed body: [%s]\n", cs.c_str());
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
	int depth = 1;
	while(depth > 0)
	{
		int ch = cl_scope_lex();
		//printf("ch=%d\n", ch);
		fflush(stdout);
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
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
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
{ yyval = yyvsp[0]; }
break;
case 27:
{
/*								printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_scope_text, cl_scope_lineno);*/
/*								syncParser();*/
							}
break;
case 28:
{ yyval = "";}
break;
case 29:
{yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 30:
{ yyval = yyvsp[0]; }
break;
case 31:
{ yyval = yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0]; }
break;
case 32:
{ yyval = yyvsp[0]; }
break;
case 33:
{ yyval = yyvsp[0]; }
break;
case 35:
{ yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 36:
{yyval = "";}
break;
case 37:
{yyval = yyvsp[0];}
break;
case 38:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 39:
{yyval = yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];}
break;
case 40:
{yyval = yyvsp[0];}
break;
case 41:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[-1];}
break;
case 42:
{yyval = "";}
break;
case 43:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 44:
{yyval = "";}
break;
case 45:
{yyval = yyvsp[0];}
break;
case 46:
{yyval = yyvsp[0];}
break;
case 47:
{yyval = yyvsp[0];}
break;
case 48:
{yyval = "";}
break;
case 49:
{yyval = yyvsp[0];}
break;
case 50:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 51:
{
							yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
						}
break;
case 52:
{
							yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
						}
break;
case 53:
{
							yyval = yyvsp[-6] + yyvsp[-5] + yyvsp[-4] +yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0] + " " ;
						}
break;
case 54:
{
						/*printf("Found using namespace %s\n", $3.c_str());*/
						gs_additionlNS.push_back(yyvsp[-2]+yyvsp[-1]);
					}
break;
case 55:
{
							currentScope.push_back(yyvsp[-1]);
							printScopeName();
						}
break;
case 56:
{
							/*anonymouse namespace*/
							increaseScope();
							printScopeName();
						}
break;
case 57:
{yyval = "";}
break;
case 58:
{yyval = yyvsp[0];}
break;
case 59:
{
					/*increase the scope level*/
					currentScope.push_back(yyvsp[-1]);
					printScopeName();
				}
break;
case 60:
{
					/*increase the scope level*/
					currentScope.push_back(yyvsp[-3]);
					printScopeName();
				}
break;
case 61:
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
case 62:
{
								/*increase random scope*/
								increaseScope();
								printScopeName();
							 }
break;
case 63:
{
							consumeNotIncluding(';');
						}
break;
case 64:
{yyval = yyvsp[0];}
break;
case 65:
{yyval = yyvsp[0];}
break;
case 66:
{yyval = yyvsp[0];}
break;
case 67:
{yyval = yyvsp[-1];}
break;
case 109:
{yyval = '{';}
break;
case 110:
{consumeInitializationList() /*eat everything including the open brace*/;}
break;
case 111:
{yyval = "";}
break;
case 112:
{yyval = yyvsp[-1];}
break;
case 113:
{consumeFuncArgList();}
break;
case 114:
{
						/*trim down trailing '::' from scope name*/
						if(yyvsp[-6].find_last_not_of(":") != std::string::npos){
							yyvsp[-6].erase(yyvsp[-6].find_last_not_of(":")+1);
						}
						currentScope.push_back(yyvsp[-6]);
						printScopeName();
					}
break;
case 115:
{consumeFuncArgList();}
break;
case 116:
{

						/*trim down trailing '::' from scope name*/
						if(yyvsp[-4].find_last_not_of(":") != std::string::npos){
							yyvsp[-4].erase(yyvsp[-4].find_last_not_of(":")+1);
						}
						currentScope.push_back(yyvsp[-4]);
						printScopeName();
					}
break;
case 117:
{consumeFuncArgList();}
break;
case 118:
{

						/*trim down trailing '::' from scope name*/
						if(yyvsp[-6].find_last_not_of(":") != std::string::npos){
							yyvsp[-6].erase(yyvsp[-6].find_last_not_of(":")+1);
						}
						currentScope.push_back(yyvsp[-6]);
						printScopeName();
					}
break;
case 119:
{yyval = "";}
break;
case 120:
{	yyval = yyvsp[-1] + yyvsp[0];}
break;
case 121:
{yyval = yyvsp[-1]+ yyvsp[0];}
break;
case 122:
{consumeTemplateDecl();}
break;
case 123:
{yyval = yyvsp[-3] + yyvsp[0];}
break;
case 124:
{yyval = ""; }
break;
case 125:
{ yyval = yyvsp[0]; }
break;
case 126:
{yyval = ""; }
break;
case 127:
{ yyval = yyvsp[0]; }
break;
case 128:
{yyval = ""; }
break;
case 129:
{ yyval = yyvsp[0]; }
break;
case 130:
{yyval = ""; }
break;
case 131:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 132:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 133:
{yyval = "";}
break;
case 134:
{ yyval = ";";}
break;
case 135:
{ yyval = ":";}
break;
case 136:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;}
break;
case 137:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;}
break;
case 138:
{yyval = yyvsp[-5] + yyvsp[-4] + yyvsp[-3]  + yyvsp[-2] + yyvsp[-1] + yyvsp[0] ;}
break;
case 139:
{currentScope.push_back(yyvsp[-1]); printScopeName();}
break;
case 140:
{
							currentScope.pop_back();/*reduce the scope*/
							printScopeName();
							/*printf("found enum: %s, args are: %s\n", $2.c_str(), $5.c_str());*/
						}
break;
case 141:
{yyval = "";}
break;
case 142:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 143:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 144:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 145:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 146:
{yyval = "";}
break;
case 147:
{yyval = yyvsp[0];}
break;
case 148:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 149:
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
