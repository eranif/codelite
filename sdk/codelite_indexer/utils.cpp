#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wx/string.h>
#include <wx/regex.h>
#include <map>

#ifdef __WXMSW__
#    include <windows.h>
#    include <process.h>
#    include <Tlhelp32.h>
#else
#    include <signal.h>
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

static std::map<wxString, wxRegEx*> s_regexPool;

extern "C" char* regReplace(const char* src, const char* key, const char* value)
{
	wxString findWhat    = wxString(key,   wxConvUTF8);
	wxString replaceWith = wxString(value, wxConvUTF8);
	wxString inputStr    = wxString(src,   wxConvUTF8);
	
	
	wxRegEx *re (NULL);
	if(s_regexPool.find(findWhat) != s_regexPool.end()) {
		re = s_regexPool.find(findWhat)->second;
	}
	
	if(re == NULL) {
		re = new wxRegEx(findWhat);
	}
	
	if(!re->IsValid()) {
		// invalid regular expression
		// return a copy of the input string
		delete re;
		return strdup(src);
	}
	
	// keep this instance
	s_regexPool[findWhat] = re;
	
	// regex is valid, try to match
	if(re->Matches(inputStr)) {
		re->ReplaceAll(&inputStr, replaceWith);
	}
	
	return strdup( inputStr.mb_str(wxConvUTF8).data() );
}
