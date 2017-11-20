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
// /usr/bin/flex --noline --batch --outfile=CxxLexer.cpp CxxScanner.l
//
//====================--------------------------------------------------------------
//
extern "C" int yywrap(void*) { return 1; }

#include <wx/string.h>
#include <string>
#include <wx/filename.h>
#include "CxxLexerAPI.h"
#include "CxxScannerTokens.h"

#define YY_FATAL_ERROR(x)
#define YYSTYPE char*
#define ECHO
#define P(s) 

#define YY_NO_UNISTD_H
#define YY_USER_ACTION  yycolumn += yyleng;
#define RETURN_WHITESPACE()                                         \
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r; \
    if(userData->IsCollectingWhitespace()) {                        \
        return T_WHITESPACE;                                        \
    }
#define RETURN_NEWLINE()                                            \
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r; \
    if(userData->IsCollectingWhitespace()) {                        \
        return T_NEWLINE;                                           \
    }
    
%}

/* regex and modes */

/* options */
%option yylineno
%option default
%option reentrant

%x PREPR
%x WRAP_PREP
%x CPP_COMMENT
%x C_COMMENT
%x INCLUDE

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
<<EOF>> {yyterminate();}
<INITIAL>alignas          { return T_ALIGNAS; }
<INITIAL>alignof          { return T_ALIGNOF; }
<INITIAL>and              { return T_AND;     }
<INITIAL>and_eq           { return T_AND_EQ;  }
<INITIAL>asm              { return T_ASM;     }
<INITIAL>auto             { return T_AUTO;    }
<INITIAL>bitand           { return T_BITAND;  }
<INITIAL>bitor            { return T_BITOR;   }
<INITIAL>bool             { return T_BOOL;    }
<INITIAL>break            { return T_BREAK;   }
<INITIAL>case             { return T_CASE;   }
<INITIAL>catch            { return T_CATCH;} 
<INITIAL>char             { return T_CHAR;} 
<INITIAL>char16_t         { return T_CHAR16_T;} 
<INITIAL>char32_t         { return T_CHAR32_T;} 
<INITIAL>class            { return T_CLASS;} 
<INITIAL>compl            { return T_COMPL;} 
<INITIAL>const            { return T_CONST;} 
<INITIAL>constexpr        { return T_CONSTEXPR;} 
<INITIAL>const_cast       { return T_CONST_CAST;} 
<INITIAL>continue         { return T_CONTINUE;} 
<INITIAL>decltype         { return T_DECLTYPE;} 
<INITIAL>default          { return T_DEFAULT;} 
<INITIAL>delete           { return T_DELETE;} 
<INITIAL>do               { return T_DO;} 
<INITIAL>double           { return T_DOUBLE;} 
<INITIAL>dynamic_cast     { return T_DYNAMIC_CAST;} 
<INITIAL>else             { return T_ELSE;} 
<INITIAL>enum             { return T_ENUM;} 
<INITIAL>explicit         { return T_EXPLICIT;} 
<INITIAL>export           { return T_EXPORT;} 
<INITIAL>extern           { return T_EXTERN;} 
<INITIAL>false            { return T_FALSE;} 
<INITIAL>final            { return T_FINAL;} 
<INITIAL>float            { return T_FLOAT;} 
<INITIAL>for              { return T_FOR;} 
<INITIAL>friend           { return T_FRIEND;} 
<INITIAL>goto             { return T_GOTO;} 
<INITIAL>if               { return T_IF;} 
<INITIAL>inline           { return T_INLINE;} 
<INITIAL>int              { return T_INT;} 
<INITIAL>long             { return T_LONG;} 
<INITIAL>mutable          { return T_MUTABLE;} 
<INITIAL>namespace        { return T_NAMESPACE;} 
<INITIAL>new              { return T_NEW;} 
<INITIAL>noexcept         { return T_NOEXCEPT;} 
<INITIAL>not              { return T_NOT;} 
<INITIAL>not_eq           { return T_NOT_EQ;} 
<INITIAL>nullptr          { return T_NULLPTR;} 
<INITIAL>operator         { return T_OPERATOR;} 
<INITIAL>or               { return T_OR;} 
<INITIAL>or_eq            { return T_OR_EQ;} 
<INITIAL>override         { return T_OVERRIDE;} 
<INITIAL>private          { return T_PRIVATE;} 
<INITIAL>protected        { return T_PROTECTED;} 
<INITIAL>public           { return T_PUBLIC;} 
<INITIAL>register         { return T_REGISTER;} 
<INITIAL>reinterpret_cast { return T_REINTERPRET_CAST;}     
<INITIAL>return           { return T_RETURN;} 
<INITIAL>short            { return T_SHORT;} 
<INITIAL>signed           { return T_SIGNED;} 
<INITIAL>sizeof           { return T_SIZEOF;} 
<INITIAL>static           { return T_STATIC;} 
<INITIAL>static_assert    { return T_STATIC_ASSERT;}  
<INITIAL>static_cast      { return T_STATIC_CAST;} 
<INITIAL>struct           { return T_STRUCT;} 
<INITIAL>switch           { return T_SWITCH;} 
<INITIAL>template         { return T_TEMPLATE;} 
<INITIAL>this             { return T_THIS;} 
<INITIAL>thread_local     { return T_THREAD_LOCAL;} 
<INITIAL>throw            { return T_THROW;} 
<INITIAL>true             { return T_TRUE;} 
<INITIAL>try              { return T_TRY;}
<INITIAL>typedef          { return T_TYPEDEF;} 
<INITIAL>typeid           { return T_TYPEID;} 
<INITIAL>typename         { return T_TYPENAME;} 
<INITIAL>union            { return T_UNION;} 
<INITIAL>unsigned         { return T_UNSIGNED;} 
<INITIAL>using            { return T_USING;} 
<INITIAL>virtual          { return T_VIRTUAL;} 
<INITIAL>void             { return T_VOID;} 
<INITIAL>volatile         { return T_VOLATILE;} 
<INITIAL>wchar_t          { return T_WCHAR_T;} 
<INITIAL>while            { return T_WHILE;} 
<INITIAL>xor              { return T_XOR;} 
<INITIAL>xor_eq           { return T_XOR_EQ;}
<INITIAL>{horizontal_white}+ { RETURN_WHITESPACE(); }
<INITIAL>({v_tab}|{c_return}|{form_feed})+ {RETURN_WHITESPACE();}
<INITIAL>({horizontal_white}|{v_tab}|{c_return}|{form_feed})*"\n" { RETURN_NEWLINE(); }
<INITIAL>{decimal_constant}  { return T_DEC_NUMBER;}
<INITIAL>{octal_constant}    { return T_OCTAL_NUMBER;}
<INITIAL>{hex_constant}      { return T_HEX_NUMBER; }
<INITIAL>{floating_constant} { return T_FLOAT_NUMBER;}
<INITIAL>".*"                { return T_DOT_STAR; }
<INITIAL>"::"                { return T_DOUBLE_COLONS; }
<INITIAL>"->"                { return T_ARROW; }
<INITIAL>"->*"               { return T_ARROW_STAR; }
<INITIAL>"++"                { return T_PLUS_PLUS; }
<INITIAL>"--"                { return T_MINUS_MINUS; }
<INITIAL>"<<"                { return T_LS; }
<INITIAL>"<="                { return T_LE; }
<INITIAL>">="                { return T_GE; }
<INITIAL>"=="                { return T_EQUAL; }
<INITIAL>"!="                { return T_NOT_EQUAL; }
<INITIAL>"&&"                { return T_AND_AND;}
<INITIAL>"||"                { return T_OR_OR;}
<INITIAL>"*="                { return T_STAR_EQUAL;}
<INITIAL>"/="                { return T_SLASH_EQUAL;}
<INITIAL>"%="                { return T_DIV_EQUAL;}
<INITIAL>"+="                { return T_PLUS_EQUAL;}
<INITIAL>"-="                { return T_MINUS_EQUAL;}
<INITIAL>"<<="               { return T_LS_ASSIGN;}
<INITIAL>">>="               { return T_RS_ASSIGN;}
<INITIAL>"&="                { return T_AND_EQUAL;}
<INITIAL>"^="                { return T_POW_EQUAL;}
<INITIAL>"|="                { return T_OR_EQUAL;}
<INITIAL>"..."               { return T_3_DOTS;}
<INITIAL>"L"?[']{c_char}+['] {
    return T_STRING;
}
<INITIAL>"L"?["]{s_char}*["] {
    return T_STRING;
}
<INITIAL>"/*" {
    // Clear the comment collected
    BEGIN C_COMMENT;
    
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->ClearComment();
        userData->AppendToComment("/*");
        userData->SetCommentStartLine(yylineno);
    }
}
<C_COMMENT>"*/" {
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->AppendToComment("*/");
        userData->SetCommentEndLine(yylineno);
    }
    BEGIN INITIAL;
    if(userData->IsCollectingComments()) {
        return T_C_COMMENT;
    }
}
<C_COMMENT>"\n" { 
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->AppendToComment("\n");
    }
}
<C_COMMENT>. {
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->AppendToComment(yytext[0]);
    }
}

<INITIAL,PREPR>"//" {
    int oldState = yyg->yy_start;
    BEGIN CPP_COMMENT;
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->ClearComment();
        userData->AppendToComment("//");
        userData->SetCommentStartLine(yylineno);
    }
    if((1 + 2 * PREPR) == oldState) {
        return T_PP_STATE_EXIT;
    }
}

<CPP_COMMENT>"\\\n" { 
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->AppendToComment("\\\n");
    } 
}
<CPP_COMMENT>"\\\r\n" { 
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->AppendToComment("\\\r\n");
    } 
}
<CPP_COMMENT>"\n" {
    BEGIN INITIAL;
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->AppendToComment("\n");
        return T_CXX_COMMENT;
    }
}
<CPP_COMMENT>. { 
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
    if(userData->IsCollectingComments()) {
        userData->AppendToComment(yytext[0]);
    }
}
<INITIAL>{identifier} { 
    return T_IDENTIFIER;
}
<INITIAL>"#" { 
    BEGIN(PREPR);
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
    userData->SetPreProcessorSection(true);
}
<INITIAL>. { 
    return yytext[0];
}
<PREPR>"\\\n" {
    /* keep the current state */ 
}
<PREPR>"\\\r\n" {
    /* keep the current state */ 
}
<PREPR>[ \t\r\v\b]+ {
    /* do nothing */
    RETURN_WHITESPACE();
}
<PREPR>"\n" { 
    BEGIN(INITIAL);
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
    userData->SetPreProcessorSection(false);
    return T_PP_STATE_EXIT;
}
<PREPR>"define" { 
    return T_PP_DEFINE;
}
<PREPR>"defined" { 
    return T_PP_DEFINED;
}
<PREPR>"if" { 
    return T_PP_IF;
}
<PREPR>"ifdef" { 
    return T_PP_IFDEF;
}
<PREPR>"ifndef" {
    return T_PP_IFNDEF;
}
<PREPR>"else" { 
    return T_PP_ELSE;
}
<PREPR>"elif" { 
    return T_PP_ELIF;
}
<PREPR>"endif" { 
    return T_PP_ENDIF;
}
<PREPR>"line" { 
    return T_PP_LINE;
}
<PREPR>"pragma" { 
    return T_PP_PRAGMA;
}
<PREPR>"undef" { 
    return T_PP_UNDEF;
}
<PREPR>"error" { 
    return T_PP_ERROR;
}
<PREPR>"&&" { 
    return T_PP_AND;
}
<PREPR>"||" { 
    return T_PP_OR;
}
<PREPR>">" { 
    return T_PP_GT;
}
<PREPR>">=" { 
    return T_PP_GTEQ;
}
<PREPR>"<" { 
    return T_PP_LT;
}
<PREPR>"<=" { 
    return T_PP_LTEQ;
}
<PREPR>"include" { 
    BEGIN(INCLUDE);
}
<PREPR>"L"?[']{c_char}+['] {
    return T_PP_STRING;
}
<PREPR>"L"?["]{s_char}*["] {
    return T_PP_STRING;
}
<PREPR>{decimal_constant}  { return T_PP_DEC_NUMBER;}
<PREPR>{octal_constant}    { return T_PP_OCTAL_NUMBER;}
<PREPR>{hex_constant}      { return T_PP_HEX_NUMBER; }
<PREPR>{floating_constant} { return T_PP_FLOAT_NUMBER;}
<PREPR>{identifier} { 
    return T_PP_IDENTIFIER;
}
<PREPR>. { 
    return yytext[0];
}
<INCLUDE>\n {
    BEGIN(INITIAL);
    CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
    userData->SetPreProcessorSection(false);
    return T_PP_STATE_EXIT;
}
<INCLUDE>[ \t]* { RETURN_WHITESPACE(); }
<INCLUDE>["<][^ \t\n]+[">] { /* got the include file name */
    return T_PP_INCLUDE_FILENAME;
}

%%

//=============-------------------------------
// API methods implementation
//=============-------------------------------

void* LexerNew(const wxString& content, size_t options )
{
    yyscan_t scanner;
    yylex_init(&scanner);
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    CppLexerUserData *userData = new CppLexerUserData(options);
    
    // keep the file pointer (and make sure we close it at the end)
    userData->SetCurrentPF(NULL);
    yyg->yyextra_r = userData;
    
    wxCharBuffer cb = content.mb_str(wxConvUTF8);
    yy_switch_to_buffer(yy_scan_string(cb.data(), scanner), scanner);
    yycolumn = 1;
    return scanner;
}

void* LexerNew(const wxFileName& filename, size_t options )
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
    CppLexerUserData *userData = new CppLexerUserData(options);
    
    // keep the file pointer (and make sure we close it at the end)
    userData->SetCurrentPF(fp);
    yyg->yyextra_r = userData;
    
    yy_switch_to_buffer(yy_create_buffer(fp, YY_BUF_SIZE, scanner), scanner);
    yycolumn = 1;
    return scanner;
}

void LexerDestroy(void** scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)(*scanner);
    delete (CppLexerUserData*)yyg->yyextra_r;
    yy_delete_buffer(YY_CURRENT_BUFFER, *scanner);

    yylex_destroy(*scanner);
    *scanner = NULL;
}

void LexerUnget(void* scanner)
{
    // return the entire token back to the input stream
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    yyless(0);
}

bool LexerNext(void* scanner, CxxLexerToken& token)
{
    token.SetColumn(0);
    token.SetType(yylex(scanner));
    if(!token.IsEOF()) {
        struct yyguts_t * yyg = (struct yyguts_t*)scanner;
        CppLexerUserData* userData = (CppLexerUserData*)yyg->yyextra_r;
        switch(token.GetType()) {
        case T_CXX_COMMENT:
            // One line up for CXX comments
            token.SetLineNumber(userData->GetCommentStartLine());
            token.SetComment(userData->GetComment());
            userData->ClearComment();
            break;
        case T_C_COMMENT:
            token.SetComment(userData->GetComment());
            userData->ClearComment();
            token.SetLineNumber(userData->GetCommentStartLine());
            break;
        default:
            token.SetLineNumber(yylineno);
            token.SetText(yytext);
            token.SetColumn(yycolumn);
            break;
        }

    } else {
        token.SetText(NULL);
        token.SetLineNumber(0);
        token.SetColumn(0);
    }
    return !token.IsEOF();
}

wxString LexerCurrentToken(void* scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    return yytext;
}

CppLexerUserData* LexerGetUserData(void* scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    return (CppLexerUserData*) yyg->yyextra_r;
}
