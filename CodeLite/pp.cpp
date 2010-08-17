#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define yyparse pp_parse
#define yylex pp_lex
#define yyerror pp_error
#define yychar pp_char
#define yyval pp_val
#define yylval pp_lval
#define yydebug pp_debug
#define yynerrs pp_nerrs
#define yyerrflag pp_errflag
#define yyss pp_ss
#define yyssp pp_ssp
#define yyvs pp_vs
#define yyvsp pp_vsp
#define yylhs pp_lhs
#define yylen pp_len
#define yydefred pp_defred
#define yydgoto pp_dgoto
#define yysindex pp_sindex
#define yyrindex pp_rindex
#define yygindex pp_gindex
#define yytable pp_table
#define yycheck pp_check
#define yyname pp_name
#define yyrule pp_rule
#define YYPREFIX "pp_"
/* Copyright Eran Ifrah(c)*/
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

/*************** Standard ytab.c continues here *********************/
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
#define YYERRCODE 256
short pp_lhs[] = {                                        -1,
    0,    0,    1,    1,    1,    1,    4,    2,    3,    5,
    5,    5,
};
short pp_len[] = {                                         2,
    0,    2,    1,    1,    1,    1,    2,    3,    6,    0,
    1,    3,
};
short pp_defred[] = {                                      1,
    0,    6,    0,    0,    2,    3,    4,    5,    0,    7,
    8,    0,   11,    0,    0,    0,    9,   12,
};
short pp_dgoto[] = {                                       1,
    5,    6,    7,    8,   14,
};
short pp_sindex[] = {                                      0,
 -249,    0, -265, -266,    0,    0,    0,    0,  -40,    0,
    0, -256,    0,  -39, -255, -253,    0,    0,
};
short pp_rindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  -38,    0,    0,    0,    0,    0,    0,
};
short pp_gindex[] = {                                      0,
    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 227
short pp_table[] = {                                      12,
    9,   15,   10,   10,   16,   10,    2,    3,    4,   13,
    0,   17,   18,    0,    0,    0,    0,    0,    0,    0,
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
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   11,
};
short pp_check[] = {                                      40,
  266,   41,   41,  270,   44,   44,  256,  257,  258,  266,
   -1,  267,  266,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
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
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  267,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 271
#if YYDEBUG
char *pp_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"PP_DEFINE","PP_IF",
"PP_IFDEF","PP_DEFINED","PP_UNDEF","PP_ELSE","PP_ELIF","PP_ENDIF","PP_POUND",
"PP_IDENTIFIER","PP_COMPLEX_REPLACEMENT","PP_IFNDEF","PP_ZERO","PP_CPLUSPLUS",
"PP_INCLUDE",
};
char *pp_rule[] = {
"$accept : translation_unit",
"translation_unit :",
"translation_unit : translation_unit macros",
"macros : define_simple_macros",
"macros : define_func_like_macros",
"macros : if_cplusplus",
"macros : error",
"if_cplusplus : PP_IF PP_CPLUSPLUS",
"define_simple_macros : PP_DEFINE PP_IDENTIFIER PP_COMPLEX_REPLACEMENT",
"define_func_like_macros : PP_DEFINE PP_IDENTIFIER '(' args_list ')' PP_COMPLEX_REPLACEMENT",
"args_list :",
"args_list : PP_IDENTIFIER",
"args_list : args_list ',' PP_IDENTIFIER",
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
case 6:
{
            /*wxPrintf(wxT("CodeLite: syntax error, unexpected token '%s' found\n"), pp_lval.c_str());*/
        }
break;
case 7:
{
            if(in_if_1 == 0)
                in_if_1 = 1;
        }
break;
case 8:
{
            PPToken token;
            token.name = yyvsp[-1];
            token.flags = (PPToken::IsValid | PPToken::IsOverridable);
            
            if(in_if_1) {
                /* the token was found in a '#if 1' block - dont allow overriding it*/
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
case 9:
{
            PPToken token;
            token.name = yyvsp[-4];
            token.replacement = g_definition;
            token.flags = (PPToken::IsFunctionLike | PPToken::IsValid | PPToken::IsOverridable);
            if(in_if_1) {
                /* the token was found in a '#if 1' block - dont allow overriding it*/
                token.flags &= ~(PPToken::IsOverridable);
            }
            token.processArgs( yyvsp[-2] );
            PPTable::Instance()->Add( token ); 
        }
break;
case 11:
{ yyval = yyvsp[0];           }
break;
case 12:
{ yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0]; }
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
