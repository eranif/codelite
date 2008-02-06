#include "precompiled_header.h"
#include "makefile_lexer.h"
#include "VariableLexer.h"
#include "MakefileParser.h"
#include "Target.h"
#include "TargetLexer.h"
#include <wx/file.h>

typedef std::map<wxString, wxString> Tokens;
typedef Tokens::iterator ITokens;

void LogMessage(const wxString& msg)
{
	wxCharBuffer buf = msg.ToAscii();
	printf("%s", buf.data());
}

Targets ImportFromMakefile(const wxString &path)
{
	LogMessage(wxT("Path: ") + path + wxT("\n"));

	VariableLexer expander(path.data());
	wxArrayString expanded = expander.getResult();

	MakefileParser parser(expanded);
	TypedStrings parsed = parser.getResult();

	TargetLexer lexer(parsed);
	Targets lexed = lexer.getResult();
	
	return lexed;
}

int main(int argv, char* argc[])
{
	
	wxString path;
	if(argv>1)
		path = _U(argc[1]);
	else
		path = wxT("input");
		
	printf("Fetching result...\n");
		
	// TypedStrings result = ImportFromMakefile(path);
	Targets result = ImportFromMakefile(path);
	
	printf("Done.\n");
	printf("Result:\n");
	
	for(Targets::iterator it = result.begin(); it != result.end(); it++)
	{
		Target token = *it;
		wxString line;
		line = wxT("Target ");
		line << token.getName();
		line << wxT(":\n"); 
		LogMessage(line);
		
		wxArrayString actions = token.getActions();
		for(wxArrayString::iterator act = actions.begin(); act != actions.end(); act++)
			LogMessage(*act + wxT("\n"));
		
		wxArrayString deps = token.getDeps();
		for(wxArrayString::iterator dep = deps.begin(); dep != deps.end(); dep++)
			LogMessage(*dep + wxT("\n"));
		
		printf("\n");
	}
	
	/*
	for(TypedStrings::iterator it = result.begin(); it != result.end(); it++)
	{
		TypedString token = *it;
		wxString msg;
		msg << LINETYPES::toString(token.type);
		msg << wxT(": '");
		msg << token.line;
		msg << wxT("'.\n");
		LogMessage(msg);
	}
	*/
	
	printf("Done.\n");
	printf("Exiting.\n");
	
	return 0;
}
