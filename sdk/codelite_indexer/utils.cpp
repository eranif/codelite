#include "utils.h"
#include <wx/tokenzr.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wx/string.h>
#include <wx/regex.h>
#include <wx/arrstr.h>
#include <map>
#include "pptable.h"

#ifdef __WXMSW__
#    include <windows.h>
#    include <process.h>
#    include <Tlhelp32.h>
#else
#    include <signal.h>
#endif

#ifdef __FreeBSD__
#include <fcntl.h>
#include <paths.h>
#include <kvm.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#endif

#ifdef _WIN32
#define CURRENT_GCC_VERSION ((__GNUC__*1000)+(__GNUC_MINOR__*100))
#define GCC_VERSION(major, minor) ((major*1000)+(minor*100))

#if CURRENT_GCC_VERSION < GCC_VERSION(4,9)
#include <string.h>
#define strdup _strdup
#endif
#endif

/**
 * helper string methods
 */

/**
 * @brief remove whitespaces from string
 * @param str
 */
void string_trim(std::string &str)
{
	str.erase(0, str.find_first_not_of(" \t\n\r\v"));
	str.erase(str.find_last_not_of(" \t\n\r\v")+1);
}

/**
 * @brief tokenize string into array of string by delimiters
 * @param str input string
 * @param delimiters delimiters to break the string according to
 * @return vector of strings
 */
std::vector<std::string> string_tokenize(const std::string &str, const std::string& delimiter)
{
	std::string::size_type start (0);
	std::vector<std::string> tokens;
	std::string token;

	std::string::size_type end = str.find(delimiter);
	while ( end != std::string::npos ) {

		if ( end != start )
			token = str.substr(start, end - start);
		else
			token.clear();

		// trim spaces
		string_trim(token);

		if ( !token.empty() )
			tokens.push_back(token);

		// next token
		start = end + delimiter.length();
		end = str.find(delimiter, start );
	}

	if ( start != (str.length() - 1) ) {
		// We have another token which is not delimited
		token = str.substr(start);
		tokens.push_back(token);
	}
	return tokens;
}

// ------------------------------------------
// Process manipulation
// ------------------------------------------
bool is_process_alive(long pid)
{
#ifdef __WXMSW__
	static HANDLE hProc = NULL;
	
	if ( hProc == NULL ) {
		hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
	}
	
	if ( hProc ) {
		int rc = WaitForSingleObject(hProc, 5);
		switch (rc) {
		case WAIT_TIMEOUT:
			return true;
		default:
			return false;
		}
	}
	return true;

#else
	return kill(pid, 0) == 0; // send signal 0 to process
#endif
}

static char *load_file(const char *fileName) {
	FILE *fp;
	long len;
	char *buf = NULL;

	fp = fopen(fileName, "rb");
	if (!fp) {
		return 0;
	}


	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buf = (char *)malloc(len+1);


	long bytes = fread(buf, sizeof(char), len, fp);
	if (bytes != len) {
		fclose(fp);
		free(buf);
		return 0;
	}

	buf[len] = 0;	// make it null terminated string
	fclose(fp);
	return buf;
}

static CLReplacementList replacements;
static int first = 1;

extern "C" char* ctagsReplacements(char* result)
{
	
	/**
	 * try to load the file once 
	 **/
	 
	if( first ) {
		char *content = (char*)0;
		char *file_name = getenv("CTAGS_REPLACEMENTS");

		first = 0;
		if(file_name) {
			/* open the file */
			content = load_file(file_name);
			if(content) {
				wxArrayString lines = wxStringTokenize(wxString::From8BitData(content), wxT("\n"), wxTOKEN_STRTOK);
				free(content);
				
				for(size_t i=0; i<lines.GetCount(); i++) {
					wxString pattern = lines.Item(i).BeforeFirst(wxT('='));
					wxString replace = lines.Item(i).AfterFirst(wxT('='));
					
					pattern.Trim().Trim(false);
					replace.Trim().Trim(false);
					
					CLReplacement repl;
					repl.construct(pattern.To8BitData().data(), replace.To8BitData().data());
					if(repl.is_ok) {
						replacements.push_back( repl );
					}
				}
			}
		}
	}

	if( replacements.empty() == false ) {

		std::string outStr = result;
		CLReplacementList::iterator iter = replacements.begin();
		for(; iter != replacements.end(); iter++) {
			CLReplacePatternA(outStr, *iter, outStr);
		}
		
		if(outStr == result)
			return NULL;
			
		return strdup(outStr.c_str());
	}
	return NULL;
}
