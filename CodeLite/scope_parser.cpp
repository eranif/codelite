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
/* Copyright Eran Ifrah(c)   */
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
   11,   11,   13,   13,   12,   12,   14,   14,   15,   15,
   15,   16,   18,   18,   17,   17,   17,   17,   19,   19,
   19,   20,    8,    7,    7,   25,   25,    3,    3,    9,
   10,   26,   26,   27,   27,   28,   28,   28,   28,   28,
   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,
   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,
   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,
   28,   28,   28,   28,   28,   28,   29,   29,   32,    6,
   33,    6,   34,    6,   22,   22,   35,   36,   35,   30,
   30,   21,   21,   37,   37,   38,   38,   23,   24,   24,
   24,   31,   31,   31,   40,    4,   41,   41,   41,   41,
   42,   39,   39,   39,    5,
};
short cl_scope_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    2,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    0,    2,    1,    3,    1,    1,    0,    4,    0,    1,
    3,    3,    0,    3,    0,    1,    1,    1,    0,    1,
    3,    4,    4,    4,    3,    0,    1,    6,    8,    1,
    1,    1,    1,    1,    2,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    2,    2,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    0,    1,    0,   11,
    0,   10,    0,   11,    0,    2,    2,    0,    4,    0,
    1,    0,    1,    0,    1,    0,    2,    2,    0,    1,
    1,    3,    3,    6,    0,    7,    0,    2,    2,    2,
    2,    0,    1,    3,    4,
};
short cl_scope_defred[] = {                                     10,
    0,   20,    0,  120,   51,  121,   50,   11,   12,   13,
   14,   15,   16,   17,   18,   19,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   45,   53,
   52,  111,    0,    0,   43,  125,  135,   25,   26,   23,
    0,    0,   44,   47,    0,  113,  105,    0,   22,    0,
   28,    0,    0,  105,    0,    0,  133,   24,   48,    0,
    6,    1,    4,    2,    5,    3,    8,    7,    9,    0,
    0,    0,  116,    0,  106,    0,    0,  131,    0,  126,
   36,   38,   37,    0,   30,    0,   84,   85,   72,   76,
   77,   68,   69,   78,   79,   80,   81,   70,   71,   75,
   73,   87,   88,   89,   90,   91,   92,   93,   94,   95,
   96,   86,   66,   67,   56,   57,   58,   59,   60,   61,
   62,   63,   64,   65,   74,    0,    0,   55,  107,    0,
  123,    0,   54,    0,  122,  101,    0,    0,  130,  129,
  128,  134,    0,   49,    0,   82,   83,    0,   40,  105,
    0,  117,  115,  118,  103,    0,  108,   99,   31,    0,
   32,    0,  116,    0,  109,    0,   98,    0,    0,    0,
   41,  124,    0,    0,  102,    0,   34,   42,  104,  100,
};
short cl_scope_dgoto[] = {                                       1,
   73,    8,    9,   10,   11,   12,   13,   14,   15,   16,
   40,   41,   42,   23,   84,   85,   86,  161,  148,  149,
  150,   53,  131,   17,   45,   33,   74,  128,  168,   34,
   54,  169,  156,  166,   75,  151,  154,  132,   56,   48,
   78,   57,
};
short cl_scope_sindex[] = {                                      0,
  -36,    0, -267,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0, -229, -250, -240, -235,
   18, -107, -224,   21,  -42,  -40, -248,  -39,    0,    0,
    0,    0, -261, -185,    0,    0,    0,    0,    0,    0,
 -215,   -6,    0,    0, -213,    0,    0, -211,    0, -248,
    0,  -25,   54,    0,   34,  -20,    0,    0,    0, -228,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -33,
  -59, -243,    0,   56,    0, -242, -234,    0, -211,    0,
    0,    0,    0,  -12,    0, -208,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   58,    7,    0,    0, -185,
    0,    9,    0,   61,    0,    0,  -58,   62,    0,    0,
    0,    0, -228,    0,   43,    0,    0,   -5,    0,    0,
 -226,    0,    0,    0,    0,   48,    0,    0,    0, -185,
    0, -185,    0, -197,    0, -185,    0,  -14, -185,   -3,
    0,    0,  -58,   -8,    0,   -7,    0,    0,    0,    0,
};
short cl_scope_rindex[] = {                                      0,
 -118,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  -74,    0,    0,    0,
    0,    0,  -47,    0,    0,    0,   -2,    0,    0,    0,
    0,    0, -191,   68,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -18,    0,   -2,
    0,    0,    0,    0,  -13,    0,    0,    0,    0,  -38,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -23,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   -9,
    0,  -41,    0,    0,    0,    0,   77,    0,    0,    0,
    0,    0, -187,    0,  -10,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   -4,    0,    0,    0,  -32,
    0, -183,    0,    0,    0,    1,    0,    0,    1,    0,
    0,    0,    2,    0,    0,    0,    0,    0,    0,    0,
};
short cl_scope_gindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   73,    0,    0,    0,    0,  -17,    0,    0,  -35,  -34,
  -16,  -29,  -53,    0,    0,    0,   -1,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   50,
};
#define YYTABLESIZE 373
short cl_scope_table[] = {                                     124,
  130,  157,  114,  119,  121,   29,  126,  119,  117,  115,
  112,   39,  125,  118,  116,   29,   54,   47,  116,  135,
  114,    6,    4,   79,   76,  132,  113,  116,  114,   39,
  127,  143,   60,   33,   39,   30,   19,   50,  162,  116,
  162,   21,   20,  116,   38,  116,  153,   18,   70,   70,
  152,   27,   39,   39,   24,   51,  163,  127,  177,   21,
  120,  133,  137,  116,   25,   81,   82,   83,   31,   26,
  134,   21,   32,  139,  138,  140,  141,   27,  110,   35,
   36,   44,   37,   43,   29,   22,    5,   46,    7,   49,
  122,   52,  123,   55,   77,  136,  145,   59,  146,  147,
  155,  158,  160,  165,   80,  167,  132,  173,  175,  172,
  144,  127,   33,   46,  179,  180,   54,   35,   97,  178,
  164,  112,   58,  112,  170,  159,    0,  171,  142,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  119,
  119,  119,    0,    0,  119,    0,    0,  119,    0,  174,
  119,    0,  176,  119,  119,  119,  119,  119,    0,    0,
  119,  119,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  119,  119,    0,    0,    0,  119,   72,
    0,    0,  119,   27,   27,   27,  119,    0,   27,    0,
    0,    0,    0,  112,   27,    0,  119,   28,   27,   27,
   27,   27,    0,    0,   27,   27,    0,    0,    0,    0,
  110,  110,    0,    0,    0,  110,    0,   27,   27,    2,
    0,  110,   27,    0,    0,  110,  110,  110,  110,    0,
   27,  110,  110,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  110,    0,    0,    0,    0,    0,
  114,    0,    0,    0,    0,   87,   88,  110,    0,    0,
    0,    0,    0,  114,    0,    0,   35,    0,  116,    0,
  129,  129,  112,    0,    0,    0,    0,    0,    0,    3,
    0,  116,    0,    0,   89,   90,   91,   92,   93,   94,
   95,   96,   97,   98,   99,  112,    0,  100,  101,  102,
  103,  104,  105,  106,  107,  108,  109,  110,  111,    0,
    0,   61,   62,    0,    0,    0,   63,    0,    0,    0,
  108,    0,   64,    0,    0,  112,  112,   65,   66,   67,
  112,    0,   68,   69,    0,    0,  112,    0,    0,    0,
    0,  112,  112,  112,    0,   70,  112,  112,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   71,  112,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  112,
};
short cl_scope_check[] = {                                      33,
   60,   60,   44,   37,   38,   44,   40,  126,   42,   43,
   44,   44,   46,   47,   38,  123,   40,   34,   42,   73,
   62,   58,   59,   44,   54,   44,   60,   61,   62,   62,
   44,   44,   58,   44,   44,  260,  266,   44,   44,   38,
   44,   44,  272,   42,  293,   44,   38,  315,  292,  292,
   42,  126,   62,  302,  305,   62,   62,   91,   62,   62,
   94,  305,  305,   62,  305,  294,  295,  296,  293,  305,
   72,  301,  297,  308,   76,  310,  311,   60,  126,   59,
  123,  343,  123,  123,  123,  315,  123,  273,  125,  305,
  124,  305,  126,  305,   61,   40,  305,  123,   41,   93,
   40,   40,   60,  330,  125,   58,  125,  305,  123,  163,
  123,  125,  123,  305,  123,  123,   40,  305,  123,  173,
  150,  305,   50,  123,  160,  143,   -1,  162,   79,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,
  259,  260,   -1,   -1,  263,   -1,   -1,  266,   -1,  166,
  269,   -1,  169,  272,  273,  274,  275,  276,   -1,   -1,
  279,  280,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  292,  293,   -1,   -1,   -1,  297,  126,
   -1,   -1,  301,  258,  259,  260,  305,   -1,  263,   -1,
   -1,   -1,   -1,  126,  269,   -1,  315,  305,  273,  274,
  275,  276,   -1,   -1,  279,  280,   -1,   -1,   -1,   -1,
  258,  259,   -1,   -1,   -1,  263,   -1,  292,  293,  256,
   -1,  269,  297,   -1,   -1,  273,  274,  275,  276,   -1,
  305,  279,  280,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  292,   -1,   -1,   -1,   -1,   -1,
  292,   -1,   -1,   -1,   -1,  289,  290,  305,   -1,   -1,
   -1,   -1,   -1,  305,   -1,   -1,  305,   -1,  292,   -1,
  330,  330,  305,   -1,   -1,   -1,   -1,   -1,   -1,  316,
   -1,  305,   -1,   -1,  318,  319,  320,  321,  322,  323,
  324,  325,  326,  327,  328,  305,   -1,  331,  332,  333,
  334,  335,  336,  337,  338,  339,  340,  341,  342,   -1,
   -1,  258,  259,   -1,   -1,   -1,  263,   -1,   -1,   -1,
  330,   -1,  269,   -1,   -1,  258,  259,  274,  275,  276,
  263,   -1,  279,  280,   -1,   -1,  269,   -1,   -1,   -1,
   -1,  274,  275,  276,   -1,  292,  279,  280,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  305,  292,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  305,
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
0,0,0,0,0,"':'","';'","'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,"'['",0,"']'","'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'{'","'|'","'}'","'~'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"LE_AUTO","LE_DOUBLE",
"LE_INT","LE_STRUCT","LE_BREAK","LE_ELSE","LE_LONG","LE_SWITCH","LE_CASE",
"LE_ENUM","LE_REGISTER","LE_TYPEDEF","LE_CHAR","LE_EXTERN","LE_RETURN",
"LE_UNION","LE_CONST","LE_FLOAT","LE_SHORT","LE_UNSIGNED","LE_CONTINUE",
"LE_FOR","LE_SIGNED","LE_VOID","LE_DEFAULT","LE_GOTO","LE_SIZEOF","LE_VOLATILE",
"LE_DO","LE_IF","LE_STATIC","LE_WHILE","LE_NEW","LE_DELETE","LE_THIS",
"LE_OPERATOR","LE_CLASS","LE_PUBLIC","LE_PROTECTED","LE_PRIVATE","LE_VIRTUAL",
"LE_FRIEND","LE_INLINE","LE_OVERLOAD","LE_TEMPLATE","LE_TYPENAME","LE_THROW",
"LE_CATCH","LE_IDENTIFIER","LE_STRINGliteral","LE_FLOATINGconstant",
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
"using_namespace : LE_USING LE_NAMESPACE LE_IDENTIFIER ';'",
"namespace_decl : stmnt_starter LE_NAMESPACE LE_IDENTIFIER '{'",
"namespace_decl : stmnt_starter LE_NAMESPACE '{'",
"opt_class_qualifier :",
"opt_class_qualifier : LE_MACRO",
"class_decl : stmnt_starter opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER '{'",
"class_decl : stmnt_starter opt_template_qualifier class_keyword opt_class_qualifier LE_IDENTIFIER ':' derivation_list '{'",
"scope_reducer : '}'",
"scope_increaer : '{'",
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
"optional_initialization_list :",
"optional_initialization_list : ':'",
"$$1 :",
"function_decl : stmnt_starter opt_template_qualifier virtual_spec const_spec variable_decl nested_scope_specifier func_name '(' $$1 const_spec '{'",
"$$2 :",
"function_decl : stmnt_starter opt_template_qualifier virtual_spec const_spec nested_scope_specifier func_name '(' $$2 optional_initialization_list '{'",
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
		
		//keep the function signature
		if(ch == '{'){
			cl_scope_less(0);
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
case 20:
{ 
								/*printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_scope_text, cl_scope_lineno);*/
								syncParser();
							}
break;
case 21:
{ yyval = "";}
break;
case 22:
{yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 23:
{ yyval = yyvsp[0]; }
break;
case 24:
{ yyval = yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0]; }
break;
case 25:
{ yyval = yyvsp[0]; }
break;
case 26:
{ yyval = yyvsp[0]; }
break;
case 28:
{ yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 29:
{yyval = "";}
break;
case 30:
{yyval = yyvsp[0];}
break;
case 31:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 32:
{yyval = yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];}
break;
case 33:
{yyval = "";}
break;
case 34:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 35:
{yyval = "";}
break;
case 36:
{yyval = yyvsp[0];}
break;
case 37:
{yyval = yyvsp[0];}
break;
case 38:
{yyval = yyvsp[0];}
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
{yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] +yyvsp[0];}
break;
case 43:
{ 	
						/*printf("Found using namespace %s\n", $3.c_str());*/
						gs_additionlNS.push_back(yyvsp[-1]);
					}
break;
case 44:
{
							currentScope.push_back(yyvsp[-1]);
							printScopeName();
						}
break;
case 45:
{
							/*anonymouse namespace*/
							increaseScope();
							printScopeName();
						}
break;
case 46:
{yyval = "";}
break;
case 47:
{yyval = yyvsp[0];}
break;
case 48:
{
					/*increase the scope level*/
					currentScope.push_back(yyvsp[-1]);
					printScopeName();
				}
break;
case 49:
{
					/*increase the scope level*/
					currentScope.push_back(yyvsp[-3]);
					printScopeName();
				}
break;
case 50:
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
case 51:
{
								/*increase random scope*/
								increaseScope();
								printScopeName();
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
{yyval = yyvsp[-1];}
break;
case 98:
{consumeInitializationList();}
break;
case 99:
{consumeFuncArgList();}
break;
case 100:
{
						/*trim down trailing '::' from scope name*/
						if(yyvsp[-5].find_last_not_of(":") != std::string::npos){
							yyvsp[-5].erase(yyvsp[-5].find_last_not_of(":")+1);
						}
						currentScope.push_back(yyvsp[-5]);
						printScopeName();
					}
break;
case 101:
{consumeFuncArgList();}
break;
case 102:
{
						
						/*trim down trailing '::' from scope name*/
						if(yyvsp[-5].find_last_not_of(":") != std::string::npos){
							yyvsp[-5].erase(yyvsp[-5].find_last_not_of(":")+1);
						}
						currentScope.push_back(yyvsp[-5]);
						printScopeName();
					}
break;
case 103:
{consumeFuncArgList();}
break;
case 104:
{
						
						/*trim down trailing '::' from scope name*/
						if(yyvsp[-6].find_last_not_of(":") != std::string::npos){
							yyvsp[-6].erase(yyvsp[-6].find_last_not_of(":")+1);
						}
						currentScope.push_back(yyvsp[-6]);
						printScopeName();
					}
break;
case 105:
{yyval = "";}
break;
case 106:
{	yyval = yyvsp[-1] + yyvsp[0];}
break;
case 107:
{yyval = yyvsp[-1]+ yyvsp[0];}
break;
case 108:
{consumeTemplateDecl();}
break;
case 109:
{yyval = yyvsp[-3] + yyvsp[0];}
break;
case 110:
{yyval = ""; }
break;
case 111:
{ yyval = yyvsp[0]; }
break;
case 112:
{yyval = ""; }
break;
case 113:
{ yyval = yyvsp[0]; }
break;
case 114:
{yyval = ""; }
break;
case 115:
{ yyval = yyvsp[0]; }
break;
case 116:
{yyval = ""; }
break;
case 117:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 118:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 119:
{yyval = "";}
break;
case 120:
{ yyval = ";";}
break;
case 121:
{ yyval = ":";}
break;
case 122:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;}
break;
case 123:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;}
break;
case 124:
{yyval = yyvsp[-5] + yyvsp[-4] + yyvsp[-3]  + yyvsp[-2] + yyvsp[-1] + yyvsp[0] ;}
break;
case 125:
{currentScope.push_back(yyvsp[-1]); printScopeName();}
break;
case 126:
{	
							currentScope.pop_back();/*reduce the scope*/
							printScopeName();
							/*printf("found enum: %s, args are: %s\n", $2.c_str(), $5.c_str());*/
						}
break;
case 127:
{yyval = "";}
break;
case 128:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 129:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 130:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 131:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 132:
{yyval = "";}
break;
case 133:
{yyval = yyvsp[0];}
break;
case 134:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 135:
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
