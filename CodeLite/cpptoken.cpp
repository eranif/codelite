//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : cpptoken.cpp              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
 #include "cpptoken.h"
#include <wx/string.h>
#include <wx/crt.h>
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
	offset = std::string::npos;
	m_id = wxNOT_FOUND;
	lineNumber = std::string::npos;
	filename.clear();
}

void CppToken::append(const char ch)
{
	name += ch;
}

void CppToken::print()
{
	wxPrintf(wxT("%s | %ld\n"), name.c_str(), offset);
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
	std::map<std::string, std::list<CppToken>* >::iterator iter = m_tokens.find(token.getName());
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

bool CppTokensMap::contains(const std::string& name)
{
	std::map<std::string, std::list<CppToken>* >::iterator iter = m_tokens.find(name);
	return iter != m_tokens.end();
}

void CppTokensMap::findTokens(const std::string& name, std::list<CppToken>& tokens)
{
	std::map<std::string, std::list<CppToken>* >::iterator iter = m_tokens.find(name);
//	std::list<CppToken> *tokensList(NULL);
	if (iter != m_tokens.end()) {
		tokens = *(iter->second);
	}
}
void CppTokensMap::clear()
{
	std::map<std::string, std::list<CppToken>* >::iterator iter = m_tokens.begin();
	for(; iter != m_tokens.end(); iter++) {
		delete iter->second;
	}
	m_tokens.clear();
}

bool CppTokensMap::is_empty()
{
	return m_tokens.empty();
}
