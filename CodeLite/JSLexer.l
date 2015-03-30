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
// /usr/bin/flex -Pjs --batch --outfile=JSLexer.cpp JSLexer.l
//
//====================--------------------------------------------------------------
//
extern "C" int yywrap(void*) { return 1; }

#include <wx/string.h>
#include <string>
#include <wx/filename.h>
#include "JSLexerAPI.h"
#include "JSLexerTokens.h"

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

%x WRAP_PREP
%x CPP_COMMENT
%x C_COMMENT

identifier [$a-zA-Z_][$0-9a-zA-Z_]*
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
<INITIAL>function {return kJS_FUNCTION;}
<INITIAL>prototype {return kJS_PROTOTYPE;}
<INITIAL>var {return kJS_VAR;}
<INITIAL>this {return kJS_THIS;}
<INITIAL>catch {return kJS_CATCH;}
<INITIAL>throw {return kJS_THROW;}
<INITIAL>switch {return kJS_SWITCH;}
<INITIAL>case {return kJS_CASE;}
<INITIAL>for {return kJS_FOR;}
<INITIAL>return {return kJS_RETURN;}
<INITIAL>new {return kJS_NEW;}
<INITIAL>delete {return kJS_DELETE;}
<INITIAL>"." {return kJS_DOT;}
<INITIAL>true {return kJS_TRUE;}
<INITIAL>false {return kJS_FALSE;}
<INITIAL>null {return kJS_NULL;}
<INITIAL>typeof {return kJS_TYPEOF;}
<INITIAL>void {return kJS_VOID;}
<INITIAL>instanceof {return kJS_INSTANCEOF;}
<INITIAL>{horizontal_white}+ { RETURN_WHITESPACE(); }
<INITIAL>({v_tab}|{c_return}|{form_feed})+ {RETURN_WHITESPACE();}
<INITIAL>({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n" { RETURN_NEWLINE(); }
<INITIAL>{decimal_constant}  { return kJS_DEC_NUMBER;}
<INITIAL>{octal_constant}    { return kJS_OCTAL_NUMBER;}
<INITIAL>{hex_constant}      { return kJS_HEX_NUMBER; }
<INITIAL>{floating_constant} { return kJS_FLOAT_NUMBER;}
<INITIAL>"++"                { return kJS_PLUS_PLUS; }
<INITIAL>"--"                { return kJS_MINUS_MINUS; }
<INITIAL>"<<"                { return kJS_LS; }
<INITIAL>">>"                { return kJS_RS; }
<INITIAL>"<="                { return kJS_LE; }
<INITIAL>">="                { return kJS_GE; }
<INITIAL>"=="                { return kJS_EQUAL; }
<INITIAL>"==="               { return kJS_EQUAL3; }
<INITIAL>"!="                { return kJS_NOT_EQUAL; }
<INITIAL>"&&"                { return kJS_AND_AND;}
<INITIAL>"||"                { return kJS_OR_OR;}
<INITIAL>"*="                { return kJS_STAR_EQUAL;}
<INITIAL>"/="                { return kJS_SLASH_EQUAL;}
<INITIAL>"%="                { return kJS_DIV_EQUAL;}
<INITIAL>"+="                { return kJS_PLUS_EQUAL;}
<INITIAL>"-="                { return kJS_MINUS_EQUAL;}
<INITIAL>"<<="               { return kJS_LS_ASSIGN;}
<INITIAL>">>="               { return kJS_RS_ASSIGN;}
<INITIAL>"&="                { return kJS_AND_EQUAL;}
<INITIAL>"^="                { return kJS_POW_EQUAL;}
<INITIAL>"|="                { return kJS_OR_EQUAL;}
<INITIAL>"L"?[']{c_char}*['] {
    return kJS_STRING;
}
<INITIAL>"L"?["]{s_char}*["] {
    return kJS_STRING;
}
<INITIAL>"/*" {
    // Clear the comment collected
    BEGIN C_COMMENT;
    
    JSLexerUserData* userData = (JSLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->ClearComment();
        userData->AppendToComment("/*");
        userData->SetCommentStartLine(yylineno);
    }
}
<C_COMMENT>"*/" {
    JSLexerUserData* userData = (JSLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->AppendToComment("*/");
        userData->SetCommentEndLine(yylineno);
    }
    BEGIN INITIAL;
    if(userData->IsCollectingComments()) {
        return kJS_C_COMMENT;
    }
}
<C_COMMENT>"\n" { 
    JSLexerUserData* userData = (JSLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->AppendToComment("\n");
    }
}
<C_COMMENT>. {
    JSLexerUserData* userData = (JSLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        std::string stdstr = std::string(1, yytext[0]);
        userData->AppendToComment(wxString(stdstr.c_str(), wxConvUTF8));
    }
}

<INITIAL>"//" {
    BEGIN CPP_COMMENT;
    JSLexerUserData* userData = (JSLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->ClearComment();
        userData->AppendToComment("//");
        userData->SetCommentStartLine(yylineno);
    }
}

<CPP_COMMENT>"\\\n" { 
}
<CPP_COMMENT>"\\\r\n" { 
}
<CPP_COMMENT>"\n" {
    BEGIN INITIAL;
    JSLexerUserData* userData = (JSLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->AppendToComment("\n");
        return kJS_CXX_COMMENT;
    }
}
<CPP_COMMENT>. { 
    JSLexerUserData* userData = (JSLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        std::string stdstr = std::string(1, yytext[0]);
        userData->AppendToComment(wxString(stdstr.c_str(), wxConvUTF8));
    }
}
<INITIAL>{identifier} { 
    return kJS_IDENTIFIER;
}
<INITIAL>. { 
    return yytext[0];
}

%%

//=============-------------------------------
// API methods implementation
//=============-------------------------------

void* jsLexerNew(const wxString& content, size_t options )
{
    yyscan_t scanner;
    yylex_init(&scanner);
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    JSLexerUserData *userData = new JSLexerUserData(options);
    
    // keep the file pointer (and make sure we close it at the end)
    userData->SetCurrentPF(NULL);
    yyg->yyextra_r = userData;
    
    wxCharBuffer cb = content.mb_str(wxConvUTF8);
    yy_switch_to_buffer(yy_scan_string(cb.data(), scanner), scanner);
    yycolumn = 1;
    yylineno = 0;
    return scanner;
}

void* jsLexerNew(const wxFileName& filename, size_t options )
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
    JSLexerUserData *userData = new JSLexerUserData(options);
    
    // keep the file pointer (and make sure we close it at the end)
    userData->SetCurrentPF(fp);
    yyg->yyextra_r = userData;
    
    yy_switch_to_buffer(yy_create_buffer(fp, YY_BUF_SIZE, scanner), scanner);
    yycolumn = 1;
    yylineno = 0;
    return scanner;
}

void jsLexerDestroy(void** scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)(*scanner);
    delete (JSLexerUserData*)yyg->yyextra_r;
    yy_delete_buffer(YY_CURRENT_BUFFER, *scanner);

    yylex_destroy(*scanner);
    *scanner = NULL;
}

void jsLexerUnget(void* scanner)
{
    // return the entire token back to the input stream
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    yyless(0);
}

wxString jsLexerText(void* scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    return wxString(yytext, wxConvUTF8);
}

bool jsLexerNext(void* scanner, JSLexerToken& token)
{
    token.column = 0;
    token.type = yylex(scanner);
    if(token.type != 0) {
        struct yyguts_t * yyg = (struct yyguts_t*)scanner;
        JSLexerUserData* userData = (JSLexerUserData*)yyg->yyextra_r;
        switch(token.type) {
        case kJS_CXX_COMMENT:
            // One line up for CXX comments
            token.lineNumber = userData->GetCommentStartLine();
            token.comment = userData->GetComment();
            userData->ClearComment();
            break;
        case kJS_C_COMMENT:
            token.comment = userData->GetComment();
            userData->ClearComment();
            // for block comments, we want the last line of the comment
            // because we usually want to match it to the statement under it
            token.lineNumber = yylineno;
            break;
        default:
            token.lineNumber = yylineno;
            token.text = wxString(yytext, wxConvUTF8);
            token.column = yycolumn;
            break;
        }

    } else {
        token.text.Clear();
        token.lineNumber = 0;
        token.column = 0;
    }
    return token.type != 0;
}

wxString jsLexerCurrentToken(void* scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    return yytext;
}

JSLexerUserData* jsLexerGetUserData(void* scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    return (JSLexerUserData*) yyg->yyextra_r;
}
