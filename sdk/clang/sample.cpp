#include <clang-c/Index.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string>

const char *cmdLineArgs[] = {
	"-IC:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include\\c++",
	"-IC:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include\\c++\\mingw32",
	"-IC:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include\\c++\\backward",
	"-IC:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include",
	"-IC:\\mingw-4.6.1\\include",
	"-IC:\\mingw-4.6.1\\lib\\gcc\\mingw32\\4.6.1\\include-fixed"
};

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
	bool needReparse = true;
	CXUnsavedFile file = {filename.c_str(), modifiedBuffer.c_str(), modifiedBuffer.length()};
	if(!*unit) {
		needReparse = false;
		*unit = clang_parseTranslationUnit(index,
	                         filename.c_str(),
	                         cmdLineArgs,
	                         6,
	                         &file,
	                         1,
	                         CXTranslationUnit_PrecompiledPreamble | CXTranslationUnit_CacheCompletionResults);
	}

	if(!*unit) {
		printf("clang_parseTranslationUnit error!\n");
		return -1;
	}

	if(needReparse) {
		int status = clang_reparseTranslationUnit(*unit, 1, &file, clang_defaultReparseOptions(*unit));
		if(status != 0) {
			printf("clang_reparseTranslationUnit error!\n");
			return -1;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	char *partBuffer      = loadFile("C:\\file-buffer.cpp");
	std::string strBuffer = partBuffer;
	free(partBuffer);

	std::string filename  = "C:\\file.cpp";
	CXUnsavedFile file = {"C:\\file.cpp", strBuffer.c_str(), strBuffer.length()};
	CXIndex index = clang_createIndex(0, 0);

	CXTranslationUnit unit = 0;
	//for(size_t i=0; i<10; i++) {
	if(parseFile(filename, partBuffer, index, &unit) == 0) {

		// Note that column can not be <= 0
		CXCodeCompleteResults* results= clang_codeCompleteAt(unit, filename.c_str(), 12, 4, &file, 1 , clang_defaultCodeCompleteOptions());
		if(!results) {
			return -1;
		}

		int numResults = results->NumResults;
		clang_sortCodeCompletionResults(results->Results, results->NumResults);

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
	//}

	clang_disposeTranslationUnit(unit);
	clang_disposeIndex(index);

	return 0;
}
