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
short cl_func_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    0,    4,    2,    2,    5,    5,    7,    7,    6,    6,
    8,    8,    9,    9,    9,   10,   14,   14,   14,   15,
   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,
   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,
   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,
   19,    3,   20,   20,   20,   22,   22,   12,   12,   21,
   21,   23,   24,   23,   17,   17,   11,   11,   25,   25,
   26,   26,   13,   16,   16,   16,   18,   18,   18,
};
short cl_func_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    2,    0,    2,    1,    0,    2,    1,    3,    1,    1,
    0,    4,    0,    1,    3,    4,    1,    2,    2,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    2,    2,    1,    1,    1,
    0,   13,    0,    6,    5,    1,    1,    0,    2,    0,
    2,    2,    0,    4,    0,    1,    0,    1,    0,    1,
    0,    2,    2,    0,    1,    1,    3,    3,    6,
};
short cl_func_defred[] = {                                     10,
    0,   14,   11,    0,   85,   86,   13,    0,    0,    0,
    0,   76,    0,   19,   20,   17,    0,    0,   78,   68,
   16,    0,   22,    0,   68,   18,    6,    1,    4,    2,
    5,    3,    8,    7,    9,    0,   81,   69,    0,   72,
    0,   88,    0,   87,    0,    0,    0,    0,    0,   24,
   68,    0,   82,   80,   83,   58,   59,   46,   50,   51,
   42,   43,   52,   53,   54,   55,   44,   45,   49,   47,
   60,   40,   41,   38,   30,   31,   32,   33,   34,   35,
   36,   37,   39,   48,    0,    0,   29,   73,   28,   61,
    0,   81,    0,   74,   56,   57,    0,   25,   89,    0,
    0,   26,    0,    0,    0,    0,    0,   81,    0,   71,
   66,   67,   62,    0,    0,   65,    0,   64,
};
short cl_func_dgoto[] = {                                       1,
   37,    3,    7,    4,   16,   17,   18,   10,   49,   50,
   51,   24,   42,   48,   87,    8,   13,   25,   97,  104,
  107,  113,   38,   52,   55,   43,
};
short cl_func_sindex[] = {                                      0,
 -231,    0,    0,   25,    0,    0,    0, -259,   13, -237,
 -255,    0, -196,    0,    0,    0, -226,  -18,    0,    0,
    0, -255,    0, -144,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -59,    0,    0, -118,    0,
 -196,    0,   -1,    0,  -33,  -58, -220,   46,    4,    0,
    0, -243,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   49,    6,    0,    0,    0,    0,
 -196,    0, -211,    0,    0,    0, -196,    0,    0,  -58,
 -205,    0,   60,   47, -117, -201,  -28,    0, -195,    0,
    0,    0,    0,   72,  -58,    0,   75,    0,
};
short cl_func_rindex[] = {                                      0,
  -37,    0,    0, -223,    0,    0,    0, -152,    0, -204,
   18,    0, -125,    0,    0,    0,    0,    0,    0,    0,
    0,   18,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -22,    0,    0,    0,    0,
  -29,    0,  -38,    0,    0,   78,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -185,    0,    0,    0,    0,    0,  -42,    0,    0,    1,
  -31,    0,    0,  -27, -179,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   26,    0,    0,    0,
};
short cl_func_gindex[] = {                                      0,
   24,    0,    0,    0,  115,    0,    0,    0,    0,   48,
    5,   -2,   -3,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 301
short cl_func_table[] = {                                      83,
   41,   88,   79,   79,   81,   79,   85,   47,   77,   75,
   71,   76,   84,   78,   23,   81,   77,   20,   77,   81,
   12,   12,   39,   79,    2,   22,   72,   63,   73,   63,
  112,   70,   23,   44,   84,   84,   54,   14,   81,   84,
   53,    9,   81,   23,   81,   84,   15,   91,   93,   84,
   84,   84,   84,   75,   75,   84,   84,   86,   75,   12,
   80,   15,   81,   81,   75,   92,   81,   81,   75,   75,
   75,   75,   11,   84,   75,   75,   19,   84,   21,   15,
   77,   84,    6,    5,   89,   90,   94,   79,   99,   95,
   82,   63,   74,  100,  111,   70,  102,  103,   96,  105,
   75,  101,  109,   81,  114,   21,   21,  106,  110,  115,
   21,  117,  116,   27,   28,  118,   21,   27,   29,   77,
   21,   21,   21,   21,   30,   68,   21,   21,  108,   31,
   32,   33,   77,   77,   34,   35,   26,   77,   98,    0,
   27,   28,    0,   77,   21,   29,    0,    0,   77,   77,
   77,   30,   21,   77,   77,    0,   31,   32,   33,    0,
   36,   34,   35,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   45,    0,    0,    0,    0,    0,   77,
    0,    0,    0,    0,    0,    0,   46,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   12,   12,    0,    0,    0,   12,    0,    0,    0,    0,
    0,   12,    0,    0,    0,   12,   12,   12,   12,    0,
    0,   12,   12,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   79,    0,   56,   57,    0,    0,   12,
   77,    0,    0,   12,    0,    0,   79,   12,    0,   81,
   40,   40,    0,    0,    0,   77,    0,    0,    0,    0,
    0,    0,   81,    0,   58,   59,   60,   61,   62,   63,
   64,   65,   66,   67,   68,    0,    0,   69,   70,    0,
   73,
};
short cl_func_check[] = {                                      33,
   60,   60,   41,   37,   38,   44,   40,  126,   42,   43,
   44,   45,   46,   47,   44,   38,   59,   13,   61,   42,
   58,   59,   25,   62,  256,   44,   60,   59,   62,   61,
   59,   59,   62,   37,  258,  259,   38,  293,   38,  263,
   42,  301,   42,   62,   44,  269,  302,   44,   51,  273,
  274,  275,  276,  258,  259,  279,  280,   91,  263,  297,
   94,   44,   62,   38,  269,   62,   41,   42,  273,  274,
  275,  276,   60,  297,  279,  280,  273,  301,  305,   62,
  123,  305,   58,   59,  305,   40,  330,  126,   92,   41,
  124,  123,  126,  305,  123,  123,  100,  303,   93,   40,
  305,   97,  105,  126,  108,  258,  259,   61,  310,  305,
  263,  115,   41,  258,  259,   41,  269,   40,  263,  305,
  273,  274,  275,  276,  269,  305,  279,  280,  105,  274,
  275,  276,  258,  259,  279,  280,   22,  263,   91,   -1,
  258,  259,   -1,  269,  297,  263,   -1,   -1,  274,  275,
  276,  269,  305,  279,  280,   -1,  274,  275,  276,   -1,
  305,  279,  280,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  292,   -1,   -1,   -1,   -1,   -1,  305,
   -1,   -1,   -1,   -1,   -1,   -1,  305,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  258,  259,   -1,   -1,   -1,  263,   -1,   -1,   -1,   -1,
   -1,  269,   -1,   -1,   -1,  273,  274,  275,  276,   -1,
   -1,  279,  280,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  292,   -1,  289,  290,   -1,   -1,  297,
  303,   -1,   -1,  301,   -1,   -1,  305,  305,   -1,  292,
  330,  330,   -1,   -1,   -1,  305,   -1,   -1,   -1,   -1,
   -1,   -1,  305,   -1,  318,  319,  320,  321,  322,  323,
  324,  325,  326,  327,  328,   -1,   -1,  331,  332,   -1,
  330,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 347
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
"function_decl : stmnt_starter opt_template_qualifier virtual_spec const_spec variable_decl nested_scope_specifier func_name '(' $$2 const_spec declare_throw opt_pure_virtual func_postfix",
"declare_throw :",
"declare_throw : LE_THROW '(' nested_scope_specifier LE_IDENTIFIER special_star_amp ')'",
"declare_throw : LE_THROW '(' basic_type_name special_star_amp ')'",
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
case 29:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 61:
{func_consumeFuncArgList();}
break;
case 62:
{
						/*trim down trailing '::' from scope name*/
						yyvsp[-7].erase(yyvsp[-7].find_last_not_of(":")+1);
						curr_func.m_isVirtual = yyvsp[-10].find("virtual") != std::string::npos;
						curr_func.m_isPureVirtual = yyvsp[-1].find("=") != std::string::npos;
						curr_func.m_isConst = yyvsp[-3].find("const") != std::string::npos;
						curr_func.m_name = yyvsp[-6];
						curr_func.m_scope = yyvsp[-7];
						curr_func.m_retrunValusConst = yyvsp[-9];
						curr_func.m_lineno = cl_scope_lineno;
						if(g_funcs)
						{
							g_funcs->push_back(curr_func);
						}
						curr_func.Reset();
					}
break;
case 63:
{yyval = "";}
break;
case 64:
{yyval = yyvsp[-3] + yyvsp[-2] + yyvsp[-1];}
break;
case 65:
{yyval = yyvsp[-2] + yyvsp[-1];}
break;
case 68:
{yyval = "";}
break;
case 69:
{	yyval = yyvsp[-1] + yyvsp[0];}
break;
case 70:
{yyval = "";}
break;
case 71:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 72:
{yyval = yyvsp[-1]+ yyvsp[0];}
break;
case 73:
{func_consumeTemplateDecl();}
break;
case 74:
{yyval = yyvsp[-3] + yyvsp[0];}
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
{ yyval = yyvsp[0]; }
break;
case 79:
{yyval = ""; }
break;
case 80:
{ yyval = yyvsp[0]; }
break;
case 81:
{yyval = ""; }
break;
case 82:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 83:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 84:
{yyval = "";}
break;
case 85:
{ yyval = ";";}
break;
case 86:
{ yyval = ":";}
break;
case 87:
{
								yyvsp[-2].erase(yyvsp[-2].find_last_not_of(":")+1);
								curr_func.m_returnValue.m_type = yyvsp[-1];
								curr_func.m_returnValue.m_typeScope = yyvsp[-2]; 
								curr_func.m_returnValue.m_starAmp = yyvsp[0];
								curr_func.m_returnValue.m_isPtr = (yyvsp[0].find("*") != (size_t)-1);
								yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];
							}
break;
case 88:
{
								yyvsp[-2].erase(yyvsp[-2].find_last_not_of(":")+1);
								curr_func.m_returnValue.m_type = yyvsp[-1];
								curr_func.m_returnValue.m_typeScope = yyvsp[-2];
								curr_func.m_returnValue.m_starAmp = yyvsp[0];
								curr_func.m_returnValue.m_isPtr = (yyvsp[0].find("*") != (size_t)-1);
								yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]  ;
							}
break;
case 89:
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
