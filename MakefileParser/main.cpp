#include "precompiled_header.h"
#include "makefile_lexer.h"
#include "VariableLexer.h"
#include <wx/file.h>

typedef std::map<wxString, wxString> Tokens;
typedef Tokens::iterator ITokens;

int main(int argv, char* argc[])
{
	
	wxString path;
	if(argv>1)
		path = _U(argc[1]);
	else
		path = wxT("input");
		
	printf("Creating lexer...\t");
	
	VariableLexer lexer(path);
	
	printf("Done.\n");
	printf("Retreiving result...\t");
	
	wxArrayString result = lexer.getResult();
	wxArrayString unmatched = lexer.getUnmatched();
	wxArrayString error = lexer.getError();
	Tokens tokens = lexer.getTokens();
	
	printf("Done.\n");
	printf("Opening output file...\t");
	
	wxFile of(wxT("outout.txt"), wxFile::write);
	
	printf("Done.\n");
	printf("Writing output...\t");
	
	of.Write(wxT("============= RESULT =============\n"));
	for(wxArrayString::iterator it = result.begin(); it != result.end(); it++)
		of.Write(*it + wxT("\n"));
	
	of.Write(wxT("============ UNMATCHED ===========\n"));
	for(wxArrayString::iterator it = unmatched.begin(); it != unmatched.end(); it++)
		of.Write(*it + wxT("\n"));

	of.Write(wxT("============== ERROR =============\n"));
	for(wxArrayString::iterator it = error.begin(); it != error.end(); it++)
		of.Write(*it + wxT("\n"));

	of.Write(wxT("============= TOKENS =============\n"));
	for(ITokens it = tokens.begin(); it != tokens.end(); it++)
	{
		wxString line;
		line << wxT("'") << it->first << wxT("' = '") << it->second << wxT("'\n");
		of.Write(line);
	}

	of.Write(wxT("============= DONE =============\n"));
	
	printf("Done.\n");
	printf("Closing output file...\t");
	
	of.Close();
	
	printf("Done.\n");
	printf("Exiting.\n");
	
	return 0;
}
