%{
// Copyright Eran Ifrah(c)
%}

%{
#include <string>
#include <stdio.h>
#include <vector>

#include "clang_output_parser_api.h"

#define YYSTYPE std::string
#define YYDEBUG 0        /* get the pretty debugging code to compile*/

#ifdef yylex
#undef yylex
#define yylex clang_yylex
#endif

extern int  clang_yylex();
extern void clang_yyless(int count);
int         clang_result_parse();

/** defined in grammer.l **/
extern bool        clang_set_lexer_input(const std::string &in);
extern const std::string& clang_lex_get_string();
extern void        clang_lex_clean();

/** the parser entry point **/
void clang_parse_string(const std::string& str);

void clang_result_error(char*);
/** read the signature **/
void clang_read_signature();
void clang_read_type_name(std::string &store);
void clang_read_entry_name(std::string &store);

/** holds the last signature read **/
static ClangEntry       clang_entry;
static ClangEntryVector clangEntryVector;
extern std::string      clang_result_lval;
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
%token CLANG_NAME
%token CLANG_OPERATOR

%%

parse: completion_output
	 | parse completion_output
	 ;

completion_output: {clang_entry.reset();} completion_entry
				 | error {
				   //printf("CodeLite: syntax error, unexpected token '%s' found\n", clang_lex_get_string().c_str());
				 }
				;

				/* COMPLETION: AddPendingEvent : [#void#][#wxEvtHandler::#]AddPendingEvent(<#wxEvent &event#>) */
				/* COMPLETION: argc : [#int#][#wxAppConsole::#]argc */
completion_entry: CLANG_COMPLETION ':' entry_name ':' type_name scope entry_name function_part 
				{
					bool is_func = !clang_entry.signature.empty();
					clang_entry.parent       = $6;
					clang_entry.type_name    = is_func ? std::string() : clang_entry.tmp;
					clang_entry.return_value = is_func ? clang_entry.tmp : std::string();
					
					if (clang_entry.type == ClangEntry::TypeUnknown) {
						if(is_func)
							clang_entry.type = ClangEntry::TypeMethod;
						else 
							clang_entry.type = ClangEntry::TypeVariable;
					}
					clangEntryVector.push_back(clang_entry);
				}
				;
				
any_operator: CLANG_OP_ARROW     {$$ = $1;}
			| CLANG_OP_ARROWstar {$$ = $1;}
			| CLANG_OP_ASSIGN    {$$ = $1;}
			| CLANG_OP_DECR      {$$ = $1;}
			| CLANG_OP_DOTstar   {$$ = $1;}
			| CLANG_OP_EQ        {$$ = $1;}
			| CLANG_OP_GE        {$$ = $1;}
			| CLANG_OP_ICR       {$$ = $1;}
			| CLANG_OP_LE        {$$ = $1;}
			| CLANG_OP_LS        {$$ = $1;}
			| CLANG_OP_NE        {$$ = $1;}
			| CLANG_OP_RS        {$$ = $1;}
			| '('')'             {$$ = "()";}
			| '['']'             {$$ = "[]";}
			;
		 
entry_name: /* empty */ { $$ = ""; }
			| CLANG_NAME
			{
				clang_entry.name.clear();
				clang_entry.name = $1;
			}
			| CLANG_OPERATOR any_operator
			{
				clang_entry.name.clear();
				clang_entry.name = $1 + $2;
			}
			| CLANG_TILDE CLANG_NAME
			{
				clang_entry.name.clear();
				clang_entry.name = $1 + $2;
				clang_entry.type = ClangEntry::TypeDtor;
			}
		  ;

function_part: /* empty, in this case it means it is a variable */
			 | '(' {clang_read_signature();} ')' function_suffix
			 ;

function_suffix: /* empty */
		       | CLANG_DELIM_OPEN {clang_read_type_name(clang_entry.func_suffix);} CLANG_DELIM_CLOSE
			   ;

scope: /* empty */ {$$ = "";}
	 | CLANG_DELIM_OPEN CLANG_NAME CLANG_CLCL CLANG_DELIM_CLOSE {$$ = $2;}
	 ;
	
type_name : CLANG_DELIM_OPEN {clang_read_type_name(clang_entry.tmp);} CLANG_DELIM_CLOSE
			{
				if(clang_entry.tmp.find("enum ") != std::string::npos) {
					clang_entry.type = ClangEntry::TypeEnum;
				}
			}
		  | CLANG_NAME CLANG_CLCL
		  {
			  clang_entry.type = ClangEntry::TypeCtor;
		  }
		  | CLANG_NAME
		  {
			clang_entry.type = ClangEntry::TypeClass;
		  }
		  ;
%%

void clang_parse_string(const std::string& str){
	clangEntryVector.clear();
	clang_lex_clean();
	clang_set_lexer_input(str);
	clang_result_parse();
	clang_lex_clean();
}

const ClangEntryVector& clang_results() {
	return clangEntryVector;
}

void clang_result_error(char*){
}

void clang_read_signature() {
	clang_entry.signature = "(";
	int depth = 1;
	while(depth > 0)
	{
		int ch = clang_yylex();
		if(ch == 0){
			break;
		}
		
		switch(ch) {
		case CLANG_ARG_DELIM_OPEN:
		case CLANG_ARG_DELIM_CLOSE:
		case CLANG_ARG_OPT_DELIM_OPEN:
		case CLANG_ARG_OPT_DELIM_CLOSE:
			break;
			
		case (int)'(':
			depth++;
			break;
			
		case (int)')':
			depth--;
			if(depth == 0)
				clang_yyless(0);
			break;
			
		default:
			clang_entry.signature += clang_lex_get_string();
			clang_entry.signature += " ";
			break;
		}
	}
	clang_entry.signature += ")";
}

void clang_read_type_name(std::string &store){
	store.clear();
	int depth = 1;
	while(true)
	{
		int ch = clang_yylex();
		if(ch == 0){
			break;
		}
		
		switch(ch) {
		case CLANG_DELIM_CLOSE:
			clang_yyless(0);
			return;
			
		default:
			store += clang_lex_get_string();
			store += " ";
			break;
		}
	}
}

void clang_read_entry_name(std::string &store){
	store.clear();
	while(true)
	{
		int ch = clang_yylex();
		if(ch == 0){
			break;
		}
		
		switch(ch) {
		case (int)':':
			clang_yyless(0);
			return;
			
		default:
			store += clang_lex_get_string();
			store += " ";
			break;
		}
	}
}
