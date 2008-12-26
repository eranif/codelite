#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#ifdef _WIN32
#include "io.h"
#endif

static const char* REVISION_PATTERN = "revision=\"";

int main(int argc, char **argv){

	if(argc < 2){
		printf("usage: autorev <directory>\n");
		return -1;
	}

	//execute the svn executable
	chdir(argv[1]);
	FILE *fp = popen("svn info --xml", "r");
	if(!fp){
		printf("failed to execute svn\n");
	}

	char line[256];
	char* posRevision = NULL;
	while(fgets(line, sizeof(line), fp) || posRevision){
		posRevision = strstr(line, REVISION_PATTERN);
		if(posRevision){
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

	fprintf(of, "#include <wx/string.h>\n");
	fprintf(of, "const wxChar* SvnRevision = wxT(\"");
	if(posRevision){
		//we got the revision number
		posRevision += strlen(REVISION_PATTERN);
		char * posRevisionEnd = strstr(posRevision, "\"");
		*posRevisionEnd = 0;
		fputs(posRevision, of);
	}
	fprintf(of, "\");\n");
	pclose(fp);
	fclose(of);
	return 0;
}
