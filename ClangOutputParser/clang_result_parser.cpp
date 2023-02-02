#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define yyparse clang_result_parse
#define yylex clang_result_lex
#define yyerror clang_result_error
#define yychar clang_result_char
#define yyval clang_result_val
#define yylval clang_result_lval
#define yydebug clang_result_debug
#define yynerrs clang_result_nerrs
#define yyerrflag clang_result_errflag
#define yyss clang_result_ss
#define yyssp clang_result_ssp
#define yyvs clang_result_vs
#define yyvsp clang_result_vsp
#define yylhs clang_result_lhs
#define yylen clang_result_len
#define yydefred clang_result_defred
#define yydgoto clang_result_dgoto
#define yysindex clang_result_sindex
#define yyrindex clang_result_rindex
#define yygindex clang_result_gindex
#define yytable clang_result_table
#define yycheck clang_result_check
#define yyname clang_result_name
#define yyrule clang_result_rule
#define YYPREFIX "clang_result_"
/* Copyright Eran Ifrah(c)*/
#include <string>
#include <stdio.h>
#include <vector>

#include "clang_output_parser_api.h"

#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

#ifdef yylex
#undef yylex
#define yylex clang_yylex
#endif

extern int  clang_yylex();
extern void clang_yyless(int count);
int         clang_result_parse();

/** defined in grammer.l **/
extern bool        clang_set_lexer_input(const std::string &in);
extern const std::string& clang_lex_get_string();
extern void        clang_lex_clean();

/** the parser entry point **/
void clang_parse_string(const std::string& str);

void clang_result_error(char*);
/** read the signature **/
void clang_read_signature();
void clang_read_type_name(std::string &store);
void clang_read_entry_name(std::string &store);

/** holds the last signature read **/
static ClangEntry       clang_entry;
static ClangEntryVector clangEntryVector;
extern std::string      clang_result_lval;
#define CLANG_COMPLETION 257
#define CLANG_HIDDEN 258
#define CLANG_DELIM_OPEN 259
#define CLANG_DELIM_CLOSE 260
#define CLANG_ARG_DELIM_OPEN 261
#define CLANG_ARG_DELIM_CLOSE 262
#define CLANG_ARG_OPT_DELIM_OPEN 263
#define CLANG_ARG_OPT_DELIM_CLOSE 264
#define CLANG_STAR 265
#define CLANG_AMP 266
#define CLANG_TILDE 267
#define CLANG_ENUM 268
#define CLANG_ANONYMOUS 269
#define CLANG_CLCL 270
#define CLANG_OP_ASSIGN 271
#define CLANG_OP_DOTstar 272
#define CLANG_OP_ARROW 273
#define CLANG_OP_ARROWstar 274
#define CLANG_OP_ICR 275
#define CLANG_OP_DECR 276
#define CLANG_OP_LS 277
#define CLANG_OP_RS 278
#define CLANG_OP_LE 279
#define CLANG_OP_GE 280
#define CLANG_OP_EQ 281
#define CLANG_OP_NE 282
#define CLANG_ELLIPSIS 283
#define CLANG_NAME 284
#define CLANG_OPERATOR 285
#define YYERRCODE 256
short clang_result_lhs[] = {                                        -1,
    0,    0,    3,    1,    1,    2,    8,    8,    8,    8,
    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,
    4,    4,    4,    4,    7,    9,    7,   10,   11,   10,
    6,    6,   12,    5,    5,    5,
};
short clang_result_len[] = {                                         2,
    1,    2,    0,    2,    1,    8,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    2,    2,
    0,    1,    2,    2,    0,    0,    4,    0,    0,    3,
    0,    4,    0,    3,    2,    1,
};
short clang_result_defred[] = {                                      0,
    5,    0,    1,    0,    2,    0,    4,    0,    0,   22,
    0,    0,   24,    9,   11,    7,    8,   14,   10,   16,
   18,   15,   13,   12,   17,    0,    0,   23,    0,   19,
   20,   33,    0,    0,    0,   35,    0,    0,   34,    0,
    0,    0,   26,    6,   32,    0,    0,   29,   27,    0,
   30,
};
short clang_result_dgoto[] = {                                       2,
    3,    7,    4,   12,   34,   38,   44,   28,   46,   49,
   50,   35,
};
short clang_result_sindex[] = {                                   -252,
    0, -252,    0, -251,    0,  -50,    0, -264, -274,    0,
  -40,  -47,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -29,  -79,    0, -257,    0,
    0,    0, -255, -243, -242,    0, -267, -264,    0, -248,
  -21, -237,    0,    0,    0,  -17, -234,    0,    0, -232,
    0,
};
short clang_result_rindex[] = {                                   -231,
    0, -231,    0,    0,    0,    0,    0,  -28,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    1,    5,    0,    0,    0,    7,    0,    0,
    9,    0,    0,    0,    0,    0,   13,    0,    0,    0,
    0,
};
short clang_result_gindex[] = {                                      0,
   27,    0,    0,   -7,    0,    0,    0,    0,    0,    0,
    0,    0,
};
#define YYTABLESIZE 290
short clang_result_table[] = {                                      26,
   36,   32,    9,    1,   31,    6,   21,    8,   25,   13,
   29,   30,   28,   31,   36,   37,   40,   39,   43,   10,
   11,   42,   45,   47,   48,    3,   33,   51,    5,   21,
   41,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   36,    0,    0,    0,   31,    0,   21,    0,    0,    0,
   27,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
   24,   25,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   36,   36,    0,   36,
   31,   31,   21,   21,   25,   25,    0,   36,   28,   28,
    0,   31,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   36,   36,    0,    0,   31,   31,
};
short clang_result_check[] = {                                      40,
    0,  259,  267,  256,    0,  257,    0,   58,    0,  284,
   58,   41,    0,   93,  270,  259,  284,  260,   40,  284,
  285,  270,  260,   41,  259,  257,  284,  260,    2,   58,
   38,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   40,   -1,   -1,   -1,   40,   -1,   40,   -1,   -1,   -1,
   91,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  271,  272,  273,  274,  275,  276,  277,  278,  279,  280,
  281,  282,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  256,  257,   -1,  259,
  256,  257,  256,  257,  256,  257,   -1,  267,  256,  257,
   -1,  267,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  284,  285,   -1,   -1,  284,  285,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 285
#if YYDEBUG
char *clang_result_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"':'",0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"CLANG_COMPLETION","CLANG_HIDDEN","CLANG_DELIM_OPEN","CLANG_DELIM_CLOSE",
"CLANG_ARG_DELIM_OPEN","CLANG_ARG_DELIM_CLOSE","CLANG_ARG_OPT_DELIM_OPEN",
"CLANG_ARG_OPT_DELIM_CLOSE","CLANG_STAR","CLANG_AMP","CLANG_TILDE","CLANG_ENUM",
"CLANG_ANONYMOUS","CLANG_CLCL","CLANG_OP_ASSIGN","CLANG_OP_DOTstar",
"CLANG_OP_ARROW","CLANG_OP_ARROWstar","CLANG_OP_ICR","CLANG_OP_DECR",
"CLANG_OP_LS","CLANG_OP_RS","CLANG_OP_LE","CLANG_OP_GE","CLANG_OP_EQ",
"CLANG_OP_NE","CLANG_ELLIPSIS","CLANG_NAME","CLANG_OPERATOR",
};
char *clang_result_rule[] = {
"$accept : parse",
"parse : completion_output",
"parse : parse completion_output",
"$$1 :",
"completion_output : $$1 completion_entry",
"completion_output : error",
"completion_entry : CLANG_COMPLETION ':' entry_name ':' type_name scope entry_name function_part",
"any_operator : CLANG_OP_ARROW",
"any_operator : CLANG_OP_ARROWstar",
"any_operator : CLANG_OP_ASSIGN",
"any_operator : CLANG_OP_DECR",
"any_operator : CLANG_OP_DOTstar",
"any_operator : CLANG_OP_EQ",
"any_operator : CLANG_OP_GE",
"any_operator : CLANG_OP_ICR",
"any_operator : CLANG_OP_LE",
"any_operator : CLANG_OP_LS",
"any_operator : CLANG_OP_NE",
"any_operator : CLANG_OP_RS",
"any_operator : '(' ')'",
"any_operator : '[' ']'",
"entry_name :",
"entry_name : CLANG_NAME",
"entry_name : CLANG_OPERATOR any_operator",
"entry_name : CLANG_TILDE CLANG_NAME",
"function_part :",
"$$2 :",
"function_part : '(' $$2 ')' function_suffix",
"function_suffix :",
"$$3 :",
"function_suffix : CLANG_DELIM_OPEN $$3 CLANG_DELIM_CLOSE",
"scope :",
"scope : CLANG_DELIM_OPEN CLANG_NAME CLANG_CLCL CLANG_DELIM_CLOSE",
"$$4 :",
"type_name : CLANG_DELIM_OPEN $$4 CLANG_DELIM_CLOSE",
"type_name : CLANG_NAME CLANG_CLCL",
"type_name : CLANG_NAME",
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

void clang_parse_string(const std::string& str){
	clangEntryVector.clear();
	clang_lex_clean();
	clang_set_lexer_input(str);
	clang_result_parse();
	clang_lex_clean();
}

const ClangEntryVector& clang_results() {
	return clangEntryVector;
}

void clang_result_error(char*){
}

void clang_read_signature() {
	clang_entry.signature = "(";
	int depth = 1;
	while(depth > 0)
	{
		int ch = clang_yylex();
		if(ch == 0){
			break;
		}
		
		switch(ch) {
		case CLANG_ARG_DELIM_OPEN:
		case CLANG_ARG_DELIM_CLOSE:
		case CLANG_ARG_OPT_DELIM_OPEN:
		case CLANG_ARG_OPT_DELIM_CLOSE:
			break;
			
		case (int)'(':
			depth++;
			break;
			
		case (int)')':
			depth--;
			if(depth == 0)
				clang_yyless(0);
			break;
			
		default:
			clang_entry.signature += clang_lex_get_string();
			clang_entry.signature += " ";
			break;
		}
	}
	clang_entry.signature += ")";
}

void clang_read_type_name(std::string &store){
	store.clear();
	int depth = 1;
	while(true)
	{
		int ch = clang_yylex();
		if(ch == 0){
			break;
		}
		
		switch(ch) {
		case CLANG_DELIM_CLOSE:
			clang_yyless(0);
			return;
			
		default:
			store += clang_lex_get_string();
			store += " ";
			break;
		}
	}
}

void clang_read_entry_name(std::string &store){
	store.clear();
	while(true)
	{
		int ch = clang_yylex();
		if(ch == 0){
			break;
		}
		
		switch(ch) {
		case (int)':':
			clang_yyless(0);
			return;
			
		default:
			store += clang_lex_get_string();
			store += " ";
			break;
		}
	}
}
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
     int yym, yyn, yystate;
#if YYDEBUG
     char *yys;
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
case 3:
{clang_entry.reset();}
break;
case 5:
{
				   /*printf("CodeLite: syntax error, unexpected token '%s' found\n", clang_lex_get_string().c_str());*/
				 }
break;
case 6:
{
					bool is_func = !clang_entry.signature.empty();
					clang_entry.parent       = yyvsp[-2];
					clang_entry.type_name    = is_func ? std::string() : clang_entry.tmp;
					clang_entry.return_value = is_func ? clang_entry.tmp : std::string();
					
					if (clang_entry.type == ClangEntry::TypeUnknown) {
						if(is_func)
							clang_entry.type = ClangEntry::TypeMethod;
						else 
							clang_entry.type = ClangEntry::TypeVariable;
					}
					clangEntryVector.push_back(clang_entry);
				}
break;
case 7:
{yyval = yyvsp[0];}
break;
case 8:
{yyval = yyvsp[0];}
break;
case 9:
{yyval = yyvsp[0];}
break;
case 10:
{yyval = yyvsp[0];}
break;
case 11:
{yyval = yyvsp[0];}
break;
case 12:
{yyval = yyvsp[0];}
break;
case 13:
{yyval = yyvsp[0];}
break;
case 14:
{yyval = yyvsp[0];}
break;
case 15:
{yyval = yyvsp[0];}
break;
case 16:
{yyval = yyvsp[0];}
break;
case 17:
{yyval = yyvsp[0];}
break;
case 18:
{yyval = yyvsp[0];}
break;
case 19:
{yyval = "()";}
break;
case 20:
{yyval = "[]";}
break;
case 21:
{ yyval = ""; }
break;
case 22:
{
				clang_entry.name.clear();
				clang_entry.name = yyvsp[0];
			}
break;
case 23:
{
				clang_entry.name.clear();
				clang_entry.name = yyvsp[-1] + yyvsp[0];
			}
break;
case 24:
{
				clang_entry.name.clear();
				clang_entry.name = yyvsp[-1] + yyvsp[0];
				clang_entry.type = ClangEntry::TypeDtor;
			}
break;
case 26:
{clang_read_signature();}
break;
case 29:
{clang_read_type_name(clang_entry.func_suffix);}
break;
case 31:
{yyval = "";}
break;
case 32:
{yyval = yyvsp[-2];}
break;
case 33:
{clang_read_type_name(clang_entry.tmp);}
break;
case 34:
{
				if(clang_entry.tmp.find("enum ") != std::string::npos) {
					clang_entry.type = ClangEntry::TypeEnum;
				}
			}
break;
case 35:
{
			  clang_entry.type = ClangEntry::TypeCtor;
		  }
break;
case 36:
{
			clang_entry.type = ClangEntry::TypeClass;
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
