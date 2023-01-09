#include <clang-c/Index.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string>
#include <set>
#include <vector>

struct VisitorClientData {
    CXCursor cursor;
};

#ifdef WIN32
const char *cmdLineArgs[] = {
    "-Ic:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include\\c++",
    "-Ic:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include\\c++\\mingw32",
    "-Ic:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include\\c++\\backward",
    "-Ic:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include",
    "-Ic:\\mingw-4.6.1\\include",
    "-Ic:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include-fixed",
    "-DHAVE_W32API_H",
    "-D__WXMSW__",
    "-DNDEBUG",
    "-D_UNICODE",
    "-IC:\\wxWidgets-2.9.4\\lib\\gcc_dll\\mswu",
    "-IC:\\wxWidgets-2.9.4\\include",
    "-DWXUSINGDLL",
    "-Wno-ctor-dtor-privacy",
    "-Wall",
    "-D__WX__"
};
const int numArgs = 16;
#else
char **cmdLineArgs;
int numArgs = 19;
#endif

bool isMethod(CXTranslationUnit unit, const std::string &filename, int line, int col, CXCursor &funcCursor);
CXCursor getFunctionReturnType(CXCursor funcCursor);
void listChildren(CXCursor cursor);

char *loadFile(const char *fileName)
{
    //std::set<std::string> myset;
    //myset.find("eran").
    FILE *fp;
    long len;
    char *buf = NULL;

    fp = fopen(fileName, "rb");
    if (!fp) {
        return NULL;
    }

    //read the whole file
    fseek(fp, 0, SEEK_END);      // go to end
    len = ftell(fp);             // get position at end (length)
    fseek(fp, 0, SEEK_SET);      // go to beginning
    buf = (char *)malloc(len+1); // malloc buffer

    //read into buffer
    long bytes = fread(buf, sizeof(char), len, fp);
    if (bytes != len) {
        fclose(fp);
        return NULL;
    }

    buf[len] = 0;
    fclose(fp);
    return buf;
}

int parseFile(const std::string& filename, const std::string &modifiedBuffer, CXIndex index, CXTranslationUnit *unit)
{
    CXUnsavedFile file = {filename.c_str(), modifiedBuffer.c_str(), modifiedBuffer.length()};
    if(!*unit) {
        *unit = clang_parseTranslationUnit(index,
                                           filename.c_str(),
                                           cmdLineArgs,
                                           numArgs,
                                           NULL,
                                           0,
                                           CXTranslationUnit_CXXPrecompiledPreamble
                                           | CXTranslationUnit_CacheCompletionResults
                                           | CXTranslationUnit_CXXChainedPCH
                                           | CXTranslationUnit_PrecompiledPreamble
                                           | CXTranslationUnit_Incomplete);
    }

    if(!*unit) {
        printf("clang_parseTranslationUnit error!\n");
        return -1;
    }

    // Report diagnostics to the log file
//	const unsigned diagCount = clang_getNumDiagnostics(*unit);
//	for(unsigned i=0; i<diagCount; i++) {
//		CXDiagnostic diag = clang_getDiagnostic(*unit, i);
//		CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diag);
//		CXString diagStr  = clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions());
//		printf("%s\n", clang_getCString(diagStr));
//		clang_disposeString(diagStr);
//		clang_disposeDiagnostic(diag);
//	}

    int status = clang_reparseTranslationUnit(*unit, 0, NULL, clang_defaultReparseOptions(*unit));
    if(status != 0) {
        printf("clang_reparseTranslationUnit error!\n");
        return -1;
    }
    return 0;
}

struct INFO {

};

#define PRINT_MACRO_NAME(k) printf("%s\n", #k)
static int displayCursorInfo(CXCursor Cursor)
{
    //if(Cursor.kind == CXCursor_MacroDefinition ) {
    if(Cursor.kind == CXCursor_MacroDefinition)
        printf("CXCursor_MacroDefinition\n");

    if(Cursor.kind == CXCursor_MacroExpansion)
        printf("CXCursor_MacroExpansion\n");

    if(Cursor.kind == CXCursor_MacroInstantiation)
        printf("CXCursor_MacroInstantiation\n");

    if(Cursor.kind == CXCursor_CompoundStmt)
        printf("CXCursor_CompoundStmt\n");

    if(Cursor.kind == CXCursor_FunctionDecl)
        printf("CXCursor_FunctionDecl\n");

    if(Cursor.kind == CXCursor_CXXMethod)
        printf("CXCursor_CXXMethod\n");

    CXString String = clang_getCursorDisplayName(Cursor);
    printf("Display: [%s]\n", clang_getCString(String));
    clang_disposeString(String);

    clang_getCursorExtent(Cursor);
    CXSourceLocation loc = clang_getCursorLocation(Cursor);

    CXFile file;
    unsigned line, col, off;
    clang_getSpellingLocation(loc, &file, &line, &col, &off);

    CXString strFileName = clang_getFileName(file);
    printf("Location: %s, %u:%u:%u\n", clang_getCString(strFileName), line, col, off);
    clang_disposeString(strFileName);
    //}
    return 0;
}

enum CXChildVisitResult VisitorCallback(CXCursor Cursor,
                                        CXCursor Parent,
                                        CXClientData ClientData)
{
    displayCursorInfo(Cursor);

    if(Cursor.kind == CXCursor_TypeRef) {
        VisitorClientData *cd = (VisitorClientData*) ClientData;
        if( cd ) {
            cd->cursor = Cursor;
        }
        return CXChildVisit_Break;
    }

//	displayCursorInfo(Parent);
//	if(Cursor.kind == CXCursor_MacroDefinition ) {
//		CXTranslationUnit TU = (CXTranslationUnit)ClientData;
//		CXSourceRange range = clang_getCursorExtent(Cursor);
//
//		unsigned  num_tokens;
//		CXToken*  tokens;
//		CXCursor* cursors = 0;
//		clang_tokenize(TU, range, &tokens, &num_tokens);
//
//		cursors = (CXCursor *)malloc(num_tokens * sizeof(CXCursor));
//		clang_annotateTokens(TU, tokens, num_tokens, cursors);
//
//		for (unsigned i = 0; i != num_tokens; ++i) {
//			CXString str = clang_getTokenSpelling(TU, tokens[i]);
//			printf("%s\n", clang_getCString(str));
//			clang_disposeString(str);
//		}
//
//		free(cursors);
//		clang_disposeTokens(TU, tokens, num_tokens);
//	} else if(Cursor.kind == CXCursor_FunctionDecl) {
//		CXTranslationUnit TU = (CXTranslationUnit)ClientData;
//		CXSourceRange range = clang_getCursorExtent(Cursor);
//
//		unsigned  num_tokens;
//		CXToken*  tokens;
//		CXCursor* cursors = 0;
//		clang_tokenize(TU, range, &tokens, &num_tokens);
//
//		cursors = (CXCursor *)malloc(num_tokens * sizeof(CXCursor));
//		clang_annotateTokens(TU, tokens, num_tokens, cursors);
//
//		for (unsigned i = 0; i != num_tokens; ++i) {
//			CXString str = clang_getTokenSpelling(TU, tokens[i]);
//			printf("%s\n", clang_getCString(str));
//			clang_disposeString(str);
//		}
//
//		printf("===================\n");
//
//		free(cursors);
//		clang_disposeTokens(TU, tokens, num_tokens);
//	}
    return CXChildVisit_Recurse;
}

void findFunction()
{
    char *partBuffer      = loadFile("../test-sources/file-buffer.cpp");
    std::string strBuffer = partBuffer;
    free(partBuffer);

    std::string filename  = "../test-sources/file.cpp";
    CXUnsavedFile file = {filename.c_str(), strBuffer.c_str(), strBuffer.length()};
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit unit = clang_parseTranslationUnit(index,
                             "../test-sources/file.cpp",
                             cmdLineArgs,
                             numArgs,
                             NULL,
                             0,
                             CXTranslationUnit_PrecompiledPreamble
                             | CXTranslationUnit_CacheCompletionResults
                             | CXTranslationUnit_CXXPrecompiledPreamble
                             | CXTranslationUnit_CXXChainedPCH
                                                       );

    clang_reparseTranslationUnit(unit, 1, &file, clang_defaultReparseOptions(unit));

    CXCursorVisitor visitor = VisitorCallback;
    //clang_visitChildren(clang_getTranslationUnitCursor(unit), visitor, (CXClientData)unit);


    CXFile file1;
    CXCursor cursor;
    CXSourceLocation loc;
    file1  = clang_getFile    (unit, "../test-sources/file.cpp");
    loc    = clang_getLocation(unit, file1, 5, 7);
    cursor = clang_getCursor  (unit, loc);

    displayCursorInfo(cursor);

}

int main(int argc, char **argv)
{
    std::vector<std::string> Lines;
    FILE* fp = fopen("clang-options", "rb");
    char line[1024];
    while ( true ) {
        memset(line, 0, sizeof(line));
        if(fgets(line, sizeof(line), fp)) {
            std::string strLine = line;
            strLine.erase(strLine.find_first_of('\n'));
            Lines.push_back(strLine);

        } else {
            break;
        }
    }

    cmdLineArgs = new char*[Lines.size()];
    for(size_t i=0; i<Lines.size(); i++) {
        cmdLineArgs[i] = strdup(Lines.at(i).c_str());
    }
    numArgs = Lines.size();

    char *partBuffer      = loadFile("../test-sources/file-buffer.cpp");

    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit unit = 0;

    {
        std::string filename  = "../test-sources/file.cpp";
        parseFile(filename, partBuffer, index, &unit);
        free(partBuffer);
    }
    
    //for(size_t i=0; i<10; i++) {
    if( true ) {

        for(size_t i=0; i<100; i++) {
            CXTranslationUnit UNIT = unit;
            clang_reparseTranslationUnit(UNIT, 0, NULL, clang_defaultReparseOptions(UNIT));
        }

        CXCursor funcCursor;
        if(isMethod(unit, "../test-sources/file.cpp", 5, 6, funcCursor)) {
            displayCursorInfo(funcCursor);

            // Get the function return type
            CXCursor cursor = getFunctionReturnType(funcCursor);
            listChildren(cursor);
        }

        clang_disposeTranslationUnit(unit);
        clang_disposeIndex(index);

        /*
        		CXCodeCompleteResults* results;
        		// Note that column can not be <= 0
        		clang_reparseTranslationUnit(unit, 0, NULL, clang_defaultReparseOptions(unit));
        		results= clang_codeCompleteAt(unit, filename.c_str(), 5, 7, &file, 1 , clang_defaultCodeCompleteOptions());
        		if(!results) {
        			return -1;
        		}

        		int numResults = results->NumResults;
        		clang_sortCodeCompletionResults(results->Results, results->NumResults);
        		printf("Found %u matches\n", numResults);

        		for (int i = 0; i < numResults; i++) {
        			CXCompletionResult result = results->Results[i];
        			CXCompletionString str    = result.CompletionString;
        			CXCursorKind       kind   = result.CursorKind;
        			int numOfChunks = clang_getNumCompletionChunks(str);

        			std::string fullSignature;

        			if(kind == CXCursor_Constructor) {
        				fullSignature += "[ constructor ] ";

        			} else if(kind == CXCursor_Destructor) {
        				fullSignature += "[ destructor ] ";
        			}

        			std::string returnValue;
        			for (int i =0 ; i< numOfChunks; i++) {

        				if(clang_getCompletionChunkKind(str, i) == CXCompletionChunk_ResultType) {
        					returnValue   = clang_getCString(clang_getCompletionChunkText(str,i));
        				} else {
        					fullSignature += clang_getCString(clang_getCompletionChunkText(str,i));
        				}
        			}
        			printf("%s: %s\n", returnValue.c_str(), fullSignature.c_str());
        		}

        		clang_disposeCodeCompleteResults(results);

        	}
        */
    }


    return 0;
}

bool isMethod(CXTranslationUnit unit, const std::string &filename, int line, int col, CXCursor &funcCursor)
{
    CXFile file1;
    CXCursor cursor, ref;
    CXSourceLocation loc;
    file1  = clang_getFile    (unit, filename.c_str());
    loc    = clang_getLocation(unit, file1, line, col);
    cursor = clang_getCursor  (unit, loc);

    if(clang_isInvalid(cursor.kind))
        return false;

    if(cursor.kind == CXCursor_MemberRef || cursor.kind == CXCursor_MemberRefExpr) {
        cursor = clang_getCursorReferenced(cursor);
    }

    bool res = (cursor.kind == CXCursor_CXXMethod);
    if(res) {
        funcCursor = cursor;
    }
    return true;
}

enum CXChildVisitResult FunctionVisitorCallback(CXCursor Cursor,
        CXCursor Parent,
        CXClientData ClientData)
{
    if(Cursor.kind == CXCursor_TypeRef) {
        VisitorClientData *cd = (VisitorClientData*) ClientData;
        if( cd ) {
            cd->cursor = Cursor;
        }
        return CXChildVisit_Break;
    }

    return CXChildVisit_Continue;
}


CXCursor getFunctionReturnType(CXCursor funcCursor)
{
    CXCursorVisitor visitor = FunctionVisitorCallback;
    VisitorClientData cd;
    cd.cursor = clang_getNullCursor();
    clang_visitChildren(funcCursor, visitor, (CXClientData)&cd);

    if(!clang_isInvalid(cd.cursor.kind)) {
        return cd.cursor;
    }
    return clang_getNullCursor();
}

enum CXChildVisitResult ListChildren(CXCursor Cursor,
                                     CXCursor Parent,
                                     CXClientData ClientData)
{
    displayCursorInfo(Cursor);
    return CXChildVisit_Continue;
}

void listChildren(CXCursor cursor)
{
    CXCursorVisitor visitor = ListChildren;


    bool cont = false;
    do {
        if (cursor.kind == CXCursor_TypeRef) {
            cursor = clang_getCursorReferenced(cursor);
            cont = true;
            continue;
        }

        if(cursor.kind == CXCursor_TypedefDecl) {
            CXType type = clang_getTypedefDeclUnderlyingType(cursor);
            cursor = clang_getTypeDeclaration(type);
            cont = true;
            continue;
        }
        cont = false;
    } while (cont);

    clang_visitChildren(cursor, visitor, (CXClientData)NULL);
}
