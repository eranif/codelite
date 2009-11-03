%{
// Copyright Eran Ifrah(c)
%}

%{

#include <string>
#include <vector>
#include <map>
#include <cstdio>
#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

#ifdef yylex
#undef yylex
#define yylex gdb_result_lex
#endif

int  gdb_result_lex();
void gdb_result_error(const char*);
bool setGdbLexerInput(const std::string &in, bool ascii, bool wantWhitespace);
void gdb_result_lex_clean();
int  gdb_result_parse();

extern std::string gdb_result_lval;
static std::map<std::string, std::string>               sg_attributes;
static std::vector<std::map<std::string, std::string> > sg_children;

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
%token GDB_CHILDREN
%token GDB_CHILD
%token GDB_MORE

%%

parse: children_list
	 | parse children_list
	 ;

children_list:    {sg_attributes.clear(); sg_children.clear();} child_pattern
				|  error {
				// printf("CodeLite: syntax error, unexpected token '%s' found\n", gdb_result_lval.c_str());
				}
			;

child_pattern :   '^' GDB_DONE ',' GDB_NUMCHILD '=' GDB_STRING ',' GDB_CHILDREN '=' list_open children list_close
				| '^' GDB_DONE ',' GDB_NAME '=' GDB_STRING ',' {sg_attributes[$4] = $6;} child_attributes
				{
					sg_children.push_back( sg_attributes );
					sg_attributes.clear();
				}
				;

list_open :  '['
			|'{'
			;

list_close: ']'
			|'}'
			;

children     : GDB_CHILD '=' '{' child_attributes '}' {
					sg_children.push_back( sg_attributes );
					sg_attributes.clear();
				}
             | GDB_CHILD '=' '{' child_attributes '}' {sg_children.push_back( sg_attributes ); sg_attributes.clear(); } ',' children
			 ;

child_attributes :  child_key '=' GDB_STRING { sg_attributes[$1] = $3; }
				 |  child_key '=' GDB_STRING { sg_attributes[$1] = $3; } ',' child_attributes
				 ;

child_key: GDB_NAME       {$$ = $1;}
		 | GDB_NUMCHILD   {$$ = $1;}
		 | GDB_TYPE       {$$ = $1;}
		 | GDB_VALUE      {$$ = $1;}
		 | GDB_IDENTIFIER {$$ = $1;}
		 ;
%%

void gdbParseListChildren( const std::string &in, std::vector<std::map<std::string, std::string> > &children)
{
	sg_attributes.clear();
	sg_children.clear();

	setGdbLexerInput(in, true, false);
	gdb_result_parse();

	children = sg_children;
	gdb_result_lex_clean();
}
