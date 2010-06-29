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
#include "comment_parser.h"

static CommentParseResult *pResults = NULL;
static std::string         strComment;

%}

%option yylineno

%%

"//" {
	strComment.clear();
	strComment += "//";
	BEGIN(cpp_comment);
}

"/*" {
	strComment.clear();
	strComment += "/*";
	BEGIN(c_comment);
}

<cpp_comment>\n {
	pResults->addComment(strComment, cp_lineno - 1, true);
	BEGIN(INITIAL);
}
<cpp_comment>. {strComment += yytext;} /* do nothing */

<c_comment>"*/" {
	strComment += "*/";
	pResults->addComment(strComment, cp_lineno, false);
	BEGIN(INITIAL);
}

<c_comment>\r {}
<c_comment>\v {}
<c_comment>\t { strComment += " ";}
<c_comment>\n { strComment += "\n";}
<c_comment>.  { strComment += yytext;}

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

int ParseComments(const char* filePath, CommentParseResult &comments)
{
	BEGIN INITIAL;
	cp_lineno = 1;

	FILE* fp = fopen(filePath, "r");
	if ( fp == NULL ) {
		return -1;
	}

	pResults    = &comments;
	yy_switch_to_buffer( yy_create_buffer(fp, YY_BUF_SIZE) );
	cp_in = fp;
	int rc = cp_lex();
	yy_delete_buffer    ( YY_CURRENT_BUFFER    );

	// Cleanup
	pResults = NULL;
	return rc;
}

