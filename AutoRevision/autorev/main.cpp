#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#ifdef _WIN32
#include "io.h"
#endif
#include <string>


int main(int argc, char **argv){
	
	if(argc < 2){
		printf("usage: autorev <directory>\n");
		return -1;
	}
	
	//execute the svn executable
	chdir(argv[1]);
	FILE *fp = popen("svn info", "r");
	if(!fp){
		printf("failed to execute svn\n");
	}
	
	char line[256];
	bool found(false);
	while(fgets(line, sizeof(line), fp)){
		if(strstr(line, "Revision: ")){
			found = true;
			break;
		}
		memset(line, 0, sizeof(line));
	}
	
	//create the output file
	FILE *of = fopen("svninfo.cpp", "w+");
	if(!of){
		perror("failed to create output file");
		return -1;
	}
	
	fprintf(of, "#define SvnRevision \"");
	if(found){
		//we got the revision number
		int counter(0);
		char *tok = strtok(line, ":");
		while(tok){
			if(counter == 1){
				std::string token = tok;
				static std::string trimString(" \r\n\t\v");
				token.erase(0, token.find_first_not_of(trimString)); 
				token.erase(token.find_last_not_of(trimString)+1);
				fprintf(of, "%s", token.c_str());
				break;
			}else{
				tok = strtok(NULL, ":");
			}
			counter++;
		}
	}
	fprintf(of, "\"\n");
	pclose(fp);
	fclose(of);
	return 0;
}
