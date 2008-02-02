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

word [0-9a-zA-Z_.\-+\*~:\\@&/|%\<\>\'^\t ]+
smallword [0-9a-zA-Z]+

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
		lineno++;
    }

":="			{return ASSIGN;	}
"="				{return '=';    }
"$"				{return '$';    }
"("				{return '(';    }
")"				{return ')';    }
","				{return ',';	}
"ifeq"			{return IFEQ;	}
"PRINT"			{return PRINT;	}
"\n"			{
				lineno++;
				return '\n';	
			}			
{word}                  {				
								yylval = yytext;
								// printf("yytext: %s\n", yytext);
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


void initLexer(const char *fileName)
{
        FILE *file = fopen(fileName, "r");
        if(!file)
        {
                printf("failed loading file 'test.h'\n");
                exit(-1);
        }

        //set the file to be our buffer
        YY_BUFFER_STATE buffState = yy_create_buffer(file, YY_BUF_SIZE);
        yy_switch_to_buffer(buffState);
}

int yywrap()
{
	return 1;
}
