%{
/*************** Includes and Defines *****************************/


#include "errno.h"
#include <string>
#define YYSTYPE std::string
#include "gdblexer.h"
#include <vector>
#include <stdlib.h>
#include <string.h>




std::string le_gdb_string_word;
static int angleDepth(0);
static std::vector<YY_BUFFER_STATE> gs_bufferStack;

#define STATE_INITIAL 1
#define STATE_ESCAPE  2
#define STATE_STRING  3

#define RESUME_STATE(x)\
	if(stateToReturn == STATE_INITIAL){\
		BEGIN INITIAL;\
	}else if(stateToReturn == STATE_ESCAPE){\
		BEGIN ESCAPE;\
	}else if(stateToReturn == STATE_STRING){\
		BEGIN STRING;\
	}
	
static int stateToReturn(STATE_INITIAL);
static bool needReset(true);

bool le_gdb_set_input(const YYSTYPE &in);
void le_gdb_lex_clean();
%}

%option yylineno

identifier [0-9a-zA-Z_]*
h_tab [\011]
form_feed [\014]
v_tab [\013]
c_return [\015]
octal_escape  [0-7]{1,3}
hex_escape "x"[0-9a-fA-F]+
simple_esc [a-z]{1}
horizontal_white [ ]|{h_tab}

%x ESCAPE
%x STRING
%x DEPTH_ANGLE
%x DEPTH_ANGLE1

%%
<INITIAL>"<" 	{
					angleDepth = 1;
					BEGIN DEPTH_ANGLE; 
					if(needReset){
						le_gdb_string_word = "";
						needReset = false;
					}

					le_gdb_string_word += yytext[0];
					stateToReturn = STATE_INITIAL;
				}
<INITIAL>"{"															{return (int)'{';}
<INITIAL>"\""															{}
<INITIAL>"}"															{return (int)'}';}
<INITIAL>"("															{return (int)'(';}
<INITIAL>")"															{return (int)')';}
<INITIAL>"["															{return (int)'[';}
<INITIAL>"]"															{return (int)']';}
<INITIAL>"="															{return (int)'=';}
<INITIAL>","															{return (int)',';}
<INITIAL>"^done"														{return LE_GDB_DONE;}
<INITIAL>"name"														{return LE_GDB_NAME;}
<INITIAL>"value"													{return LE_GDB_VALUE;}
<INITIAL>"locals"													{return LE_GDB_LOCALS;}
<INITIAL>{identifier}												{return LE_GDB_WORD;}
<INITIAL>{horizontal_white}+ 										{}
<INITIAL>({v_tab}|{c_return}|{form_feed})+   						{}
<INITIAL>({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n"   	{}
<INITIAL>"\\\""	{BEGIN STRING; le_gdb_string_word = "";}
<INITIAL>"\\\\" {BEGIN ESCAPE;}
<INITIAL>.		{return yytext[0];}
<ESCAPE>"\\\""	{BEGIN INITIAL; return (int)'"';}
<ESCAPE>.		 {BEGIN INITIAL;}
<STRING>"\\\\\"" 	{
						le_gdb_string_word += "\\\"";
					}
<STRING>"\\\"" {BEGIN INITIAL; return LE_GDB_STRING_LITERAL;}
<STRING>"<" 	{
					angleDepth = 1;
					if(needReset){
						le_gdb_string_word = "";
						needReset = false;
					}
					le_gdb_string_word += yytext[0];
					BEGIN DEPTH_ANGLE; 
					stateToReturn = STATE_STRING;
				}
<STRING>.		{le_gdb_string_word += yytext[0];}
<DEPTH_ANGLE>"<" 	{
						angleDepth++; 
						le_gdb_string_word += yytext[0];
					}
<DEPTH_ANGLE>">" 	{
						angleDepth--;
						le_gdb_string_word += yytext[0];
						if(angleDepth == 0){
							RESUME_STATE(stateToReturn);
							needReset = true;
							return LE_GDB_STRING_LITERAL;
						}
					}
					
<DEPTH_ANGLE>.		{le_gdb_string_word += yytext[0];}
%%

void le_gdb_lex_clean(){
	yy_flush_buffer(YY_CURRENT_BUFFER); 
	yy_delete_buffer(YY_CURRENT_BUFFER);
	le_gdb_lineno = 1;
}

void le_gdb_push_buffer(const std::string &new_input){
	// keep current buffer state
	gs_bufferStack.push_back(YY_CURRENT_BUFFER);
	
	// create new buffer and use it
	yy_switch_to_buffer( yy_scan_string(new_input.c_str()) );
}

void le_gdb_pop_buffer(){
	// clean current buffer
	yy_delete_buffer(YY_CURRENT_BUFFER);
	
	// create new buffer and use it
	yy_switch_to_buffer( gs_bufferStack.back() );
	gs_bufferStack.pop_back();
}

bool le_gdb_set_input(const YYSTYPE &in){
	yy_scan_string(in.c_str());
	
	//update the working file name
	return true;
}

int yywrap(){
	return 1;
}
