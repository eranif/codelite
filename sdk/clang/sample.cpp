#include <clang-c/Index.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string>

#ifdef WIN32
const char *cmdLineArgs[] = {
	"-Ic:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include\\c++",
	"-Ic:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include\\c++\\mingw32",
	"-Ic:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include\\c++\\backward",
	"-Ic:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include",
	"-Ic:\\mingw-4.6.1\\include",
	"-Ic:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include-fixed",
	"-IC:\\Users\\eran\\software\\nzbtracker",
	"-IC:\\Users\\eran\\software\\nzbtracker\\curl\\include",
	"-IC:\\Users\\eran\\software\\nzbtracker",
	"-IC:\\Users\\eran\\software\\nzbtracker\\sqlite3",
	"-DHAVE_W32API_H",
	"-D__WXMSW__",
	"-DNDEBUG",
	"-D_UNICODE",
	"-IC:\\wxWidgets-2.9.2\\lib\\gcc_dll\\mswu",
	"-IC:\\wxWidgets-2.9.2\\include",
	"-DWXUSINGDLL",
	"-Wno-ctor-dtor-privacy",
	"-Wall",
	"-D__WX__"
};
const int numArgs = 20;
#else
const char *cmdLineArgs[] = {
"-I/usr/include/c++/4.6",
"-I/usr/include/c++/4.6/x86_64-linux-gnu",
"-I/usr/include/c++/4.6/backward",
"-I/usr/lib/gcc/x86_64-linux-gnu/4.6.1/include",
"-I/usr/local/include",
"-I/usr/lib/gcc/x86_64-linux-gnu/4.6.1/include-fixed",
"-I/usr/include/x86_64-linux-gnu",
"-I/usr/include",
"-g",
"-I/home/eran/devl/nzbtracker/",
"-I/home/eran/devl/nzbtracker/curl/include",
"-I/home/eran/devl/nzbtracker/",
"-I/home/eran/devl/nzbtracker/sqlite3",
"-ferror-limit=1000",
"-D__WXGTK__",
"-D_UNICODE",
"-D__WX__",
"-I/usr/lib/wx/include/gtk2-unicode-debug-2.8",
"-I/usr/include/wx-2.8/"
};
const int numArgs = 19;
#endif

char *loadFile(const char *fileName)
{
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
	fseek(fp, 0, SEEK_SET);      // go to begining
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

static int displayCursorInfo(CXCursor Cursor)
{
	if(Cursor.kind == CXCursor_MacroDefinition || Cursor.kind == CXCursor_MacroExpansion || Cursor.kind == CXCursor_MacroInstantiation)
	{
		printf(" ==>\n");
		if(Cursor.kind == CXCursor_MacroDefinition)
			printf("CXCursor_MacroDefinition\n");
		if(Cursor.kind == CXCursor_MacroExpansion)
			printf("CXCursor_MacroExpansion\n");
		if(Cursor.kind == CXCursor_MacroInstantiation)
			printf("CXCursor_MacroInstantiation\n");

		CXString String = clang_getCursorDisplayName(Cursor);
		printf("Display: [%s]\n", clang_getCString(String));
		clang_disposeString(String);

		CXSourceLocation loc = clang_getCursorLocation(Cursor);
		CXFile file;
		unsigned line, col, off;
		clang_getSpellingLocation(loc, &file, &line, &col, &off);

		CXString strFileName = clang_getFileName(file);
		printf("Location: %s, %u:%u\n", clang_getCString(strFileName), line, col);
		clang_disposeString(strFileName);
	} else {
		//----------------
		CXString String = clang_getCursorDisplayName(Cursor);
		printf(" ==> Display: [%s]\n", clang_getCString(String));
		clang_disposeString(String);
	}
	return 0;
}

enum CXChildVisitResult VisitorCallback(CXCursor Cursor,
                                        CXCursor Parent,
                                        CXClientData ClientData)
{
	displayCursorInfo(Cursor);
	return CXChildVisit_Continue;
}

void findFunction()
{

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
							 | CXTranslationUnit_DetailedPreprocessingRecord);

	CXCursorVisitor visitor = VisitorCallback;
	clang_visitChildren(clang_getTranslationUnitCursor(unit), visitor, NULL);
	/*
	CXFile file;
	CXCursor cursor;
	CXSourceLocation loc;
	file   = clang_getFile    (unit, "../test-sources/file.cpp");
	loc    = clang_getLocation(unit, file, 5, 8);
	cursor = clang_getCursor  (unit, loc);

	if(cursor.kind == CXCursor_MemberRefExpr || cursor.kind == CXCursor_TypeRef) {
		CXCursor refCur = clang_getCursorReferenced(cursor);
		displayCursorInfo(refCur);
	}
	*/
}

int main(int argc, char **argv)
{
	char *partBuffer      = loadFile("../test-sources/file-buffer.cpp");
	std::string strBuffer = partBuffer;
	free(partBuffer);

	std::string filename  = "../test-sources/file.cpp";
	CXUnsavedFile file = {filename.c_str(), strBuffer.c_str(), strBuffer.length()};
	CXIndex index = clang_createIndex(0, 0);

	findFunction();
	CXTranslationUnit unit = 0;
/*
	//for(size_t i=0; i<10; i++) {
	if(parseFile(filename, partBuffer, index, &unit) == 0) {

		//
		CXCodeCompleteResults* results;
		for(size_t i=0; i<10; i++) {
		// Note that column can not be <= 0
			clang_reparseTranslationUnit(unit, 0, NULL, clang_defaultReparseOptions(unit));
			results= clang_codeCompleteAt(unit, filename.c_str(), 64, 8, &file, 1 , clang_defaultCodeCompleteOptions());
			if(!results) {
				return -1;
			}
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
		//	printf("%s: %s\n", returnValue.c_str(), fullSignature.c_str());
		}

		clang_disposeCodeCompleteResults(results);

	}
	//}

	clang_disposeTranslationUnit(unit);
	clang_disposeIndex(index);
*/
	return 0;
}
