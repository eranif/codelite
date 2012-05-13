#include <wx/init.h>
#include <wx/string.h>
#include <clang-c/Index.h>
#include "clang.h"

void usage(const char* program)
{
    printf("Usage: %s {parse|write-pch|reparse|print-macros} <file> [output-folder] <compiler options>\n", program);
    printf("    parse <file> and compile it into an AST\n");
    printf("    write-pch <header-file> <output-folder> <compiler-options> compile <header-file> and store it as PCH for boosting compilation\n");
    printf("    print-macros <source-file> <AST-file> load the AST and print all macros into stdout\n");
	printf("    code-complete <source-file> <AST-file> <line:col>\n");
    exit (0);
}

int main( int argc, char** argv )
{
    wxInitialize(argc, argv);
    if(argc < 3) {
        usage(argv[0]);
    }
    argv++; // Skip the program name
    
    const char* command = argv[0];
    argv++;
    
    const char* file    = argv[0];
    argv++;
    
    Clang clang(file, command, argc - 3, argv);
    if(!clang.IsOK()) {
        usage(argv[0]);
    }
    return clang.Run();
}
