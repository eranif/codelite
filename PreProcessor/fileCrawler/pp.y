%{
// Copyright Eran Ifrah(c)
%}

%{
/*************** Includes and Defines *****************************/
#include <wx/string.h>
#include "pptable.h"

#define YYSTYPE wxString
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

#ifdef yylex
#undef yylex
#define yylex pp_lex
#endif

extern void pp_error(char *st);
extern int pp_lex();
extern wxString g_definition;
extern int in_if_1;
/*************** Standard ytab.c continues here *********************/
%}


/* This group is used by the C/C++ language parser */
%token  PP_DEFINE PP_IF         PP_IFDEF               PP_DEFINED 
%token  PP_UNDEF  PP_ELSE       PP_ELIF                PP_ENDIF
%token  PP_POUND  PP_IDENTIFIER PP_COMPLEX_REPLACEMENT PP_IFNDEF
%token  PP_ZERO   PP_CPLUSPLUS  PP_INCLUDE

%start   translation_unit

%%

translation_unit :      /*empty*/
                | translation_unit macros
                ;

/**
 * We currently support 3 types of macros:
 * 1) Simple define macros (e.g. #define VALUE 1)
 * 2) Function like macros (e.g. #define FOO(x) printf("%s\n", x);)
 * 3) Conditional macros:
 *   3.1) #if (condition)
 *   3.2) #ifdef (NAME)
 *   3.3) #if defined(COND) && defiend(COND2)
 */
macros:   define_simple_macros
        | define_func_like_macros
        | if_cplusplus
        | error {
            //wxPrintf(wxT("CodeLite: syntax error, unexpected token '%s' found\n"), pp_lval.c_str());
        }
        ;

if_cplusplus : PP_IF PP_CPLUSPLUS
        {
            if(in_if_1 == 0)
                in_if_1 = 1;
        }
        ;
        
define_simple_macros: PP_DEFINE PP_IDENTIFIER PP_COMPLEX_REPLACEMENT 
        {
            PPToken token;
            token.name = $2;
            token.flags = (PPToken::IsValid | PPToken::IsOverridable);
            
            if(in_if_1) {
                // the token was found in a '#if 1' block - dont allow overriding it
                token.flags &= ~(PPToken::IsOverridable);
            }
            
            token.replacement = g_definition;
            PPTable::Instance()->Add( token );
        }
        ;
        
define_func_like_macros: PP_DEFINE PP_IDENTIFIER '(' args_list ')' PP_COMPLEX_REPLACEMENT
        {
            PPToken token;
            token.name = $2;
            token.replacement = g_definition;
            token.flags = (PPToken::IsFunctionLike | PPToken::IsValid | PPToken::IsOverridable);
            if(in_if_1) {
                // the token was found in a '#if 1' block - dont allow overriding it
                token.flags &= ~(PPToken::IsOverridable);
            }
            token.processArgs( $4 );
            PPTable::Instance()->Add( token ); 
        }
        ;

args_list: /* empty */
        | PP_IDENTIFIER                 { $$ = $1;           }
        | args_list ',' PP_IDENTIFIER   { $$ = $1 + $2 + $3; }
        ;

%%

