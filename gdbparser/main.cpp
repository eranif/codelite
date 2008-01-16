#include "stdio.h"
#include "string"
#include "gdblexer.h"

char *loadFile(const char *fileName);
int main(int argc, char **argv){
	
	char *buff = loadFile("test.h");
	le_gdb_set_input(buff);
	int type = le_gdb_lex();
	while(type != 0){
		if(type == LE_GDB_STRING_LITERAL){
			printf("[%d] %s\n", type, le_gdb_string_word.c_str());
		}else{
			printf("[%d] %s\n", type, le_gdb_text.c_str());
		}
		type = le_gdb_lex(); 
	}
	fflush(stdout);
	free( buff );
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
	if(!fp)
	{
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
	if(bytes != len)
	{
		fclose(fp);
		printf("failed to read from file 'test.h': %s\n", strerror(errno));
		return NULL;
	}
	
	buf[len] = 0;	// make it null terminated string
	fclose(fp);
	return buf;
}
