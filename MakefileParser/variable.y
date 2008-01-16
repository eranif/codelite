%{
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
	| input line			{	/* Do Nothing */				}
;

line:	'\n'				{	/* Do Nothing */				}
	| optwords vars_line '\n'	{	TheOutput.push_back($1+$2);			}
	| wordsline '\n'		{	TheOutput.push_back($1);			}
	| assgnline '\n'		{	/* Do Nothing */				}
	| printline '\n'		{	/* Do Nothing */				}
	| error	'\n'			{
						YYSTYPE msg;
						msg << wxT("Line ") << lineno << wxT(": Unexpected token '") << yylval << wxT("'.");
						TheError.push_back(msg);
						yyerrok;
					}
;

open:	'$' '('				{	/* do nothing */			}

name:	wordvars			{	$$ = $1;				}

close:	')'				{	/* do nothing */			}

variable: open name close 		{
						wxString token = $2;
						TrimString(token);

						if(TheTokens[token].size() > 0)
						{
							$$ = TheTokens[token];
						}
						else
						{
							TheUnmatched.push_back(token);
							$$ = wxEmptyString;
						}
					}

words: WORD				{	$$ = $1;				}
     | words WORD 			{	$$ = $1 + $2;				}
;

optwords:				{	$$ = wxEmptyString;			}
	| words				{	$$ = $1;				}
;	

optvars:				{	$$ = wxEmptyString;			}	
	| wordvars			{	$$ = $1;				}
;


vars_line: variable optwords		{	$$ = $1 + $2;				}
         | vars_line variable optwords	{	$$ = $1 + $2 + $3;			}
;

wordsline: words			{	$$ = $1;				}

assignm:	ASSIGN			{	append = true;				}
       |	'='			{	append = false;				}
;

assgnline: words assignm optvars	{
	 					wxString name = $1;
						wxString value = $3;
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
	 					$$ = name + wxT("=") + value;			
					}

printline:	PRINT			{
	 					YYSTYPE result = wxT("Tokens: \n");
						for(Itokens it = TheTokens.begin(); it != TheTokens.end(); it++)
						{
							result += wxT("'") + it->first + wxT("'='") + it->second + wxT("'\n");
						}
						result += wxT("Done.");
						$$ = result;
					}

wordvars: WORD 				{	$$ = $1;				}
	| variable 			{	$$ = $1;				}
	| wordvars variable		{	$$ = $1 + $2;				}
	| wordvars WORD 		{	$$ = $1 + $2;				}
;
%%
/* End of grammar */










