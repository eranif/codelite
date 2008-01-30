%{
/**** Includes and Defines *****************************/
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

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
	std::string string = " a test yo ";
	std::cout << "Before: '" << param << "'\n";
	// string = string.Trim(true);
	std::cout << "AfterT: '" << string << "'\n";
	// string = string.Trim(false);
	std::cout << "AfterF: '" << string << "'\n";
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

/* Start of grammar */
%%
input:	/* empty */
	| input line			{	$$ = "";				}
;

line:	'\n'					{	$$ = "";				}
	| optwords vars_line '\n'	{	$$ = $1+$2; TheOutput.push_back($$);			}
	| wordsline '\n'			{	$$ = $1; TheOutput.push_back($1);			}
	| assgnline '\n'			{	$$ = "";				}
	| printline '\n'			{	$$ = "";				}
	| error	'\n'				{
							YYSTYPE msg;
							msg.append("Line ").append(": Unexpected token '").append(yylval).append("'.");
							TheError.push_back(msg);
							yyerrok;
						}
;

open:	'$' '('				{	$$ = "";			}

name:	wordvars			{	$$ = $1;					}

close:	')'					{	$$ = "";			}

variable: open name close 		{
						YYSTYPE token = $2;
						TrimString(token);

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

words: WORD				{	$$ = $1;				}
     | words WORD 		{	$$ = $1 + $2;			}
;

optwords:				{	$$ = "";				}
	| words				{	$$ = $1;				}
;	

optvars:				{	$$ = "";				}	
	| wordvars			{	$$ = $1;				}
;


vars_line: variable optwords			{	$$ = $1 + $2;		}
         | vars_line variable optwords	{	$$ = $1 + $2 + $3;	}
;

wordsline: words			{	$$ = $1;				}

assignm:	ASSIGN			{	$$ = ""; append = true;			}
       |	'='				{	$$ = ""; append = false;			}
;

assgnline: words assignm optvars	{
	 					YYSTYPE name = $1;
						YYSTYPE value = $3;
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
;
%%
/* End of grammar */










