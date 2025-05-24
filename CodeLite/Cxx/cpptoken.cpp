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
#include <wx/crt.h>
#include <map>

CppToken::CppToken() { reset(); }

CppToken::~CppToken() {}

bool CppToken::operator<(const CppToken& rhs) const
{
    return this->getName() < rhs.getName();
}

void CppToken::reset()
{
    name.clear();
    offset = wxString::npos;
    m_id = wxNOT_FOUND;
    lineNumber = wxString::npos;
    filename.clear();
}

void CppToken::append(wxChar ch) { name << ch; }

CppToken::CppToken(wxSQLite3ResultSet& res)
{
    setId(res.GetInt(0));
    setName(res.GetString(1));
    setOffset(res.GetInt(2));
    setLineNumber(res.GetInt(4));
}

//-----------------------------------------------------------------
// CppTokensMap
//-----------------------------------------------------------------
CppTokensMap::CppTokensMap() {}

CppTokensMap::~CppTokensMap() { clear(); }

void CppTokensMap::addToken(const CppToken& token)
{
    // try to locate an entry with this name
    std::unordered_map<wxString, std::vector<CppToken>*>::iterator iter = m_tokens.find(token.getName());
    std::vector<CppToken>* tokensList(NULL);
    if(iter != m_tokens.end()) {
        tokensList = iter->second;
    } else {
        // create new list and add it to the map
        tokensList = new std::vector<CppToken>;
        m_tokens[token.getName()] = tokensList;
    }
    tokensList->push_back(token);
}

void CppTokensMap::clear()
{
    for (auto& [_, p] : m_tokens) {
        delete p;
    }
    m_tokens.clear();
}

bool CppTokensMap::is_empty() { return m_tokens.empty(); }
