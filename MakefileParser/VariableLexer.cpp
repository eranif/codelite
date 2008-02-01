#include "precompiled_header.h"
#include "makefile_lexer.h"
#include "VariableLexer.h"
#include <string>
#include <vector>
#include <map>
#include <wx/log.h>
#include <wx/string.h>

#define YYSTYPE std::string

extern void initLexer(const char *filename);
extern int yyparse();

typedef std::vector<std::string> Strings;
typedef Strings::iterator IStrings;
typedef std::map<std::string, std::string> Tokens;
typedef Tokens::iterator ITokens;

Strings TheOutput;
Strings TheUnmatched;
Strings TheError;
Tokens TheTokens;

VariableLexer::VariableLexer(const wxString& path) 
{
	const wxCharBuffer pathBuffer = path.ToAscii();
	const char *cstr_path = pathBuffer.data();
	initLexer(cstr_path);
	initTokens();
	yyparse();
	
	for(IStrings it = TheOutput.begin(); it != TheOutput.end(); it++)
		m_output.Add(_U(it->c_str()));

	for(IStrings it = TheUnmatched.begin(); it != TheUnmatched.end(); it++)
		m_unmatched.Add(_U(it->c_str()));

	for(IStrings it = TheError.begin(); it != TheError.end(); it++)
		m_error.Add(_U(it->c_str()));
		
	for(ITokens it = TheTokens.begin(); it != TheTokens.end(); it++)
		m_tokens[_U(it->first.c_str())] = _U(it->second.c_str());
}

/**
 * Get result, constructor ensures that m_result is available when we get to this point.
 */ 
const wxArrayString& VariableLexer::getResult()
{
	return m_output;
}
		
const wxArrayString& VariableLexer::getUnmatched()
{
	return m_unmatched;
}

const wxArrayString& VariableLexer::getError()
{
	return m_error;
}

const std::map<wxString,wxString>& VariableLexer::getTokens()
{
	return m_tokens;
}

void VariableLexer::initTokens()
{
	TheTokens["RM"] = "rm";
	TheTokens["MAKE"] = "make";
}
