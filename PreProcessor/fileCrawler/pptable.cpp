#include "pptable.h"
#include <wx/tokenzr.h>

bool IsWordChar(const wxString &s, int strSize)
{
	if(strSize) {
		return s.find_first_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_")) != wxString::npos;

	} else {
		return s.find_first_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")) != wxString::npos;
	}
}

// Helper string find metho
wxString ReplaceWord(const wxString &str, const wxString &word, const wxString &replaceWith)
{
	wxString currChar;
	wxString nextChar;
	wxString currentWord;
	wxString output;
	for(size_t i=0; i<str.Length(); i++) {
		// Look ahead
		if( str.Length() > i + 1 ) {
			nextChar = str[i+1];
		} else {
			// we are at the end of buffer
			nextChar = wxT('\0');
		}

		currChar = str[i];
		if(!IsWordChar( currChar, currentWord.Length() )) {
			output << str[i];
			currentWord.Clear();

		} else {

			currentWord << currChar;
			if(IsWordChar(nextChar, currentWord.Length())) {
				// do nothing

			} else if( !IsWordChar(nextChar, currentWord.Length()) && currentWord == word ) {
				output << replaceWith;
				currentWord.Clear();

			} else {
				output << currentWord;
				currentWord.Clear();
			}

		}
	}
	return output;
}

wxArrayString TokenizeWords(const wxString &str)
{
	wxString currChar;
	wxString nextChar;
	wxString currentWord;
	wxArrayString outputArr;
	for(size_t i=0; i<str.Length(); i++) {
		// Look ahead
		if( str.Length() > i + 1 ) {
			nextChar = str[i+1];
		} else {
			// we are at the end of buffer
			nextChar = wxT('\0');
		}

		currChar = str[i];
		if(!IsWordChar( currChar, currentWord.Length() )) {
			currentWord.Clear();

		} else {

			currentWord << currChar;
			if(IsWordChar(nextChar, currentWord.Length())) {
				// do nothing

			} else {

				outputArr.Add(currentWord);
				currentWord.Clear();
			}

		}
	}
	return outputArr;
}

void PPToken::processArgs(const wxString &argsList)
{
	args = wxStringTokenize(argsList, wxT(","), wxTOKEN_STRTOK);

	// replace all occurances of 'arg' with %1, %2 etc
	for(size_t i=0; i<args.GetCount(); i++) {
		wxString replaceWith = wxString::Format(wxT("%%%d"), (int)i);
		replacement = ReplaceWord(replacement, args.Item(i), replaceWith);
	}
}

void PPToken::print(wxFFile &fp)
{
#if 0
	wxString buff;
	buff << wxT("Name          : ") << name << wxT("\n");
	buff << wxT("replacement   : ") << replacement << wxT("\n");
	buff << wxT("isFunctionLike: ") << isFunctionLike << wxT("\n");
	if(isFunctionLike) {
		for(size_t i=0; i<args.size(); i++) {
			buff << wxT("Arg: ") << args.Item(i) << wxT("\n");
		}
	}
	buff << wxT(" ---- \n");
	fp.Write(buff);
#else
	wxString buff;
	buff << name << wxT("(") << (flags & IsFunctionLike) << wxT(")") << wxT("=") << replacement << wxT("\n");
	fp.Write(buff);
#endif
}

wxString PPToken::fullname()
{
	wxString fullname;
	fullname << name;
	if(flags & IsFunctionLike) {
		fullname << wxT("(");
		for(size_t i=0; i<args.size(); i++) {
			fullname << wxT("%") << i << wxT(",");
		}
		if(args.size()) {
			fullname.RemoveLast();
		}
		fullname << wxT(")");
	}
	return fullname;
}

void PPToken::squeeze()
{
	// perform the squeeze 5 times max
	for(size_t count=0; count < 5; count++) {
		bool modified(false);
		// get list of possible macros in the replacement
		wxArrayString words = TokenizeWords(replacement);
		for(size_t i=0; i<words.size(); i++) {
			PPToken tok = PPTable::Instance()->Token(words.Item(i));
			if(tok.flags & IsValid) {
				if(tok.flags & IsFunctionLike) {
					int where = replacement.Find(words.Item(i));
					if(where != wxNOT_FOUND) {
						wxString      initList;
						wxArrayString initListArr;
						if(readInitList( replacement, where + words.Item(i).Length(), initList, initListArr )) {
							tok.expandOnce(initListArr);

							replacement.Remove(where, words.Item(i).Length() + initList.Length());
							tok.replacement.Replace(wxT("##"), wxT(""));
							replacement.insert(where, tok.replacement);
							modified = true;
						}
					}

				} else {
					if(replacement.Replace(words.Item(i), tok.replacement)) {
						modified = true;
					}
				}
			}
		}

		if(!modified)
			break;
	}
	replacement.Replace(wxT("##"), wxT(""));
}

bool PPToken::readInitList(const std::string& in, size_t from, std::string& initList, std::vector<std::string>& initListArr)
{
	if(in.length() < from) {
		return false;
	}

	std::string tmpString = in.substr(from);
	size_t      start     = tmpString.find('(');
	if(start == std::string::npos ) {
		return false;
	}
	
	// skip the open brace
	tmpString = tmpString.substr(start+1);

	for(size_t i=0; i<start; i++) {
		initList += " ";
	}

	initList += "(";
	std::string word;
	int depth(1);

	for(size_t i=0; i<tmpString.length(); i++) {
		char ch = tmpString[i];
		initList += ch;
		switch(ch) {
		case ')':
			depth --;
			if(depth == 0) {
				initListArr.push_back(word);
				return true;
			} else {
				word += ch;
			}
			break;
		case '(':
			depth++;
			word += ch;
			break;
		case ',':
			if(depth == 1) {
				initListArr.push_back(word);
				word.clear();
			} else {
				word += ch;
			}
			break;
		default:
			word += ch;
			break;
		}
	}
	return false;
}

bool PPToken::readInitList(const wxString &in, int from, wxString& initList, wxArrayString& initListArr)
{
	if(in.Length() < from) {
		return false;
	}

	wxString tmpString = in.Mid(from);
	int start = tmpString.Find(wxT("("));
	if(start == wxNOT_FOUND ) {
		return false;
	}
	tmpString = tmpString.Mid(start+1);

	for(size_t i=0; i<start; i++) {
		initList << wxT(" ");
	}

	initList << wxT("(");
	wxString word;
	int depth(1);

	for(size_t i=0; i<tmpString.Length(); i++) {
		wxChar ch = tmpString[i];
		initList << ch;
		switch(ch) {
		case wxT(')'):
			depth --;
			if(depth == 0) {
				initListArr.Add(word);
				return true;
			} else {
				word << ch;
			}
			break;
		case wxT('('):
			depth++;
			word << ch;
			break;
		case wxT(','):
			if(depth == 1) {
				initListArr.Add(word);
				word.Clear();
			} else {
				word << ch;
			}
			break;
		default:
			word << ch;
			break;
		}
	}
	return false;
}

void PPToken::expandOnce(const wxArrayString& initList)
{
	if(initList.size() != args.size())
		return;

	for(size_t i=0; i<args.size(); i++) {
		wxString placeHolder;
		placeHolder << wxT("%") << i;
		replacement.Replace(placeHolder, initList.Item(i));
	}
}

///////////////////////////////////////////////////

PPTable* PPTable::ms_instance = 0;

PPTable::PPTable()
{
}

PPTable::~PPTable()
{
}

PPTable* PPTable::Instance()
{
	if(ms_instance == 0) {
		ms_instance = new PPTable();
	}
	return ms_instance;
}

void PPTable::Release()
{
	if(ms_instance) {
		delete ms_instance;
	}
	ms_instance = 0;
}

PPToken PPTable::Token(const wxString& name)
{
	std::map<wxString, PPToken>::iterator iter = m_table.find(name);
	if(iter == m_table.end()) {
		return PPToken();
	}

	return iter->second;
}

void PPTable::Add(const PPToken& token)
{
	if(token.name.IsEmpty())
		return;

	wxString name = token.name;
	name.Trim().Trim(false);
	std::map<wxString, PPToken>::iterator iter = m_table.find(name);
	if(iter == m_table.end())
		m_table[name] = token;
	else {
		// if the new token's replacement is empty and the current one is NOT empty,
		// replace the two (we prefer empty replacements)
		if(iter->second.flags & PPToken::IsOverridable && !iter->second.replacement.IsEmpty() && token.replacement.IsEmpty()) {
			m_table[name] = token;
		}
	}
}

void PPTable::Print(wxFFile &fp)
{
	std::map<wxString, PPToken>::iterator iter = m_table.begin();
	for(; iter != m_table.end(); iter++) {
		iter->second.print(fp);
	}
}

bool PPTable::Contains(const wxString& name)
{
	std::map<wxString, PPToken>::iterator iter = m_table.find(name);
	return iter != m_table.end();
}

wxString PPTable::Export()
{
	wxString table;
	std::map<wxString, PPToken>::iterator iter = m_table.begin();
	for(; iter != m_table.end(); iter++) {
		iter->second.squeeze();
		wxString replacement = iter->second.replacement;
		replacement.Trim().Trim(false);
		if(replacement.IsEmpty()) {
			table << iter->second.fullname() << wxT("\n");

		} else if(iter->second.flags & PPToken::IsFunctionLike) {
			table << iter->second.fullname() << wxT("=") << replacement << wxT("\n");

		} else {
			// macros with replacement but they are not in a form of a function
			// we take only macros that thier replacement is not a number
			long v(-1);
			if(!replacement.ToLong(&v) && !replacement.ToLong(&v, 8) && !replacement.ToLong(&v, 16)) {
				table << iter->second.fullname() << wxT("=") << replacement << wxT("\n");
			}
		}
	}
	return table;
}

void PPTable::Squeeze()
{
	std::map<wxString, PPToken>::iterator iter = m_table.begin();
	for(; iter != m_table.end(); iter++) {
		iter->second.squeeze();
	}
}

void PPTable::Clear()
{
	m_table.clear();
}

wxString CLReplacePattern(const wxString& in, const wxString& pattern, const wxString& replaceWith)
{
	int where = pattern.Find(wxT("%0"));
	if(where != wxNOT_FOUND) {
		wxString replacement(replaceWith);

		// a patterened expression
		wxString searchFor = pattern.BeforeFirst(wxT('('));
		where = in.Find(searchFor);
		if(where == wxNOT_FOUND) {
			return in;
		}

		wxString      initList;
		wxArrayString initListArr;
		if(PPToken::readInitList(in, searchFor.Length() + where, initList, initListArr) == false)
			return in;

		wxString outStr(in);
		// update the 'replacement' with the actual values ( replace %0..%n)
		for(size_t i=0; i<initListArr.size(); i++) {
			wxString placeHolder;
			placeHolder << wxT("%") << i;
			replacement.Replace(placeHolder, initListArr.Item(i));
		}

		outStr.Remove(where, searchFor.Length() + initList.Length());
		outStr.insert(where, replacement);
		return outStr;

	} else {
		if(in.Find(pattern) == wxNOT_FOUND) {
			return in;
		}
		// simple replacement
		return ReplaceWord(in, pattern, replaceWith);
	}
}

std::string CLReplacePatternA(const std::string& in, const std::string& pattern, const std::string& replaceWith)
{
	size_t where = in.find("%0");
	if(where != std::string::npos) {
		std::string replacement(replaceWith);

		// a patterened expression
		where = pattern.find('(');
		if(where == std::string::npos)
			return in;

		std::string searchFor = pattern.substr(0, where);

		std::string              initList;
		std::vector<std::string> initListArr;
		if(PPToken::readInitList(in, searchFor.length() + where, initList, initListArr) == false)
			return in;

		std::string outStr(in);
		// update the 'replacement' with the actual values ( replace %0..%n)
		for(size_t i=0; i<initListArr.size(); i++) {
			wxString placeHolder;
			placeHolder << wxT("%") << i;
			replacement.Replace(placeHolder, initListArr.Item(i));
		}

		outStr.Remove(where, searchFor.Length() + initList.Length());
		outStr.insert(where, replacement);
		return outStr;

	} else {
		if(in.Find(pattern) == wxNOT_FOUND) {
			return in;
		}
		// simple replacement
		return ReplaceWord(in, pattern, replaceWith);
	}
}
