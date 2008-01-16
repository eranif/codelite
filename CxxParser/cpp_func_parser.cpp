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
extern bool setLexerInput(const std::string &in, const std::map<std::string, bool> &ignoreTokens);
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
#define LE_IDENTIFIER 303
#define LE_STRINGliteral 304
#define LE_FLOATINGconstant 305
#define LE_INTEGERconstant 306
#define LE_CHARACTERconstant 307
#define LE_OCTALconstant 308
#define LE_HEXconstant 309
#define LE_POUNDPOUND 310
#define LE_CComment 311
#define LE_CPPComment 312
#define LE_NAMESPACE 313
#define LE_USING 314
#define LE_TYPEDEFname 315
#define LE_ARROW 316
#define LE_ICR 317
#define LE_DECR 318
#define LE_LS 319
#define LE_RS 320
#define LE_LE 321
#define LE_GE 322
#define LE_EQ 323
#define LE_NE 324
#define LE_ANDAND 325
#define LE_OROR 326
#define LE_ELLIPSIS 327
#define LE_CLCL 328
#define LE_DOTstar 329
#define LE_ARROWstar 330
#define LE_MULTassign 331
#define LE_DIVassign 332
#define LE_MODassign 333
#define LE_PLUSassign 334
#define LE_MINUSassign 335
#define LE_LSassign 336
#define LE_RSassign 337
#define LE_ANDassign 338
#define LE_ERassign 339
#define LE_ORassign 340
#define LE_MACRO 341
#define LE_DYNAMIC_CAST 342
#define LE_STATIC_CAST 343
#define LE_CONST_CAST 344
#define LE_REINTERPRET_CAST 345
#define YYERRCODE 256
short cl_func_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    0,    4,    2,    2,    5,    5,    7,    7,    6,    6,
    8,    8,    9,    9,    9,   10,   14,   14,   15,   15,
   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,
   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,
   15,   15,   15,   15,   15,   15,   15,   15,   15,   19,
    3,   21,   21,   12,   12,   20,   20,   22,   23,   22,
   17,   17,   11,   11,   24,   24,   25,   25,   13,   16,
   16,   16,   18,   18,   18,
};
short cl_func_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    2,    0,    2,    1,    0,    2,    1,    3,    1,    1,
    0,    4,    0,    1,    3,    4,    1,    2,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    2,    2,    1,    1,    1,    0,
   12,    1,    1,    0,    2,    0,    2,    2,    0,    4,
    0,    1,    0,    1,    0,    1,    0,    2,    2,    0,
    1,    1,    3,    3,    6,
};
short cl_func_defred[] = {                                     10,
    0,   14,   11,    0,   81,   82,   13,    0,    0,    0,
    0,   72,    0,   19,   20,   17,    0,    0,   74,   64,
   16,    0,   22,    0,   64,   18,    6,    1,    4,    2,
    5,    3,    8,    7,    9,    0,   77,   65,    0,   68,
    0,   84,    0,   83,    0,    0,    0,    0,   24,   64,
    0,   78,   76,   79,   57,   58,   45,   49,   50,   41,
   42,   51,   52,   53,   54,   43,   44,   48,   46,   59,
   39,   40,   29,   30,   31,   32,   33,   34,   35,   36,
   37,   38,   47,    0,    0,   28,   69,   60,    0,   77,
    0,   70,   55,   56,    0,   25,   85,    0,    0,   26,
    0,    0,   67,   62,   63,   61,
};
short cl_func_dgoto[] = {                                       1,
   37,    3,    7,    4,   16,   17,   18,   10,   48,   49,
   50,   24,   42,   47,   86,    8,   13,   25,   95,  102,
  106,   38,   51,   54,   43,
};
short cl_func_sindex[] = {                                      0,
 -219,    0,    0,  -27,    0,    0,    0, -263,  -16, -243,
 -212,    0, -211,    0,    0,    0, -236,    4,    0,    0,
    0, -212,    0, -163,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -59,    0,    0, -221,    0,
 -211,    0,   59,    0,  -33,  -58,   29,   13,    0,    0,
 -254,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   36,   -4,    0,    0,    0, -211,    0,
 -209,    0,    0,    0, -211,    0,    0,  -58,   38,    0,
 -201,  -51,    0,    0,    0,    0,
};
short cl_func_rindex[] = {                                      0,
  -39,    0,    0, -233,    0,    0,    0, -224,    0, -171,
   16,    0, -138,    0,    0,    0,    0,    0,    0,    0,
    0,   16,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -21,    0,    0,    0,    0,
  -29,    0,  -38,    0,    0,   70,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -189,    0,
    0,    0,    0,    0,  -43,    0,    0,   21,  -37,    0,
    0,    0,    0,    0,    0,    0,
};
short cl_func_gindex[] = {                                      0,
    0,    0,    0,    0,   93,    0,    0,    0,    0,   30,
  -10,    3,  -14,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 299
short cl_func_table[] = {                                      82,
   41,   87,   20,   77,   79,   75,   84,  105,   75,   73,
   70,   74,   83,   76,   23,   73,   77,   73,   12,   12,
   77,   66,   44,   75,   80,   80,   71,   39,   72,   80,
    6,    5,   23,   21,   21,   80,    2,    9,   21,   80,
   80,   80,   80,   11,   21,   80,   80,   22,   21,   21,
   21,   21,   91,   12,   21,   21,   89,   85,   77,   15,
   78,   19,   77,   80,   77,   23,   21,   80,   88,   80,
   45,  104,   21,   92,   90,   97,   93,   15,   21,   73,
   14,   46,   77,  100,   99,   66,   71,   71,   94,   15,
   80,   71,   81,   98,   27,   28,   53,   71,  101,   29,
   52,   71,   71,   71,   71,   30,  103,   71,   71,   27,
   31,   32,   33,   73,   26,   34,   35,    0,   96,   73,
   73,    0,    0,    0,   73,    0,    0,    0,    0,    0,
   73,   71,    0,    0,    0,   73,   73,   73,    0,   36,
   73,   73,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   73,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   12,   12,
    0,    0,    0,   12,    0,    0,    0,    0,    0,   12,
    0,    0,    0,   12,   12,   12,   12,    0,    0,   12,
   12,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   75,    0,   55,   56,   12,    0,    0,
    0,   12,    0,   12,   75,    0,    0,    0,   40,   40,
   77,    0,    0,   73,    0,    0,    0,    0,    0,    0,
    0,   77,   57,   58,   59,   60,   61,   62,   63,   64,
   65,   66,   67,    0,    0,   68,   69,    0,   69,
};
short cl_func_check[] = {                                      33,
   60,   60,   13,   37,   38,   44,   40,   59,   42,   43,
   44,   45,   46,   47,   44,   59,   38,   61,   58,   59,
   42,   59,   37,   62,  258,  259,   60,   25,   62,  263,
   58,   59,   62,  258,  259,  269,  256,  301,  263,  273,
  274,  275,  276,   60,  269,  279,  280,   44,  273,  274,
  275,  276,   50,  297,  279,  280,   44,   91,   38,   44,
   94,  273,   42,  297,   44,   62,  303,  301,   40,  303,
  292,  123,  297,  328,   62,   90,   41,   62,  303,  123,
  293,  303,   62,   98,   95,  123,  258,  259,   93,  302,
  124,  263,  126,  303,  258,  259,   38,  269,   61,  263,
   42,  273,  274,  275,  276,  269,  308,  279,  280,   40,
  274,  275,  276,  303,   22,  279,  280,   -1,   89,  258,
  259,   -1,   -1,   -1,  263,   -1,   -1,   -1,   -1,   -1,
  269,  303,   -1,   -1,   -1,  274,  275,  276,   -1,  303,
  279,  280,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  303,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,  259,
   -1,   -1,   -1,  263,   -1,   -1,   -1,   -1,   -1,  269,
   -1,   -1,   -1,  273,  274,  275,  276,   -1,   -1,  279,
  280,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  292,   -1,  289,  290,  297,   -1,   -1,
   -1,  301,   -1,  303,  303,   -1,   -1,   -1,  328,  328,
  292,   -1,   -1,  303,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  303,  316,  317,  318,  319,  320,  321,  322,  323,
  324,  325,  326,   -1,   -1,  329,  330,   -1,  328,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 345
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
"LE_UNION","LE_CONST","LE_FLOAT","LE_SHORT","LE_UNSIGNED","LE_CONTINUE",
"LE_FOR","LE_SIGNED","LE_VOID","LE_DEFAULT","LE_GOTO","LE_SIZEOF","LE_VOLATILE",
"LE_DO","LE_IF","LE_STATIC","LE_WHILE","LE_NEW","LE_DELETE","LE_THIS",
"LE_OPERATOR","LE_CLASS","LE_PUBLIC","LE_PROTECTED","LE_PRIVATE","LE_VIRTUAL",
"LE_FRIEND","LE_INLINE","LE_OVERLOAD","LE_TEMPLATE","LE_TYPENAME",
"LE_IDENTIFIER","LE_STRINGliteral","LE_FLOATINGconstant","LE_INTEGERconstant",
"LE_CHARACTERconstant","LE_OCTALconstant","LE_HEXconstant","LE_POUNDPOUND",
"LE_CComment","LE_CPPComment","LE_NAMESPACE","LE_USING","LE_TYPEDEFname",
"LE_ARROW","LE_ICR","LE_DECR","LE_LS","LE_RS","LE_LE","LE_GE","LE_EQ","LE_NE",
"LE_ANDAND","LE_OROR","LE_ELLIPSIS","LE_CLCL","LE_DOTstar","LE_ARROWstar",
"LE_MULTassign","LE_DIVassign","LE_MODassign","LE_PLUSassign","LE_MINUSassign",
"LE_LSassign","LE_RSassign","LE_ANDassign","LE_ERassign","LE_ORassign",
"LE_MACRO","LE_DYNAMIC_CAST","LE_STATIC_CAST","LE_CONST_CAST",
"LE_REINTERPRET_CAST",
};
char *cl_func_rule[] = {
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
"func_name : LE_IDENTIFIER",
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
"function_decl : stmnt_starter opt_template_qualifier virtual_spec const_spec variable_decl nested_scope_specifier func_name '(' $$2 const_spec opt_pure_virtual func_postfix",
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
void get_functions(const std::string &in, FunctionList &li, const std::map<std::string, bool> &ignoreTokens)
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
case 12:
{curr_func.Reset();}
break;
case 14:
{ 
						/*printf("CodeLite: syntax error, unexpected token '%s' found\n", cl_func_lval.c_str());*/
					}
break;
case 15:
{ yyval = "";}
break;
case 16:
{yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 17:
{ yyval = yyvsp[0]; }
break;
case 18:
{ yyval = yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0]; }
break;
case 19:
{ yyval = yyvsp[0]; }
break;
case 20:
{ yyval = yyvsp[0]; }
break;
case 22:
{ yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 23:
{yyval = "";}
break;
case 24:
{yyval = yyvsp[0];}
break;
case 25:
{yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 26:
{yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1] +yyvsp[0];}
break;
case 27:
{yyval = yyvsp[0];}
break;
case 28:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 60:
{func_consumeFuncArgList();}
break;
case 61:
{
						/*trim down trailing '::' from scope name*/
						yyvsp[-6].erase(yyvsp[-6].find_last_not_of(":")+1);
						curr_func.m_name = yyvsp[-5];
						curr_func.m_scope = yyvsp[-6];
						curr_func.m_retrunValusConst = yyvsp[-8];
						curr_func.m_lineno = cl_scope_lineno;
						if(g_funcs)
						{
							g_funcs->push_back(curr_func);
						}
						curr_func.Reset();
					}
break;
case 64:
{yyval = "";}
break;
case 65:
{	yyval = yyvsp[-1] + yyvsp[0];}
break;
case 68:
{yyval = yyvsp[-1]+ yyvsp[0];}
break;
case 69:
{func_consumeTemplateDecl();}
break;
case 70:
{yyval = yyvsp[-3] + yyvsp[0];}
break;
case 71:
{yyval = ""; }
break;
case 72:
{ yyval = yyvsp[0]; }
break;
case 73:
{yyval = ""; }
break;
case 74:
{ yyval = yyvsp[0]; }
break;
case 75:
{yyval = ""; }
break;
case 76:
{ yyval = yyvsp[0]; }
break;
case 77:
{yyval = ""; }
break;
case 78:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 79:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 80:
{yyval = "";}
break;
case 81:
{ yyval = ";";}
break;
case 82:
{ yyval = ":";}
break;
case 83:
{
								yyvsp[-2].erase(yyvsp[-2].find_last_not_of(":")+1);
								curr_func.m_returnValue.m_type = yyvsp[-1];
								curr_func.m_returnValue.m_typeScope = yyvsp[-2]; 
								curr_func.m_returnValue.m_starAmp = yyvsp[0];
								curr_func.m_returnValue.m_isPtr = (yyvsp[0].find("*") != (size_t)-1);
								yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];
							}
break;
case 84:
{
								yyvsp[-2].erase(yyvsp[-2].find_last_not_of(":")+1);
								curr_func.m_returnValue.m_type = yyvsp[-1];
								curr_func.m_returnValue.m_typeScope = yyvsp[-2];
								curr_func.m_returnValue.m_starAmp = yyvsp[0];
								curr_func.m_returnValue.m_isPtr = (yyvsp[0].find("*") != (size_t)-1);
								yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;
							}
break;
case 85:
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
