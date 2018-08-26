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

void CppToken::reset()
{
    name.clear();
    offset = wxString::npos;
    m_id = wxNOT_FOUND;
    lineNumber = wxString::npos;
    filename.clear();
}

void CppToken::append(wxChar ch) { name << ch; }

void CppToken::print() { wxPrintf(wxT("%s | %ld\n"), name.c_str(), offset); }

int CppToken::store(wxSQLite3Database* db, wxLongLong fileID) const
{
    try {

        wxSQLite3Statement st = db->PrepareStatement(
            "REPLACE INTO TOKENS_TABLE (ID, NAME, OFFSET, FILE_ID, LINE_NUMBER) VALUES(NULL, ?, ?, ?, ?)");
        st.Bind(1, getName());
        st.Bind(2, (int)getOffset());
        st.Bind(3, fileID);
        st.Bind(4, (int)getLineNumber());
        st.ExecuteUpdate();
        return db->GetLastRowId().ToLong();

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
    return wxNOT_FOUND;
}

CppToken::Vec_t CppToken::loadByNameAndFile(wxSQLite3Database* db, const wxString& name, wxLongLong fileID)
{
    CppToken::Vec_t matches;
    try {
        wxSQLite3Statement st = db->PrepareStatement("select * from TOKENS_TABLE where FILE_ID=? AND NAME=?");
        st.Bind(1, fileID);
        st.Bind(2, name);
        wxSQLite3ResultSet res = st.ExecuteQuery();
        while(res.NextRow()) {
            CppToken token(res);
            matches.push_back(token);
        }

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
    return matches;
}

CppToken::CppToken(wxSQLite3ResultSet& res)
{
    setId(res.GetInt(0));
    setName(res.GetString(1));
    setOffset(res.GetInt(2));
    setLineNumber(res.GetInt(4));
}

CppToken::Vec_t CppToken::loadByName(wxSQLite3Database* db, const wxString& name)
{
    CppToken::Vec_t matches;
    std::unordered_map<size_t, wxString> fileIdToFile;
    try {
        wxSQLite3Statement st = db->PrepareStatement("select * from TOKENS_TABLE where NAME=?");
        st.Bind(1, name);
        wxSQLite3ResultSet res = st.ExecuteQuery();
        while(res.NextRow()) {
            CppToken token(res);
            size_t fileID = res.GetInt64(3).ToLong();
            if(fileIdToFile.count(fileID)) {
                token.setFilename(fileIdToFile.find(fileID)->second);
            } else {
                // load from the db
                wxSQLite3Statement st1 = db->PrepareStatement("SELECT FILE_NAME FROM FILES WHERE ID=? LIMIT 1");
                st1.Bind(1, wxLongLong(fileID));
                wxSQLite3ResultSet res1 = st1.ExecuteQuery();
                if(res1.NextRow()) {
                    token.setFilename(res1.GetString(0));
                    fileIdToFile.insert(std::make_pair(fileID, token.getFilename()));
                }
            }
            matches.push_back(token);
        }

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
    return matches;
}

//-----------------------------------------------------------------
// CppTokensMap
//-----------------------------------------------------------------
CppTokensMap::CppTokensMap() {}

CppTokensMap::~CppTokensMap() { clear(); }

void CppTokensMap::addToken(const wxString& name, const CppToken::Vec_t& list)
{
    // try to locate an entry with this name
    std::unordered_map<wxString, std::vector<CppToken>*>::iterator iter = m_tokens.find(name);
    std::vector<CppToken>* tokensList(NULL);
    if(iter != m_tokens.end()) {
        tokensList = iter->second;
    } else {
        // create new list and add it to the map
        tokensList = new std::vector<CppToken>;
        m_tokens.insert(std::make_pair(name, tokensList));
    }
    tokensList->insert(tokensList->end(), list.begin(), list.end());
}

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

bool CppTokensMap::contains(const wxString& name)
{
    std::unordered_map<wxString, std::vector<CppToken>*>::iterator iter = m_tokens.find(name);
    return iter != m_tokens.end();
}

void CppTokensMap::findTokens(const wxString& name, std::vector<CppToken>& tokens)
{
    std::unordered_map<wxString, std::vector<CppToken>*>::iterator iter = m_tokens.find(name);
    //	std::vector<CppToken> *tokensList(NULL);
    if(iter != m_tokens.end()) {
        tokens = *(iter->second);
    }
}
void CppTokensMap::clear()
{
    std::unordered_map<wxString, std::vector<CppToken>*>::iterator iter = m_tokens.begin();
    for(; iter != m_tokens.end(); ++iter) {
        delete iter->second;
    }
    m_tokens.clear();
}

bool CppTokensMap::is_empty() { return m_tokens.empty(); }
