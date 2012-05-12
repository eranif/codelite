#ifndef CLANGUTILS_H
#define CLANGUTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <clang-c/Index.h>

class ClangUtils 
{
public:
    static void printDiagnosticsToLog(CXTranslationUnit& TU);
};

#endif // CLANGUTILS_H
