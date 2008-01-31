#include "precompiled_header.h"
#include "makefile_lexer.h"
#include "VariableLexer.h"
#include "stdio.h"

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
	
	FILE *of = fopen("outout.txt", "w+");
	fprintf(of, "============= RESULT =============\n");
	for(int i = 0; i < result.size(); i++)
	{
		fprintf(of, "%s\n", result[i].c_str());
	}

	fprintf(of, "============ UNMATCHED ===========\n");
	for(int i = 0; i < unmatched.size(); i++)
	{
		fprintf(of, "%s\n", unmatched[i].c_str());
	}

	fprintf(of, "============== ERROR =============\n");
	for(int i = 0; i < error.size(); i++)
	{
		fprintf(of, "%s\n", error[i].c_str());
	}

	fprintf(of, "============= TOKENS =============\n");
        for(ITokens it = tokens.begin(); it != tokens.end(); it++)
        {
         	fprintf(of, "'%s'='%s'\n", it->first.c_str(), it->second.c_str());
        }

	fprintf(of, "=============== DONE =============\n");
	fclose(of);
	return 0;
}
