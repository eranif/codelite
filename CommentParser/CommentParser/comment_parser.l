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
static std::string         strCppComment;
static int                 iCppCommentLine = -1;
%}

%option yylineno
cpp_comment_start [//]{2,}
%%

{cpp_comment_start} {
	if(strCppComment.empty() == false) {
		strCppComment += "\n";
	}
	iCppCommentLine = cp_lineno;
	BEGIN(cpp_comment);
}

"/*" {
	/* if we kept some C++ comment, it is time to flush it to the 
	 * result output */
	if(strCppComment.empty() == false && pResults) {
		pResults->addComment(strCppComment, iCppCommentLine, false);
		strCppComment.clear();
		iCppCommentLine = -1;
	}
	strComment.clear();
	BEGIN(c_comment);
}

"/*!" {
	/* if we kept some C++ comment, it is time to flush it to the 
	 * result output */
	if(strCppComment.empty() == false && pResults) {
		pResults->addComment(strCppComment, iCppCommentLine, false);
		strCppComment.clear();
		iCppCommentLine = -1;
	}
	strComment.clear();
	BEGIN(c_comment);
}

[\t \v] { /* skip whitespaces */}

\n   {
	/* if we kept some C++ comment, it is time to flush it to the 
	 * result output */
	if(strCppComment.empty() == false && pResults) {
		pResults->addComment(strCppComment, iCppCommentLine, false);
		strCppComment.clear();
		iCppCommentLine = -1;
	}
}

.    {
	/* if we kept some C++ comment, it is time to flush it to the 
	 * result output */
	if(strCppComment.empty() == false && pResults) {
		pResults->addComment(strCppComment, iCppCommentLine, false);
		strCppComment.clear();
		iCppCommentLine = -1;
	}
}

<cpp_comment>\n {
	BEGIN(INITIAL);
}
<cpp_comment>. {strCppComment += yytext;} /* do nothing */

<c_comment>"*/" {
	pResults->addComment(strComment, cp_lineno, false);
	BEGIN(INITIAL);
}

<c_comment>\r {}
<c_comment>\v {}
<c_comment>[\t ] { 
	if(strComment.empty() == false) {
		if(strComment.at(strComment.length() -1) == '\n'){
			// dont add indentation tabs
		} else {
			strComment += " ";
		}
	} else {
		// empty comment: dont prepend whitespaces at start
	}
}
<c_comment>"*" {
	if(strComment.empty() == false) {
		if(strComment.at(strComment.length() -1) == '\n'){
			// dont add indentation tabs
		} else {
			strComment += "*";
		}
	} else {
		// empty comment: dont prepend '*'
	}
}

<c_comment>\n { strComment += "\n";}
<c_comment>.  { strComment += yytext;}

<<EOF>> {
	if(strCppComment.empty() == false && pResults) {
		pResults->addComment(strCppComment, iCppCommentLine, false);
		strCppComment.clear();
		iCppCommentLine = -1;
	}
	
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
	strComment.clear();
	strCppComment.clear();
	iCppCommentLine = -1;
	
	yy_switch_to_buffer( yy_create_buffer(fp, YY_BUF_SIZE) );
	cp_in = fp;
	int rc = cp_lex();
	yy_delete_buffer    ( YY_CURRENT_BUFFER    );

	// Cleanup
	pResults = NULL;
	strComment.clear();
	strCppComment.clear();
	iCppCommentLine = -1;
	return rc;
}

