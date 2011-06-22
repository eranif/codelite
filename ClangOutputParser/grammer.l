%{
/* Included code before lex code */
#include <string>
#include "clang_result_parser.h"

#define YYSTYPE std::string

#ifdef yylex
#undef yylex
#define yylex clang_yylex
#endif

/**
 * clang_lex_string contains the current token string representation
 */
static std::string clang_lex_string;
extern std::string clang_result_lval;

/**
 * reset the lexer
 */
void clang_lex_clean();

void clang_yyless(int count);

/**
 * accessors
 */
const std::string& clang_lex_get_string()
{
	return clang_lex_string;
}

/**
 * return value macro
 */
#define RETURN_VALUE(x) {\
					    clang_lex_string  = yytext;\
						clang_result_lval = yytext;\
						return(x);\
					  }
%}

%option yylineno

/**
 * Some basic regexes, delete them if you dont really need them
 */

identifier [a-zA-Z_][0-9a-zA-Z_\-]*
exponent_part [eE][-+]?[0-9]+
fractional_constant ([0-9]*"."[0-9]+)|([0-9]+".")
floating_constant (({fractional_constant}{exponent_part}?)|([0-9]+{exponent_part}))[FfLl]?

integer_suffix_opt ([uU]?[lL]?)|([lL][uU])
decimal_constant [1-9][0-9]*{integer_suffix_opt}
octal_constant "0"[0-7]*{integer_suffix_opt}
hex_constant "0"[xX][0-9a-fA-F]+{integer_suffix_opt}

octal_escape  [0-7]{3}

h_tab [\011]
form_feed [\014]
v_tab [\013]
c_return [\015]
horizontal_white [ ]|{h_tab}

n_escape      [\]{2}n
v_escape      [\]{2}v
r_escape      [\]{2}r
t_escape      [\]{2}t
quotes_escape [\]{2}"

%%

{horizontal_white}+                                        {}
({v_tab}|{c_return}|{form_feed})+                          {}
({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n"   {}
"COMPLETION"                                               {RETURN_VALUE(CLANG_COMPLETION)};
operator                                                   {RETURN_VALUE(CLANG_OPERATOR)};
"(hidden)"                                                 {RETURN_VALUE(CLANG_HIDDEN);}
"[#"                                                       {RETURN_VALUE(CLANG_DELIM_OPEN);}
"#]"                                                       {RETURN_VALUE(CLANG_DELIM_CLOSE);}
"<#"                                                       {RETURN_VALUE(CLANG_ARG_DELIM_OPEN);}
"#>"                                                       {RETURN_VALUE(CLANG_ARG_DELIM_CLOSE);}
"{#"                                                       {RETURN_VALUE(CLANG_ARG_OPT_DELIM_OPEN);}
"#}"                                                       {RETURN_VALUE(CLANG_ARG_OPT_DELIM_CLOSE);}
"*"                                                        {RETURN_VALUE(CLANG_STAR);}
"&"                                                        {RETURN_VALUE(CLANG_AMP);}
"~"                                                        {RETURN_VALUE(CLANG_TILDE);}
"enum"                                                     {RETURN_VALUE(CLANG_ENUM);}
"<anonymous>"                                              {RETURN_VALUE(CLANG_ANONYMOUS);}
"::"                                                       {RETURN_VALUE(CLANG_CLCL);}
"="                                                        {RETURN_VALUE(CLANG_OP_ASSIGN);}
".*"                                                       {RETURN_VALUE(CLANG_OP_DOTstar);}
"->"                                                       {RETURN_VALUE(CLANG_OP_ARROW);}
"->*"                                                      {RETURN_VALUE(CLANG_OP_ARROWstar);}
"++"                                                       {RETURN_VALUE(CLANG_OP_ICR);}
"--"                                                       {RETURN_VALUE(CLANG_OP_DECR);}
"<<"                                                       {RETURN_VALUE(CLANG_OP_LS);}
">>"                                                       {RETURN_VALUE(CLANG_OP_RS);}
"<="                                                       {RETURN_VALUE(CLANG_OP_LE);}
">="                                                       {RETURN_VALUE(CLANG_OP_GE);}
"=="                                                       {RETURN_VALUE(CLANG_OP_EQ);}
"!="                                                       {RETURN_VALUE(CLANG_OP_NE);}
"..."                                                      {RETURN_VALUE(CLANG_ELLIPSIS);}
{identifier}                                               {RETURN_VALUE(CLANG_NAME);}
.                                                          {RETURN_VALUE((int)*yytext);}

%%

void clang_lex_clean()
{
	yy_flush_buffer(YY_CURRENT_BUFFER);
	yy_delete_buffer(YY_CURRENT_BUFFER);
}

void clang_set_lexer_input(const std::string &in) 
{
	BEGIN INITIAL;
	yy_scan_string(in.c_str());
}

int clang_result_wrap()
{
	return 1;
}

void clang_yyless(int count)
{
	yyless(count);
}