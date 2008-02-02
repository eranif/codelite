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
#include <stack>

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
std::stack<bool> enableExecution;

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
#define ENDIF 262
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    1,    1,    1,    1,    1,    1,    1,    1,
    8,    9,   11,    7,   12,   13,   13,    2,    2,   14,
   14,    3,    3,    4,   15,   15,    5,    6,   10,   10,
   10,   10,   10,
};
short yylen[] = {                                         2,
    0,    2,    1,    3,    2,    2,    2,    2,    2,    2,
    2,    1,    1,    9,    3,    1,    2,    0,    1,    0,
    1,    2,    3,    1,    1,    1,    3,    1,    1,    1,
    2,    2,    2,
};
short yydefred[] = {                                      1,
    0,    0,    0,   28,    0,    3,    2,    0,    0,    0,
    0,    0,    0,   10,   25,    0,   26,    0,    9,    0,
    0,    0,    0,    5,    6,    7,    8,   17,    0,   29,
    0,   30,   27,   11,    4,    0,    0,    0,   16,   22,
    0,    0,   32,   33,   31,   23,   13,   15,    0,    0,
    0,    0,   14,
};
short yydgoto[] = {                                       1,
    7,    8,   21,    9,   10,   11,   12,   22,   37,   31,
   48,   32,   41,   33,   18,
};
short yysindex[] = {                                      0,
  -10,    2,  -36,    0,    3,    0,    0,  -26,    4,    5,
    6,    7, -239,    0,    0,  -16,    0,  -34,    0,  -18,
   -5,  -34, -234,    0,    0,    0,    0,    0,  -14,    0,
  -33,    0,    0,    0,    0, -234,  -17,  -33,    0,    0,
 -239, -227,    0,    0,    0,    0,    0,    0,   -7,  -12,
 -219,    8,    0,
};
short yyrindex[] = {                                      0,
    9,    0,   -9,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   -3,    0,    0,    0,    0,   29,    0,    0,
    0,    0,   -1,    0,    0,    0,    0,    0,    0,    0,
   30,    0,    0,    0,    0,   -1,    0,   10,    0,    0,
    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,
};
short yygindex[] = {                                      0,
    0,  -15,    0,    0,    0,    0,    0,    0,    0,   19,
    0,   -2,   41,    0,    0,
};
#define YYTABLESIZE 252
short yytable[] = {                                       6,
   16,   20,   20,   16,   35,   23,   24,   40,   18,   20,
   19,   14,   19,   24,   25,   26,   27,   28,   36,   29,
   46,   34,   39,   47,   17,   42,   16,   44,   45,   49,
   20,   51,   19,   50,   18,   45,   19,   52,   20,   21,
   38,   13,    0,    0,   18,    0,    0,    0,   53,    0,
   12,    0,    0,    0,    0,    0,    0,    0,    0,    0,
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
    0,   15,   30,   43,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    2,    3,   16,    4,    0,
    0,    5,
};
short yycheck[] = {                                      10,
   10,   36,   36,   40,   10,    8,   10,   23,   10,   36,
   10,   10,   10,   10,   10,   10,   10,  257,   21,   36,
   36,   40,  257,   41,   61,   40,   36,   61,   31,  257,
   36,   44,   36,   41,   36,   38,   36,  257,   10,   10,
   22,    1,   -1,   -1,   36,   -1,   -1,   -1,   41,   -1,
   41,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
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
   -1,  258,  257,  257,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  256,  257,  257,  259,   -1,
   -1,  262,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 262
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,"'\\n'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,"'$'",0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'='",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"WORD","ASSIGN","PRINT","SHELL","IFEQ","ENDIF",
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
"line : ENDIF '\\n'",
"line : error '\\n'",
"open : '$' '('",
"name : wordvars",
"close : ')'",
"ifline : WORD '(' '$' '(' WORD ')' ',' WORD ')'",
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
"assgnline : WORD assignm optvars",
"printline : PRINT",
"wordvars : WORD",
"wordvars : variable",
"wordvars : wordvars variable",
"wordvars : wordvars WORD",
"wordvars : wordvars '='",
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
{	
									yyval = "";
									printf("empty input\n");
								}
break;
case 2:
{
									yyval = "";
									printf("input line\n");
								}
break;
case 3:
{	
									yyval = "";
									printf("empty line\n");
								}
break;
case 4:
{	
									printf("varsline\n");
									if(enableExecution.size())
									{
										YYSTYPE msg;
										msg.append("Line ").append(itoa(lineno)).append(": Unexpected token inside if '").append(yylval).append("'.");
										TheError.push_back(msg);
										yyval = "";
									}
									else
									{
										yyval = yyvsp[-2]+yyvsp[-1];
										TheOutput.push_back(yyval);
									}
								}
break;
case 5:
{
									printf("wordsline\n");
									if(enableExecution.size())
									{
										YYSTYPE msg;
										msg.append("Line ").append(itoa(lineno)).append(": Unexpected token inside if '").append(yylval).append("'.");
										TheError.push_back(msg);
										yyval = "";
									}
									else
									{
										yyval = yyvsp[-1];
										TheOutput.push_back(yyvsp[-1]);
									}
								}
break;
case 6:
{	
									yyval = "";
									printf("assign line\n");
								}
break;
case 7:
{	
									yyval = "";
									printf("printline\n");
								}
break;
case 8:
{	
									yyval = "";
									printf("ifline\n");
								}
break;
case 9:
{	
									yyval = "";
									printf("endif\n");
									
									if(enableExecution.size() > 0)
									{
										enableExecution.pop();
									}
									else
									{
										YYSTYPE msg;
										msg.append("Line ").append(itoa(lineno)).append(": Unexpected endif token.");
										TheError.push_back(msg);
									}
								}
break;
case 10:
{
									YYSTYPE msg;
									printf("error line\n");
									msg.append("Line ").append(itoa(lineno)).append(": Unexpected token '").append(yylval).append("'.");
									TheError.push_back(msg);
									yyerrok;
								}
break;
case 11:
{	
									yyval = "";
									printf("open\n");
								}
break;
case 12:
{	
									yyval = yyvsp[0];
									printf("name\n");
								}
break;
case 13:
{
									yyval = "";
									printf("close\n");
								}
break;
case 14:
{
									yyval = ""; 
									printf("ifline\n");

									YYSTYPE command = yyvsp[-8];
									if(!command.substr(0, 4).compare("ifeq"))
									{							
										YYSTYPE varname = yyvsp[-4];
										YYSTYPE target = yyvsp[-1];
										if(TheTokens[varname].compare(target))
											enableExecution.push(false);
										else
											enableExecution.push(true);
									}
								}
break;
case 15:
{
									printf("variable\n");
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
case 16:
{	
									yyval = yyvsp[0];
									printf("words\n");
								}
break;
case 17:
{	
									yyval = yyvsp[-1] + yyvsp[0];
									printf("words\n");
								}
break;
case 18:
{	
									yyval = "";
									printf("optwords\n");
								}
break;
case 19:
{	
									yyval = yyvsp[0];
									printf("optwords\n");
								}
break;
case 20:
{	
									yyval = "";
									printf("optvars\n");
								}
break;
case 21:
{	
									yyval = yyvsp[0];
									printf("optvars\n");
								}
break;
case 22:
{
									yyval = yyvsp[-1] + yyvsp[0];
									printf("vars_line\n");
								}
break;
case 23:
{	
									yyval = yyvsp[-2] + yyvsp[-1] + yyvsp[0];
									printf("vars_line\n");
								}
break;
case 24:
{	
									yyval = yyvsp[0];
									printf("wordline\n");
								}
break;
case 25:
{	
									yyval = ""; 
									append = true;
									printf("assignm\n");
								}
break;
case 26:
{	
									yyval = ""; 
									append = false;
									printf("assignm\n");
								}
break;
case 27:
{
									printf("assgnline\n");
									if(enableExecution.size() == 0 || enableExecution.top() == false)
									{
										yyval = "";
									}
									else
									{
										YYSTYPE name = yyvsp[-2];
										YYSTYPE value = yyvsp[0];
										TrimString(name);
										TrimString(value);

										if(append)
											TheTokens[name] += value;
										else
											TheTokens[name] = value;

										yyval = name + "=" + value;										
									}
								}
break;
case 28:
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
case 29:
{	
									yyval = yyvsp[0];
									printf("wordvars\n");
								}
break;
case 30:
{	
									yyval = yyvsp[0];
									printf("wordvars\n");
								}
break;
case 31:
{	
									yyval = yyvsp[-1] + yyvsp[0];
									printf("wordvars\n");
								}
break;
case 32:
{	
									yyval = yyvsp[-1] + yyvsp[0];
									printf("wordvars\n");
								}
break;
case 33:
{	
									yyval = yyvsp[-1] + "=";
									printf("wordvars\n");
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
