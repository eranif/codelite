#include "makefile_lexer.h"
#include "precompiled_header.h"
#include <wx/wx.h>

#include "VariableLexer.h"

typedef std::map<wxString, wxString> Tokens;
typedef Tokens::iterator ITokens;

int main(int argv, char* argc[])
{
	
	wxString path;
	if(argv>1)
		path = _U(argc[1]);
	else
		path = wxT("input");

	VariableLexer lexer(path);
	wxArrayString result = lexer.getResult();
	wxArrayString unmatched = lexer.getUnmatched();
	wxArrayString error = lexer.getError();
	Tokens tokens = lexer.getTokens();

	printf("============= RESULT =============\n");
	for(int i = 0; i < result.size(); i++)
	{
		printf("%s\n", result[i].c_str());
	}

	printf("============ UNMATCHED ===========\n");
	for(int i = 0; i < unmatched.size(); i++)
	{
		printf("%s\n", unmatched[i].c_str());
	}

	printf("============== ERROR =============\n");
	for(int i = 0; i < error.size(); i++)
	{
		printf("%s\n", error[i].c_str());
	}

	printf("============= TOKENS =============\n");
        for(ITokens it = tokens.begin(); it != tokens.end(); it++)
        {
         	printf("'%s'='%s'\n", it->first.c_str(), it->second.c_str());
        }

	printf("=============== DONE =============\n");
	return 0;
}
