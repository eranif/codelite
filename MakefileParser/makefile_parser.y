%{ 
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
	
	// std::cout << "Before: '" << param << "'\n";
	param.erase(param.find_last_not_of(whitespace)+1);
	// std::cout << "AfterRight: '" << param << "'\n";
	param.erase(0, param.find_first_not_of(whitespace));
	// std::cout << "AfterLeft: '" << param << "'\n";
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
//	printf("parser error: %s\n", string);
}

/*************** Standard variable.y: continues here *********************/
%}

/* Keywords */
%token WORD
%token ASSIGN
%token PRINT
%token SHELL
%token IFEQ
%token ENDIF

/* Start of grammar */
%%
input:	/* empty */				{	
									$$ = "";
									printf("empty input\n");
								}
	| input line				{
									$$ = "";
									printf("input line\n");
								}
;

line:	'\n'					{	
									$$ = "";
									printf("empty line\n");
								}
	| optwords vars_line '\n'	{	
									printf("varsline\n");
									if(enableExecution.size())
									{
										YYSTYPE msg;
										msg.append("Line ").append(itoa(lineno)).append(": Unexpected token inside if '").append(yylval).append("'.");
										TheError.push_back(msg);
										$$ = "";
									}
									else
									{
										$$ = $1+$2;
										TheOutput.push_back($$);
									}
								}
	| wordsline '\n'			{
									printf("wordsline\n");
									if(enableExecution.size())
									{
										YYSTYPE msg;
										msg.append("Line ").append(itoa(lineno)).append(": Unexpected token inside if '").append(yylval).append("'.");
										TheError.push_back(msg);
										$$ = "";
									}
									else
									{
										$$ = $1;
										TheOutput.push_back($1);
									}
								}
	| assgnline '\n'			{	
									$$ = "";
									printf("assign line\n");
								}
	| printline '\n'			{	
									$$ = "";
									printf("printline\n");
								}
	| ifline '\n'				{	
									$$ = "";
									printf("ifline\n");
								}
	| ENDIF '\n'				{	
									$$ = "";
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
	| error	'\n'				{
									YYSTYPE msg;
									printf("error line\n");
									msg.append("Line ").append(itoa(lineno)).append(": Unexpected token '").append(yylval).append("'.");
									TheError.push_back(msg);
									yyerrok;
								}
;

open:	'$' '('					{	
									$$ = "";
									printf("open\n");
								}

name:	wordvars				{	
									$$ = $1;
									printf("name\n");
								}

close:	')'						{
									$$ = "";
									printf("close\n");
								}

ifline:	WORD '(' '$' '(' WORD ')' ',' WORD ')'	
								{
									$$ = ""; 
									printf("ifline\n");

									YYSTYPE command = $1;
									if(!command.substr(0, 4).compare("ifeq"))
									{							
										YYSTYPE varname = $5;
										YYSTYPE target = $8;
										if(TheTokens[varname].compare(target))
											enableExecution.push(false);
										else
											enableExecution.push(true);
									}
								}

variable: open name close 		{
									printf("variable\n");
									YYSTYPE token = $2;
									TrimString(token);
									
									if(!token.substr(0, 5).compare("shell"))
									{
										token.erase(0, 5);
										TrimString(token);
										printf("SHELL! '%s'\n", token.c_str());
										YYSTYPE result = getShellResult(token);
										TrimString(result);
										printf("result: '%s'\n", result.c_str());
										$$ = result;
									}
									else
									{
										if(TheTokens[token].size() > 0)
										{
											$$ = TheTokens[token];
										}
										else
										{
											TheUnmatched.push_back(token);
											$$ = "";
										}
									}
								}

words: WORD						{	
									$$ = $1;
									printf("words\n");
								}
     | words WORD 				{	
									$$ = $1 + $2;
									printf("words\n");
								}
;

optwords:						{	
									$$ = "";
									printf("optwords\n");
								}
	| words						{	
									$$ = $1;
									printf("optwords\n");
								}
;	

optvars:						{	
									$$ = "";
									printf("optvars\n");
								}	
	| wordvars					{	
									$$ = $1;
									printf("optvars\n");
								}
;


vars_line: variable optwords	{
									$$ = $1 + $2;
									printf("vars_line\n");
								}
         | vars_line variable optwords	{	
									$$ = $1 + $2 + $3;
									printf("vars_line\n");
								}
;

wordsline: words				{	
									$$ = $1;
									printf("wordline\n");
								}

assignm:	ASSIGN				{	
									$$ = ""; 
									append = true;
									printf("assignm\n");
								}
       |	'='					{	
									$$ = ""; 
									append = false;
									printf("assignm\n");
								}
;

assgnline: WORD assignm optvars	{
									printf("assgnline\n");
									if(enableExecution.size() == 0 || enableExecution.top() == false)
									{
										$$ = "";
									}
									else
									{
										YYSTYPE name = $1;
										YYSTYPE value = $3;
										TrimString(name);
										TrimString(value);

										if(append)
											TheTokens[name] += value;
										else
											TheTokens[name] = value;

										$$ = name + "=" + value;										
									}
								}

printline:	PRINT				{
									YYSTYPE result ="Tokens: \n";
									for(Itokens it = TheTokens.begin(); it != TheTokens.end(); it++)
									{
										result += "'" + it->first + "'='" + it->second + "'\n";
									}
									result += "Done.";
									$$ = result;
								}

wordvars: WORD 					{	
									$$ = $1;
									printf("wordvars\n");
								}
	| variable 					{	
									$$ = $1;
									printf("wordvars\n");
								}
	| wordvars variable			{	
									$$ = $1 + $2;
									printf("wordvars\n");
								}
	| wordvars WORD 			{	
									$$ = $1 + $2;
									printf("wordvars\n");
								}
	| wordvars '='				{	
									$$ = $1 + "=";
									printf("wordvars\n");
								}
;

%%
/* End of grammar */
