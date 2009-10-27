%{
/* Included code before lex code */
#include "string"
#include "gdb_result_parser.h"
#include <vector>
#include <sstream>

#define YYSTYPE std::string

static std::vector<YY_BUFFER_STATE> gs_bufferStack;
static std::string                  gs_string;
static bool                         gs_ascii;

std::string gdb_result_string;
bool setGdbLexerInput(const std::string &in, bool ascii);

void gdb_result_lex_clean();
void gdb_result_less(int count);


#define RETURN_VAL(x) {\
					gdb_result_string = yytext;\
					return(x);}

#define RETURN_ESCAPED_STRING(x) {\
					gdb_result_string = yytext;\
					std::string str;\
					str = gdb_result_string.substr(1);\
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

octal_escape  [0-7]{3}

h_tab [\011]
form_feed [\014]
v_tab [\013]
c_return [\015]
escp_str_termi [\},]{1}
horizontal_white [ ]|{h_tab}

%x string_state
%x esc_string_state

%%

{horizontal_white}+     			                       {}
({v_tab}|{c_return}|{form_feed})+                          {}
({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n"   {}
{decimal_constant}  				                       {RETURN_VAL(GDB_INTEGER);}
{octal_constant}    				                       {RETURN_VAL(GDB_OCTAL);}
{hex_constant}      				                       {RETURN_VAL(GDB_HEX);}
{floating_constant} 				                       {RETURN_VAL(GDB_FLOAT);}
done 								                       {RETURN_VAL(GDB_DONE);}
running 							                       {RETURN_VAL(GDB_RUNNING);}
connected 							                       {RETURN_VAL(GDB_CONNECTED);}
error								                       {RETURN_VAL(GDB_ERROR);}
exit	 							                       {RETURN_VAL(GDB_EXIT);}
stack-args							                       {RETURN_VAL(GDB_STACK_ARGS);}
value								                       {RETURN_VAL(GDB_VALUE);}
name								                       {RETURN_VAL(GDB_NAME);}
args								                       {RETURN_VAL(GDB_ARGS);}
level								                       {RETURN_VAL(GDB_LEVEL);}
frame                                                      {RETURN_VAL(GDB_FRAME);}
locals								                       {RETURN_VAL(GDB_LOCALS);}
numchild							                       {RETURN_VAL(GDB_NUMCHILD);}
type								                       {RETURN_VAL(GDB_TYPE);}
data								                       {RETURN_VAL(GDB_DATA);}
addr								                       {RETURN_VAL(GDB_ADDR);}
ascii								                       {RETURN_VAL(GDB_ASCII);}
{identifier}        				                       {RETURN_VAL(GDB_IDENTIFIER);}
"^"									                       {RETURN_VAL((int)*yytext);}
"="									                       {RETURN_VAL((int)*yytext);}
"{"									                       {RETURN_VAL((int)*yytext);}
"}"									                       {RETURN_VAL((int)*yytext);}
","									                       {RETURN_VAL((int)*yytext);}
"L"?["]                    			                       {BEGIN (string_state); gs_string = "\"";}
"L"?[\\]["]                             	               {BEGIN (esc_string_state); gs_string = "\"";}
.  									                       {RETURN_VAL((int)*yytext);}
<string_state>"\\"["]                                      { gs_string += "\\\"";}
<string_state>["]                                          {
	gs_string += "\"";
	gdb_result_string = gs_string;
	gs_string.clear();
	BEGIN (0);
	return GDB_STRING;
}
<string_state>. { gs_string += yytext; }
<esc_string_state>[\\]{2}{octal_escape} {
	if(gs_ascii) {
		unsigned int number(0);
		std::stringstream s;
		s << std::oct << yytext+2;
		s >> number;

		if(number) {
			gs_string += (unsigned char)number;
		}
	} else {
		gs_string += yytext;
	}
}

<esc_string_state>"\\"["]                                          {
	gs_string += "\"";
	gdb_result_string = gs_string;
	gs_string.clear();
	BEGIN (0);
	return GDB_ESCAPED_STRING;
}
<esc_string_state>. { gs_string += yytext; }
%%

/*******************************************************************/
void gdb_result_lex_clean()
{
	yy_flush_buffer(YY_CURRENT_BUFFER);
	yy_delete_buffer(YY_CURRENT_BUFFER);
	gdb_result_lineno = 1;
	gs_ascii = false;
}

bool setGdbLexerInput(const std::string &in, bool ascii) {
	BEGIN INITIAL;
	gs_ascii = ascii;
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
