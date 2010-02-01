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
static  clTypedefList            gs_typedefs;
static  clTypedef                gs_currentTypedef;

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
#define YYERRCODE 256
short cl_var_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    2,    2,    2,    2,    2,    0,    0,    5,    3,    3,
    3,    6,    7,    8,   10,   10,   10,   11,   11,   11,
    4,    4,    4,    4,    4,    4,   20,   20,   16,   16,
   19,   19,   18,   18,   18,   18,   17,   17,   17,   17,
   17,   21,   21,   13,   13,   12,   12,   12,   12,   12,
   22,   22,   23,   23,   14,   15,   15,   15,   15,   15,
    9,    9,    9,   24,    9,    9,
};
short cl_var_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    2,    2,    2,    2,    1,    0,    2,    0,    2,    1,
    1,    4,    1,    1,    0,    1,    3,    4,    4,    7,
    6,    6,    6,    5,    4,    3,    1,    1,    1,    4,
    1,    1,    0,    1,    1,    1,    1,    1,    1,    1,
    1,    2,    5,    0,    2,    0,    1,    2,    2,    3,
    0,    1,    0,    2,    2,    0,    1,    1,    1,    1,
    2,    3,    6,    0,    6,    4,
};
short cl_var_defred[] = {                                     16,
    0,   21,    0,   17,    0,   20,    0,    0,   23,    0,
   67,    0,    0,   68,   69,   70,   19,    0,    0,    0,
   59,   24,    0,    6,    1,   54,    0,    2,    5,    3,
    0,   10,    0,    9,   15,   71,    0,   63,   63,   63,
    0,   60,   22,    0,   14,   13,    4,    8,    7,   11,
   12,    0,   55,    0,    0,    0,    0,   36,    0,   52,
    0,   41,   42,    0,   35,   64,   62,   65,    0,    0,
    0,   74,    0,   26,   54,    0,    0,   34,   39,    0,
    0,   75,    0,    0,    0,   45,   44,   46,   33,   32,
   47,   63,   50,   49,   48,   51,   31,    0,   27,   53,
    0,   63,    0,    0,   28,   29,   40,    0,    0,   30,
};
short cl_var_dgoto[] = {                                       1,
   35,   36,    4,   17,    5,    6,    8,   23,    9,   73,
   74,   10,   37,   54,   18,   80,   97,   89,   65,   19,
   53,   68,   55,   82,
};
short cl_var_sindex[] = {                                      0,
 -222,    0, -262,    0,   26,    0,  -29, -286,    0,  165,
    0, -262, -262,    0,    0,    0,    0, -262, -295,    5,
    0,    0,    8,    0,    0,    0, -219,    0,    0,    0,
  -70,    0,  -70,    0,    0,    0, -234,    0,    0,    0,
   32,    0,    0, -226,    0,    0,    0,    0,    0,    0,
    0,  -58,    0,  -27,   54, -262, -262,    0,  -60,    0,
 -262,    0,    0, -225,    0,    0,    0,    0,  -34, -220,
 -262,    0,  -12,    0,    0,  -13,  -13,    0,    0,   74,
   12,    0, -262, -244,  114,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -244,    0,    0,
  -52,    0, -212, -262,    0,    0,    0,   13, -244,    0,
};
short cl_var_rindex[] = {                                      0,
   39,    0,  105,    0,  -79,    0,  -38,    0,    0, -211,
    0, -170, -106,    0,    0,    0,    0,  105,    0,   72,
    0,    0,    0,    0,    0,    0,  -26,    0,    0,    0,
  -19,    0,  -11,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   11,    0, -204,  -20,  -15, -204,    0,   20,    0,
   81,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   81,    0,    0,    0,    0,    1,    1,    0,    0,    0,
    0,    0,  138,   27,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   79,    0,    0,   81,    0,    0,    0,    0,   79,    0,
};
short cl_var_gindex[] = {                                      0,
   89,   18,    0,    0,    0,    0,    0,    0,   64,  -67,
   29,   73,  -21,   -1,    0,    0,    0,   42,   40,    0,
    0,    0,    0,    0,
};
#define YYTABLESIZE 446
short cl_var_table[] = {                                      71,
   43,   61,   57,   81,   44,   57,   63,  104,   21,   62,
    7,    4,   20,   63,    4,    4,   62,    4,    8,   22,
   61,    8,    8,   61,    8,   56,    7,   86,   56,    7,
    7,   83,    7,    2,   41,    4,  108,   56,   57,   45,
   43,   61,    8,   46,   43,    3,   42,   87,   72,   84,
    7,   72,   72,   85,   72,   83,   83,   76,   43,   43,
   76,   76,   72,   76,   73,   13,   43,   73,   73,   12,
   73,   52,   58,   98,  109,   38,   39,   88,   18,   59,
   76,   40,   18,   16,   11,   79,  100,   56,   56,   56,
  103,   67,   56,  107,   54,   66,   18,   18,   56,  105,
  106,   56,  102,   56,   56,   56,   56,  110,   78,   56,
   56,   99,   58,   93,   94,   58,   63,   92,   90,   50,
   63,   51,   63,   43,   25,   43,   64,    0,   69,   70,
    0,    0,   91,   75,   95,   56,    0,    0,    0,    0,
   63,    0,   25,   75,    0,    0,    0,    0,   14,    0,
   15,   56,   56,   56,    0,   75,   56,    0,    0,   38,
    0,   18,   56,   18,   96,    0,    0,   56,   56,   56,
   56,    0,    0,   56,   56,    0,   75,    0,   66,   66,
   66,    0,    0,   66,    0,    0,    0,   24,   25,   66,
    0,    0,   47,   66,   66,   66,   66,   66,   28,   56,
   66,   66,    0,   29,   30,   48,   32,    0,    0,   49,
   34,    0,    0,    0,    0,    0,    0,    0,    0,   57,
   57,   57,    0,   37,   57,    0,   66,    0,    0,    0,
   57,    0,    0,    0,    0,   57,   57,   57,   57,    0,
    0,   57,   57,    0,    0,    7,    4,    0,    0,    0,
    0,    0,   61,    8,    0,    0,   43,    0,   43,   43,
   43,    7,    0,   43,    0,    0,    0,   57,   43,   43,
   60,   77,   60,   43,   43,   43,   43,   43,   60,    4,
   43,   43,    0,   72,    0,   61,    8,    0,    0,    0,
   56,    0,   76,    0,    7,    0,   18,   18,   18,   73,
    0,   18,    0,    0,    0,    0,   43,   18,    0,    0,
    0,   18,   18,   18,   18,   18,   72,    0,   18,   18,
    0,    0,    0,    0,    0,   76,    0,    0,    0,   58,
   58,   58,   73,    0,   58,    0,    0,    0,   56,   56,
   58,    0,    0,   56,   18,   58,   58,   58,   58,   56,
    0,   58,   58,    0,   56,   56,   56,   56,    0,    0,
   56,   56,   56,   56,   56,    0,    0,   56,    0,    0,
    0,   24,   25,   56,    0,    0,   27,   58,   56,   56,
   56,   56,   28,    0,   56,   56,   56,   29,   30,   31,
   32,    0,    0,   33,   34,   56,   56,    0,    0,    0,
   56,    0,    0,    0,    0,    0,   56,    0,    0,    0,
   56,   56,   56,   56,   56,    0,    0,   56,   56,  101,
    0,    0,   24,   25,   26,    0,    0,   27,    0,    0,
    0,    0,    0,   28,    0,    0,    0,    0,   29,   30,
   31,   32,    0,   56,   33,   34,
};
short cl_var_check[] = {                                      60,
    0,   60,   41,   71,   26,   44,   41,   60,   38,   44,
  273,   38,   42,   41,   41,   42,   44,   44,   38,  306,
   41,   41,   42,   44,   44,   41,   38,   41,   44,   41,
   42,   44,   44,  256,  330,   62,  104,   39,   40,  259,
   40,   62,   62,  263,   44,  268,   42,   61,   38,   62,
   62,   41,   42,   75,   44,   44,   44,   38,   58,   59,
   41,   42,  123,   44,   38,   40,   59,   41,   42,   44,
   44,  306,   41,   62,   62,   12,   13,   91,   40,  306,
  306,   18,   44,   58,   59,  306,  331,  258,  259,  260,
   92,   38,  263,  306,  306,   42,   58,   59,  269,  101,
  102,  306,   85,  274,  275,  276,  277,  109,   69,  280,
  281,   83,   41,   40,   41,   44,   38,   44,   77,   31,
   42,   33,   44,  123,   44,  125,   54,   -1,   56,   57,
   -1,   -1,   59,   61,   61,  306,   -1,   -1,   -1,   -1,
   62,   -1,   62,   71,   -1,   -1,   -1,   -1,  123,   -1,
  125,  258,  259,  260,   -1,   83,  263,   -1,   -1,  330,
   -1,  123,  269,  125,   91,   -1,   -1,  274,  275,  276,
  277,   -1,   -1,  280,  281,   -1,  104,   -1,  258,  259,
  260,   -1,   -1,  263,   -1,   -1,   -1,  258,  259,  269,
   -1,   -1,  263,  273,  274,  275,  276,  277,  269,  306,
  280,  281,   -1,  274,  275,  276,  277,   -1,   -1,  280,
  281,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,
  259,  260,   -1,  330,  263,   -1,  306,   -1,   -1,   -1,
  269,   -1,   -1,   -1,   -1,  274,  275,  276,  277,   -1,
   -1,  280,  281,   -1,   -1,  273,  273,   -1,   -1,   -1,
   -1,   -1,  273,  273,   -1,   -1,  256,   -1,  258,  259,
  260,  273,   -1,  263,   -1,   -1,   -1,  306,  268,  269,
  331,  306,  331,  273,  274,  275,  276,  277,  331,  306,
  280,  281,   -1,  273,   -1,  306,  306,   -1,   -1,   -1,
  306,   -1,  273,   -1,  306,   -1,  258,  259,  260,  273,
   -1,  263,   -1,   -1,   -1,   -1,  306,  269,   -1,   -1,
   -1,  273,  274,  275,  276,  277,  306,   -1,  280,  281,
   -1,   -1,   -1,   -1,   -1,  306,   -1,   -1,   -1,  258,
  259,  260,  306,   -1,  263,   -1,   -1,   -1,  258,  259,
  269,   -1,   -1,  263,  306,  274,  275,  276,  277,  269,
   -1,  280,  281,   -1,  274,  275,  276,  277,   -1,   -1,
  280,  281,  258,  259,  260,   -1,   -1,  263,   -1,   -1,
   -1,  258,  259,  269,   -1,   -1,  263,  306,  274,  275,
  276,  277,  269,   -1,  280,  281,  306,  274,  275,  276,
  277,   -1,   -1,  280,  281,  258,  259,   -1,   -1,   -1,
  263,   -1,   -1,   -1,   -1,   -1,  269,   -1,   -1,   -1,
  306,  274,  275,  276,  277,   -1,   -1,  280,  281,  306,
   -1,   -1,  258,  259,  260,   -1,   -1,  263,   -1,   -1,
   -1,   -1,   -1,  269,   -1,   -1,   -1,   -1,  274,  275,
  276,  277,   -1,  306,  280,  281,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 348
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
"LE_REINTERPRET_CAST",
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
"basic_type_name : LE_UNSIGNED basic_type_name_inter",
"basic_type_name : LE_SIGNED basic_type_name_inter",
"basic_type_name : LE_LONG LE_LONG",
"basic_type_name : LE_LONG LE_INT",
"basic_type_name : basic_type_name_inter",
"translation_unit :",
"translation_unit : translation_unit external_decl",
"$$1 :",
"external_decl : $$1 variables",
"external_decl : typedefs",
"external_decl : error",
"typedefs : LE_TYPEDEF real_type new_name ';'",
"real_type : variable_decl",
"new_name : LE_IDENTIFIER",
"parameter_list :",
"parameter_list : template_parameter",
"parameter_list : parameter_list ',' template_parameter",
"template_parameter : const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp",
"template_parameter : const_spec nested_scope_specifier basic_type_name special_star_amp",
"template_parameter : const_spec nested_scope_specifier LE_IDENTIFIER '<' parameter_list '>' special_star_amp",
"variables : stmnt_starter variable_decl special_star_amp const_spec variable_name_list postfix",
"variables : '(' variable_decl special_star_amp const_spec LE_IDENTIFIER postfix2",
"variables : ',' variable_decl special_star_amp const_spec LE_IDENTIFIER postfix2",
"variables : '(' variable_decl special_star_amp const_spec postfix3",
"variables : ',' variable_decl special_star_amp postfix3",
"variables : ellipsis_prefix LE_ELLIPSIS ')'",
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
"$$2 :",
"variable_decl : const_spec LE_STRUCT nested_scope_specifier LE_IDENTIFIER '{' $$2",
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
	gs_typedefs.clear();
	
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

// return the scope name at the end of the input string
void get_typedefs(const std::string &in, clTypedefList &li)
{
	std::map<std::string, std::string> dummy;
	
    // provide the lexer with new input
	if( !setLexerInput(in, dummy) ){
    	return;
    }
	
	// set the parser local output to our variable list
	cl_var_parse();
	li = gs_typedefs;

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
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 14:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 15:
{ yyval = yyvsp[0]; }
break;
case 18:
{curr_var.Reset(); gs_names.clear(); s_tmpString.clear(); gs_currentTypedef.clear(); }
break;
case 21:
{
                            yyclearin;    /*clear lookahead token*/
                            yyerrok;
                            /*printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_var_lval.c_str(), cl_scope_lineno);*/
                            }
break;
case 22:
{
							gs_typedefs.push_back(gs_currentTypedef);
						}
break;
case 23:
{ 
				gs_currentTypedef.m_realType = curr_var;
			}
break;
case 24:
{ gs_currentTypedef.m_name = yyvsp[0]; }
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
{
                        	if(gs_vars)
                            {
                            	Variable var;
                            	std::string pattern;
                            	curr_var.m_isPtr = (yyvsp[-3].find("*") != (size_t)-1);
                            	curr_var.m_starAmp = yyvsp[-3];
                            	curr_var.m_lineno = cl_scope_lineno;
                            	for(size_t i=0; i< gs_names.size(); i++)
                                {
                                    /*create new variable for every variable name found*/
                                	var = curr_var;
                                	var.m_pattern = "/^" + yyvsp[-5] + " " + yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] +  " " + gs_names.at(i) + " $/";
                                	var.m_name = gs_names.at(i);
                                	gs_vars->push_back(var);
                                }
                            	curr_var.Reset();
                            	gs_names.clear();
                            }
                        }
break;
case 32:
{
                        	if(gs_vars)
                            {
                            	Variable var;
                            	std::string pattern;
                            	curr_var.m_pattern       = "/^";
                            	curr_var.m_pattern       += yyvsp[-5] + " " + yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0] + "$/";
                            	curr_var.m_isPtr         = (yyvsp[-3].find("*") != (size_t)-1);
                            	curr_var.m_starAmp       = yyvsp[-3];
                            	curr_var.m_arrayBrackets = yyvsp[0];
                            	curr_var.m_lineno        = cl_scope_lineno;
								/*create new variable for every variable name found*/
                                var = curr_var;
                            	var.m_name               = yyvsp[-1];
                            	gs_vars->push_back(var);
                            	curr_var.Reset();
                            	gs_names.clear();
                            }
                        }
break;
case 33:
{
                        	if(gs_vars && g_isUsedWithinFunc)
                            {
                            	Variable var;
                            	std::string pattern;
                            	curr_var.m_pattern       = "/^";
                            	curr_var.m_pattern      += yyvsp[-5] + " " + yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0] + " $/";
                            	curr_var.m_isPtr         = (yyvsp[-3].find("*") != (size_t)-1);
                            	curr_var.m_starAmp       = yyvsp[-3];
                            	curr_var.m_arrayBrackets = yyvsp[0];
                            	curr_var.m_lineno        = cl_scope_lineno;

                                /*create new variable for every variable name found*/
                            	var = curr_var;
                            	var.m_name = yyvsp[-1];
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
                            	curr_var.m_pattern = "/^";
                            	curr_var.m_pattern += yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1] + " $/";
                            	curr_var.m_isPtr = (yyvsp[-2].find("*") != (size_t)-1);
                            	curr_var.m_starAmp = yyvsp[-2];
                            	curr_var.m_lineno = cl_scope_lineno;

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
case 35:
{
                        	if(gs_vars && g_isUsedWithinFunc)
                            {
                            	Variable var;
                            	std::string pattern;
                            	curr_var.m_pattern = "/^";
                            	curr_var.m_pattern += yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1] + " " + "$/";
                            	curr_var.m_isPtr = (yyvsp[-1].find("*") != (size_t)-1);
                            	curr_var.m_starAmp = yyvsp[-1];
                            	curr_var.m_lineno = cl_scope_lineno;

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
                            /* special type of argument: Ellipsis, can only be at the end of function argument */
                        	if(gs_vars && g_isUsedWithinFunc)
                            {
                            	Variable var;
                            	std::string pattern;
                            	curr_var.m_pattern = "/^";
                            	curr_var.m_pattern += yyvsp[-2] + " " + yyvsp[-1] + " " + yyvsp[0] + " " + "$/";
                            	curr_var.m_isPtr = false;
                            	curr_var.m_starAmp = "";
                            	curr_var.m_lineno = cl_scope_lineno;
                            	curr_var.m_isEllipsis = true;

                            	var = curr_var;
                            	gs_vars->push_back(var);

                            	curr_var.Reset();
                            	gs_names.clear();
                            }
                        }
break;
case 37:
{yyval = yyvsp[0];}
break;
case 38:
{yyval = yyvsp[0];}
break;
case 39:
{
                        	gs_names.push_back(yyvsp[0]);
                            yyval = yyvsp[0];
                        }
break;
case 40:
{
                            /*collect all the names*/
                        	gs_names.push_back(yyvsp[0]);
                            yyval = yyvsp[-3] + yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];
                        }
break;
case 43:
{yyval = "";}
break;
case 44:
{var_consumeDefaultValue(',', ')'); yyval = ""; }
break;
case 45:
{ yyval = ""; }
break;
case 46:
{ yyval = yyvsp[0] + var_consumBracketsContent('[');}
break;
case 50:
{ yyval = yyvsp[0] + var_consumBracketsContent('(');}
break;
case 51:
{ yyval = yyvsp[0] + var_consumBracketsContent('[');}
break;
case 52:
{yyval = yyvsp[-1]+ yyvsp[0]; }
break;
case 53:
{yyval = yyvsp[-4] + yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 54:
{yyval = "";}
break;
case 55:
{    yyval = yyvsp[-1] + yyvsp[0];}
break;
case 56:
{yyval = "";                          }
break;
case 57:
{ yyval = yyvsp[0];                         }
break;
case 58:
{ yyval = yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;     }
break;
case 59:
{ yyval = yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;     }
break;
case 60:
{ yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;}
break;
case 61:
{yyval = ""; }
break;
case 62:
{ yyval = yyvsp[0]; }
break;
case 63:
{yyval = ""; }
break;
case 64:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 65:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 66:
{yyval = "";}
break;
case 67:
{ yyval = ";";}
break;
case 68:
{ yyval = "{";}
break;
case 69:
{ yyval = "}";}
break;
case 70:
{ yyval = ":";}
break;
case 71:
{
                            yyval = yyvsp[-1] + " " + yyvsp[0];
                            yyvsp[0].erase(yyvsp[0].find_last_not_of(":")+1);
                        	curr_var.m_type = yyvsp[0];
							curr_var.m_isBasicType = true;
                        	curr_var.m_isConst = !yyvsp[-1].empty();
                        }
break;
case 72:
{
							yyval = yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];
                            yyvsp[-1].erase(yyvsp[-1].find_last_not_of(":")+1);
                        	curr_var.m_typeScope = yyvsp[-1];
                        	curr_var.m_type = yyvsp[0];
                        	curr_var.m_isConst = !yyvsp[-2].empty();
                        	s_tmpString.clear();
                        }
break;
case 73:
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
case 74:
{s_tmpString = var_consumBracketsContent('{');}
break;
case 75:
{
                            yyval = yyvsp[-5] + " " + yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] + yyvsp[-1] + yyvsp[0] + s_tmpString;
                            yyvsp[-3].erase(yyvsp[-3].find_last_not_of(":")+1);
                        	curr_var.m_typeScope = yyvsp[-3];
                        	curr_var.m_type = yyvsp[-2];
                        	curr_var.m_isTemplate = false;
                        	curr_var.m_isConst = !yyvsp[-5].empty();
                        	s_tmpString.clear();
                        }
break;
case 76:
{
                            yyval = yyvsp[-4];
                            yyvsp[-1].erase(yyvsp[-1].find_last_not_of(":")+1);
                        	curr_var.m_typeScope = yyvsp[-1];
                        	curr_var.m_type = yyvsp[0];
                        	curr_var.m_isTemplate = false;
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
