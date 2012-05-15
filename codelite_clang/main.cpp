#include <wx/init.h>
#include <wx/string.h>
#include <clang-c/Index.h>
#include "clang.h"

#ifdef _WIN32
#    include <Windows.h>
#endif

void usage(const char* program)
{
    printf("Usage: %s {parse|parse-macros|code-complete} <file> [output-folder] <compiler options>\n", program);
    printf("    parse <source-file> <output-folder> <compiler-agruments> and compile it into an AST\n");
    printf("    parse-macros <source-file> <output-folder> <compiler-arguments> parse if needed and print all macros found into stdout\n");
	printf("    code-complete <source-file> <AST-file> <line:col>\n");
    exit (0);
}

int main( int argc, char** argv )
{
#ifdef _WIN32
	// No windows crash dialogs
	SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOOPENFILEERRORBOX);
#endif

    wxInitialize();
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
