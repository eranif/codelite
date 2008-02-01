%{ 
// #include <wx/string.h>
#include <string>

// #define yylval clplugin_scope_lval
#define YYSTYPE std::string
extern YYSTYPE yylval;

#include "makefile_lexer.h"
#define YY_NO_UNISTD_H 1
#undef YY_NO_INPUT
int lineno = 0;
%}

word [0-9a-zA-Z_.\-+\*~:\\@&/|%\<\>,\'^ \t]+

%%

"#" {
       register int c;
       #ifdef __cplusplus
       while((c = yyinput()) != '\n' && c != EOF){
                ;
        }
       #else
       while((c = input()) != '\n' && c != EOF){
               ;
       }
       #endif
    }

":="			{return ASSIGN;	}
"="                     {return '=';    }
"$"                     {return '$';    }
"("                     {return '(';    }
")"                     {return ')';    }
"PRINT"			{return PRINT;	}
"\n"			{
				lineno++;
				return '\n';	
			}			
{word}                  {				
								yylval = yytext;
                                // yylval.Printf(wxT("%s"),yytext);
                                return WORD;
                        }
<<EOF>> 				{	
							//reset lexer
							yyterminate();
						}
					
.           {
				//printf("?> '%s'\n", yytext);
			}

%%


void initLexer(const char *data)
{
	BEGIN INITIAL;
	yy_scan_string(data);
}

int yywrap()
{
	return 1;
}
