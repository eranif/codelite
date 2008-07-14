#include <stdio.h>
#include <string>
#include "gdb_result_parser.h"
extern int gdb_result_lex();
extern bool setGdbLexerInput(const std::string &in);
extern void gdb_parse_result(const std::string &in);
char *loadFile(const char *fileName);
extern void gdb_result_lex_clean();
extern std::string gdb_result_string;

int main(int argc, char **argv)
{
	char *data = loadFile("test.txt");
	setGdbLexerInput(data);
	
	int type = gdb_result_lex();
	while(type != 0){
		printf("%d--> %s\n", type, gdb_result_string.c_str());
		type = gdb_result_lex();
	}
	
	gdb_result_lex_clean();
	free(data);
	
	return 0;
}


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

