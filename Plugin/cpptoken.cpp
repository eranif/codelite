#include "cpptoken.h"
CppToken::CppToken()
{
	reset();
}

CppToken::~CppToken()
{
}

void CppToken::reset()
{
	name.clear();
	offset = wxString::npos;
	m_id = wxNOT_FOUND;
	filename.clear();
}

void CppToken::append(const char ch)
{
	name += ch;
}

void CppToken::print()
{
	wxPrintf(wxT("%s | %ld\n"), name.GetData(), offset);
}

//-----------------------------------------------------------------
// CppTokensMap
//-----------------------------------------------------------------
CppTokensMap::CppTokensMap()
{
}

CppTokensMap::~CppTokensMap()
{
	clear();
}

void CppTokensMap::addToken(const CppToken& token)
{
	// try to locate an entry with this name
	std::map<wxString, std::list<CppToken>* >::iterator iter = m_tokens.find(token.getName());
	std::list<CppToken> *tokensList(NULL);
	if (iter != m_tokens.end()) {
		tokensList = iter->second;
	} else {
		// create new list and add it to the map
		tokensList = new std::list<CppToken>;
		m_tokens[token.getName()] = tokensList;
	}
	tokensList->push_back( token );
}

bool CppTokensMap::contains(const wxString& name)
{
	std::map<wxString, std::list<CppToken>* >::iterator iter = m_tokens.find(name);
	return iter != m_tokens.end();
}

void CppTokensMap::findTokens(const wxString& name, std::list<CppToken>& tokens)
{
	std::map<wxString, std::list<CppToken>* >::iterator iter = m_tokens.find(name);
//	std::list<CppToken> *tokensList(NULL);
	if (iter != m_tokens.end()) {
		tokens = *(iter->second);
	}
}
void CppTokensMap::clear()
{
	std::map<wxString, std::list<CppToken>* >::iterator iter = m_tokens.begin();
	for(; iter != m_tokens.end(); iter++) {
		delete iter->second;
	}
	m_tokens.clear();
}
