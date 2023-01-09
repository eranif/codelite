#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clang_output_parser_api.h"

char *loadFile(const char *fileName)
{
	FILE *fp;
	long len;
	char *buf = NULL;

	fp = fopen(fileName, "rb");
	if (!fp) {
		printf("failed to open file 'test.txt': %s\n", strerror(errno));
		return NULL;
	}

	//read the whole file
	fseek(fp, 0, SEEK_END);      //go to end
	len = ftell(fp);             //get position at end (length)
	fseek(fp, 0, SEEK_SET);      //go to beginning
	buf = (char *)malloc(len+1); //malloc buffer

	//read into buffer
	long bytes = fread(buf, sizeof(char), len, fp);
	printf("read: %ld\n", bytes);
	if (bytes != len) {
		fclose(fp);
		printf("failed to read from file '%s': %s\n", fileName, strerror(errno));
		free(buf);
		return NULL;
	}

	buf[len] = 0; // make it null terminated string
	fclose(fp);
	return buf;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	std::string str;
	char * content = loadFile("../input-file.txt");
	if(content) {
		str = content;
		free(content);
	}
	clang_parse_string(str);
	
	const ClangEntryVector &res = clang_results();
	for(size_t i=0; i<res.size(); i++) {
		res[i].print();
	}
	return 0;
}
