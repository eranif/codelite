%{
/* Included code before lex code */
#include <string>
#define YYSTYPE std::string
#include "gdb_result_parser.h"

#include <sstream>
#include <vector>


static std::vector<YY_BUFFER_STATE> gs_bufferStack;
static std::string                  gs_string;
static bool                         gs_ascii;
static bool                         gs_want_whitespace = false;

std::string gdb_result_string;
bool setGdbLexerInput(const std::string &in, bool ascii);

void gdb_result_lex_clean();
void gdb_result_less(int count);

extern std::string gdb_result_lval;

#define RETURN_VAL(x) {\
					gdb_result_string = yytext;\
					gdb_result_lval   = yytext;\
					return(x);}

#define RETURN_ESCAPED_STRING(x) {\
					gdb_result_string = yytext;\
					std::string str;\
					str = gdb_result_string.substr(1);\
					gdb_result_string = str;\
					gdb_result_lval = str;\
					return(x);}
%}

%option yylineno
%option nounput
%option never-interactive

/**
 * Some basic regexes
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

%x string_state
%x esc_string_state

%%

{horizontal_white}+                                        {if(gs_want_whitespace) RETURN_VAL((int)' '); }
({v_tab}|{c_return}|{form_feed})+                          {if(gs_want_whitespace) RETURN_VAL((int)' '); }
({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n"   {if(gs_want_whitespace) RETURN_VAL((int)'\n'); }
{decimal_constant}                                         {RETURN_VAL(GDB_INTEGER);}
{octal_constant}                                           {RETURN_VAL(GDB_OCTAL);}
{hex_constant}                                             {RETURN_VAL(GDB_HEX);}
{floating_constant}                                        {RETURN_VAL(GDB_FLOAT);}
done                                                       {RETURN_VAL(GDB_DONE);}
running                                                    {RETURN_VAL(GDB_RUNNING);}
connected                                                  {RETURN_VAL(GDB_CONNECTED);}
error                                                      {RETURN_VAL(GDB_ERROR);}
exit                                                       {RETURN_VAL(GDB_EXIT);}
stack-args                                                 {RETURN_VAL(GDB_STACK_ARGS);}
variables                                                  {RETURN_VAL(GDB_VARIABLES);}
register-names                                             {RETURN_VAL(GDB_REGISTER_NAMES);}
value                                                      {RETURN_VAL(GDB_VALUE);}
name                                                       {RETURN_VAL(GDB_NAME);}
args                                                       {RETURN_VAL(GDB_ARGS);}
level                                                      {RETURN_VAL(GDB_LEVEL);}
frame                                                      {RETURN_VAL(GDB_FRAME);}
locals                                                     {RETURN_VAL(GDB_LOCALS);}
numchild                                                   {RETURN_VAL(GDB_NUMCHILD);}
type                                                       {RETURN_VAL(GDB_TYPE);}
data                                                       {RETURN_VAL(GDB_DATA);}
addr                                                       {RETURN_VAL(GDB_ADDR);}
ascii                                                      {RETURN_VAL(GDB_ASCII);}
children                                                   {RETURN_VAL(GDB_CHILDREN);}
child                                                      {RETURN_VAL(GDB_CHILD);}
varobj                                                     {RETURN_VAL(GDB_VAROBJ);}
"{...}"                                                    {RETURN_VAL(GDB_MORE);}
BreakpointTable                                            {RETURN_VAL(GDB_BREAKPOINT_TABLE);}
nr_rows                                                    {RETURN_VAL(GDB_NR_ROWS);}
nr_cols                                                    {RETURN_VAL(GDB_NR_COLS);}
hdr                                                        {RETURN_VAL(GDB_HDR);}
body                                                       {RETURN_VAL(GDB_BODY);}
bkpt                                                       {RETURN_VAL(GDB_BKPT);}
stopped                                                    {RETURN_VAL(GDB_STOPPED);}
time                                                       {RETURN_VAL(GDB_TIME);}
reason                                                     {RETURN_VAL(GDB_REASON);}
changelist                                                 {RETURN_VAL(GDB_CHANGELIST);}
has_more                                                   {RETURN_VAL(GDB_HAS_MORE);}
dynamic                                                    {RETURN_VAL(GDB_DYNAMIC);}
new_children                                               {RETURN_VAL(GDB_NEW_CHILDREN);}
thread-groups                                              {RETURN_VAL(GDB_THREAD_GROUPS);}
new_num_children                                           {RETURN_VAL(GDB_NEW_NUM_CHILDREN);}
displayhint                                                {RETURN_VAL(GDB_DISPLAYHINT);}
func-name                                                  {RETURN_VAL(GDB_FUNC_NAME);}
offset                                                     {RETURN_VAL(GDB_OFFSET);}
inst                                                       {RETURN_VAL(GDB_INSTRUCTION);}
address                                                    {RETURN_VAL(GDB_ADDRESS);}
asm_insns                                                  {RETURN_VAL(GDB_ASM_INSTS);}
{identifier}                                               {RETURN_VAL(GDB_IDENTIFIER);}
"^"                                                        {RETURN_VAL((int)*yytext);}
"="                                                        {RETURN_VAL((int)*yytext);}
"{"                                                        {RETURN_VAL((int)*yytext);}
"}"                                                        {RETURN_VAL((int)*yytext);}
","                                                        {RETURN_VAL((int)*yytext);}
"L"?["]                                                    {BEGIN (string_state); gs_string = "\"";}
"L"?[\\]["]                                                {BEGIN (esc_string_state); gs_string = "\"";}
.                                                          {RETURN_VAL((int)*yytext);}
<string_state>[\\]{1,2}{octal_escape} {
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
<string_state>"\\\\"n                                      { gs_string += "\\n"; }
<string_state>"\\\\"v                                      { gs_string += "\\v"; }
<string_state>"\\\\"r                                      { gs_string += "\\r"; }
<string_state>"\\\\"t                                      { gs_string += "\\t"; }
<string_state>"\\\\\\"["]                                  { gs_string += "\\\""; }
<string_state>"\\\\\\\\"                                   { gs_string += "\\"; }
<string_state>"\\"["]{1}                                   { gs_string += "\\\"";}
<string_state>"\\\\"                                         { gs_string += "\\";}
<string_state>["]                                          {
	gs_string += "\"";
	gdb_result_string = gs_string;
	gdb_result_lval   = gdb_result_string;
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
<esc_string_state>"\\\\"n                                        { gs_string += "\\n"; }
<esc_string_state>"\\\\"v                                        { gs_string += "\\v"; }
<esc_string_state>"\\\\"r                                        { gs_string += "\\r"; }
<esc_string_state>"\\\\"t                                        { gs_string += "\\t"; }
<esc_string_state>"\\\\\\"["]                                    { gs_string += "\\\""; }
<esc_string_state>"\\\\\\\\"                                     { gs_string += "\\"; }
<esc_string_state>"\\"["]                                        {
	gs_string += "\"";
	gdb_result_string = gs_string;
	gdb_result_lval   = gdb_result_string;
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
	gs_want_whitespace = false;
}

bool setGdbLexerInput(const std::string &in, bool ascii, bool wantWhitespace) {
	BEGIN INITIAL;
	gs_ascii = ascii;
	gs_want_whitespace = wantWhitespace;
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

void gdb_result_error(const char* msg)
{
}
