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
short cl_var_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    2,    2,    2,    2,    2,    0,    0,    5,
    3,    3,    3,    6,    6,    6,    7,    7,    7,   12,
   13,   16,    4,   19,    4,    4,    4,    4,    4,    4,
    4,   14,   14,   14,   14,   24,   24,   15,   15,   23,
   23,   26,   18,   27,   18,   22,   22,   21,   21,   21,
   21,   20,   20,   20,   20,   20,   25,   25,    9,    9,
   28,   28,    8,    8,    8,    8,    8,    8,    8,   29,
   29,   30,   30,   31,   31,   10,   11,   11,   11,   11,
   11,   17,   17,   17,   17,
};
short cl_var_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    2,    2,    1,    2,    2,    0,    2,    0,
    2,    3,    1,    0,    1,    3,    4,    4,    7,    0,
    0,    0,   13,    0,    8,    6,    6,    6,    5,    5,
    3,    0,    1,    2,    2,    1,    2,    1,    3,    1,
    1,    0,    2,    0,    5,    1,    1,    0,    1,    1,
    1,    1,    1,    1,    1,    1,    2,    5,    0,    2,
    0,    1,    0,    2,    2,    2,    3,    1,    2,    0,
    1,    0,    1,    0,    2,    3,    0,    1,    1,    1,
    1,    2,    3,    6,    4,
};
short cl_var_defred[] = {                                     18,
    0,   23,    0,   19,    0,    6,    1,    0,    2,    5,
    0,    0,   10,    0,    9,   12,   11,   15,    0,   88,
    0,   89,    0,   90,   91,   21,    0,    0,   17,   16,
    4,    3,    8,    7,   13,   14,   22,    0,    0,   78,
    0,   84,   84,    0,   84,    0,   79,   72,    0,   76,
   74,   69,   92,    0,    0,    0,    0,    0,    0,   41,
   77,    0,    0,   70,    0,   85,   81,    0,    0,   30,
    0,    0,   67,    0,    0,   56,   57,   40,   83,   86,
    0,   39,   31,    0,    0,   52,    0,    0,    0,   25,
   69,   59,   60,   61,   38,   37,    0,   62,   84,   63,
   34,   65,   64,   66,   36,   53,   47,    0,    0,    0,
    0,    0,    0,    0,    0,   26,   68,    0,   84,    0,
   54,   35,    0,   27,   28,   43,    0,    0,    0,   55,
    0,   44,   45,   32,    0,    0,   29,   48,    0,   33,
    0,   49,
};
short cl_var_dgoto[] = {                                       1,
   18,   19,    4,   26,    5,   89,   90,   91,   54,   55,
   27,   83,   97,  129,  139,  136,   42,   85,  114,  105,
   95,   78,   28,   86,   64,  106,  130,   51,   68,   80,
   56,
};
short cl_var_sindex[] = {                                      0,
  -26,    0, -219,    0,  -23,    0,    0, -219,    0,    0,
 -219,  274,    0,  274,    0,    0,    0,    0,  -48,    0,
 -168,    0, -168,    0,    0,    0,   94, -289,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -168,  -30,    0,
  302,    0,    0, -256,    0,   13,    0,    0,   26,    0,
    0,    0,    0, -234, -168,   69, -168,   16, -168,    0,
    0, -228,  -54,    0,  -19,    0,    0,   22,  -18,    0,
 -202,  -53,    0, -168,    6,    0,    0,    0,    0,    0,
    6,    0,    0,  -53,   51,    0, -200, -168,  -42,    0,
    0,    0,    0,    0,    0,    0, -159,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -29, -168, -210,
  322, -133, -202,   70, -210,    0,    0,  -46,    0,  -37,
    0,    0, -168,    0,    0,    0, -180, -172,   15,    0,
  -13,    0,    0,    0, -210,  -37,    0,    0,   -6,    0,
  -37,    0,
};
short cl_var_rindex[] = {                                      0,
  -40,    0,    0,    0,  212,    0,    0,   93,    0,    0,
  118,  131,    0,  145,    0,    0,    0,    0,    0,    0,
 -187,    0,  236,    0,    0,    0,  265,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   25,   55,    0,
 -169,    0,    0,    0,    0,    0,    0,    0,   81,    0,
    0,    0,    0,    0,  -17,  144,  -17,    0, -167,    0,
    0,    0,   37,    0,    0,    0,    0,  169,    0,    0,
    0,  158,    0,  106,    1,    0,    0,    0,    0,    0,
    1,    0,    0,  574,    0,    0,    0,  106,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  331,  183,
    0,    0,    0,    0,    0,    0,    0,  330,    0,   17,
    0,    0,  106,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  330,    4,    0,    0,    0,    0,
    4,    0,
};
short cl_var_gindex[] = {                                      0,
   20,    5,    0,    0,    0,  -59,   34,   92,  -49,  -15,
    0,    0,    0,  -71,    0,    0,   91,    0,    0,    0,
   63,   77,    0,   35,  -62,    0,    0,    0,    0,    0,
    0,
};
#define YYTABLESIZE 697
short cl_var_table[] = {                                      20,
   58,  109,   62,   20,  127,   74,   88,   50,   87,    3,
   37,   49,   29,  123,  109,   30,   23,   20,   20,  110,
   21,   77,   77,   73,   76,   76,   73,   57,  108,   59,
  109,   35,  115,   36,   25,   20,   58,  141,    6,    7,
   58,  111,   46,    8,   58,   53,   93,   42,  135,    9,
   87,   58,  140,   60,   10,   11,   12,   13,   58,   58,
   14,   15,   42,  131,  138,   73,   92,   61,   73,  142,
   73,   73,   73,   63,   93,   73,   70,   93,   93,   72,
   93,   73,   20,  113,   20,   79,   73,   73,   73,   73,
  102,  103,   73,   73,   99,   71,   94,   38,   71,   22,
   93,   24,  124,  125,   39,   84,   67,  107,  112,   98,
   66,  100,   41,   43,   41,  119,   40,   45,   41,  137,
   73,   75,  117,   58,   75,   58,  120,  132,  122,   47,
    4,   16,   17,    4,    4,  133,    4,  134,   69,   42,
   73,  104,  116,   96,   51,   82,   65,  121,   69,   24,
   71,    4,    0,    0,    4,    3,    4,    0,    3,    3,
    0,    3,    0,   73,   73,    0,    0,   24,    8,    0,
    0,    8,    8,  101,    8,    0,    3,    0,    0,    3,
    0,    3,    7,    0,   80,    7,    7,   80,    7,    8,
    0,    0,    8,    0,    8,   95,    0,    0,   95,   95,
    0,   95,    0,    7,    0,   80,    7,   80,    7,   82,
    0,    0,   82,    0,    0,    0,   20,   20,   20,   20,
   94,   95,   20,   94,   94,   20,   94,    0,   20,    2,
   82,    0,   20,   20,   20,   20,   20,    0,    0,   20,
   20,    0,    0,    0,   20,    0,   94,    0,    0,    0,
    0,    0,    0,    0,   48,    0,   58,   58,   58,   58,
   58,    0,    0,   58,    0,    0,   58,   20,    0,   58,
  126,    0,    0,   58,   58,   58,   58,   58,   73,   73,
   58,   58,   73,   73,   73,   58,   73,   73,   75,   81,
   73,    0,    0,   73,    0,    0,    0,    0,   73,   73,
   73,   73,   93,    0,   73,   73,    0,    0,   58,   93,
   20,   20,   71,   71,   71,  128,    0,   71,    0,    0,
    0,   93,    0,   71,    0,    0,    0,    0,   71,   71,
   71,   71,   73,    0,   71,   71,    0,    0,   75,   75,
   75,    0,    0,   75,   93,    0,    0,    0,    0,   75,
   44,   58,   58,    0,   75,   75,   75,   75,    4,   38,
   75,   75,   71,   73,   73,    4,   39,   84,   73,    0,
    0,   84,    0,   84,   73,   73,   73,    4,   40,   73,
   73,   73,   73,    3,    0,   73,   73,    0,   75,    0,
    3,   84,    0,   84,    0,    0,    8,    0,    0,    0,
    4,    0,    3,    8,    0,   71,   71,    0,    0,   80,
    7,    0,    0,   73,    0,    8,   80,    7,    0,    0,
    0,    0,    0,   95,    0,    3,    0,    0,   80,    7,
   95,   75,   75,    0,   82,    0,    0,    0,    8,    0,
    0,   82,   95,    0,    0,    0,    0,    0,   94,    0,
    0,   80,    7,   82,    0,   94,   73,   73,    0,    0,
    0,    0,    0,    0,    0,   95,    0,   94,   87,   87,
   87,   87,    0,    0,   87,    0,   82,   87,    0,    0,
   87,    0,    0,    0,   87,   87,   87,   87,   87,    0,
   94,   87,   87,   73,   73,   73,   87,    0,   73,    0,
    0,    0,    0,    0,   73,    0,    0,    0,    0,   73,
   73,   73,   73,    0,    0,   73,   73,    0,    0,   87,
    0,    0,   73,   73,   73,    0,    0,   73,    0,    0,
    0,    6,    7,   73,    0,    0,   31,    0,   73,   73,
   73,   73,    9,   73,   73,   73,    0,   10,   32,   33,
   13,    0,    0,   34,   15,    0,    0,    0,    0,    6,
    7,   52,   87,   87,    8,    0,    0,   50,    0,    0,
    9,    0,   73,    0,    0,   10,   11,   12,   13,    6,
    7,   14,   15,    0,    8,    0,   73,   73,   73,   73,
    9,    0,    0,   73,    0,   10,   11,   12,   13,   73,
    0,   14,   15,    0,   73,   73,   73,   73,    0,    0,
   73,   73,    0,   46,   46,   73,   73,   46,    0,    0,
    0,    0,    0,    0,   16,   17,    0,    0,    0,  118,
    0,    0,   46,    0,   46,    0,    0,    0,   73,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   16,   17,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   46,    0,    0,    0,    0,    0,
    0,    0,   16,   17,    0,    0,    0,    0,    0,    0,
    0,   73,   73,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   46,
};
short cl_var_check[] = {                                      40,
    0,   44,   52,   44,   42,   60,   60,   38,   71,   36,
   59,   42,    8,   60,   44,   11,   40,   58,   59,   62,
   44,   41,   41,   41,   44,   44,   44,   43,   88,   45,
   44,   12,   62,   14,   58,   59,   36,   44,  258,  259,
   40,   91,  332,  263,   44,   41,   41,   44,   62,  269,
  113,  308,   59,   41,  274,  275,  276,  277,   58,   59,
  280,  281,   59,  123,  136,   41,   61,   42,   44,  141,
  258,  259,  260,  308,   38,  263,   61,   41,   42,  308,
   44,  269,  123,   99,  125,   64,  274,  275,  276,  277,
   40,   41,  280,  281,   44,   41,   91,  266,   44,  123,
   64,  125,  118,  119,  273,  308,   38,  308,  268,   59,
   42,   61,   21,   23,   23,  111,  285,   27,   27,  135,
  308,   41,  333,  123,   44,  125,  260,  308,   59,   38,
   38,  351,  352,   41,   42,  308,   44,  123,  308,  123,
  308,   91,  109,   81,  332,   69,   55,  113,   57,   44,
   59,   59,   -1,   -1,   62,   38,   64,   -1,   41,   42,
   -1,   44,   -1,  351,  352,   -1,   -1,   62,   38,   -1,
   -1,   41,   42,  123,   44,   -1,   59,   -1,   -1,   62,
   -1,   64,   38,   -1,   41,   41,   42,   44,   44,   59,
   -1,   -1,   62,   -1,   64,   38,   -1,   -1,   41,   42,
   -1,   44,   -1,   59,   -1,   62,   62,   64,   64,   41,
   -1,   -1,   44,   -1,   -1,   -1,  257,  258,  259,  260,
   38,   64,  263,   41,   42,  266,   44,   -1,  269,  256,
   62,   -1,  273,  274,  275,  276,  277,   -1,   -1,  280,
  281,   -1,   -1,   -1,  285,   -1,   64,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  285,   -1,  256,  257,  258,  259,
  260,   -1,   -1,  263,   -1,   -1,  266,  308,   -1,  269,
  308,   -1,   -1,  273,  274,  275,  276,  277,  333,  333,
  280,  281,  258,  259,  260,  285,  333,  263,  308,  308,
  308,   -1,   -1,  269,   -1,   -1,   -1,   -1,  274,  275,
  276,  277,  266,   -1,  280,  281,   -1,   -1,  308,  273,
  351,  352,  258,  259,  260,  353,   -1,  263,   -1,   -1,
   -1,  285,   -1,  269,   -1,   -1,   -1,   -1,  274,  275,
  276,  277,  308,   -1,  280,  281,   -1,   -1,  258,  259,
  260,   -1,   -1,  263,  308,   -1,   -1,   -1,   -1,  269,
  257,  351,  352,   -1,  274,  275,  276,  277,  266,  266,
  280,  281,  308,  258,  259,  273,  273,   38,  263,   -1,
   -1,   42,   -1,   44,  269,  351,  352,  285,  285,  274,
  275,  276,  277,  266,   -1,  280,  281,   -1,  308,   -1,
  273,   62,   -1,   64,   -1,   -1,  266,   -1,   -1,   -1,
  308,   -1,  285,  273,   -1,  351,  352,   -1,   -1,  266,
  266,   -1,   -1,  308,   -1,  285,  273,  273,   -1,   -1,
   -1,   -1,   -1,  266,   -1,  308,   -1,   -1,  285,  285,
  273,  351,  352,   -1,  266,   -1,   -1,   -1,  308,   -1,
   -1,  273,  285,   -1,   -1,   -1,   -1,   -1,  266,   -1,
   -1,  308,  308,  285,   -1,  273,  351,  352,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  308,   -1,  285,  257,  258,
  259,  260,   -1,   -1,  263,   -1,  308,  266,   -1,   -1,
  269,   -1,   -1,   -1,  273,  274,  275,  276,  277,   -1,
  308,  280,  281,  258,  259,  260,  285,   -1,  263,   -1,
   -1,   -1,   -1,   -1,  269,   -1,   -1,   -1,   -1,  274,
  275,  276,  277,   -1,   -1,  280,  281,   -1,   -1,  308,
   -1,   -1,  258,  259,  260,   -1,   -1,  263,   -1,   -1,
   -1,  258,  259,  269,   -1,   -1,  263,   -1,  274,  275,
  276,  277,  269,  308,  280,  281,   -1,  274,  275,  276,
  277,   -1,   -1,  280,  281,   -1,   -1,   -1,   -1,  258,
  259,  260,  351,  352,  263,   -1,   -1,  332,   -1,   -1,
  269,   -1,  308,   -1,   -1,  274,  275,  276,  277,  258,
  259,  280,  281,   -1,  263,   -1,  351,  352,  258,  259,
  269,   -1,   -1,  263,   -1,  274,  275,  276,  277,  269,
   -1,  280,  281,   -1,  274,  275,  276,  277,   -1,   -1,
  280,  281,   -1,   40,   41,  351,  352,   44,   -1,   -1,
   -1,   -1,   -1,   -1,  351,  352,   -1,   -1,   -1,  308,
   -1,   -1,   59,   -1,   61,   -1,   -1,   -1,  308,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  351,  352,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   91,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  351,  352,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  351,  352,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  123,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 353
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
"LE_OVERRIDE","LE_FINAL","LE_TEMPLATE","LE_TYPENAME","LE_THROW","LE_CATCH",
"LE_IDENTIFIER","LE_STRINGliteral","LE_FLOATINGconstant","LE_INTEGERconstant",
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
"basic_type_name : LE_UNSIGNED basic_type_name_inter",
"basic_type_name : LE_SIGNED basic_type_name_inter",
"basic_type_name : basic_type_name_inter",
"basic_type_name : LE_SHORT basic_type_name",
"basic_type_name : LE_LONG basic_type_name",
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
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 14:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 15:
{ yyval = yyvsp[0]; }
break;
case 16:
{ yyval = yyvsp[-1] + " " + yyvsp[0];}
break;
case 17:
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
break;
case 20:
{curr_var.Reset(); gs_names.clear(); s_tmpString.clear(); s_templateInitList.clear();}
break;
case 22:
{
                            isBasicType = true;
                        }
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
{var_consumeAutoAssignment(yyvsp[-1]);}
break;
case 31:
{}
break;
case 32:
{var_consumBracketsContent('{');}
break;
case 33:
{
                    }
break;
case 34:
{var_consumBracketsContent('{');}
break;
case 35:
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
case 36:
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
case 37:
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
case 38:
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
case 39:
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
case 41:
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
case 46:
{yyval = yyvsp[0];}
break;
case 47:
{yyval = yyvsp[0];}
break;
case 50:
{yyval = yyvsp[0];}
break;
case 51:
{yyval = yyvsp[0];}
break;
case 52:
{var_consumeDefaultValueIfNeeded();}
break;
case 53:
{
                        curr_var.m_name = yyvsp[-1];
                        gs_names.push_back(curr_var);
                    }
break;
case 54:
{var_consumeDefaultValueIfNeeded();}
break;
case 55:
{
                        /*collect all the names*/
                        curr_var.m_name = yyvsp[-1];
                           curr_var.m_isPtr = (yyvsp[-2].find("*") != (size_t)-1);
                        curr_var.m_starAmp = yyvsp[-2];
                        gs_names.push_back(curr_var);
                        yyval = yyvsp[-4] + yyvsp[-3] + " " + yyvsp[-2] + yyvsp[-1];
                    }
break;
case 58:
{yyval = "";}
break;
case 59:
{var_consumeDefaultValue(',', ')'); yyval = ""; }
break;
case 60:
{ yyval = ""; }
break;
case 61:
{ yyval = yyvsp[0] + var_consumBracketsContent('[');}
break;
case 65:
{ yyval = yyvsp[0] + var_consumBracketsContent('(');}
break;
case 66:
{ yyval = yyvsp[0] + var_consumBracketsContent('[');}
break;
case 67:
{
                    yyval = yyvsp[-1]+ yyvsp[0]; 
                }
break;
case 68:
{
                    yyval = yyvsp[-4] + yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0]; 
                    s_templateInitList = yyvsp[-3] + yyvsp[-2] + yyvsp[-1];
                }
break;
case 69:
{yyval = "";}
break;
case 70:
{    yyval = yyvsp[-1] + yyvsp[0];}
break;
case 71:
{ yyval = "";           }
break;
case 72:
{ yyval = yyvsp[0]; curr_var.m_isVolatile = true; }
break;
case 73:
{ yyval = "";           }
break;
case 74:
{ yyval = yyvsp[-1];           }
break;
case 75:
{ yyval = yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;     }
break;
case 76:
{ yyval = yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;     }
break;
case 77:
{ yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;}
break;
case 78:
{ yyval = yyvsp[0]; curr_var.m_isVolatile = true; }
break;
case 79:
{ curr_var.m_enumInTypeDecl = true; }
break;
case 80:
{yyval = ""; }
break;
case 81:
{ yyval = yyvsp[0]; }
break;
case 82:
{yyval = "";}
break;
case 83:
{yyval = "*";}
break;
case 84:
{yyval = ""; }
break;
case 85:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 86:
{ yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]; }
break;
case 87:
{yyval = "";}
break;
case 88:
{ yyval = ";";}
break;
case 89:
{ yyval = "{";}
break;
case 90:
{ yyval = "}";}
break;
case 91:
{ yyval = ":";}
break;
case 92:
{
                            yyval = yyvsp[-1] + " " + yyvsp[0];
                            yyvsp[0].erase(yyvsp[0].find_last_not_of(":")+1);
                            curr_var.m_type = yyvsp[0];
                            curr_var.m_isBasicType = true;
                            curr_var.m_isConst = (yyvsp[-1].find("const") != std::string::npos);
                        }
break;
case 93:
{
                            yyval = yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];
                            yyvsp[-1].erase(yyvsp[-1].find_last_not_of(":")+1);
                            curr_var.m_typeScope = yyvsp[-1];
                            curr_var.m_type = yyvsp[0];
                            curr_var.m_isConst = (yyvsp[-2].find("const") != std::string::npos);
                            s_tmpString.clear();
                        }
break;
case 94:
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
case 95:
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
