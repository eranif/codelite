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
	
	/* std::cout << "Before: '" << param << "'\n";*/
	param.erase(param.find_last_not_of(whitespace)+1);
	/* std::cout << "AfterRight: '" << param << "'\n";*/
	param.erase(0, param.find_first_not_of(whitespace));
	/* std::cout << "AfterLeft: '" << param << "'\n";*/
}

/* inverse of atoi*/
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
#define SHELL 260
#define word 261
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    1,    1,    1,    1,    1,    1,    7,    8,
   10,   11,   12,   12,   13,   13,    2,    2,   14,   14,
    3,    3,    4,   15,   15,    5,    6,    9,    9,    9,
    9,
};
short yylen[] = {                                         2,
    0,    2,    1,    3,    2,    2,    2,    2,    2,    1,
    1,    2,    3,    1,    1,    2,    0,    1,    0,    1,
    2,    3,    1,    1,    1,    3,    1,    1,    1,    2,
    2,
};
short yydefred[] = {                                      1,
    0,    0,   15,   27,    3,    2,    0,    0,    0,    0,
    0,    8,    0,    0,    0,    0,   14,    0,    5,    6,
    7,   16,   24,   25,    0,   12,    9,    4,    0,   28,
    0,    0,   29,   21,    0,    0,   26,   22,   11,   13,
   31,   30,
};
short yydgoto[] = {                                       1,
    6,    7,   15,    8,    9,   10,   16,   31,   32,   40,
   17,   33,   35,   37,   25,
};
short yysindex[] = {                                      0,
   -2,    1,    0,    0,    0,    0,  -30,    4,    5,    6,
  -52,    0, -248,  -23,  -10,  -32,    0, -238,    0,    0,
    0,    0,    0,    0,  -32,    0,    0,    0, -238,    0,
  -21,  -31,    0,    0, -236,  -31,    0,    0,    0,    0,
    0,    0,
};
short yyrindex[] = {                                      0,
  -29,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   -9,    0,    0,    0,    0,    0,    0,   -8,    0,    0,
    0,    0,    0,    0,    4,    0,    0,    0,   -8,    0,
    0,  -19,    0,    0,   -7,    5,    0,    0,    0,    0,
    0,    0,
};
short yygindex[] = {                                      0,
    0,   -6,    0,    0,    0,    0,    0,    0,   -1,    0,
    0,    3,   24,    0,    0,
};
#define YYTABLESIZE 257
short yytable[] = {                                      28,
   23,   17,   18,   14,   14,   14,   17,    5,   24,   18,
   12,   34,   26,   19,   20,   21,   27,   29,    3,   39,
   22,   10,   38,   36,   11,   14,   18,   17,   18,    0,
    0,    0,    0,    0,   42,    0,    0,    0,   42,    0,
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
    0,    0,    0,    0,   22,   23,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   30,   41,    0,   13,   13,   13,
   17,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   13,
   18,   17,   18,    2,    3,    0,    4,
};
short yycheck[] = {                                      10,
   10,   10,   10,   36,   36,   36,   36,   10,   61,    7,
   10,   18,  261,   10,   10,   10,   40,   15,  257,   41,
  257,   41,   29,   25,    1,   36,   36,   36,   36,   -1,
   -1,   -1,   -1,   -1,   32,   -1,   -1,   -1,   36,   -1,
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
   -1,   -1,   -1,   -1,  257,  258,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  257,  257,   -1,  260,  260,  260,
  260,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  260,
  260,  260,  260,  256,  257,   -1,  259,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 261
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,"'\\n'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,"'$'",0,0,0,"'('","')'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'='",0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"WORD",
"ASSIGN","PRINT","SHELL","word",
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
"shellcommand : SHELL word",
"variable : open name close",
"variable : shellcommand",
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
{	yyval = yyvsp[0];			}
break;
case 11:
{	yyval = "";			}
break;
case 12:
{	yyval = yyvsp[-1];	}
break;
case 13:
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
case 14:
{	
			yyval = yyvsp[0];			
			printf("Matched\n");
			}
break;
case 15:
{	yyval = yyvsp[0];				}
break;
case 16:
{	yyval = yyvsp[-1] + yyvsp[0];			}
break;
case 17:
{	yyval = "";				}
break;
case 18:
{	yyval = yyvsp[0];				}
break;
case 19:
{	yyval = "";				}
break;
case 20:
{	yyval = yyvsp[0];				}
break;
case 21:
{	yyval = yyvsp[-1] + yyvsp[0];		}
break;
case 22:
{	yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];	}
break;
case 23:
{	yyval = yyvsp[0];				}
break;
case 24:
{	yyval = ""; append = true;			}
break;
case 25:
{	yyval = ""; append = false;			}
break;
case 26:
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
case 27:
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
case 28:
{	yyval = yyvsp[0];				}
break;
case 29:
{	yyval = yyvsp[0];				}
break;
case 30:
{	yyval = yyvsp[-1] + yyvsp[0];			}
break;
case 31:
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
