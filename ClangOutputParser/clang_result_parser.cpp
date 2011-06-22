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

#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

#ifdef yylex
#undef yylex
#define yylex clang_yylex
#endif

extern int clang_yylex();

/** defined in grammer.l **/
extern bool        clang_set_lexer_input(const std::string &in);
extern const std::string& clang_lex_get_string();
extern void        clang_lex_clean();

/** the parser entry point **/
void clang_parse_string(const std::string& str);

void clang_result_error(char*);
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
#define CLANG_IDENTIFIER 284
#define YYERRCODE 256
short clang_result_lhs[] = {                                        -1,
    0,    0,    1,
};
short clang_result_len[] = {                                         2,
    1,    2,    0,
};
short clang_result_defred[] = {                                      3,
    3,    1,    2,
};
short clang_result_dgoto[] = {                                       1,
    2,
};
short clang_result_sindex[] = {                                      0,
    0,    0,    0,
};
short clang_result_rindex[] = {                                      0,
    0,    0,    0,
};
short clang_result_gindex[] = {                                      0,
   -1,
};
#define YYTABLESIZE 0
short clang_result_table[] = {                                       3,
};
short clang_result_check[] = {                                       1,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 284
#if YYDEBUG
char *clang_result_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"CLANG_COMPLETION","CLANG_HIDDEN",
"CLANG_DELIM_OPEN","CLANG_DELIM_CLOSE","CLANG_ARG_DELIM_OPEN",
"CLANG_ARG_DELIM_CLOSE","CLANG_ARG_OPT_DELIM_OPEN","CLANG_ARG_OPT_DELIM_CLOSE",
"CLANG_STAR","CLANG_AMP","CLANG_TILDE","CLANG_ENUM","CLANG_ANONYMOUS",
"CLANG_CLCL","CLANG_OP_ASSIGN","CLANG_OP_DOTstar","CLANG_OP_ARROW",
"CLANG_OP_ARROWstar","CLANG_OP_ICR","CLANG_OP_DECR","CLANG_OP_LS","CLANG_OP_RS",
"CLANG_OP_LE","CLANG_OP_GE","CLANG_OP_EQ","CLANG_OP_NE","CLANG_ELLIPSIS",
"CLANG_IDENTIFIER",
};
char *clang_result_rule[] = {
"$accept : parse",
"parse : completion_entry",
"parse : parse completion_entry",
"completion_entry :",
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

void clang_parse_string(const std::string& str)
{
	clang_lex_clean();
	clang_set_lexer_input(str);
}

void clang_result_error(char*)
{
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
