/**
 * the "incl" state is used for picking up the name
 * of an include file
 */
%x incl
%x c_comment
%x cpp_comment

simple_escape [abfnrtv'"?\\]
octal_escape  [0-7]{1,3}
hex_escape "x"[0-9a-fA-F]+

escape_sequence [\\]({simple_escape}|{octal_escape}|{hex_escape})
c_char [^'\\\n]|{escape_sequence}
s_char [^"\\\n]|{escape_sequence}


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
#include "Cxx/include_finder.h"

static std::vector<IncludeStatement>* pIncludes = NULL;
static std::string currentFile;

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
#[ \t]*include          {BEGIN(incl);}
.                       {}
<incl>\n                {BEGIN(INITIAL);}
<incl>[ \t]*            {}      /* eat the whitespace        */
<incl>["<][^ \t\n]+[">] { /* got the include file name */

	std::string mod_path ( inclf_text );
	static std::string trimString("\"<> \t");

	mod_path.erase(0, mod_path.find_first_not_of(trimString));
	mod_path.erase(mod_path.find_last_not_of    (trimString)+1);

	IncludeStatement includeStatement;
	includeStatement.line         = inclf_lineno;
	includeStatement.file         = mod_path;
	includeStatement.includedFrom = currentFile;
	includeStatement.pattern      = inclf_text;
	pIncludes->push_back( includeStatement );
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
	if ( YY_CURRENT_BUFFER->yy_input_file ) {
		fclose( YY_CURRENT_BUFFER->yy_input_file );
		YY_CURRENT_BUFFER->yy_input_file = NULL;
	}

	yy_delete_buffer    ( YY_CURRENT_BUFFER    );
	yyterminate();
}

%%

int yywrap() {
	return 1;
}

int IncludeFinder( const char* filePath, std::vector<IncludeStatement> &includes )
{
	BEGIN INITIAL;
	inclf_lineno = 1;

	FILE* fp = fopen(filePath, "r");
	if ( fp == NULL ) {
		return -1;
	}

	currentFile = filePath;
	pIncludes   = &includes;
	yy_switch_to_buffer( yy_create_buffer(fp, YY_BUF_SIZE) );
	inclf_in = fp;
	int rc = inclf_lex();
	yy_delete_buffer    ( YY_CURRENT_BUFFER    );

	// Cleanup
	pIncludes = NULL;
	currentFile.clear();
	return rc;
}

