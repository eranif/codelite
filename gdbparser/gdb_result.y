%{ 
// Copyright Eran Ifrah(c)
%} 

%{ 

#include <string>

#define YYDEBUG_LEXER_TEXT gdb_result_lval
#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

#ifdef yylex  
#undef yylex
#define yylex gdb_result_lex
#endif 
%}

%token GDB_DONE     
%token GDB_RUNNING  
%token GDB_CONNECTED
%token GDB_ERROR    
%token GDB_EXIT     
%token GDB_STACK_ARGS
%token GDB_VALUE     
%token GDB_ARGS      
%token GDB_FRAME     
%token GDB_NAME      
%token GDB_STRING
%token GDB_LEVEL
%token GDB_FRAME
%token GDB_ESCAPED_STRING
%token GDB_LOCALS
%token GDB_INTEGER
%token GDB_OCTAL
%token GDB_HEX
%token GDB_FLOAT
%token GDB_IDENTIFIER
%token GDB_NUMCHILD
%token GDB_TYPE
%token GDB_DATA
%token GDB_ADDR
%token GDB_ASCII

%%
dummy:
%%

