#include "cppwordscanner.h"
#include <stdio.h>
#include <errno.h>

#include "cppwordscanner.h"

char *loadFile(const char *fileName);

int main(int argc, char **argv)
{
	CppTokenList l;
	char *data = loadFile("/home/eran/devl/codelite/trunk/sqlite3/sqlite3.c");
	
	CppWordScanner scanner( data ) ;
	scanner.parse(l);
	
	CppTokenList::iterator iter = l.begin();
//	for(; iter != l.end(); iter++){
//		(*iter).print();
//	}
//	printf("Word count: %d\n", l.size());
	free(data);
	return 0;
}

//-------------------------------------------------------
// Help function
//-------------------------------------------------------
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
