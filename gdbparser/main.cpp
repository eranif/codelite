#include "stdio.h"
#include "string"
#include "gdblexer.h"

char *loadFile(const char *fileName);
extern void le_gdb_push_buffer(const std::string &new_input);
extern void le_gdb_pop_buffer();
	
int main(int argc, char **argv){
	
	char *buff = loadFile("test.h");
	char *buff2 = loadFile("test2.h");
	le_gdb_set_input(buff);
	int type = le_gdb_lex();
	while(type != 0){
		if(type == LE_GDB_STRING_LITERAL){
			printf("String [%d] %s\n", type, le_gdb_string_word.c_str());
			le_gdb_push_buffer(buff2);
			
		}else{
			printf("Text   [%d] %s\n", type, le_gdb_text);
		}
		type = le_gdb_lex(); 
	}
	
	// set back old buffer
	le_gdb_pop_buffer();
	type = le_gdb_lex();
	while(type != 0){
		if(type == LE_GDB_STRING_LITERAL){
			printf("String [%d] %s\n", type, le_gdb_string_word.c_str());
		}else{
			printf("Text   [%d] %s\n", type, le_gdb_text);
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
