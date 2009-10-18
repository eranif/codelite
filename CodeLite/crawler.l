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
#include "fc_fileopener.h"
std::list<YY_BUFFER_STATE> include_stack;
%}

%%

"//" {
	BEGIN(cpp_comment);
}

"/*" {
	BEGIN(c_comment);
}

"L"?[']{c_char}+[']     {/* eat a string */}
"L"?["]{s_char}*["]     {/* eat a string */}
include                 {BEGIN(incl);}
.                       {}
<incl>\n                {BEGIN(INITIAL);}
<incl>[ \t]*            {}      /* eat the whitespace        */
<incl>["<][^ \t\n]+[">] { /* got the include file name */

	// Open the new file
	FILE * new_file(NULL);
	if ( fcFileOpener::Instance()->getDepth() < fcFileOpener::Instance()->getMaxDepth() ) {
		new_file = fcFileOpener::Instance()->OpenFile(yytext);
	}

	if ( ! new_file ) {
		// We got some error
		BEGIN(INITIAL);

	} else {
		// keep the current buffer
		fc_in = new_file;
		include_stack.push_back(YY_CURRENT_BUFFER);
		yy_switch_to_buffer( yy_create_buffer( new_file, YY_BUF_SIZE ) );
		BEGIN(INITIAL);

		// since we are moving into another file, increase the current
		// depth by 1
		fcFileOpener::Instance()->incDepth();
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
	if ( include_stack.empty() == false ) {

		if ( YY_CURRENT_BUFFER->yy_input_file ) {
			fclose( YY_CURRENT_BUFFER->yy_input_file );
			YY_CURRENT_BUFFER->yy_input_file = NULL;
		}

		yy_delete_buffer    ( YY_CURRENT_BUFFER    );
		yy_switch_to_buffer ( include_stack.back() );
		include_stack.pop_back();

		// reduce the current depth
		fcFileOpener::Instance()->decDepth();

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
	fcFileOpener::Instance()->ClearResults();
	FILE* fp = fopen(filePath, "r");
	if ( fp == NULL ) {
		//printf("%s\n", strerror(errno));
		// failed to open input file...
		return -1;
	}
	yy_switch_to_buffer( yy_create_buffer(fp, YY_BUF_SIZE) );
	fc_in = fp;
	int rc = fc_lex();
	yy_delete_buffer    ( YY_CURRENT_BUFFER    );

	return rc;
}
