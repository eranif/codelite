%{ 
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

/* Start of grammar */
%%
input:	/* empty */			{	$$ = "";				}
	| input line			{	$$ = "";				}
;

line:	'\n'					{	$$ = "";		printf("empty line\n");		}
	| optwords vars_line '\n'	{	$$ = $1+$2; 	TheOutput.push_back($$);	}
	| wordsline '\n'			{	$$ = $1; 		TheOutput.push_back($1);	}
	| assgnline '\n'			{	$$ = "";		printf("assign line\n");	}
	| printline '\n'			{	$$ = "";									}
	| ifline '\n'				{	$$ = "";		printf("ifline\n");			}
	| error	'\n'				{					printf("error line\n");
							YYSTYPE msg;
							msg.append("Line ").append(itoa(lineno)).append(": Unexpected token '").append(yylval).append("'.");
							TheError.push_back(msg);
							yyerrok;
						}
;

open:	'$' '('				{	$$ = "";			}

name:	wordvars			{	$$ = $1;			}

close:	')'					{	$$ = "";			}

ifline:	IFEQ ' ' '(' '$' '(' WORD ')' ',' WORD ')'	{	$$ = ""; printf("gotcha\n"); }

variable: open name close 		{
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

words: WORD				{	$$ = $1;				}
     | words WORD 		{	$$ = $1 + $2;			}
;

whitespace: ' '			{	$$ = " ";				}
	|	whitespace ' '	{	$$ = $1 + " ";			}
;

optwords:				{	$$ = "";				}
	| words				{	$$ = $1;				}
;	

optvars:				{	$$ = "";				}	
	| wordvars			{	$$ = $1;				}
;

optspace:				{	$$ = "";				}
	|	whitespace		{	$$ = $1;				}
;


vars_line: variable optwords			{	$$ = $1 + $2;		}
         | vars_line variable optwords	{	$$ = $1 + $2 + $3;	}
;

wordsline: words			{	$$ = $1;				}

assignm:	ASSIGN			{	$$ = ""; append = true;			}
       |	'='				{	$$ = ""; append = false;			}
;

assgnline: WORD optspace assignm optspace optvars	{
	 					YYSTYPE name = $1;
						YYSTYPE value = $5;
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
	 					$$ = name + "=" + value;			
					}

printline:	PRINT			{
	 					YYSTYPE result ="Tokens: \n";
						for(Itokens it = TheTokens.begin(); it != TheTokens.end(); it++)
						{
							result += "'" + it->first + "'='" + it->second + "'\n";
						}
						result += "Done.";
						$$ = result;
					}

wordvars: WORD 				{	$$ = $1;				}
	| variable 				{	$$ = $1;				}
	| wordvars variable		{	$$ = $1 + $2;			}
	| wordvars WORD 		{	$$ = $1 + $2;			}
	| wordvars '='			{	$$ = $1 + "=";			}
	| wordvars ' '			{	$$ = $1 + " ";			}
;
%%
/* End of grammar */










