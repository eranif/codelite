/**
 * the "incl" state is used for picking up the name
 * of an include file
 */  
%x incl
%x c_comment
%x cpp_comment
%x using_namespace

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
#include <wx/filename.h>
#include "crawler_include.h"

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
{using_ns}              {BEGIN(using_namespace);}
<using_namespace>[a-zA-Z][a-zA-Z:_]* { /* got the namespace */
	fcFileOpener::Get()->AddNamespace(yytext);
	BEGIN(INITIAL);
}
<using_namespace>;      {BEGIN(INITIAL);}
<using_namespace>.      {}
{ns_alias}              {fcFileOpener::Get()->AddNamespaceAlias(yytext);}
include                 {BEGIN(incl);}
.                       {}
<incl>\n                {BEGIN(INITIAL);}
<incl>[ \t]*            {}      /* eat the whitespace        */
<incl>["<][^ \t\n]+[">] { /* got the include file name */
    // Open the new file
    FILE * new_file(NULL);
    
    // keep the include statement
    fcFileOpener::Get()->AddIncludeStatement( yytext );
    wxString filepath;
    if ( fcFileOpener::Get()->getDepth() < fcFileOpener::Get()->getMaxDepth() ) {
        new_file = fcFileOpener::Get()->OpenFile( yytext, filepath );
    }

    if ( ! new_file ) {
        // We got some error
        BEGIN(INITIAL);

    } else {
        // keep the current buffer
        fc_in = new_file;
        fcFileOpener::Get()->PushBufferState( YY_CURRENT_BUFFER, filepath );
        
        yy_switch_to_buffer( yy_create_buffer( new_file, YY_BUF_SIZE ) );
        BEGIN(INITIAL);
    }
}

<cpp_comment>\n {
	BEGIN(INITIAL);
}
<cpp_comment>. {} /* do nothing */

<c_comment>"*/" {
	BEGIN(INITIAL);
}

<c_comment>.  {}

<<EOF>> {
    if ( !fcFileOpener::Get()->IsStateStackEmpty() ) {

        if ( YY_CURRENT_BUFFER->yy_input_file ) {
            fclose( YY_CURRENT_BUFFER->yy_input_file );
            YY_CURRENT_BUFFER->yy_input_file = NULL;
        }

        yy_delete_buffer    ( YY_CURRENT_BUFFER    );
        yy_switch_to_buffer ( fcFileOpener::Get()->PopBufferState() );
        
    } else {

        if ( YY_CURRENT_BUFFER->yy_input_file ) {
            fclose( YY_CURRENT_BUFFER->yy_input_file );
            YY_CURRENT_BUFFER->yy_input_file = NULL;

        }

        yy_delete_buffer    ( YY_CURRENT_BUFFER    );
        yyterminate();
    }
}

%%

int yywrap() {
	return 1;
}

int crawlerScan( const char* filePath )
{
    BEGIN INITIAL;
    fc_lineno = 1;
    
    wxFileName fn(filePath);
    if ( fn.IsRelative() ) {
        fn.MakeAbsolute();
    }
    
    FILE* fp = fopen(fn.GetFullPath().mb_str(wxConvUTF8).data(), "r");
    if ( fp == NULL ) {
        //printf("%s\n", strerror(errno));
        // failed to open input file...
        return -1;
    }
    
    // set the initial search directory
    fcFileOpener::Get()->SetCwd( fn.GetPath() );
    
    yy_switch_to_buffer( yy_create_buffer(fp, YY_BUF_SIZE) );
    fc_in = fp;
    int rc = fc_lex();
    yy_delete_buffer( YY_CURRENT_BUFFER );

    return rc;
}

