%{
/* Included code before lex code */
#include "Markup.h" // Macros
#include <wx/string.h>

#define YYSTYPE wxString
static YYSTYPE currentText;

bool setMarkupLexerInput(const wxString &in);

void MarkupClean();

#define RETURN_VAL(x) {\
					currentText = wxString(yytext, wxConvUTF8);\
					return(x);}
%}

word [a-zA-Z_ ]
color_name ['"]{word}+['"]

%%

"<hr>"                   {RETURN_VAL(HORIZONTAL_LINE);}
"<b>"                    {RETURN_VAL(BOLD_START);}
"</b>"                   {RETURN_VAL(BOLD_END);}
"<i>"                    {RETURN_VAL(ITALIC_START);}
"</i>"                   {RETURN_VAL(ITALIC_END);}
"<code>"                 {RETURN_VAL(CODE_START);}
"</code>"                {RETURN_VAL(CODE_END);}
"<color="{color_name}">" {RETURN_VAL(COLOR_START);}
"</color>"               {RETURN_VAL(COLOR_END);}

\n     {RETURN_VAL(NEW_LINE);}
[\r\v] {}
.      {RETURN_VAL((int)*yytext);}
%%

/*******************************************************************/
void MarkupClean()
{
    yy_flush_buffer(YY_CURRENT_BUFFER);
    yy_delete_buffer(YY_CURRENT_BUFFER);
}

bool setMarkupLexerInput(const wxString &in) {
    BEGIN INITIAL;
    yy_scan_string(in.mb_str(wxConvUTF8).data());
    return true;
}

int yywrap(){
    return 1;
}

void yyerror(const char* msg)
{
}

int MarkupLex()
{
    return Markup_lex();
}

wxString MarkupText() {
    return currentText;
}
