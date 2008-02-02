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

extern std::string getShellResult(const std::string& command);
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
#define IFEQ 261
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    1,    1,    1,    1,    1,    1,    1,    8,
    9,   11,    7,   12,   13,   13,   15,   15,    2,    2,
   16,   16,   14,   14,    3,    3,    4,   17,   17,    5,
    6,   10,   10,   10,   10,   10,   10,
};
short yylen[] = {                                         2,
    0,    2,    1,    3,    2,    2,    2,    2,    2,    2,
    1,    1,   10,    3,    1,    3,    1,    2,    0,    1,
    0,    1,    0,    1,    2,    3,    1,    1,    1,    5,
    1,    1,    1,    2,    2,    2,    2,
};
short yydefred[] = {                                      1,
    0,    0,    0,   31,    0,    3,    2,    0,    0,    0,
    0,    0,    0,    9,   17,    0,    0,    0,    0,    0,
    0,    0,    5,    6,    7,    8,    0,   28,   29,    0,
   18,    0,   10,    4,    0,   32,    0,    0,   33,   15,
   25,    0,   16,    0,    0,   26,   12,   14,   35,   37,
   36,   34,    0,   30,    0,    0,    0,    0,    0,   13,
};
short yydgoto[] = {                                       1,
    7,    8,   20,    9,   10,   11,   12,   21,   37,   38,
   48,   39,   42,   27,   17,   54,   30,
};
short yysindex[] = {                                      0,
  -10,    5,  -16,    0,  -15,    0,    0,  -17,   10,   11,
   12,   14,  -16,    0,    0,  -57,   -6,   -8,    1,    3,
  -34, -232,    0,    0,    0,    0, -229,    0,    0,  -16,
    0,    6,    0,    0, -232,    0,  -11,  -24,    0,    0,
    0,  -16,    0,  -34,    7,    0,    0,    0,    0,    0,
    0,    0,  -24,    0, -223,    2,    9, -213,   13,    0,
};
short yyrindex[] = {                                      0,
  -17,    0,   -5,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   -9,    0,    0,    0,   -3,    0,    0,    0,
    0,    4,    0,    0,    0,    0,    0,    0,    0,   -7,
    0,    0,    0,    0,    4,    0,    0,   16,    0,    0,
    0,   -1,    0,   35,    0,    0,    0,    0,    0,    0,
    0,    0,   36,    0,    0,    0,    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
    0,  -12,    0,    0,    0,    0,    0,    0,    0,   15,
    0,   -2,   47,    8,    0,    0,    0,
};
#define YYTABLESIZE 256
short yytable[] = {                                       6,
   27,   19,   23,   29,   15,   22,   24,   50,   20,   41,
   16,   19,   34,   19,   14,   15,   18,   35,   19,   23,
   24,   25,   46,   26,   40,   31,   20,   43,   23,   47,
   15,   32,   24,   56,   20,   52,   51,   44,   19,   19,
   33,   45,   57,   59,   21,   22,   55,   13,    0,    0,
   52,    0,   58,   60,    0,   23,   11,   24,   53,    0,
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
   28,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   36,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   49,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    2,    3,   23,    4,   23,
    5,   15,   23,   24,   24,   23,
};
short yycheck[] = {                                      10,
   10,   36,   10,   61,   10,    8,   10,   32,   10,   22,
    3,   36,   10,   10,   10,   32,   32,   20,   36,   10,
   10,   10,   35,   10,  257,   32,   36,  257,   36,   41,
   36,   40,   36,  257,   36,   38,   61,   30,   36,   36,
   40,   36,   41,  257,   10,   10,   40,    1,   -1,   -1,
   53,   -1,   44,   41,   -1,   61,   41,   61,   44,   -1,
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
  258,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  257,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  257,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  256,  257,  257,  259,  257,
  261,  257,  258,  257,  258,  257,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 261
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,"'\\n'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,"' '",0,0,0,"'$'",0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'='",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"WORD","ASSIGN","PRINT","SHELL","IFEQ",
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
"line : ifline '\\n'",
"line : error '\\n'",
"open : '$' '('",
"name : wordvars",
"close : ')'",
"ifline : IFEQ ' ' '(' '$' '(' WORD ')' ',' WORD ')'",
"variable : open name close",
"words : WORD",
"words : words optspace WORD",
"whitespace : ' '",
"whitespace : whitespace ' '",
"optwords :",
"optwords : words",
"optvars :",
"optvars : wordvars",
"optspace :",
"optspace : whitespace",
"vars_line : variable optwords",
"vars_line : vars_line variable optwords",
"wordsline : words",
"assignm : ASSIGN",
"assignm : '='",
"assgnline : WORD optspace assignm optspace optvars",
"printline : PRINT",
"wordvars : WORD",
"wordvars : variable",
"wordvars : wordvars variable",
"wordvars : wordvars WORD",
"wordvars : wordvars '='",
"wordvars : wordvars ' '",
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
case 1:
{	yyval = "";				}
break;
case 2:
{	yyval = "";				}
break;
case 3:
{	yyval = "";		printf("empty line\n");		}
break;
case 4:
{	yyval = yyvsp[-2]+yyvsp[-1]; 	TheOutput.push_back(yyval);	}
break;
case 5:
{	yyval = yyvsp[-1]; 		TheOutput.push_back(yyvsp[-1]);	}
break;
case 6:
{	yyval = "";		printf("assign line\n");	}
break;
case 7:
{	yyval = "";									}
break;
case 8:
{	yyval = "";		printf("ifline\n");			}
break;
case 9:
{					printf("error line\n");
							YYSTYPE msg;
							msg.append("Line ").append(itoa(lineno)).append(": Unexpected token '").append(yylval).append("'.");
							TheError.push_back(msg);
							yyerrok;
						}
break;
case 10:
{	yyval = "";			}
break;
case 11:
{	yyval = yyvsp[0];			}
break;
case 12:
{	yyval = "";			}
break;
case 13:
{	yyval = ""; printf("gotcha\n"); }
break;
case 14:
{
						YYSTYPE token = yyvsp[-1];
						TrimString(token);
						
						if(!token.substr(0, 5).compare("shell"))
						{
							token.erase(0, 5);
							TrimString(token);
							printf("SHELL! '%s'\n", token.c_str());
							YYSTYPE result = getShellResult(token);
							TrimString(result);
							printf("result: '%s'\n", result.c_str());
							yyval = result;
						}
						else
						{
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
					}
break;
case 15:
{	yyval = yyvsp[0];				}
break;
case 16:
{	yyval = yyvsp[-2] + yyvsp[-1];			}
break;
case 17:
{	yyval = " ";				}
break;
case 18:
{	yyval = yyvsp[-1] + " ";			}
break;
case 19:
{	yyval = "";				}
break;
case 20:
{	yyval = yyvsp[0];				}
break;
case 21:
{	yyval = "";				}
break;
case 22:
{	yyval = yyvsp[0];				}
break;
case 23:
{	yyval = "";				}
break;
case 24:
{	yyval = yyvsp[0];				}
break;
case 25:
{	yyval = yyvsp[-1] + yyvsp[0];		}
break;
case 26:
{	yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];	}
break;
case 27:
{	yyval = yyvsp[0];				}
break;
case 28:
{	yyval = ""; append = true;			}
break;
case 29:
{	yyval = ""; append = false;			}
break;
case 30:
{
	 					YYSTYPE name = yyvsp[-4];
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
case 31:
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
case 32:
{	yyval = yyvsp[0];				}
break;
case 33:
{	yyval = yyvsp[0];				}
break;
case 34:
{	yyval = yyvsp[-1] + yyvsp[0];			}
break;
case 35:
{	yyval = yyvsp[-1] + yyvsp[0];			}
break;
case 36:
{	yyval = yyvsp[-1] + "=";			}
break;
case 37:
{	yyval = yyvsp[-1] + " ";			}
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
