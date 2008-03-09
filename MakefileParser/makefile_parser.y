%{ 
/**** Includes and Defines *****************************/
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <stack>

#if 0
	#define DEBUGPRINTF printf
#else
	#define DEBUGPRINTF
#endif

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
//	DEBUGPRINTF("parser error: %s\n", string);
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
									DEBUGPRINTF("empty input\n");
								}
	| input line				{
									$$ = "";
									DEBUGPRINTF("input line\n");
								}
;

line:	'\n'					{	
									$$ = "";
									DEBUGPRINTF("empty line\n");
								}
	| optwords vars_line '\n'	{	
									DEBUGPRINTF("varsline\n");
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
									DEBUGPRINTF("wordsline\n");
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
									DEBUGPRINTF("assign line\n");
								}
	| printline '\n'			{	
									$$ = "";
									DEBUGPRINTF("printline\n");
								}
	| ifline '\n'				{	
									$$ = "";
									DEBUGPRINTF("ifline\n");
								}
	| ENDIF '\n'				{	
									$$ = "";
									DEBUGPRINTF("endif\n");
									
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
									DEBUGPRINTF("error line\n");
									msg.append("Line ").append(itoa(lineno)).append(": Unexpected token '").append(yylval).append("'.");
									TheError.push_back(msg);
									yyerrok;
								}
;

open:	'$' '('					{	
									$$ = "";
									DEBUGPRINTF("open\n");
								}

name:	wordvars				{	
									$$ = $1;
									DEBUGPRINTF("name\n");
								}

close:	')'						{
									$$ = "";
									DEBUGPRINTF("close\n");
								}

ifline:	WORD '(' '$' '(' WORD ')' ',' WORD ')'	
								{
									$$ = ""; 
									DEBUGPRINTF("ifline\n");

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
									DEBUGPRINTF("variable\n");
									YYSTYPE token = $2;
									TrimString(token);
									
									if(!token.substr(0, 5).compare("shell"))
									{
										token.erase(0, 5);
										TrimString(token);
										DEBUGPRINTF("SHELL! '%s'\n", token.c_str());
										YYSTYPE result = getShellResult(token);
										TrimString(result);
										DEBUGPRINTF("result: '%s'\n", result.c_str());
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
									DEBUGPRINTF("words\n");
								}
     | words WORD 				{	
									$$ = $1 + $2;
									DEBUGPRINTF("words\n");
								}
;

optwords:						{	
									$$ = "";
									DEBUGPRINTF("optwords\n");
								}
	| words						{	
									$$ = $1;
									DEBUGPRINTF("optwords\n");
								}
;	

optvars:						{	
									$$ = "";
									DEBUGPRINTF("optvars\n");
								}	
	| wordvars					{	
									$$ = $1;
									DEBUGPRINTF("optvars\n");
								}
;


vars_line: variable optwords	{
									$$ = $1 + $2;
									DEBUGPRINTF("vars_line\n");
								}
         | vars_line variable optwords	{	
									$$ = $1 + $2 + $3;
									DEBUGPRINTF("vars_line\n");
								}
;

wordsline: words				{	
									$$ = $1;
									DEBUGPRINTF("wordline\n");
								}

assignm:	ASSIGN				{	
									$$ = ""; 
									append = true;
									DEBUGPRINTF("assignm\n");
								}
       |	'='					{	
									$$ = ""; 
									append = false;
									DEBUGPRINTF("assignm\n");
								}
;

assgnline: WORD assignm optvars	{
									DEBUGPRINTF("assgnline\n");
									if(enableExecution.size() != 0 && enableExecution.top() == false)
									{
										$$ = "";
									}
									else
									{
										YYSTYPE name = $1;
										YYSTYPE value = $3;
										TrimString(name);
																		
										if(name[name.size()-1] == ':')
										{
											append = true;
											name.erase(name.size()-1);
										}
											
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
									DEBUGPRINTF("wordvars\n");
								}
	| variable 					{	
									$$ = $1;
									DEBUGPRINTF("wordvars\n");
								}
	| wordvars variable			{	
									$$ = $1 + $2;
									DEBUGPRINTF("wordvars\n");
								}
	| wordvars WORD 			{	
									$$ = $1 + $2;
									DEBUGPRINTF("wordvars\n");
								}
	| wordvars '='				{	
									$$ = $1 + "=";
									DEBUGPRINTF("wordvars\n");
								}
;

%%
/* End of grammar */
