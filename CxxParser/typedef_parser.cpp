#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define yyparse cl_typedef_parse
#define yylex cl_typedef_lex
#define yyerror cl_typedef_error
#define yychar cl_typedef_char
#define yyval cl_typedef_val
#define yylval cl_typedef_lval
#define yydebug cl_typedef_debug
#define yynerrs cl_typedef_nerrs
#define yyerrflag cl_typedef_errflag
#define yyss cl_typedef_ss
#define yyssp cl_typedef_ssp
#define yyvs cl_typedef_vs
#define yyvsp cl_typedef_vsp
#define yylhs cl_typedef_lhs
#define yylen cl_typedef_len
#define yydefred cl_typedef_defred
#define yydgoto cl_typedef_dgoto
#define yysindex cl_typedef_sindex
#define yyrindex cl_typedef_rindex
#define yygindex cl_typedef_gindex
#define yytable cl_typedef_table
#define yycheck cl_typedef_check
#define yyname cl_typedef_name
#define yyrule cl_typedef_rule
#define YYPREFIX "cl_typedef_"
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

int  cl_typedef_parse();
void syncParser();
static std::string typedef_consumBracketsContent(char openBrace);
static void typedef_consumeDefaultValue(char c1, char c2);

static  VariableList *           gs_vars = NULL;
static  std::vector<std::string> gs_names;
static  bool                     g_isUsedWithinFunc = false;
static  std::string              s_tmpString;
static  Variable                 curr_var;
static  clTypedefList            gs_typedefs;
static  clTypedef                gs_currentTypedef;
static  std::string              s_templateInitList;

/*---------------------------------------------*/
/* externs defined in the lexer*/
/*---------------------------------------------*/
extern char *cl_scope_text;
extern int   cl_scope_lex();
extern void  cl_scope_less(int count);
extern int   cl_scope_lineno;
extern void  cl_scope_lex_clean();
extern bool  setLexerInput(const std::string &in, const std::map<std::string, std::string> &ignoreMap);
extern void  setUseIgnoreMacros(bool ignore);


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
short cl_typedef_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    2,    2,    2,    2,    2,    0,    0,    5,    3,    3,
    4,    4,    8,    9,    7,    7,   10,   13,   10,   14,
   14,   14,   15,   15,   15,   18,   18,   17,   17,   16,
   16,   16,   16,   16,   19,   19,   20,   20,   12,    6,
    6,    6,    6,    6,   11,   11,   11,   21,   11,   11,
};
short cl_typedef_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    2,    2,    2,    2,    1,    0,    2,    0,    2,    1,
    6,    8,    2,    1,    0,    1,    0,    0,    3,    0,
    1,    3,    4,    4,    7,    2,    5,    0,    2,    0,
    1,    2,    2,    3,    0,    1,    0,    2,    2,    0,
    1,    1,    1,    1,    2,    3,    6,    0,    6,    4,
};
short cl_typedef_defred[] = {                                     16,
    0,   20,   17,    0,   51,   52,   53,   54,   19,    0,
    0,    0,    0,   26,    0,   28,    0,    0,    0,   47,
    0,   29,   24,    0,    0,   43,    0,   23,    0,    6,
    1,   38,    0,    2,    5,    3,    0,   10,    0,    9,
   15,   55,    0,    0,   44,   21,   48,   46,   49,    0,
   14,   13,    4,    8,    7,   11,   12,    0,   39,    0,
    0,   36,    0,    0,    0,   58,    0,   31,   38,   22,
    0,   59,    0,    0,    0,    0,   32,   37,    0,   47,
    0,   33,   34,    0,    0,   35,
};
short cl_typedef_dgoto[] = {                                       1,
   41,   42,    3,    9,    4,   10,   15,   19,   24,   13,
   20,   28,   22,   67,   68,   69,   43,   59,   49,   29,
   72,
};
short cl_typedef_sindex[] = {                                      0,
 -222,    0,    0,  -50,    0,    0,    0,    0,    0, -235,
 -264,   -1, -254,    0, -207,    0, -237,   11, -237,    0,
   20,    0,    0,   23,   30,    0,   26,    0,   22,    0,
    0,    0, -180,    0,    0,    0,  -30,    0,  -30,    0,
    0,    0, -220, -264,    0,    0,    0,    0,    0, -219,
    0,    0,    0,    0,    0,    0,    0,  -59,    0, -207,
  -60,    0, -207,   27, -207,    0,   -7,    0,    0,    0,
   -5,    0, -207, -243,  -85, -243,    0,    0,  -58,    0,
 -207,    0,    0,   -4, -243,    0,
};
short cl_typedef_rindex[] = {                                      0,
  -55,    0,    0, -252,    0,    0,    0,    0,    0, -223,
 -182,    0,    0,    0, -158,    0,    0, -129,    0,    0,
 -211,    0,    0,    0, -105,    0,    0,    0,  -15,    0,
    0,    0,  -32,    0,    0,    0,  -31,    0,  -24,    0,
    0,    0,    0, -182,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -23,    0, -158,
  -17,    0,  -39,    0,  -39,    0,    0,    0,    0,    0,
    0,    0,  -76,  -16,    0,    0,    0,    0,   12,    0,
  -39,    0,    0,    0,   12,    0,
};
short cl_typedef_gindex[] = {                                      0,
   43,   28,    0,    0,    0,    0,   60,   46,   88,    0,
    0,  -34,    0,  -33,   35,    2,   -8,    0,    0,    0,
    0,
};
#define YYTABLESIZE 301
short cl_typedef_table[] = {                                      65,
   63,   81,   18,   18,   30,    4,    8,    8,    5,    4,
    8,    4,    8,    7,   56,   50,   21,    7,   56,    7,
   60,   57,   30,   50,   60,   57,    4,    8,   45,    4,
    8,   71,   11,    2,    7,   56,   73,    7,   73,   73,
   14,   60,   57,   45,   82,   83,   45,   84,   26,   47,
   86,   50,   25,   47,   74,   47,   76,   85,   16,   48,
   75,   21,   66,   47,   17,   18,   50,   18,   12,   18,
   23,   45,    6,   47,    7,   25,   25,   25,   51,   56,
   25,   57,   52,   44,   46,   70,   25,   58,   61,   78,
   25,   25,   25,   25,   25,   27,   38,   25,   25,   40,
   40,   40,   80,   60,   40,   64,   27,   77,    0,    0,
   40,    0,    0,    0,    0,   40,   40,   40,   40,    0,
    0,   40,   40,    0,    0,   25,    0,    0,   41,   41,
   41,    0,    0,   41,    0,    0,    0,    0,    0,   41,
    0,    0,    0,    0,   41,   41,   41,   41,    0,   40,
   41,   41,   42,   42,   42,    0,    0,   42,    0,    0,
    0,    0,    0,   42,    0,    0,    0,    0,   42,   42,
   42,   42,   30,   31,   42,   42,    0,   33,   41,    0,
    0,   40,   40,   34,    0,    0,   40,    0,   35,   36,
   37,   38,   40,    0,   39,   40,    0,   40,   40,   40,
   40,    0,   42,   40,   40,    0,    0,    0,    0,    0,
    0,    0,   18,    0,    0,    0,    0,    0,   40,   40,
    0,    0,   79,   40,    0,    0,    0,   30,   31,   40,
    0,   40,   53,    0,   40,   40,   40,   40,   34,    0,
   40,   40,    0,   35,   36,   54,   38,    0,   18,   55,
   40,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   18,    0,    0,    0,    0,   40,    0,
    0,    0,   62,   62,   62,    4,    8,   30,   31,   32,
    0,    0,   33,    7,   56,    0,    0,    0,   34,    0,
   60,   57,   45,   35,   36,   37,   38,    0,    0,   39,
   40,
};
short cl_typedef_check[] = {                                      60,
   60,   60,   58,   59,   44,   38,   38,   58,   59,   42,
   42,   44,   44,   38,   38,  268,   15,   42,   42,   44,
   38,   38,   62,   32,   42,   42,   59,   59,   44,   62,
   62,   65,  268,  256,   59,   59,   44,   62,   44,   44,
  305,   59,   59,   59,   79,   80,   62,   81,   38,   38,
   85,  304,   42,   42,   62,   44,   62,   62,   60,   38,
   69,   60,  123,   42,  319,  273,  319,  123,  304,  125,
  308,   42,  123,   62,  125,  258,  259,  260,  259,   37,
  263,   39,  263,   61,   59,   59,  269,  308,  308,  333,
  273,  274,  275,  276,  277,  319,  308,  280,  281,  258,
  259,  260,   75,   44,  263,   60,   19,   73,   -1,   -1,
  269,   -1,   -1,   -1,   -1,  274,  275,  276,  277,   -1,
   -1,  280,  281,   -1,   -1,  308,   -1,   -1,  258,  259,
  260,   -1,   -1,  263,   -1,   -1,   -1,   -1,   -1,  269,
   -1,   -1,   -1,   -1,  274,  275,  276,  277,   -1,  308,
  280,  281,  258,  259,  260,   -1,   -1,  263,   -1,   -1,
   -1,   -1,   -1,  269,   -1,   -1,   -1,   -1,  274,  275,
  276,  277,  258,  259,  280,  281,   -1,  263,  308,   -1,
   -1,  258,  259,  269,   -1,   -1,  263,   -1,  274,  275,
  276,  277,  269,   -1,  280,  281,   -1,  274,  275,  276,
  277,   -1,  308,  280,  281,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  268,   -1,   -1,   -1,   -1,   -1,  258,  259,
   -1,   -1,  308,  263,   -1,   -1,   -1,  258,  259,  269,
   -1,  308,  263,   -1,  274,  275,  276,  277,  269,   -1,
  280,  281,   -1,  274,  275,  276,  277,   -1,  304,  280,
  281,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  319,   -1,   -1,   -1,   -1,  308,   -1,
   -1,   -1,  333,  333,  333,  308,  308,  258,  259,  260,
   -1,   -1,  263,  308,  308,   -1,   -1,   -1,  269,   -1,
  308,  308,  308,  274,  275,  276,  277,   -1,   -1,  280,
  281,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 352
#if YYDEBUG
char *cl_typedef_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,"'&'",0,0,0,"'*'",0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,"':'","';'","'<'",
"'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,"LE_AUTO","LE_DOUBLE","LE_INT","LE_STRUCT","LE_BREAK","LE_ELSE",
"LE_LONG","LE_SWITCH","LE_CASE","LE_ENUM","LE_REGISTER","LE_TYPEDEF","LE_CHAR",
"LE_EXTERN","LE_RETURN","LE_UNION","LE_CONST","LE_FLOAT","LE_SHORT",
"LE_UNSIGNED","LE_BOOL","LE_CONTINUE","LE_FOR","LE_SIGNED","LE_VOID",
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
"LE_REINTERPRET_CAST","LE_SIZE_T","LE_TIME_T",
};
char *cl_typedef_rule[] = {
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
"external_decl : $$1 typedefs",
"external_decl : error",
"typedefs : stmnt_starter LE_TYPEDEF opt_typename_specifier real_type new_name ';'",
"typedefs : stmnt_starter opt_template_qualifier LE_USING new_name '=' opt_typename_specifier real_type ';'",
"real_type : variable_decl special_star_amp",
"new_name : LE_IDENTIFIER",
"opt_typename_specifier :",
"opt_typename_specifier : LE_TYPENAME",
"opt_template_qualifier :",
"$$2 :",
"opt_template_qualifier : LE_TEMPLATE '<' $$2",
"parameter_list :",
"parameter_list : template_parameter",
"parameter_list : parameter_list ',' template_parameter",
"template_parameter : const_spec nested_scope_specifier LE_IDENTIFIER special_star_amp",
"template_parameter : const_spec nested_scope_specifier basic_type_name special_star_amp",
"template_parameter : const_spec nested_scope_specifier LE_IDENTIFIER '<' parameter_list '>' special_star_amp",
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


std::string typedef_consumBracketsContent(char openBrace)
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

void typedef_consumeDefaultValue(char c1, char c2)
{
	int depth = 0;

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

void do_clean_up()
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
void get_typedefs(const std::string &in, clTypedefList &li)
{
	std::map<std::string, std::string> dummy;

    // provide the lexer with new input
	if( !setLexerInput(in, dummy) ){
    	return;
    }

	// set the parser local output to our variable list
	cl_typedef_parse();
	li = gs_typedefs;

	do_clean_up();
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
{curr_var.Reset(); gs_names.clear(); s_tmpString.clear(); gs_currentTypedef.clear(); s_templateInitList.clear();}
break;
case 20:
{
                            yyclearin;    /*clear lookahead token*/
                            yyerrok;
/*                            printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_scope_text, cl_scope_lineno);*/
                            }
break;
case 21:
{
						if(gs_currentTypedef.m_realType.m_templateDecl.empty())
							gs_currentTypedef.m_realType.m_templateDecl = s_templateInitList;
						s_templateInitList.clear();
						gs_typedefs.push_back(gs_currentTypedef);

					}
break;
case 22:
{
						if(gs_currentTypedef.m_realType.m_templateDecl.empty())
							gs_currentTypedef.m_realType.m_templateDecl = s_templateInitList;
						s_templateInitList.clear();
						gs_typedefs.push_back(gs_currentTypedef);
					 }
break;
case 23:
{
				gs_currentTypedef.m_realType = curr_var;
				gs_currentTypedef.m_realType.m_isPtr = (yyvsp[0].find("*") != std::string::npos);
			}
break;
case 24:
{ gs_currentTypedef.m_name = yyvsp[0]; }
break;
case 25:
{ yyval = ""; }
break;
case 26:
{ yyval = yyvsp[0]; }
break;
case 28:
{s_tmpString = typedef_consumBracketsContent('<');}
break;
case 29:
{
                                yyval = yyvsp[-2] + yyvsp[-1] + s_tmpString;
                                s_tmpString.clear();
                            }
break;
case 30:
{yyval = "";}
break;
case 31:
{yyval = yyvsp[0];}
break;
case 32:
{yyval = yyvsp[-2] + yyvsp[-1] + " " + yyvsp[0];}
break;
case 33:
{
						yyval = yyvsp[-2] + yyvsp[-1] +yyvsp[0];
					}
break;
case 34:
{
						yyval = yyvsp[-2] + yyvsp[-1] +yyvsp[0];
					}
break;
case 35:
{
						yyval = yyvsp[-5] + yyvsp[-4] +yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0] + " " ;
					}
break;
case 36:
{yyval = yyvsp[-1]+ yyvsp[0]; }
break;
case 37:
{yyval = yyvsp[-4] ; s_templateInitList = yyvsp[-3] + yyvsp[-2] + yyvsp[-1];}
break;
case 38:
{yyval = "";}
break;
case 39:
{    yyval = yyvsp[-1] + yyvsp[0];}
break;
case 40:
{yyval = "";                          }
break;
case 41:
{ yyval = yyvsp[0];                         }
break;
case 42:
{ yyval = yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;     }
break;
case 43:
{ yyval = yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;     }
break;
case 44:
{ yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]; curr_var.m_rightSideConst = yyval;}
break;
case 45:
{yyval = ""; }
break;
case 46:
{ yyval = yyvsp[0]; }
break;
case 47:
{yyval = ""; }
break;
case 48:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 49:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 50:
{yyval = "";}
break;
case 51:
{ yyval = ";";}
break;
case 52:
{ yyval = "{";}
break;
case 53:
{ yyval = "}";}
break;
case 54:
{ yyval = ":";}
break;
case 55:
{
                            yyval = yyvsp[-1] + " " + yyvsp[0];
                            yyvsp[0].erase(yyvsp[0].find_last_not_of(":")+1);
                        	curr_var.m_type = yyvsp[0];
							curr_var.m_isBasicType = true;
                        	curr_var.m_isConst = !yyvsp[-1].empty();
                        }
break;
case 56:
{
							yyval = yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];
                            yyvsp[-1].erase(yyvsp[-1].find_last_not_of(":")+1);
                        	curr_var.m_typeScope = yyvsp[-1];
                        	curr_var.m_type = yyvsp[0];
                        	curr_var.m_isConst = !yyvsp[-2].empty();
                        	s_tmpString.clear();
                        }
break;
case 57:
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
case 58:
{s_tmpString = typedef_consumBracketsContent('{');}
break;
case 59:
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
case 60:
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
