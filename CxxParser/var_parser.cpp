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

#ifdef yylex
#undef yylex
#define yylex cl_scope_lex
#endif

#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

void cl_scope_error(char *string);
int  cl_var_parse();
void syncParser();
void var_consumeUntil(char c1, char c2);

static VariableList *gs_vars = NULL;
static std::vector<std::string> gs_names;
static bool g_isUsedWithinFunc = false;
Variable curr_var;

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
short cl_var_lhs[] = {                                        -1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    2,
    2,    2,    2,    2,    0,    0,    5,    3,    3,    6,
    6,    6,    7,    7,    7,    4,    4,    4,    4,    4,
   13,   13,   16,   16,   15,   15,   15,   14,   14,   14,
   14,   14,   17,   17,    9,    9,    8,    8,   18,   18,
   19,   19,   10,   11,   11,   11,   11,   11,   12,   12,
   12,
};
short cl_var_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    2,
    2,    2,    2,    1,    0,    2,    0,    2,    1,    0,
    1,    3,    4,    4,    7,    5,    5,    5,    4,    4,
    1,    4,    1,    1,    0,    1,    1,    1,    1,    1,
    1,    1,    2,    5,    0,    2,    0,    1,    0,    1,
    0,    2,    2,    0,    1,    1,    1,    1,    2,    3,
    6,
};
short cl_var_defred[] = {                                     15,
    0,   19,   16,    0,    0,    0,   55,   56,   57,   58,
   18,    0,   48,    0,   51,   51,   51,    6,    1,    0,
    2,    5,    3,    0,    0,    9,   14,   59,    0,    0,
    0,    0,    0,   13,   12,    4,    8,    7,   10,   11,
    0,   46,    0,   33,   34,   30,   50,   52,   53,    0,
   29,   31,    0,   43,    0,   37,   36,   28,   27,   51,
   41,   40,   39,   38,   42,   26,    0,   21,   45,    0,
    0,    0,    0,   32,   22,   44,    0,   51,    0,   23,
   24,    0,    0,   25,
};
short cl_var_dgoto[] = {                                       1,
   27,   28,    3,   11,    4,   67,   68,   14,   29,   30,
   12,   15,   53,   66,   58,   46,   42,   49,   31,
};
short cl_var_sindex[] = {                                      0,
 -250,    0,    0,   -2, -264, -264,    0,    0,    0,    0,
    0, -264,    0,  -97,    0,    0,    0,    0,    0, -196,
    0,    0,    0,  -88,  -88,    0,    0,    0, -293,   -5,
   39,    5, -240,    0,    0,    0,    0,    0,    0,    0,
  -58,    0,  -28,    0,    0,    0,    0,    0,    0,  -28,
    0,    0,   10,    0, -264,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  -39,    0,    0, -239,
 -264, -256, -147,    0,    0,    0,  -57,    0, -264,    0,
    0,   11, -256,    0,
};
short cl_var_rindex[] = {                                      0,
   28,    0,    0, -180, -120, -120,    0,    0,    0,    0,
    0, -120,    0, -230,    0,    0,    0,    0,    0,  -34,
    0,    0,    0,  -27,  -22,    0,    0,    0,    0,    0,
  -10,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -17,    0,    1,    0,    0,    0,    0,    0,    0,    1,
    0,    0,    0,    0,  -44,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -120,  -12,    0,    0,    0,    0,   20,    0,  -44,    0,
    0,    0,   20,    0,
};
short cl_var_gindex[] = {                                      0,
   19,    7,    0,    0,    0,    6,   13,  -18,   21,   31,
    0,   64,    0,    0,   38,   60,    0,    0,    0,
};
#define YYTABLESIZE 333
short cl_var_table[] = {                                      20,
   35,   55,   79,    4,   71,    2,    4,    4,   13,    4,
    8,   41,   56,    8,    8,    7,    8,   20,    7,    7,
   60,    7,   72,   60,   60,   61,   60,    4,   61,   61,
   49,   61,   57,   49,    8,   45,   69,    6,   44,    7,
   35,    5,   39,   40,   35,   45,   32,   33,   44,   61,
   62,   49,   69,   60,   71,   10,    7,   51,   35,   35,
   69,   51,   34,   51,   52,   74,   35,   17,   64,   16,
   63,   17,   83,   76,   45,   17,   47,   54,   54,   78,
   48,   51,   54,   75,   82,   17,   17,   59,   54,   73,
   70,   51,   54,   54,   54,   54,    0,    0,   54,   54,
   65,    0,    0,    0,    0,    0,    0,   80,   81,    0,
   18,   19,    0,   84,    0,   20,    0,    0,    0,    0,
    8,   21,    9,   35,   54,   35,   22,   23,   24,    0,
    0,   25,   26,    0,    0,    0,    0,   47,   47,    0,
    0,    0,   47,    0,    0,    0,    0,    0,   47,    0,
   17,    0,   17,   47,   47,   47,    0,   77,   47,   47,
   18,   19,    0,    0,    0,   20,    0,    0,    0,   18,
   19,   21,    0,    0,   36,    0,   22,   23,   24,    0,
   21,   25,   26,    0,   47,   22,   23,   37,    0,    0,
   38,   26,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   47,   47,    0,    0,    0,   47,    0,
    0,    0,    0,    0,   47,    0,    0,    0,    0,   47,
   47,   47,    0,    0,   47,   47,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   35,    0,   35,   35,
   47,    0,    0,   35,    0,    0,    0,    0,    0,   35,
    4,   54,   54,   35,   35,   35,   35,    8,    0,   35,
   35,    0,    7,    0,    0,   17,   17,   60,    0,    0,
   17,    0,   61,    0,   49,    0,   17,    0,    0,   43,
   17,   17,   17,   17,    0,   35,   17,   17,    0,   50,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   17,
};
short cl_var_check[] = {                                      44,
    0,   60,   60,   38,   44,  256,   41,   42,  273,   44,
   38,  305,   41,   41,   42,   38,   44,   62,   41,   42,
   38,   44,   62,   41,   42,   38,   44,   62,   41,   42,
   41,   44,   61,   44,   62,   41,   55,   40,   44,   62,
   40,   44,   24,   25,   44,   41,   16,   17,   44,   40,
   41,   62,   71,   44,   44,   58,   59,   38,   58,   59,
   79,   42,  259,   44,  305,  305,  263,   40,   59,    6,
   61,   44,   62,  330,  305,   12,   38,  258,  259,   73,
   42,   62,  263,   71,   79,   58,   59,   50,  269,   69,
   60,   32,  273,  274,  275,  276,   -1,   -1,  279,  280,
   91,   -1,   -1,   -1,   -1,   -1,   -1,   77,   78,   -1,
  258,  259,   -1,   83,   -1,  263,   -1,   -1,   -1,   -1,
  123,  269,  125,  123,  305,  125,  274,  275,  276,   -1,
   -1,  279,  280,   -1,   -1,   -1,   -1,  258,  259,   -1,
   -1,   -1,  263,   -1,   -1,   -1,   -1,   -1,  269,   -1,
  123,   -1,  125,  274,  275,  276,   -1,  305,  279,  280,
  258,  259,   -1,   -1,   -1,  263,   -1,   -1,   -1,  258,
  259,  269,   -1,   -1,  263,   -1,  274,  275,  276,   -1,
  269,  279,  280,   -1,  305,  274,  275,  276,   -1,   -1,
  279,  280,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  258,  259,   -1,   -1,   -1,  263,   -1,
   -1,   -1,   -1,   -1,  269,   -1,   -1,   -1,   -1,  274,
  275,  276,   -1,   -1,  279,  280,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  256,   -1,  258,  259,
  305,   -1,   -1,  263,   -1,   -1,   -1,   -1,   -1,  269,
  305,  330,  330,  273,  274,  275,  276,  305,   -1,  279,
  280,   -1,  305,   -1,   -1,  258,  259,  305,   -1,   -1,
  263,   -1,  305,   -1,  305,   -1,  269,   -1,   -1,  305,
  273,  274,  275,  276,   -1,  305,  279,  280,   -1,  305,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  305,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 347
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
"LE_UNSIGNED","LE_CONTINUE","LE_FOR","LE_SIGNED","LE_VOID","LE_DEFAULT",
"LE_GOTO","LE_SIZEOF","LE_VOLATILE","LE_DO","LE_IF","LE_STATIC","LE_WHILE",
"LE_NEW","LE_DELETE","LE_THIS","LE_OPERATOR","LE_CLASS","LE_PUBLIC",
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
"basic_type_name : LE_UNSIGNED basic_type_name_inter",
"basic_type_name : LE_SIGNED basic_type_name_inter",
"basic_type_name : LE_LONG LE_LONG",
"basic_type_name : LE_LONG LE_INT",
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
"variables : stmnt_starter variable_decl special_star_amp variable_name_list postfix",
"variables : '(' variable_decl special_star_amp LE_IDENTIFIER postfix2",
"variables : ',' variable_decl special_star_amp LE_IDENTIFIER postfix2",
"variables : '(' variable_decl special_star_amp postfix3",
"variables : ',' variable_decl special_star_amp postfix3",
"variable_name_list : LE_IDENTIFIER",
"variable_name_list : variable_name_list ',' special_star_amp LE_IDENTIFIER",
"postfix3 : ','",
"postfix3 : ')'",
"postfix2 :",
"postfix2 : '='",
"postfix2 : ')'",
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
		//printf("ch=%d\n", ch);
		//fflush(stdout);
		if(ch == 0){
			break;
		}

		consumedData += cl_scope_text;
		consumedData += " ";
		if(ch == closeBrace)
		{
			depth--;
			continue;
		}
		else if(ch == openBrace)
		{
			depth ++ ;
			continue;
		}
	}
	return consumedData;
}

void var_consumeUntil(char c1, char c2)
{
	int depth = 0;
	bool cont(true);

	while (depth >= 0) {
		int ch = cl_scope_lex();
		if(ch == 0)					{ break;}
		if(ch == c1 && depth == 0) {
			cl_scope_less(0);
			break;
		}

		if(ch == c2 && depth == 0) {
			cl_scope_less(0);
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

void var_syncParser(){
//	int depth = 1;
//	bool cont(true);
//
//	while (depth > 0 && cont) {
//		int ch = cl_scope_lex();
//		if(ch == 0)					{ break;}
//		if(ch == ',' && depth == 0) { break;}
//		if(ch == ';' && depth == 0) { break;}
//		if(ch == ')' && depth == 0) { break;}
//
//		if(ch == ')'){
//			depth--;
//			continue;
//		}
//		else if(ch == '('){
//			depth ++ ;
//			continue;
//		}
//		printf("%d ", ch);
//	}
//	printf("\n");
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
	gs_vars = NULL;

	// restore settings
	setUseIgnoreMacros(true);
	g_isUsedWithinFunc = false;

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
{ yyval = yyvsp[-1] + " " + yyvsp[0]; }
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
{ yyval = yyvsp[0]; }
break;
case 17:
{curr_var.Reset(); gs_names.clear();}
break;
case 19:
{
								yyclearin;	/*clear lookahead token*/
								yyerrok;
								/*printf("CodeLite: syntax error, unexpected token '%s' found at line %d \n", cl_var_lval.c_str(), cl_scope_lineno);*/
								var_syncParser();
							}
break;
case 20:
{yyval = "";}
break;
case 21:
{yyval = yyvsp[0];}
break;
case 22:
{yyval = yyvsp[-2] + yyvsp[-1] + " " + yyvsp[0];}
break;
case 23:
{
							yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
						}
break;
case 24:
{
							yyval = yyvsp[-3] +  yyvsp[-2] + yyvsp[-1] +yyvsp[0];
						}
break;
case 25:
{
							yyval = yyvsp[-6] + yyvsp[-5] + yyvsp[-4] +yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0] + " " ;
						}
break;
case 26:
{
							if(gs_vars)
							{
								Variable var;
								std::string pattern;
								curr_var.m_isPtr = (yyvsp[-2].find("*") != (size_t)-1);
								curr_var.m_starAmp = yyvsp[-2];
								curr_var.m_lineno = cl_scope_lineno;
								for(size_t i=0; i< gs_names.size(); i++)
								{
									/*create new variable for every variable name found*/
									var = curr_var;
									var.m_pattern = "/^" + yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] + gs_names.at(i) + " $/";
									var.m_name = gs_names.at(i);
									gs_vars->push_back(var);
								}
								curr_var.Reset();
								gs_names.clear();
							}
						}
break;
case 27:
{
							if(gs_vars)
							{
								Variable var;
								std::string pattern;
								curr_var.m_pattern = "/^";
								curr_var.m_pattern += yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1] + "$/";
								curr_var.m_isPtr = (yyvsp[-2].find("*") != (size_t)-1);
								curr_var.m_starAmp = yyvsp[-2];
								curr_var.m_lineno = cl_scope_lineno;
								/*create new variable for every variable name found*/
								var = curr_var;
								var.m_name = yyvsp[-1];;
								gs_vars->push_back(var);
								curr_var.Reset();
								gs_names.clear();
							}
						}
break;
case 28:
{
							if(gs_vars && g_isUsedWithinFunc)
							{
								Variable var;
								std::string pattern;
								curr_var.m_pattern = "/^";
								curr_var.m_pattern += yyvsp[-4] + " " + yyvsp[-3] + " " + yyvsp[-2] + " " + yyvsp[-1] + "$/";
								curr_var.m_isPtr = (yyvsp[-2].find("*") != (size_t)-1);
								curr_var.m_starAmp = yyvsp[-2];
								curr_var.m_lineno = cl_scope_lineno;

								/*create new variable for every variable name found*/
								var = curr_var;
								var.m_name = yyvsp[-1];
								gs_vars->push_back(var);

								curr_var.Reset();
								gs_names.clear();
							}
						}
break;
case 29:
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
case 30:
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
case 31:
{
							gs_names.push_back(yyvsp[0]);
							yyval = yyvsp[0];
						}
break;
case 32:
{
							/*collect all the names*/
							gs_names.push_back(yyvsp[0]);
							yyval = yyvsp[-3] + yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];
						}
break;
case 36:
{var_consumeUntil(',', ')');}
break;
case 41:
{ yyval = yyvsp[0] + var_consumBracketsContent('(');}
break;
case 42:
{ yyval = yyvsp[0] + var_consumBracketsContent('[');}
break;
case 43:
{yyval = yyvsp[-1]+ yyvsp[0]; }
break;
case 44:
{yyval = yyvsp[-4] + yyvsp[-3] + yyvsp[-2] + yyvsp[-1] + yyvsp[0];}
break;
case 45:
{yyval = "";}
break;
case 46:
{	yyval = yyvsp[-1] + yyvsp[0];}
break;
case 47:
{yyval = ""; }
break;
case 48:
{ yyval = yyvsp[0]; }
break;
case 49:
{yyval = ""; }
break;
case 50:
{ yyval = yyvsp[0]; }
break;
case 51:
{yyval = ""; }
break;
case 52:
{yyval = yyvsp[-1] + yyvsp[0];}
break;
case 53:
{ yyval = yyvsp[-1] + yyvsp[0]; }
break;
case 54:
{yyval = "";}
break;
case 55:
{ yyval = ";";}
break;
case 56:
{ yyval = "{";}
break;
case 57:
{ yyval = "}";}
break;
case 58:
{ yyval = ":";}
break;
case 59:
{
							yyval = yyvsp[-1] + " " + yyvsp[0];
							yyvsp[0].erase(yyvsp[0].find_last_not_of(":")+1);
							curr_var.m_type = yyvsp[0];
							curr_var.m_isConst = !yyvsp[-1].empty();
						}
break;
case 60:
{
							yyval = yyvsp[-2] + " " + yyvsp[-1] + yyvsp[0];
							yyvsp[-1].erase(yyvsp[-1].find_last_not_of(":")+1);
							curr_var.m_typeScope = yyvsp[-1];
							curr_var.m_type = yyvsp[0];
							curr_var.m_isConst = !yyvsp[-2].empty();
						}
break;
case 61:
{
							yyval = yyvsp[-5] + " " + yyvsp[-4] + yyvsp[-3] + " " + yyvsp[-2] + yyvsp[-1] + yyvsp[0];
							yyvsp[-4].erase(yyvsp[-4].find_last_not_of(":")+1);
							curr_var.m_typeScope = yyvsp[-4];
							curr_var.m_type = yyvsp[-3];
							curr_var.m_isTemplate = true;
							curr_var.m_templateDecl = yyvsp[-2] +yyvsp[-1] +yyvsp[0];
							curr_var.m_isConst = !yyvsp[-5].empty();
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
