#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
 
/**** Includes and Defines *****************************/
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

#define YYDEBUG 0        		/* get the pretty debugging code to compile*/
#define YYSTYPE std::string

typedef std::vector<std::string> Strings;
typedef std::map<std::string, std::string> tokens;
typedef tokens::iterator Itokens;

extern Strings TheOutput;
extern Strings TheUnmatched;
extern Strings TheError;
extern tokens TheTokens;
extern int lineno;

bool append = false;
int yylex(void);

void TrimString(std::string& param)
{
	static std::string whitespace = " \t";
	
	std::cout << "Before: '" << param << "'\n";
	param.erase(param.find_last_not_of(whitespace)+1);
	std::cout << "AfterRight: '" << param << "'\n";
	param.erase(0, param.find_first_not_of(whitespace));
	std::cout << "AfterLeft: '" << param << "'\n";
}

// inverse of atoi
std::string itoa(const int x)
{
	std::ostringstream o;
	if(!(o << x)) return "itoa(), ERROR!";
	return o.str();
}

void yyerror(char* param)
{
/*	printf("parser error: %s\n", string);*/
}

/*************** Standard variable.y: continues here *********************/
#define WORD 257
#define ASSIGN 258
#define PRINT 259
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    1,    1,    1,    1,    1,    1,    7,    8,
   10,   11,   12,   12,    2,    2,   13,   13,    3,    3,
    4,   14,   14,    5,    6,    9,    9,    9,    9,
};
short yylen[] = {                                         2,
    0,    2,    1,    3,    2,    2,    2,    2,    2,    1,
    1,    3,    1,    2,    0,    1,    0,    1,    2,    3,
    1,    1,    1,    3,    1,    1,    1,    2,    2,
};
short yydefred[] = {                                      1,
    0,    0,   13,   25,    3,    2,    0,    0,    0,    0,
    0,    8,    0,    0,    0,    0,    5,    6,    7,   14,
   22,   23,    0,    9,    4,    0,   26,    0,    0,   27,
   19,    0,    0,   24,   20,   11,   12,   29,   28,
};
short yydgoto[] = {                                       1,
    6,    7,   14,    8,    9,   10,   15,   28,   29,   37,
   30,   32,   34,   23,
};
short yysindex[] = {                                      0,
  -10,    2,    0,    0,    0,    0,  -26,    3,    4,    5,
  -58,    0,  -24,   -5,  -35, -239,    0,    0,    0,    0,
    0,    0,  -35,    0,    0, -239,    0,  -22,  -34,    0,
    0, -237,  -34,    0,    0,    0,    0,    0,    0,
};
short yyrindex[] = {                                      0,
  -15,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   -4,    0,    0,    0,    0,   -2,    0,    0,    0,    0,
    0,    0,    3,    0,    0,   -2,    0,    0,  -19,    0,
    0,   -1,    4,    0,    0,    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
    0,   -9,    0,    0,    0,    0,    0,    0,    1,    0,
   -3,   22,    0,    0,
};
#define YYTABLESIZE 249
short yytable[] = {                                       5,
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
short yycheck[] = {                                      10,
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
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,"'\\n'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,"'$'",0,0,0,"'('","')'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'='",0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"WORD",
"ASSIGN","PRINT",
};
char *yyrule[] = {
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
/* End of grammar */










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
case 2:
{	yyval = "";				}
break;
case 3:
{	yyval = "";				}
break;
case 4:
{	yyval = yyvsp[-2]+yyvsp[-1]; TheOutput.push_back(yyval);			}
break;
case 5:
{	yyval = yyvsp[-1]; TheOutput.push_back(yyvsp[-1]);			}
break;
case 6:
{	yyval = "";				}
break;
case 7:
{	yyval = "";				}
break;
case 8:
{
							YYSTYPE msg;
							msg.append("Line ").append(itoa(lineno)).append(": Unexpected token '").append(yylval).append("'.");
							TheError.push_back(msg);
							yyerrok;
						}
break;
case 9:
{	yyval = "";			}
break;
case 10:
{	yyval = yyvsp[0];					}
break;
case 11:
{	yyval = "";			}
break;
case 12:
{
						YYSTYPE token = yyvsp[-1];
						TrimString(token);

						if(TheTokens[token].size() > 0)
						{
							yyval = TheTokens[token];
						}
						else
						{
							TheUnmatched.push_back(token);
							yyval = "";
						}
					}
break;
case 13:
{	yyval = yyvsp[0];				}
break;
case 14:
{	yyval = yyvsp[-1] + yyvsp[0];			}
break;
case 15:
{	yyval = "";				}
break;
case 16:
{	yyval = yyvsp[0];				}
break;
case 17:
{	yyval = "";				}
break;
case 18:
{	yyval = yyvsp[0];				}
break;
case 19:
{	yyval = yyvsp[-1] + yyvsp[0];		}
break;
case 20:
{	yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];	}
break;
case 21:
{	yyval = yyvsp[0];				}
break;
case 22:
{	yyval = ""; append = true;			}
break;
case 23:
{	yyval = ""; append = false;			}
break;
case 24:
{
	 					YYSTYPE name = yyvsp[-2];
						YYSTYPE value = yyvsp[0];
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
	 					yyval = name + "=" + value;			
					}
break;
case 25:
{
	 					YYSTYPE result ="Tokens: \n";
						for(Itokens it = TheTokens.begin(); it != TheTokens.end(); it++)
						{
							result += "'" + it->first + "'='" + it->second + "'\n";
						}
						result += "Done.";
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
{	yyval = yyvsp[-1] + yyvsp[0];			}
break;
case 29:
{	yyval = yyvsp[-1] + yyvsp[0];			}
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
