#include "makefile_lexer.h"
#include "VariableLexer.h"
#include <string>
#include <vector>
#include <map>
//#include <wx/msgdlg.h> // wxMessageBox
#include <wx/log.h>
#include <wx/string.h>

void initLexer(const char *filename);

#define YYSTYPE wxString

YYSTYPE yyparse();

typedef wxArrayString Strings;
typedef std::map<wxString, wxString> Tokens;
typedef Tokens::iterator Itokens;

Strings TheOutput;
Strings TheUnmatched;
Strings TheError;
Tokens TheTokens;

void VariableLexer::DebugMessage(const wxString& msg)
{
	wxUnusedVar(msg);
} 

VariableLexer::VariableLexer(wxString path) 
{
	initLexer(path.mb_str());
	yyparse();

	m_output = TheOutput;
	m_unmatched = TheUnmatched;
	m_error = TheError;
	m_tokens = TheTokens;
}

/**
 * Get result, constructor ensures that m_result is available when we get to this point.
 */ 
wxArrayString VariableLexer::getResult()
{
	return m_output;
}
		
