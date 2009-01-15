#include "tagentry.h"
#include <map>

/**
 * helper string methods
 */

/**
 * @brief return the string before the first occurance of 'c'
 * @param s input string
 * @param c delimiter char
 * @return string or the entire 's' string of 'c' is not found
 */
static std::string before_first(const std::string &s, char c) {
	std::string::size_type where = s.find(c);
	if(where == std::string::npos){
		return s;
	}
	return s.substr(0, where);
}

/**
 * @brief return string after the first occurance of 'c'
 * @param s input string
 * @param c delimiter char
 * @return string or empty string if 'c' is not found
 */
static std::string after_first(const std::string &s, char c) {
	std::string::size_type where = s.find(c);
	if(where == std::string::npos){
		return "";
	}
	return s.substr(where);
}

/**
 * @brief return true if s starts with prefix, false otherwise
 */
static bool starts_with(const std::string &s, const std::string &prefix){
	if(s.find(prefix) == 0){
		return true;
	}
	return false;
}

TagEntry::TagEntry(const char *line)
{
	m_line = line;
	this->initialize();
}

TagEntry::~TagEntry()
{
}

void TagEntry::initialize()
{
//	std::string pattern, kind;
//	std::string strLine = line;
//	long lineNumber = -1;
//	std::map<std::string, std::string> extFields;
//
//	// get the token name
//	std::string name = before_first(strLine, '\t');
//	strLine	= after_first(strLine, '\t');
//
//	// get the file name
//	std::string fileName = before_first(strLine, '\t');
//	strLine	= after_first(strLine, '\t');
//
//	// here we can get two options:
//	// pattern followed by ;"
//	// or
//	// line number followed by ;"
//	std::string::size_type end = strLine.find(wxT(";\""));
//	if (end == std::string::npos) {
//		// invalid pattern found
//		return;
//	}
//
//	if (strLine.StartsWith(wxT("/^"))) {
//		//regular expression pattern found
//		pattern = strLine.substr(0, end);
//		strLine	= strLine.substr(strLine.length() - (end + 2));
//	} else {
//		//line number pattern found, this is usually the case when
//		//dealing with macros in C++
//		pattern = strLine.Mid(0, end);
//		strLine	= strLine.Right(strLine.Length() - (end + 2));
//
//		pattern = pattern.Trim();
//		pattern = pattern.Trim(false);
//		pattern.ToLong(&lineNumber);
//	}
//
//	//next is the kind of the token
//	if (strLine.StartsWith(wxT("\t"))) {
//		strLine	= strLine.AfterFirst(wxT('\t'));
//	}
//
//	kind = strLine.BeforeFirst(wxT('\t'));
//	strLine	= strLine.AfterFirst(wxT('\t'));
//
//	if (strLine.IsEmpty() == false) {
//		wxStringTokenizer tkz(strLine, wxT('\t'));
//		while (tkz.HasMoreTokens()) {
//			std::string token = tkz.NextToken();
//			std::string key = token.BeforeFirst(wxT(':'));
//			std::string val = token.AfterFirst(wxT(':'));
//			key = key.Trim();
//			key = key.Trim(false);
//
//			val = val.Trim();
//			val = val.Trim(false);
//			if (key == wxT("line") && !val.IsEmpty()) {
//				val.ToLong(&lineNumber);
//			} else {
//				if (key == wxT("union") || key == wxT("struct")) {
//
//					// remove the anonymous part of the struct / union
//					if (!val.StartsWith(wxT("__anon"))) {
//						// an internal anonymous union / struct
//						// remove all parts of the
//						wxArrayString scopeArr;
//						std::string tmp, new_val;
//
//						scopeArr = wxStringTokenize(val, wxT(":"), wxTOKEN_STRTOK);
//						for (size_t i=0; i<scopeArr.GetCount(); i++) {
//							if (scopeArr.Item(i).StartsWith(wxT("__anon")) == false) {
//								tmp << scopeArr.Item(i) << wxT("::");
//							}
//						}
//
//						tmp.EndsWith(wxT("::"), &new_val);
//						val = new_val;
//					}
//				}
//
//				extFields[key] = val;
//			}
//		}
//	}
//
//	kind = kind.Trim();
//	name = name.Trim();
//	fileName = fileName.Trim();
//	pattern = pattern.Trim();
//
//	if (kind == wxT("enumerator")) {
//		// enums are specials, they are not really a scope so they should appear when I type:
//		// enumName::
//		// they should be member of their parent (which can be <global>, or class)
//		// but we want to know the "enum" type they belong to, so save that in typeref,
//		// then patch the enum field to lift the enumerator into the enclosing scope.
//		// watch out for anonymous enums -- leave their typeref field blank.
//		std::map<std::string,std::string>::iterator e = extFields.find(wxT("enum"));
//		if (e != extFields.end()) {
//			std::string typeref = e->second;
//			e->second = e->second.BeforeLast(wxT(':')).BeforeLast(wxT(':'));
//			if (!typeref.AfterLast(wxT(':')).StartsWith(wxT("__anon"))) {
//				extFields[wxT("typeref")] = typeref;
//			}
//		}
//	}
//

}
