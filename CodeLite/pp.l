/**
 * the "incl" state is used for picking up the name
 * of an include file
 */
%x incl
%x c_comment
%x cpp_comment
%x using_namespace
%x PP

/** #define related states **/
%x define_state
%x define_state_2
%x define_state_signature
%x define_state_definition
%x define_generic_c_comment
%x define_generic_cpp_comment
%x ifdef_state
%x if_state

%option yylineno

identifier [a-zA-Z_][0-9a-zA-Z_]*
simple_escape [abfnrtv'"?\\]
octal_escape  [0-7]{1,3}
hex_escape "x"[0-9a-fA-F]+

escape_sequence [\\]({simple_escape}|{octal_escape}|{hex_escape})
c_char [^'\\\n]|{escape_sequence}
s_char [^"\\\n]|{escape_sequence}

ns_name  [a-zA-Z][a-zA-Z:_]*
using_ns "using "[ ]*"namespace"
ns_alias "namespace "{ns_name}[ ]*"="[ ]*{ns_name}[ ]*";"

%{
// Avoid spam output
#ifdef  ECHO
#undef  ECHO
#endif
#define ECHO

// Never exit
#ifdef  YY_FATAL_ERROR
#undef  YY_FATAL_ERROR
#endif
#define YY_FATAL_ERROR(msg)

#include <list>
#include <wx/string.h>
#include <string>
#include "pp_include.h"
#include "pp_lexer.h"
#include "pptable.h"

#define YYSTYPE wxString
extern wxString pp_lval;

int return_to_state = 0;
int in_if_1         = 0;

#define RET_VAL(x) {\
    pp_lval = wxString::From8BitData(pp_text);\
    return x;\
}

std::string                _definition;
wxString                   g_definition;
bool                       g_forCC = false;
wxString                   g_filename;

extern int pp_parse();
%}

%option yylineno

%%

"//" {
	BEGIN(cpp_comment);
}

"/*" {
	BEGIN(c_comment);
}

"L"?[']{c_char}+[']     {/* eat a string */}
"L"?["]{s_char}*["]     {/* eat a string */}

#                           { BEGIN(PP);                                          }

<PP>define                  { BEGIN(define_state); RET_VAL(PP_DEFINE);            }
<PP>if                      { if(in_if_1) in_if_1++; BEGIN(if_state); RET_VAL(PP_IF);        }
<PP>0                       { RET_VAL(PP_ZERO);                                   }
<PP>__cplusplus             { RET_VAL(PP_CPLUSPLUS);                              }
<PP>ifdef                   { if(in_if_1) in_if_1++;BEGIN(ifdef_state); RET_VAL(PP_IFDEF);              }
<PP>defined                 { RET_VAL(PP_DEFINED);                                }
<PP>ifndef                  { if(in_if_1) in_if_1++; BEGIN(ifdef_state); RET_VAL(PP_IFDEF);  }
<PP>undef                   { RET_VAL(PP_UNDEF);                                  }
<PP>else                    { if(in_if_1 == 1) in_if_1 = 0;RET_VAL(PP_ELSE);      }
<PP>elif                    { if(in_if_1 == 1) in_if_1--; BEGIN(if_state); RET_VAL(PP_ELIF);       }
<PP>endif                   { if(in_if_1) in_if_1--; RET_VAL(PP_ENDIF);           }
<PP>include                 { BEGIN(incl); RET_VAL(PP_INCLUDE);                   }
<PP>\n                      { BEGIN(INITIAL);                                     }
<PP>.                       {}

<ifdef_state>__cplusplus    { RET_VAL(PP_CPLUSPLUS);                              }
<ifdef_state>{identifier}   { RET_VAL(PP_IDENTIFIER);                             }
<ifdef_state>"("            { RET_VAL(((int)'('));                                }
<ifdef_state>")"            { RET_VAL(((int)')'));                                }
<ifdef_state>\n             { BEGIN(INITIAL);                                     }
<ifdef_state>.              {}

<if_state>__cplusplus    { RET_VAL(PP_CPLUSPLUS);                              }
<if_state>!defined       { RET_VAL(PP_DEFINED);                                }
<if_state>defined        { RET_VAL(PP_DEFINED);                                }
<if_state>{identifier}   { RET_VAL(PP_IDENTIFIER);                             }
<if_state>"("            { RET_VAL(((int)'('));                                }
<if_state>")"            { RET_VAL(((int)')'));                                }
<if_state>"&&"           { RET_VAL(PP_AND);                                    }
<if_state>"||"           { RET_VAL(PP_OR);                                     }
<if_state>"="            { RET_VAL(PP_EQUAL);                                  }
<if_state>"!="           { RET_VAL(PP_NEQUAL);                                 }
<if_state>">"            { RET_VAL(PP_GREATERTHAN);                            }
<if_state>"<"            { RET_VAL(PP_LOWERTHAN);                              }
<if_state>[0-9]+         { RET_VAL(PP_INT);                                    }
<if_state>\n             { BEGIN(INITIAL);                                     }
<if_state>.              {}
 
<define_state>{identifier}  { BEGIN(define_state_2); RET_VAL(PP_IDENTIFIER); }
<define_state>\\[\n\r]{1,2} { /* continue define_state */}
<define_state>\n            { BEGIN(INITIAL);}
<define_state>.             {}

<define_state_2>[ \t]       {
/* whitespaces are NOT allowed between the signature and the macro's name, 
 * if we found a whitespace, handle this macro as a simple macro */
 BEGIN(define_state_definition);   
 _definition.clear();
}
<define_state_2>"/*"        { return_to_state = define_state_2; BEGIN(define_generic_c_comment);}
<define_state_2>"//"        { 
	BEGIN(define_generic_cpp_comment);
	_definition.clear(); 
	g_definition.Clear(); 
	RET_VAL(PP_COMPLEX_REPLACEMENT);
}
<define_state_2>"("         { BEGIN(define_state_signature); RET_VAL(((int)'(')); }
<define_state_2>\n          { BEGIN(INITIAL); _definition.clear(); g_definition.Clear(); RET_VAL(PP_COMPLEX_REPLACEMENT);}
<define_state_2>.           { BEGIN(define_state_definition);   _definition = yytext;}

<define_state_signature>{identifier} { RET_VAL(PP_IDENTIFIER);                             }
<define_state_signature>,            { RET_VAL(((int)*yytext));                            }
<define_state_signature>")"          { BEGIN(define_state_definition); _definition.clear(); g_definition.Clear(); RET_VAL(((int)*yytext));}
<define_state_signature>.            {}

<define_state_definition>"/*"        { return_to_state = define_state_definition; BEGIN(define_generic_c_comment);}
<define_state_definition>"//"        { 
	BEGIN(define_generic_cpp_comment);
	g_definition = wxString::From8BitData(_definition.c_str()); 
	RET_VAL(PP_COMPLEX_REPLACEMENT);
}
<define_state_definition>\\\n        {}
<define_state_definition>\n          { BEGIN(INITIAL); g_definition = wxString::From8BitData(_definition.c_str()); RET_VAL(PP_COMPLEX_REPLACEMENT);}
<define_state_definition>\t          { _definition += " ";}
<define_state_definition>\r          {}
<define_state_definition>.           { _definition += yytext;}

.                                    {}

<incl>\n                             {BEGIN(INITIAL);}
<incl>.                              {}

<define_generic_c_comment>"*/" {
	BEGIN(return_to_state);
}

<define_generic_c_comment>.  {}

<define_generic_cpp_comment>\n {
	BEGIN(INITIAL);
}
<define_generic_cpp_comment>. {}

<cpp_comment>\n {
	BEGIN(INITIAL);
}
<cpp_comment>. {} /* do nothing */

<c_comment>"*/" {
	BEGIN(INITIAL);
}

<c_comment>.  {}

<<EOF>> {
    yyterminate();
}

%%

int yywrap() {
	return 1;
}

int PPScan( const wxString &filePath, bool forCC )
{
	g_filename = filePath;
	
	BEGIN INITIAL;
	pp_lineno = 1;
	FILE* fp = fopen(filePath.To8BitData(), "r");
	if ( fp == NULL ) {
		//printf("%s\n", strerror(errno));
		// failed to open input file...
		return -1;
	}

	yy_switch_to_buffer( yy_create_buffer(fp, YY_BUF_SIZE) );
	pp_in = fp;
	g_forCC = forCC;
	int rc = pp_parse();
    g_forCC = false;
	
    if ( YY_CURRENT_BUFFER->yy_input_file ) {
        fclose( YY_CURRENT_BUFFER->yy_input_file );
        YY_CURRENT_BUFFER->yy_input_file = NULL;

    }

    yy_delete_buffer    ( YY_CURRENT_BUFFER    );
    yyterminate();
	return rc;
}

int PPLex( const wxString &filePath)
{
	g_filename = filePath;
	
	BEGIN INITIAL;
	pp_lineno = 1;
	FILE* fp = fopen(filePath.To8BitData(), "r");
	if ( fp == NULL ) {
		return -1;
	}

	yy_switch_to_buffer( yy_create_buffer(fp, YY_BUF_SIZE) );
	pp_in = fp;
	g_forCC = false;
	int type = 0;
	while( true ) {
		type = pp_lex();
		if(type == 0) break;
		//printf("%s, %d\n", pp_lval.c_str(), type);
	}
	
    if ( YY_CURRENT_BUFFER->yy_input_file ) {
        fclose( YY_CURRENT_BUFFER->yy_input_file );
        YY_CURRENT_BUFFER->yy_input_file = NULL;

    }

    yy_delete_buffer    ( YY_CURRENT_BUFFER    );
	return type;
}

int PPScanString( const wxString &inputString )
{
	BEGIN INITIAL;
	yy_scan_string( inputString.To8BitData() );
	pp_lineno = 1;
	
	int rc = pp_parse();
    
    yyterminate();
	return rc;
}

void pp_error(char* st)
{
}
