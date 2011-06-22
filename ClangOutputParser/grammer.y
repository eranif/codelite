%{
// Copyright Eran Ifrah(c)
%}

%{
#include <string>

#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

#ifdef yylex
#undef yylex
#define yylex clang_yylex
#endif

extern int clang_yylex();

/** defined in grammer.l **/
extern bool        clang_set_lexer_input(const std::string &in);
extern const std::string& clang_lex_get_string();
extern void        clang_lex_clean();

/** the parser entry point **/
void clang_parse_string(const std::string& str);

void clang_result_error(char*);
%}

%token CLANG_COMPLETION
%token CLANG_HIDDEN
%token CLANG_DELIM_OPEN
%token CLANG_DELIM_CLOSE
%token CLANG_ARG_DELIM_OPEN
%token CLANG_ARG_DELIM_CLOSE
%token CLANG_ARG_OPT_DELIM_OPEN
%token CLANG_ARG_OPT_DELIM_CLOSE
%token CLANG_STAR
%token CLANG_AMP
%token CLANG_TILDE
%token CLANG_ENUM
%token CLANG_ANONYMOUS
%token CLANG_CLCL
%token CLANG_OP_ASSIGN
%token CLANG_OP_DOTstar
%token CLANG_OP_ARROW
%token CLANG_OP_ARROWstar
%token CLANG_OP_ICR
%token CLANG_OP_DECR
%token CLANG_OP_LS
%token CLANG_OP_RS
%token CLANG_OP_LE
%token CLANG_OP_GE
%token CLANG_OP_EQ
%token CLANG_OP_NE
%token CLANG_ELLIPSIS
%token CLANG_IDENTIFIER

%%

parse: completion_entry
	 | parse completion_entry
	 ;

completion_entry: 
%%

void clang_parse_string(const std::string& str)
{
	clang_lex_clean();
	clang_set_lexer_input(str);
}

void clang_result_error(char*)
{
}
