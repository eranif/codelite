#ifndef lint
static const char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif

#include <stdlib.h>

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20070509

#define YYEMPTY (-1)
#define yyclearin    (yychar = YYEMPTY)
#define yyerrok      (yyerrflag = 0)
#define YYRECOVERING (yyerrflag != 0)

extern int yyparse(void);

static int yygrowstack(void);
#define yyparse clplugin_scope_parse
#define yylex clplugin_scope_lex
#define yyerror clplugin_scope_error
#define yychar clplugin_scope_char
#define yyval clplugin_scope_val
#define yylval clplugin_scope_lval
#define yydebug clplugin_scope_debug
#define yynerrs clplugin_scope_nerrs
#define yyerrflag clplugin_scope_errflag
#define yyss clplugin_scope_ss
#define yyssp clplugin_scope_ssp
#define yyvs clplugin_scope_vs
#define yyvsp clplugin_scope_vsp
#define yylhs clplugin_scope_lhs
#define yylen clplugin_scope_len
#define yydefred clplugin_scope_defred
#define yydgoto clplugin_scope_dgoto
#define yysindex clplugin_scope_sindex
#define yyrindex clplugin_scope_rindex
#define yygindex clplugin_scope_gindex
#define yytable clplugin_scope_table
#define yycheck clplugin_scope_check
#define yyname clplugin_scope_name
#define yyrule clplugin_scope_rule
#define YYPREFIX "clplugin_scope_"
/**** Includes and Defines *****************************/
#include <stdio.h>
#include <wx/string.h>
#include <map>
#include <wx/arrstr.h>
#include <wx/wxchar.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/app.h>

#define YYDEBUG 0        		/* get the pretty debugging code to compile*/
#define YYSTYPE wxString


typedef wxArrayString Strings;
typedef std::map<wxString, wxString> tokens;
typedef tokens::iterator Itokens;

extern Strings TheOutput;
extern Strings TheUnmatched;
extern Strings TheError;
extern tokens TheTokens;
extern int lineno;

bool append = false;
int yylex(void);
void TrimString(wxString &string)
{
        bool good = wxInitialize();
	if(!good)
	{
		printf("wx could not be initialized, aborting.\n");
		exit(-1);
	}
	else
	{
		printf("wx initialized succesfully!\n");
		wxFFileOutputStream MYoutput( stderr );
		wxTextOutputStream MYcout( MYoutput );
		wxString string = wxT(" a test yo ");
		MYcout << wxT("Before: '") << string << wxT("'\n");
		string = string.Trim(true);
		MYcout << wxT("AfterT: '") << string << wxT("'\n");
		string = string.Trim(false);
		MYcout << wxT("AfterF: '") << string << wxT("'\n");
	}
	wxUninitialize();
}

void yyerror(char* string)
{
/*	printf("parser error: %s\n", string);*/
}

/*************** Standard variable.y: continues here *********************/
#define WORD 257
#define ASSIGN 258
#define PRINT 259
#define YYERRCODE 256
short clplugin_scope_lhs[] = {                                        -1,
    0,    0,    1,    1,    1,    1,    1,    1,    7,    8,
   10,   11,   12,   12,    2,    2,   13,   13,    3,    3,
    4,   14,   14,    5,    6,    9,    9,    9,    9,
};
short clplugin_scope_len[] = {                                         2,
    0,    2,    1,    3,    2,    2,    2,    2,    2,    1,
    1,    3,    1,    2,    0,    1,    0,    1,    2,    3,
    1,    1,    1,    3,    1,    1,    1,    2,    2,
};
short clplugin_scope_defred[] = {                                      1,
    0,    0,   13,   25,    3,    2,    0,    0,    0,    0,
    0,    8,    0,    0,    0,    0,    5,    6,    7,   14,
   22,   23,    0,    9,    4,    0,   26,    0,    0,   27,
   19,    0,    0,   24,   20,   11,   12,   29,   28,
};
short clplugin_scope_dgoto[] = {                                       1,
    6,    7,   14,    8,    9,   10,   15,   28,   29,   37,
   30,   32,   34,   23,
};
short clplugin_scope_sindex[] = {                                      0,
  -10,    2,    0,    0,    0,    0,  -26,    3,    4,    5,
  -58,    0,  -24,   -5,  -35, -239,    0,    0,    0,    0,
    0,    0,  -35,    0,    0, -239,    0,  -22,  -34,    0,
    0, -237,  -34,    0,    0,    0,    0,    0,    0,
};
short clplugin_scope_rindex[] = {                                      0,
  -15,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   -4,    0,    0,    0,    0,   -2,    0,    0,    0,    0,
    0,    0,    3,    0,    0,   -2,    0,    0,  -19,    0,
    0,   -1,    4,    0,    0,    0,    0,    0,    0,
};
short clplugin_scope_gindex[] = {                                      0,
    0,   -9,    0,    0,    0,    0,    0,    0,    1,    0,
   -3,   22,    0,    0,
};
#define YYTABLESIZE 249
short clplugin_scope_table[] = {                                       5,
   13,   13,   22,   16,   25,   21,   31,   15,   16,   13,
   26,   12,   17,   18,   19,   24,   35,    3,   36,   20,
   15,   10,   11,   33,    0,   39,    0,    0,    0,   39,
   13,   16,    0,   15,   16,    0,    0,    0,    0,    0,
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
    0,    0,    0,    0,    0,    0,    0,    0,   20,   21,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   27,   38,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    2,    3,    0,    4,
};
short clplugin_scope_check[] = {                                      10,
   36,   36,   61,    7,   10,   10,   16,   10,   10,   36,
   14,   10,   10,   10,   10,   40,   26,  257,   41,  257,
   36,   41,    1,   23,   -1,   29,   -1,   -1,   -1,   33,
   36,   36,   -1,   36,   36,   -1,   -1,   -1,   -1,   -1,
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
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  257,  257,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  256,  257,   -1,  259,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 259
#if YYDEBUG
char *clplugin_scope_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,"'\\n'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,"'$'",0,0,0,"'('","')'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'='",0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"WORD",
"ASSIGN","PRINT",
};
char *clplugin_scope_rule[] = {
"$accept : input",
"input :",
"input : input line",
"line : '\\n'",
"line : optwords vars_line '\\n'",
"line : wordsline '\\n'",
"line : assgnline '\\n'",
"line : printline '\\n'",
"line : error '\\n'",
"open : '$' '('",
"name : wordvars",
"close : ')'",
"variable : open name close",
"words : WORD",
"words : words WORD",
"optwords :",
"optwords : words",
"optvars :",
"optvars : wordvars",
"vars_line : variable optwords",
"vars_line : vars_line variable optwords",
"wordsline : words",
"assignm : ASSIGN",
"assignm : '='",
"assgnline : words assignm optvars",
"printline : PRINT",
"wordvars : WORD",
"wordvars : variable",
"wordvars : wordvars variable",
"wordvars : wordvars WORD",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#if YYDEBUG
#include <stdio.h>
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
int      yyerrflag;
int      yychar;
short   *yyssp;
YYSTYPE *yyvsp;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* variables for the parser stack */
static short   *yyss;
static short   *yysslim;
static YYSTYPE *yyvs;
static int      yystacksize;
/* End of grammar */










/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(void)
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = yyssp - yyss;
    newss = (yyss != 0)
          ? (short *)realloc(yyss, newsize * sizeof(*newss))
          : (short *)malloc(newsize * sizeof(*newss));
    if (newss == 0)
        return -1;

    yyss  = newss;
    yyssp = newss + i;
    newvs = (yyvs != 0)
          ? (YYSTYPE *)realloc(yyvs, newsize * sizeof(*newvs))
          : (YYSTYPE *)malloc(newsize * sizeof(*newvs));
    if (newvs == 0)
        return -1;

    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse(void)
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register const char *yys;

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

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
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
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
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
                if (yyssp >= yysslim && yygrowstack())
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
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 2:
{	/* Do Nothing */				}
break;
case 3:
{	/* Do Nothing */				}
break;
case 4:
{	TheOutput.push_back(yyvsp[-2]+yyvsp[-1]);			}
break;
case 5:
{	TheOutput.push_back(yyvsp[-1]);			}
break;
case 6:
{	/* Do Nothing */				}
break;
case 7:
{	/* Do Nothing */				}
break;
case 8:
{
						YYSTYPE msg;
						msg << wxT("Line ") << lineno << wxT(": Unexpected token '") << yylval << wxT("'.");
						TheError.push_back(msg);
						yyerrok;
					}
break;
case 9:
{	/* do nothing */			}
break;
case 10:
{	yyval = yyvsp[0];				}
break;
case 11:
{	/* do nothing */			}
break;
case 12:
{
						wxString token = yyvsp[-1];
						TrimString(token);

						if(TheTokens[token].size() > 0)
						{
							yyval = TheTokens[token];
						}
						else
						{
							TheUnmatched.push_back(token);
							yyval = wxEmptyString;
						}
					}
break;
case 13:
{	yyval = yyvsp[0];				}
break;
case 14:
{	yyval = yyvsp[-1] + yyvsp[0];				}
break;
case 15:
{	yyval = wxEmptyString;			}
break;
case 16:
{	yyval = yyvsp[0];				}
break;
case 17:
{	yyval = wxEmptyString;			}
break;
case 18:
{	yyval = yyvsp[0];				}
break;
case 19:
{	yyval = yyvsp[-1] + yyvsp[0];				}
break;
case 20:
{	yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];			}
break;
case 21:
{	yyval = yyvsp[0];				}
break;
case 22:
{	append = true;				}
break;
case 23:
{	append = false;				}
break;
case 24:
{
	 					wxString name = yyvsp[-2];
						wxString value = yyvsp[0];
						TrimString(name);
						TrimString(value);

	 					if(append)
						{
	 						TheTokens[name] += value;
						}
						else
						{
							TheTokens[name] = value;
						}
	 					yyval = name + wxT("=") + value;			
					}
break;
case 25:
{
	 					YYSTYPE result = wxT("Tokens: \n");
						for(Itokens it = TheTokens.begin(); it != TheTokens.end(); it++)
						{
							result += wxT("'") + it->first + wxT("'='") + it->second + wxT("'\n");
						}
						result += wxT("Done.");
						yyval = result;
					}
break;
case 26:
{	yyval = yyvsp[0];				}
break;
case 27:
{	yyval = yyvsp[0];				}
break;
case 28:
{	yyval = yyvsp[-1] + yyvsp[0];				}
break;
case 29:
{	yyval = yyvsp[-1] + yyvsp[0];				}
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
    if (yyssp >= yysslim && yygrowstack())
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
