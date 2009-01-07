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
#define LE_CONTINUE 277
#define LE_FOR 278
#define LE_SIGNED 279
#define LE_VOID 280
#define LE_DEFAULT 281
#define LE_GOTO 282
#define LE_SIZEOF 283
#define LE_VOLATILE 284
#define LE_DO 285
#define LE_IF 286
#define LE_STATIC 287
#define LE_WHILE 288
#define LE_NEW 289
#define LE_DELETE 290
#define LE_THIS 291
#define LE_OPERATOR 292
#define LE_CLASS 293
#define LE_PUBLIC 294
#define LE_PROTECTED 295
#define LE_PRIVATE 296
#define LE_VIRTUAL 297
#define LE_FRIEND 298
#define LE_INLINE 299
#define LE_OVERLOAD 300
#define LE_TEMPLATE 301
#define LE_TYPENAME 302
#define LE_THROW 303
#define LE_CATCH 304
#define LE_IDENTIFIER 305
#define LE_STRINGliteral 306
#define LE_FLOATINGconstant 307
#define LE_INTEGERconstant 308
#define LE_CHARACTERconstant 309
#define LE_OCTALconstant 310
#define LE_HEXconstant 311
#define LE_POUNDPOUND 312
#define LE_CComment 313
#define LE_CPPComment 314
#define LE_NAMESPACE 315
#define LE_USING 316
#define LE_TYPEDEFname 317
#define LE_ARROW 318
#define LE_ICR 319
#define LE_DECR 320
#define LE_LS 321
#define LE_RS 322
#define LE_LE 323
#define LE_GE 324
#define LE_EQ 325
#define LE_NE 326
#define LE_ANDAND 327
#define LE_OROR 328
#define LE_ELLIPSIS 329
#define LE_CLCL 330
#define LE_DOTstar 331
#define LE_ARROWstar 332
#define LE_MULTassign 333
#define LE_DIVassign 334
#define LE_MODassign 335
#define LE_PLUSassign 336
#define LE_MINUSassign 337
#define LE_LSassign 338
#define LE_RSassign 339
#define LE_ANDassign 340
#define LE_ERassign 341
#define LE_ORassign 342
#define LE_MACRO 343
#define LE_DYNAMIC_CAST 344
#define LE_STATIC_CAST 345
#define LE_CONST_CAST 346
#define LE_REINTERPRET_CAST 347
#define YYERRCODE 256
short cl_scope_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    0,    2,    2,    2,    2,    2,    2,    2,    2,    2,
    2,   12,   12,   14,   14,   13,   13,   15,   15,   16,
   16,   16,   17,   19,   19,   18,   18,   18,   18,   20,
   20,   20,   21,   21,   21,    8,    7,    7,   26,   26,
    3,    3,    9,   10,   11,   27,   27,   28,   28,   29,
   29,   29,   29,   29,   29,   29,   29,   29,   29,   29,
   29,   29,   29,   29,   29,   29,   29,   29,   29,   29,
   29,   29,   29,   29,   29,   29,   29,   29,   29,   29,
   29,   29,   29,   29,   29,   29,   29,   29,   29,   29,
   30,   30,   31,   31,   34,    6,   35,    6,   36,    6,
   23,   23,   37,   38,   37,   32,   32,   22,   22,   39,
   39,   40,   40,   24,   25,   25,   25,   33,   33,   33,
   42,    4,   43,   43,   43,   43,   44,   41,   41,   41,
    5,
};
short cl_scope_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    2,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    0,    2,    1,    3,    1,    1,    0,    4,    0,
    1,    3,    3,    0,    3,    0,    1,    1,    1,    0,
    1,    3,    4,    4,    7,    5,    4,    3,    0,    1,
    6,    8,    1,    1,    1,    1,    1,    1,    2,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    2,    2,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    0,    4,    0,   12,    0,    9,    0,   11,
    0,    2,    2,    0,    4,    0,    1,    0,    1,    0,
    1,    0,    2,    2,    0,    1,    1,    3,    3,    6,
    0,    7,    0,    2,    2,    2,    2,    0,    1,    3,
    4,
};
short cl_scope_defred[] = {                                     10,
    0,   21,    0,  126,   54,  127,   53,   55,   11,   12,
   13,   14,   15,   16,   17,   18,   19,   20,    0,  111,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   48,   57,   56,  117,    0,    0,    0,  112,  131,  141,
   26,   27,   24,    0,    0,   47,   50,    0,  119,  111,
  113,  114,   46,    0,   23,    0,   29,    0,    0,  111,
    0,    0,    0,  139,   25,   51,    0,    6,    1,    4,
    2,    5,    3,    8,    7,    9,    0,    0,    0,  122,
    0,    0,  115,    0,  137,    0,  132,   37,   39,   38,
    0,   31,    0,   88,   89,   76,   80,   81,   72,   73,
   82,   83,   84,   85,   74,   75,   79,   77,   91,   92,
   93,   94,   95,   96,   97,   98,   99,  100,   90,   70,
   71,   60,   61,   62,   63,   64,   65,   66,   67,   68,
   69,   78,    0,    0,   59,    0,  129,    0,   58,    0,
  128,  107,    0,    0,  136,  135,  134,  140,    0,   52,
    0,   86,   87,    0,   41,  111,  123,  121,  124,  109,
    0,  105,   32,    0,   33,    0,  122,    0,    0,  101,
  102,  108,    0,    0,   42,  130,    0,  122,    0,    0,
   35,    0,   43,   44,  110,    0,    0,    0,    0,  106,
  122,    0,   45,  104,
};
short cl_scope_dgoto[] = {                                       1,
   80,    9,   10,   11,   12,   13,   14,   15,   16,   17,
   18,   43,   44,   45,   25,   91,   92,   93,  165,  154,
  155,  156,   26,  137,   19,   48,   35,   81,  135,  172,
  187,   36,   60,  173,  161,  169,   38,   61,  159,  138,
   63,   54,   85,   64,
};
short cl_scope_sindex[] = {                                      0,
  -37,    0, -290,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -223,    0,
 -242, -231,   25, -105, -210, -201,  -13,    2, -247,    9,
    0,    0,    0,    0, -227, -178,  -58,    0,    0,    0,
    0,    0,    0, -188,    3,    0,    0, -171,    0,    0,
    0,    0,    0, -164,    0, -247,    0,  -20,  108,    0,
 -187,   81,  -12,    0,    0,    0, -156,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  -33,  -57, -248,    0,
  110, -191,    0, -180,    0, -164,    0,    0,    0,    0,
   -4,    0, -157,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  112,   58,    0, -178,    0,   67,    0,  114,
    0,    0,  -54,  117,    0,    0,    0,    0, -156,    0,
   96,    0,    0,    4,    0,    0,    0,    0,    0,    0,
  -16,    0,    0, -178,    0, -178,    0, -207, -178,    0,
    0,    0, -178,   38,    0,    0,  -52,    0,   41, -135,
    0, -178,    0,    0,    0,  129,   47,   40, -178,    0,
    0,   71,    0,    0,
};
short cl_scope_rindex[] = {                                      0,
 -114,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   76,    0,
    0,    0,    0,    0,  -47,    0,    0,    0,   46,    0,
    0,    0,    0,    0, -134,  116,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   -7,    0,   46,    0,    0,    0,    0,
    0,   -5,    0,    0,    0,    0,  -24,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -23,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   -9,    0,   -8,    0,    0,
    0,    0,  132,    0,    0,    0,    0,    0, -132,    0,
    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -28,    0, -199,    0,    0,   51,    0,
    0,    0, -100,    0,    0,    0,   85,    0,    0,   52,
    0,   -9,    0,    0,    0,    0,    0,    0,   53,    0,
    0,    0,    0,    0,
};
short cl_scope_gindex[] = {                                      0,
    8,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  121,    0,    0,    0,    0,   31,    0,    0,  -93,
   15,   -6,  -19,  -56,    0,    0,    0,   54,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   98,
};
#define YYTABLESIZE 421
short cl_scope_table[] = {                                     131,
   53,   52,  136,  126,  128,   52,  133,  182,  124,  122,
  119,  125,  132,  125,  122,   40,   58,   31,  122,   30,
    6,    4,  118,  141,   20,    8,  120,  123,  121,   50,
   59,   86,  120,   40,   40,  120,  138,   67,  133,  149,
   82,  171,   21,   77,   34,   41,   56,  166,   22,   32,
   68,   69,   40,  120,   42,   70,  139,  134,  118,  118,
  127,   71,   27,  118,   57,  167,   72,   73,   74,  118,
  174,   75,   76,   28,  118,  118,  118,   23,  116,  118,
  118,  166,   33,  166,   29,    5,   34,    7,  188,   22,
  129,   24,  130,   40,   49,  192,   40,  177,   30,  181,
   77,  191,   66,   37,  158,  118,  170,   22,  157,   39,
  176,  194,   87,  143,  166,   47,   55,  138,  150,  133,
  183,  184,  122,   34,   40,  122,  122,  145,  122,  146,
  147,   46,  140,   58,  193,  144,  168,   88,   89,   90,
   62,   84,   83,  125,  125,  125,  122,  151,  125,  142,
  153,  125,  152,  160,  125,  164,  162,  125,  125,  125,
  125,  125,  179,  185,  125,  125,  180,  186,  189,  190,
   49,   58,   36,  118,  103,  178,   65,  125,  125,  163,
  175,    0,  125,  148,    0,    0,  125,    0,    0,    0,
  125,    0,    0,    0,    0,    0,    0,    0,    0,   30,
  125,   28,  118,    0,    0,    0,    0,    0,    0,    0,
  116,  116,    0,    0,    0,  116,    0,    0,    2,    0,
    0,  116,    0,    0,    0,  116,  116,  116,  116,  118,
  118,  116,  116,   79,  118,    0,    0,    0,    0,    0,
  118,  118,    0,    0,  116,  118,  118,  118,  118,  118,
  118,  118,    0,  118,    0,   94,   95,  116,    0,  118,
    0,    0,    0,    0,  118,  118,  118,    0,  122,  118,
  118,   51,   51,    0,    0,   51,  118,   51,    3,    0,
   36,  122,    0,  120,   96,   97,   98,   99,  100,  101,
  102,  103,  104,  105,  106,  118,  120,  107,  108,  109,
  110,  111,  112,  113,  114,  115,  116,  117,  118,    0,
  118,  118,    0,    0,    0,  118,    0,    0,    0,    0,
  114,  118,    0,    0,    0,    0,  118,  118,  118,    0,
    0,  118,  118,   28,   28,   28,    0,    0,   28,    0,
    0,    0,    0,    0,   28,    0,    0,    0,   28,   28,
   28,   28,    0,    0,   28,   28,    0,  118,    0,    0,
    0,    0,    0,    0,    0,   68,   69,   28,   28,    0,
   70,    0,   28,  118,  118,    0,   71,    0,  118,    0,
   28,   72,   73,   74,  118,    0,   75,   76,    0,  118,
  118,  118,    0,    0,  118,  118,    0,    0,    0,   77,
    0,    0,    0,    0,    0,    0,    0,  118,    0,    0,
    0,    0,   78,    0,    0,    0,    0,    0,    0,    0,
  118,
};
short cl_scope_check[] = {                                      33,
   59,   60,   60,   37,   38,   60,   40,   60,   42,   43,
   44,  126,   46,   47,   38,   44,   40,  123,   42,   44,
   58,   59,  123,   80,  315,   63,   60,   61,   62,   36,
   50,   44,   41,   62,   44,   44,   44,   58,   44,   44,
   60,   58,  266,  292,   44,  293,   44,   44,  272,  260,
  258,  259,   62,   62,  302,  263,  305,   91,  258,  259,
   94,  269,  305,  263,   62,   62,  274,  275,  276,  269,
  164,  279,  280,  305,  274,  275,  276,  301,  126,  279,
  280,   44,  293,   44,   60,  123,  297,  125,  182,   44,
  124,  315,  126,   41,  273,  189,   44,  305,  123,   62,
  292,   62,  123,  305,   38,  305,  123,   62,   42,  123,
  167,   41,  125,  305,   44,  343,  305,  125,  123,  125,
  177,  178,   38,  123,  123,   41,   42,  308,   44,  310,
  311,  123,   79,  305,  191,   82,  156,  294,  295,  296,
  305,   61,  330,  258,  259,  260,   62,  305,  263,   40,
   93,  266,   41,   40,  269,   60,   40,  272,  273,  274,
  275,  276,  169,  123,  279,  280,  173,  303,   40,  123,
  305,   40,  305,  123,  123,  168,   56,  292,  293,  149,
  166,   -1,  297,   86,   -1,   -1,  301,   -1,   -1,   -1,
  305,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  305,
  315,  126,  303,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  258,  259,   -1,   -1,   -1,  263,   -1,   -1,  256,   -1,
   -1,  269,   -1,   -1,   -1,  273,  274,  275,  276,  258,
  259,  279,  280,  126,  263,   -1,   -1,   -1,   -1,   -1,
  269,  126,   -1,   -1,  292,  274,  275,  276,  258,  259,
  279,  280,   -1,  263,   -1,  289,  290,  305,   -1,  269,
   -1,   -1,   -1,   -1,  274,  275,  276,   -1,  292,  279,
  280,  330,  330,   -1,   -1,  330,  305,  330,  316,   -1,
  305,  305,   -1,  292,  318,  319,  320,  321,  322,  323,
  324,  325,  326,  327,  328,  305,  305,  331,  332,  333,
  334,  335,  336,  337,  338,  339,  340,  341,  342,   -1,
  258,  259,   -1,   -1,   -1,  263,   -1,   -1,   -1,   -1,
  330,  269,   -1,   -1,   -1,   -1,  274,  275,  276,   -1,
   -1,  279,  280,  258,  259,  260,   -1,   -1,  263,   -1,
   -1,   -1,   -1,   -1,  269,   -1,   -1,   -1,  273,  274,
  275,  276,   -1,   -1,  279,  280,   -1,  305,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  258,  259,  292,  293,   -1,
  263,   -1,  297,  258,  259,   -1,  269,   -1,  263,   -1,
  305,  274,  275,  276,  269,   -1,  279,  280,   -1,  274,
  275,  276,   -1,   -1,  279,  280,   -1,   -1,   -1,  292,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  292,   -1,   -1,
   -1,   -1,  305,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  305,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 347
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
"LE_RETURN","LE_UNION","LE_CONST","LE_FLOAT","LE_SHORT","LE_UNSIGNED",
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
"basic_type_name : LE_INT",
"basic_type_name : LE_CHAR",
"basic_type_name : LE_SHORT",
"basic_type_name : LE_LONG",
"basic_type_name : LE_FLOAT",
"basic_type_name : LE_DOUBLE",
"basic_type_name : LE_SIGNED",
"basic_type_name : LE_UNSIGNED",
"basic_type_name : LE_VOID",
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
"parent_class : access_specifier LE_IDENTIFIER opt_template_specifier",
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
case 21:
{
/*								printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_scope_text, cl_scope_lineno);*/
/*								syncParser();*/
							}
break;
case 22:
{ yyval = "";}
break;
case 23:
{yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 24:
{ yyval = yyvsp[0]; }
break;
case 25:
{ yyval = yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0]; }
break;
case 26:
{ yyval = yyvsp[0]; }
break;
case 27:
{ yyval = yyvsp[0]; }
break;
case 29:
{ yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 30:
{yyval = "";}
break;
case 31:
{yyval = yyvsp[0];}
break;
case 32:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 33:
{yyval = yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];}
break;
case 34:
{yyval = "";}
break;
case 35:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 36:
{yyval = "";}
break;
case 37:
{yyval = yyvsp[0];}
break;
case 38:
{yyval = yyvsp[0];}
break;
case 39:
{yyval = yyvsp[0];}
break;
case 40:
{yyval = "";}
break;
case 41:
{yyval = yyvsp[0];}
break;
case 42:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 43:
{
							yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
						}
break;
case 44:
{
							yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
						}
break;
case 45:
{
							yyval = yyvsp[-6] + yyvsp[-5] + yyvsp[-4] +yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0] + " " ;
						}
break;
case 46:
{
						/*printf("Found using namespace %s\n", $3.c_str());*/
						gs_additionlNS.push_back(yyvsp[-2]+yyvsp[-1]);
					}
break;
case 47:
{
							currentScope.push_back(yyvsp[-1]);
							printScopeName();
						}
break;
case 48:
{
							/*anonymouse namespace*/
							increaseScope();
							printScopeName();
						}
break;
case 49:
{yyval = "";}
break;
case 50:
{yyval = yyvsp[0];}
break;
case 51:
{
					/*increase the scope level*/
					currentScope.push_back(yyvsp[-1]);
					printScopeName();
				}
break;
case 52:
{
					/*increase the scope level*/
					currentScope.push_back(yyvsp[-3]);
					printScopeName();
				}
break;
case 53:
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
case 54:
{
								/*increase random scope*/
								increaseScope();
								printScopeName();
							 }
break;
case 55:
{
							consumeNotIncluding(';');
						}
break;
case 56:
{yyval = yyvsp[0];}
break;
case 57:
{yyval = yyvsp[0];}
break;
case 58:
{yyval = yyvsp[0];}
break;
case 59:
{yyval = yyvsp[-1];}
break;
case 101:
{yyval = '{';}
break;
case 102:
{consumeInitializationList() /*eat everything including the open brace*/;}
break;
case 103:
{yyval = "";}
break;
case 104:
{yyval = yyvsp[-1];}
break;
case 105:
{consumeFuncArgList();}
break;
case 106:
{
						/*trim down trailing '::' from scope name*/
						if(yyvsp[-6].find_last_not_of(":") != std::string::npos){
							yyvsp[-6].erase(yyvsp[-6].find_last_not_of(":")+1);
						}
						currentScope.push_back(yyvsp[-6]);
						printScopeName();
					}
break;
case 107:
{consumeFuncArgList();}
break;
case 108:
{

						/*trim down trailing '::' from scope name*/
						if(yyvsp[-4].find_last_not_of(":") != std::string::npos){
							yyvsp[-4].erase(yyvsp[-4].find_last_not_of(":")+1);
						}
						currentScope.push_back(yyvsp[-4]);
						printScopeName();
					}
break;
case 109:
{consumeFuncArgList();}
break;
case 110:
{

						/*trim down trailing '::' from scope name*/
						if(yyvsp[-6].find_last_not_of(":") != std::string::npos){
							yyvsp[-6].erase(yyvsp[-6].find_last_not_of(":")+1);
						}
						currentScope.push_back(yyvsp[-6]);
						printScopeName();
					}
break;
case 111:
{yyval = "";}
break;
case 112:
{	yyval = yyvsp[-1] + yyvsp[0];}
break;
case 113:
{yyval = yyvsp[-1]+ yyvsp[0];}
break;
case 114:
{consumeTemplateDecl();}
break;
case 115:
{yyval = yyvsp[-3] + yyvsp[0];}
break;
case 116:
{yyval = ""; }
break;
case 117:
{ yyval = yyvsp[0]; }
break;
case 118:
{yyval = ""; }
break;
case 119:
{ yyval = yyvsp[0]; }
break;
case 120:
{yyval = ""; }
break;
case 121:
{ yyval = yyvsp[0]; }
break;
case 122:
{yyval = ""; }
break;
case 123:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 124:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 125:
{yyval = "";}
break;
case 126:
{ yyval = ";";}
break;
case 127:
{ yyval = ":";}
break;
case 128:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;}
break;
case 129:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;}
break;
case 130:
{yyval = yyvsp[-5] + yyvsp[-4] + yyvsp[-3]  + yyvsp[-2] + yyvsp[-1] + yyvsp[0] ;}
break;
case 131:
{currentScope.push_back(yyvsp[-1]); printScopeName();}
break;
case 132:
{
							currentScope.pop_back();/*reduce the scope*/
							printScopeName();
							/*printf("found enum: %s, args are: %s\n", $2.c_str(), $5.c_str());*/
						}
break;
case 133:
{yyval = "";}
break;
case 134:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 135:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 136:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 137:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 138:
{yyval = "";}
break;
case 139:
{yyval = yyvsp[0];}
break;
case 140:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 141:
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
