%{
//
//====================--------------------------------------------------------------
// Eran Ifrah 2014 (c)
//====================--------------------------------------------------------------
//
// To generate a source file from this .l file, you will need
// a flex version 2.5.34 and later
// Under Windows, you will need to run the following command 
// from within *MSYS* terminal (or run codelite from an MSYS shell):
// /usr/bin/flex -Pwords --noline --batch --outfile=WordsScanner.cpp Words.l
//
//====================--------------------------------------------------------------
//
extern "C" int yywrap(void*) { return 1; }

#include <wx/string.h>
#include <string>
#include <wx/filename.h>
#include "WordsTokens.h"

#define YY_FATAL_ERROR(x)
#define YYSTYPE wxString
#define ECHO
#define P(s) fprintf(stderr, "%s\n", s)
#define YY_NO_UNISTD_H

%}

/* regex and modes */
/* options */
%option default
%option reentrant

identifier [#$a-zA-Z_][#0-9a-zA-Z_]*
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
<<EOF>> {yyterminate();}
<INITIAL>{horizontal_white}+ {}
<INITIAL>({v_tab}|{c_return}|{form_feed})+ {}
<INITIAL>({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n" {}
<INITIAL>{decimal_constant}  { return kWORD_T_NUMBER;}
<INITIAL>{octal_constant}    { return kWORD_T_NUMBER;}
<INITIAL>{hex_constant}      { return kWORD_T_NUMBER; }
<INITIAL>{floating_constant} { return kWORD_T_NUMBER;}
<INITIAL>"L"?[']{c_char}+['] {
    return kWORD_T_STRING;
}
<INITIAL>"L"?["]{s_char}*["] {
    return kWORD_T_STRING;
}
<INITIAL>{identifier} { 
    return kWORD_T_WORD;
}
<INITIAL>. { return yytext[0]; }
%%

//=============-------------------------------
// API methods implementation
//=============-------------------------------

void* wordsLexerNew(const wxString& content, size_t options)
{
    wxUnusedVar(options);
    yyscan_t scanner;
    wordslex_init(&scanner);
    wxCharBuffer cb = content.mb_str(wxConvUTF8);
    words_switch_to_buffer(words_scan_string(cb.data(), scanner), scanner);
    return scanner;
}

void wordsLexerDestroy(void** scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)(*scanner);
    yy_delete_buffer(YY_CURRENT_BUFFER, *scanner);
    yylex_destroy(*scanner);
    *scanner = NULL;
}

bool wordsLexerNext(void* scanner, WordToken& token)
{
    token.type = wordslex(scanner);
    if(token.type != 0) {
        token.word = wordsget_text(scanner);
    }
    return token.type != 0;
}
