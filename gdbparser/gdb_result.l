%{ 
/* Included code before lex code */
#include "string"
#include "gdb_result_parser.h"
#include <vector>

#define YYSTYPE std::string

static std::vector<YY_BUFFER_STATE> gs_bufferStack;

std::string gdb_result_string;
bool setGdbLexerInput(const std::string &in);

void gdb_result_lex_clean();
void gdb_result_less(int count);


#define RETURN_VAL(x) {\
					gdb_result_string = yytext;\
					return(x);}
					
#define RETURN_ESCAPED_STRING(x) {\
					gdb_result_string = yytext;\
					std::string str;\
					str = gdb_result_string.substr(1);\
					str.erase(str.size()-2, 1);\
					gdb_result_string = str;\
					return(x);}
%}

%option yylineno

/**
 * Some basic regexes
 */
 
identifier [a-zA-Z_][0-9a-zA-Z_]*
exponent_part [eE][-+]?[0-9]+
fractional_constant ([0-9]*"."[0-9]+)|([0-9]+".")
floating_constant (({fractional_constant}{exponent_part}?)|([0-9]+{exponent_part}))[FfLl]?

integer_suffix_opt ([uU]?[lL]?)|([lL][uU])
decimal_constant [1-9][0-9]*{integer_suffix_opt}
octal_constant "0"[0-7]*{integer_suffix_opt}
hex_constant "0"[xX][0-9a-fA-F]+{integer_suffix_opt}

simple_escape [abfnrtv'"?\\]
octal_escape  [0-7]{1,3}
hex_escape "x"[0-9a-fA-F]+

escape_sequence [\\]({simple_escape}|{octal_escape}|{hex_escape})
c_char [^'\\\n]|{escape_sequence}
s_char [^"\\\n]|{escape_sequence}
h_tab [\011]
form_feed [\014]
v_tab [\013]
c_return [\015]

horizontal_white [ ]|{h_tab}
%%

{horizontal_white}+     			{}
({v_tab}|{c_return}|{form_feed})+   {}
({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n"   {}
{decimal_constant}  				{RETURN_VAL(GDB_INTEGER);}
{octal_constant}    				{RETURN_VAL(GDB_OCTAL);}
{hex_constant}      				{RETURN_VAL(GDB_HEX);}
{floating_constant} 				{RETURN_VAL(GDB_FLOAT);}
done 								{RETURN_VAL(GDB_DONE);}
running 							{RETURN_VAL(GDB_RUNNING);}
connected 							{RETURN_VAL(GDB_CONNECTED);}
error								{RETURN_VAL(GDB_ERROR);}
exit	 							{RETURN_VAL(GDB_EXIT);}
stack-args							{RETURN_VAL(GDB_STACK_ARGS);}
value								{RETURN_VAL(GDB_VALUE);}
name								{RETURN_VAL(GDB_NAME);}
args								{RETURN_VAL(GDB_ARGS);}
level								{RETURN_VAL(GDB_LEVEL);}
frame                               {RETURN_VAL(GDB_FRAME);}
locals								{RETURN_VAL(GDB_LOCALS);}
{identifier}        				{RETURN_VAL(GDB_IDENTIFIER);}
"^"									{RETURN_VAL((int)*yytext);}
"="									{RETURN_VAL((int)*yytext);}
"{"									{RETURN_VAL((int)*yytext);}
"}"									{RETURN_VAL((int)*yytext);}
","									{RETURN_VAL((int)*yytext);}
["]{s_char}*["]     				{RETURN_VAL(GDB_STRING);}
[\\]["]{s_char}*[\\]["]     		{RETURN_ESCAPED_STRING(GDB_ESCAPED_STRING);}
.  									{RETURN_VAL((int)*yytext);}
%%

/*******************************************************************/
void gdb_result_lex_clean()
{
	yy_flush_buffer(YY_CURRENT_BUFFER); 
	yy_delete_buffer(YY_CURRENT_BUFFER);
	gdb_result_lineno = 1;
}

bool setGdbLexerInput(const std::string &in) {
	BEGIN INITIAL;
	yy_scan_string(in.c_str());
	return true;
}

int yywrap(){
	return 1;
}

void gdb_result_less(int count){
	yyless(count);
}

void gdb_result_push_buffer(const std::string &new_input){
	// keep current buffer state
	gs_bufferStack.push_back(YY_CURRENT_BUFFER);
	
	// create new buffer and use it
	yy_switch_to_buffer( yy_scan_string(new_input.c_str()) );
}

void gdb_result_pop_buffer(){
	// clean current buffer
	yy_delete_buffer(YY_CURRENT_BUFFER);
	
	// create new buffer and use it
	yy_switch_to_buffer( gs_bufferStack.back() );
	gs_bufferStack.pop_back();
}
