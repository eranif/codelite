#ifndef lint
static const char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20100610

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)


#ifndef yyparse
#define yyparse    pp_parse
#endif /* yyparse */

#ifndef yylex
#define yylex      pp_lex
#endif /* yylex */

#ifndef yyerror
#define yyerror    pp_error
#endif /* yyerror */

#ifndef yychar
#define yychar     pp_char
#endif /* yychar */

#ifndef yyval
#define yyval      pp_val
#endif /* yyval */

#ifndef yylval
#define yylval     pp_lval
#endif /* yylval */

#ifndef yydebug
#define yydebug    pp_debug
#endif /* yydebug */

#ifndef yynerrs
#define yynerrs    pp_nerrs
#endif /* yynerrs */

#ifndef yyerrflag
#define yyerrflag  pp_errflag
#endif /* yyerrflag */

#ifndef yylhs
#define yylhs      pp_lhs
#endif /* yylhs */

#ifndef yylen
#define yylen      pp_len
#endif /* yylen */

#ifndef yydefred
#define yydefred   pp_defred
#endif /* yydefred */

#ifndef yydgoto
#define yydgoto    pp_dgoto
#endif /* yydgoto */

#ifndef yysindex
#define yysindex   pp_sindex
#endif /* yysindex */

#ifndef yyrindex
#define yyrindex   pp_rindex
#endif /* yyrindex */

#ifndef yygindex
#define yygindex   pp_gindex
#endif /* yygindex */

#ifndef yytable
#define yytable    pp_table
#endif /* yytable */

#ifndef yycheck
#define yycheck    pp_check
#endif /* yycheck */

#ifndef yyname
#define yyname     pp_name
#endif /* yyname */

#ifndef yyrule
#define yyrule     pp_rule
#endif /* yyrule */
#define YYPREFIX "pp_"

#define YYPURE 0


/* Copyright Eran Ifrah(c)*/

/*************** Includes and Defines *****************************/
#include <wx/string.h>
#include "pptable.h"

#define YYSTYPE wxString
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

#ifdef yylex
#undef yylex
#define yylex pp_lex
#endif

extern void pp_error(char *st);
extern int pp_lex();
extern wxString g_definition;
extern int in_if_1;
extern bool g_forCC;
extern wxString g_filename;

/* Static*/
static std::vector<wxString> g_tmpMacros;

/*************** Standard ytab.c continues here *********************/

#ifndef YYSTYPE
typedef int YYSTYPE;
#endif

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

extern int YYPARSE_DECL();
extern int YYLEX_DECL();

#define PP_DEFINE 257
#define PP_IF 258
#define PP_IFDEF 259
#define PP_DEFINED 260
#define PP_UNDEF 261
#define PP_ELSE 262
#define PP_ELIF 263
#define PP_ENDIF 264
#define PP_POUND 265
#define PP_IDENTIFIER 266
#define PP_COMPLEX_REPLACEMENT 267
#define PP_IFNDEF 268
#define PP_ZERO 269
#define PP_CPLUSPLUS 270
#define PP_INCLUDE 271
#define PP_AND 272
#define PP_OR 273
#define PP_EQUAL 274
#define PP_NEQUAL 275
#define PP_INT 276
#define PP_LOWERTHAN 277
#define PP_GREATERTHAN 278
#define YYERRCODE 256
static const short pp_lhs[] = {                          -1,
    0,    0,    3,    1,    2,    2,    2,    2,    2,    2,
    2,    2,   10,    6,    4,    5,   11,   11,   11,    7,
    8,    9,   12,   13,   13,   14,   14,   14,   14,   14,
   14,   15,   15,   16,   16,   16,   16,
};
static const short pp_len[] = {                           2,
    0,    2,    0,    2,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    2,    3,    6,    0,    1,    3,    2,
    2,    2,    1,    1,    3,    0,    3,    1,    3,    5,
    4,    1,    1,    1,    1,    1,    1,
};
static const short pp_defred[] = {                        1,
    3,    2,    0,   12,    0,    0,    0,    0,   13,    4,
    5,    6,    7,    8,    9,   10,   11,    0,    0,    0,
   14,    0,   21,    0,   24,   20,   22,   15,    0,    0,
   34,   35,   36,   37,    0,    0,   32,   33,    0,   18,
    0,    0,   29,   27,    0,   25,    0,    0,   31,    0,
   16,   19,   30,
};
static const short pp_dgoto[] = {                         1,
    2,   10,    3,   11,   12,   13,   14,   15,   16,   17,
   41,   23,   24,   25,   39,   35,
};
static const short pp_sindex[] = {                        0,
    0,    0, -250,    0, -244,  -38, -241,  -35,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -37,  -36, -259,
    0, -240,    0, -262,    0,    0,    0,    0, -239, -238,
    0,    0,    0,    0, -247,  -41,    0,    0,  -35,    0,
  -24,  -11,    0,    0, -245,    0, -235, -233,    0,   -7,
    0,    0,    0,
};
static const short pp_rindex[] = {                        0,
    0,    0,    0,    0,    0,    1,    0,    1,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   12,
    0,    0,    0,   23,    0,    0,    0,    0,  -20,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    1,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,
};
static const short pp_gindex[] = {                        0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   27,    0,   -3,    0,    2,
};
#define YYTABLESIZE 287
static const short pp_table[] = {                        44,
   26,   22,   29,   30,   22,    4,    5,    6,    7,   37,
   38,   28,    8,    9,   31,   32,   47,   33,   34,   48,
   17,   18,   23,   17,   26,   36,   40,   42,   43,   49,
   50,   51,   52,   53,   27,   46,    0,   45,    0,    0,
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
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   19,    0,    0,   19,    0,    0,   20,    0,   28,
   20,   21,   31,   32,    0,   33,   34,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   26,   26,   26,   26,
    0,    0,    0,   26,   26,    0,    0,   28,   28,   28,
   28,    0,   26,   26,   28,   28,    0,    0,   23,   23,
   23,   23,    0,   28,   28,   23,   23,
};
static const short pp_check[] = {                        41,
    0,   40,   40,   40,   40,  256,  257,  258,  259,  272,
  273,    0,  263,  264,  274,  275,   41,  277,  278,   44,
   41,  266,    0,   44,  266,  266,  266,  266,  276,   41,
  276,  267,  266,   41,    8,   39,   -1,   36,   -1,   -1,
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
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  260,   -1,   -1,  260,   -1,   -1,  266,   -1,  267,
  266,  270,  274,  275,   -1,  277,  278,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  256,  257,  258,  259,
   -1,   -1,   -1,  263,  264,   -1,   -1,  256,  257,  258,
  259,   -1,  272,  273,  263,  264,   -1,   -1,  256,  257,
  258,  259,   -1,  272,  273,  263,  264,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 278
#if YYDEBUG
static const char *yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"PP_DEFINE","PP_IF",
"PP_IFDEF","PP_DEFINED","PP_UNDEF","PP_ELSE","PP_ELIF","PP_ENDIF","PP_POUND",
"PP_IDENTIFIER","PP_COMPLEX_REPLACEMENT","PP_IFNDEF","PP_ZERO","PP_CPLUSPLUS",
"PP_INCLUDE","PP_AND","PP_OR","PP_EQUAL","PP_NEQUAL","PP_INT","PP_LOWERTHAN",
"PP_GREATERTHAN",
};
static const char *yyrule[] = {
"$accept : translation_unit",
"translation_unit :",
"translation_unit : translation_unit the_macros_rule",
"$$1 :",
"the_macros_rule : $$1 macros",
"macros : define_simple_macros",
"macros : define_func_like_macros",
"macros : if_cplusplus",
"macros : ifdef_simple_macro",
"macros : if_macros",
"macros : elif_macros",
"macros : end_if",
"macros : error",
"end_if : PP_ENDIF",
"if_cplusplus : PP_IF PP_CPLUSPLUS",
"define_simple_macros : PP_DEFINE PP_IDENTIFIER PP_COMPLEX_REPLACEMENT",
"define_func_like_macros : PP_DEFINE PP_IDENTIFIER '(' args_list ')' PP_COMPLEX_REPLACEMENT",
"args_list :",
"args_list : PP_IDENTIFIER",
"args_list : args_list ',' PP_IDENTIFIER",
"ifdef_simple_macro : PP_IFDEF PP_IDENTIFIER",
"if_macros : PP_IF if_condition",
"elif_macros : PP_ELIF if_condition",
"if_condition : generic_condition",
"generic_condition : generic_condition_base",
"generic_condition : generic_condition logical_operator generic_condition_base",
"generic_condition_base :",
"generic_condition_base : '(' PP_IDENTIFIER ')'",
"generic_condition_base : PP_IDENTIFIER",
"generic_condition_base : PP_IDENTIFIER test_operator PP_INT",
"generic_condition_base : '(' PP_IDENTIFIER test_operator PP_INT ')'",
"generic_condition_base : PP_DEFINED '(' PP_IDENTIFIER ')'",
"logical_operator : PP_AND",
"logical_operator : PP_OR",
"test_operator : PP_EQUAL",
"test_operator : PP_NEQUAL",
"test_operator : PP_LOWERTHAN",
"test_operator : PP_GREATERTHAN",

};
#endif
/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH  500
#endif
#endif

#define YYINITSTACKSIZE 500

int      yydebug;
int      yynerrs;

typedef struct {
    unsigned stacksize;
    short    *s_base;
    short    *s_mark;
    short    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* variables for the parser stack */
static YYSTACKDATA yystack;



#if YYDEBUG
#include <stdio.h>		/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = data->s_mark - data->s_base;
    newss = (data->s_base != 0)
          ? (short *)realloc(data->s_base, newsize * sizeof(*newss))
          : (short *)malloc(newsize * sizeof(*newss));
    if (newss == 0)
        return -1;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (data->l_base != 0)
          ? (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs))
          : (YYSTYPE *)malloc(newsize * sizeof(*newvs));
    if (newvs == 0)
        return -1;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack)) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = YYLEX) < 0) yychar = 0;
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
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack))
        {
            goto yyoverflow;
        }
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
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

    yyerror("syntax error");

    goto yyerrlab;

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yystack.s_mark]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack))
                {
                    goto yyoverflow;
                }
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
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
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 3:
	{g_tmpMacros.clear();}
break;
case 12:
	{
            /*wxPrintf(wxT("CodeLite: syntax error, unexpected token '%s' found\n"), pp_lval.c_str());*/
        }
break;
case 14:
	{
            if(in_if_1 == 0)
                in_if_1 = 1;
        }
break;
case 15:
	{
            PPToken token;
			token.fileName = g_filename;
            token.name     = yystack.l_mark[-1];
            token.flags    = (PPToken::IsValid | PPToken::IsOverridable);
            
            if(in_if_1) {
                /* the token was found in a '#if 1' block - dont allow overriding it*/
                token.flags &= ~(PPToken::IsOverridable);
            }
            
            token.replacement = g_definition;
			
			if(g_forCC) {
				
				if(!token.replacement.empty()) {
					wxChar ch = token.replacement.at(0);
					if( !(ch >= (int)wxT('0') && ch <= (int)wxT('9')) )
						PPTable::Instance()->Add( token );
				}
				
			} else {
				PPTable::Instance()->Add( token );
			}
		}
break;
case 16:
	{
            PPToken token;
			token.fileName    = g_filename;
            token.name        = yystack.l_mark[-4];
            token.replacement = g_definition;
            token.flags       = (PPToken::IsFunctionLike | PPToken::IsValid | PPToken::IsOverridable);
            if(in_if_1) {
                /* the token was found in a '#if 1' block - dont allow overriding it*/
                token.flags &= ~(PPToken::IsOverridable);
            }
            token.processArgs( yystack.l_mark[-2] );
            PPTable::Instance()->Add( token ); 
        }
break;
case 18:
	{ yyval = yystack.l_mark[0];           }
break;
case 19:
	{ yyval = yystack.l_mark[-2] + yystack.l_mark[-1] + yystack.l_mark[0]; }
break;
case 20:
	{
			PPTable::Instance()->AddUsed(yystack.l_mark[0]);
		}
break;
case 23:
	{
			for(size_t i=0; i<g_tmpMacros.size(); i++) {
				/*wxPrintf(wxT("Collected: %s\n"), g_tmpMacros[i].c_str());*/
				PPTable::Instance()->AddUsed(g_tmpMacros[i]);
			}
		}
break;
case 27:
	{g_tmpMacros.push_back(yystack.l_mark[-1]);}
break;
case 28:
	{g_tmpMacros.push_back(yystack.l_mark[0]);}
break;
case 29:
	{g_tmpMacros.push_back(yystack.l_mark[-2]);}
break;
case 30:
	{g_tmpMacros.push_back(yystack.l_mark[-3]);}
break;
case 31:
	{g_tmpMacros.push_back(yystack.l_mark[-1]);}
break;
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            if ((yychar = YYLEX) < 0) yychar = 0;
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
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack))
    {
        goto yyoverflow;
    }
    *++yystack.s_mark = (short) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    yyerror("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
