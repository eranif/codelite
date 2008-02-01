#include "precompiled_header.h"
#include "makefile_lexer.h"
#include "VariableLexer.h"
#include "stdio.h"
#include "errno.h"

typedef std::map<wxString, wxString> Tokens;
typedef Tokens::iterator ITokens;

char *loadFile(const char *fileName)
{
	FILE *fp;
	long len;
	char *buf = NULL;

	fp = fopen(fileName, "rb");
	if (!fp) {
		printf("failed to open file 'test.h': %s\n", strerror(errno));
		return NULL;
	}

	//read the whole file
	fseek(fp, 0, SEEK_END); 		//go to end
	len = ftell(fp); 					//get position at end (length)
	fseek(fp, 0, SEEK_SET); 		//go to begining
	buf = (char *)malloc(len+1); 	//malloc buffer

	//read into buffer
	long bytes = fread(buf, sizeof(char), len, fp);
	printf("read: %ld\n", bytes);
	if (bytes != len) {
		fclose(fp);
		printf("failed to read from file 'test.h': %s\n", strerror(errno));
		return NULL;
	}

	buf[len] = 0;	// make it null terminated string
	fclose(fp);
	return buf;
}

int main(int argv, char* argc[])
{
	char* path(NULL);
	if(argv>1)
		path = argc[1];
	else
		path = "input";
	
	char *data = loadFile(path);
	VariableLexer lexer(data);
	
	//release the input data
	free(data);
	
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
