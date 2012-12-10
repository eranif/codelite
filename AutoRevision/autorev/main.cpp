#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#ifdef _WIN32
#include "io.h"
#endif
#include <string>

static const char* REVISION_PATTERN = "revision=\"";

int main(int argc, char **argv)
{

    if(argc < 2) {
        printf("usage: autorev <directory> [output-file] [variable-name]\n");
        return -1;
    }
    
    std::string output_file = "svninfo.cpp";
    std::string variable_name = "SvnRevision";
    
    if ( argc > 2 ) {
        output_file = argv[2];
    }

    if ( argc > 3 ) {
        variable_name = argv[3];
    }
    
    //execute the svn executable
    chdir(argv[1]);
    FILE *fp = popen("svn info --xml", "r");
    if(!fp) {
        printf("failed to execute svn\n");
    }

    char line[256];
    char* posRevision = NULL;
    while(fgets(line, sizeof(line), fp) || posRevision) {
        posRevision = strstr(line, REVISION_PATTERN);
        if(posRevision) {
            break;
        }
        memset(line, 0, sizeof(line));
    }

    //create the output file
    
    FILE *of = ::fopen(output_file.c_str(), "w+");
    if(!of) {
        perror("failed to create output file");
        return -1;
    }
    
    printf("Creating file :%s\n", output_file.c_str());
    printf("Variable      :%s\n", variable_name.c_str());
    
    fprintf(of, "#include <wx/string.h>\n");
    fprintf(of, "const wxChar* %s = wxT(\"", variable_name.c_str());
    
    if(posRevision) {
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
