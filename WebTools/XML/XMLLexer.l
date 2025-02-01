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
// /usr/bin/flex -Pxml --batch --outfile=XmlLexer.cpp XMLLexer.l
//
//====================--------------------------------------------------------------
//
extern "C" int yywrap(void*) { return 1; }

#include "XML/XMLLexerAPI.h"
#include "XML/XMLLexerTokens.h"

#include <string>
#include <wx/filename.h>
#include <wx/string.h>

#define YY_FATAL_ERROR(x)
#define YYSTYPE char*
#define ECHO
#define P(s) fprintf(stderr, "%s\n", s)

#define YY_NO_UNISTD_H
#define YY_USER_ACTION  yycolumn += yyleng;

#define RETURN_WHITESPACE() 
#define RETURN_NEWLINE() 

%}

/* regex and modes */

/* options */
%option yylineno
%option default
%option reentrant

identifier [$a-zA-Z_][$0-9a-zA-Z_\-]*
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

%x CDATA
%x COMMENT
%x NOXML

%% 
<<EOF>> {yyterminate();}
<INITIAL>{horizontal_white} {}
<INITIAL>"/>" {return kXML_T_CLOSE_TAG_SUFFIX;}
<INITIAL>"</" {return kXML_T_CLOSE_TAG_PREFIX;}
<INITIAL>"<?" { BEGIN NOXML; return kXML_T_XML_OPEN_TAG;}
<INITIAL>"<!DOCTYPE" {return kXML_T_HTML_DOCTYPE;}
<INITIAL>"<![CDATA[" { BEGIN CDATA; return kXML_T_CDATA_START; }
<INITIAL>"&lt;" {return kXML_T_ENTITY_LT;}
<INITIAL>"&gt;" {return kXML_T_ENTITY_GT;}
<INITIAL>"&amp;" {return kXML_T_ENTITY_AMP;}
<INITIAL>"&apos;" {return kXML_T_ENTITY_APOS;}
<INITIAL>"&quot;" {return kXML_T_ENTITY_QUOATATION_MARK;}
<INITIAL>"<!--" { BEGIN COMMENT; return kXML_T_COMMENT_START;}
<INITIAL>{identifier} {return kXML_T_IDENTIFIER;}
<INITIAL>[']{c_char}*['] { return kXML_T_STRING;}
<INITIAL>["]{s_char}*["] { return kXML_T_STRING;}
<INITIAL>. { return yytext[0];}
<CDATA>"]]>" { 
    BEGIN INITIAL;
    return kXML_T_CDATA_END; 
}
<CDATA>. { return yytext[0];}
<COMMENT>"-->" { 
    BEGIN INITIAL;
    return kXML_T_COMMENT_END;
}
<COMMENT>. { return yytext[0];}
<NOXML>"?>" { 
    BEGIN INITIAL;
    return kXML_T_XML_CLOSE_TAG;
}
<NOXML>. { return yytext[0];}
%%

//=============-------------------------------
// API methods implementation
//=============-------------------------------

void* xmlLexerNew(const wxString& content)
{
    yyscan_t scanner;
    yylex_init(&scanner);
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    XMLLexerUserData *userData = new XMLLexerUserData();
    
    // keep the file pointer (and make sure we close it at the end)
    userData->SetCurrentPF(NULL);
    yyg->yyextra_r = userData;
    
    wxCharBuffer cb = content.mb_str(wxConvUTF8);
    yy_switch_to_buffer(yy_scan_string(cb.data(), scanner), scanner);
    yycolumn = 1;
    yylineno = 0;
    return scanner;
}

void* xmlLexerNew(const wxFileName& filename)
{
    wxFileName fn = filename;
    if(fn.IsRelative()) {
        fn.MakeAbsolute();
    }
    
    FILE* fp = ::fopen(fn.GetFullPath().mb_str(wxConvUTF8).data(), "rb");
    if(!fp) {
        return NULL;
    }
    yyscan_t scanner;
    yylex_init(&scanner);
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    XMLLexerUserData *userData = new XMLLexerUserData();
    
    // keep the file pointer (and make sure we close it at the end)
    userData->SetCurrentPF(fp);
    yyg->yyextra_r = userData;
    
    yy_switch_to_buffer(yy_create_buffer(fp, YY_BUF_SIZE, scanner), scanner);
    yycolumn = 1;
    yylineno = 0;
    return scanner;
}

void xmlLexerDestroy(void** scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)(*scanner);
    delete (XMLLexerUserData*)yyg->yyextra_r;
    yy_delete_buffer(YY_CURRENT_BUFFER, *scanner);

    yylex_destroy(*scanner);
    *scanner = NULL;
}

void xmlLexerUnget(void* scanner)
{
    // return the entire token back to the input stream
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    yyless(0);
}

wxString xmlLexerText(void* scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    return wxString(yytext, wxConvUTF8);
}

bool xmlLexerNext(void* scanner, XMLLexerToken& token)
{
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    token.Clear();
    token.column = 0;
    token.type = yylex(scanner);
    if(token.type != 0) {
        token.lineNumber = yylineno;
        token.text = wxString(yytext, wxConvUTF8);
        token.column = yycolumn;
    }
    return token.type != 0;
}

wxString xmlLexerCurrentToken(void* scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    return yytext;
}

XMLLexerUserData* xmlLexerGetUserData(void* scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    return (XMLLexerUserData*) yyg->yyextra_r;
}
