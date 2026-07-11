%{

//====================--------------------------------------------------------
// /usr/bin/flex --noline -Pword --noline --batch --outfile=WordTokenizer.cpp WordTokenizer.l
//====================--------------------------------------------------------

extern "C" int yywrap(void*) { return 1; }

#include "WordTokenizerAPI.h"

#define YY_FATAL_ERROR(x)
#define YYSTYPE char*
#define ECHO
#define P(s) fprintf(stderr, "%s\n", s)

#define YY_NO_UNISTD_H
#define YY_USER_ACTION  yycolumn += yyleng;

%}


/* options */
%option yylineno
%option default
%option reentrant

exponent_part [eE][-+]?[0-9]+
fractional_constant ([0-9]*"."[0-9]+)|([0-9]+".")
floating_constant (({fractional_constant}{exponent_part}?)|([0-9]+{exponent_part}))[FfLl]?

integer_suffix_opt ([uU]?[lL]?)|([lL][uU])
decimal_constant [1-9][0-9]*{integer_suffix_opt}
octal_constant "0"[0-7]*{integer_suffix_opt}
hex_constant "0"[xX][0-9a-fA-F]+{integer_suffix_opt}

%% 

<<EOF>> {yyterminate();}
<INITIAL>{decimal_constant}  { return kWordNumber;}
<INITIAL>{octal_constant}    { return kWordNumber;}
<INITIAL>{hex_constant}      { return kWordNumber;}
<INITIAL>{floating_constant} { return kWordNumber;}
<INITIAL>"->" { return kWordDelim;}
<INITIAL>"\r" { return kWordDelim;}
<INITIAL>"\n" { return kWordDelim;}
<INITIAL>" "  { return kWordDelim;}
<INITIAL>"\t" { return kWordDelim;}
<INITIAL>"."  { return kWordDelim;}
<INITIAL>"/"  { return kWordDelim;}
<INITIAL>"\\" { return kWordDelim;}
<INITIAL>"\"" { return kWordDelim;}
<INITIAL>"'"  { return kWordDelim;}
<INITIAL>"["  { return kWordDelim;}
<INITIAL>"]"  { return kWordDelim;}
<INITIAL>"("  { return kWordDelim;}
<INITIAL>")"  { return kWordDelim;}
<INITIAL>"<"  { return kWordDelim;}
<INITIAL>">"  { return kWordDelim;}
<INITIAL>"*"  { return kWordDelim;}
<INITIAL>"&"  { return kWordDelim;}
<INITIAL>"^"  { return kWordDelim;}
<INITIAL>"%"  { return kWordDelim;}
<INITIAL>"#"  { return kWordDelim;}
<INITIAL>"!"  { return kWordDelim;}
<INITIAL>"@"  { return kWordDelim;}
<INITIAL>"+"  { return kWordDelim;}
<INITIAL>"="  { return kWordDelim;}
<INITIAL>":"  { return kWordDelim;}
<INITIAL>";"  { return kWordDelim;}
<INITIAL>","  { return kWordDelim;}
<INITIAL>"{"  { return kWordDelim;}
<INITIAL>"}"  { return kWordDelim;}
<INITIAL>"|"  { return kWordDelim;}
<INITIAL>"`"  { return kWordDelim;}
<INITIAL>"?"  { return kWordDelim;}
<INITIAL>. {
    // Everything else
    return yytext[0];
}

%%

void WordLexerDestroy(WordScanner_t* scanner)
{
    struct yyguts_t * yyg = (struct yyguts_t*)(*scanner);
    yy_delete_buffer(YY_CURRENT_BUFFER, *scanner);
    yylex_destroy(*scanner);
    *scanner = NULL;
}

WordScanner_t WordLexerNew(const wxString& buffer)
{
    WordScanner_t scanner;
    yylex_init(&scanner);
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    //wxScopedCharBuffer cb = buffer.mb_str(wxConvUTF8);
    yy_switch_to_buffer(yy_scan_string(buffer.ToUTF8(), scanner), scanner);
    yycolumn = 1;
    return scanner;
}

bool WordLexerNext(WordScanner_t scanner, WordLexerToken& token)
{
    struct yyguts_t * yyg = (struct yyguts_t*)scanner;
    token.type = wordlex(scanner);
    token.text = wordget_text(scanner);;
    return (token.type != 0);
}
