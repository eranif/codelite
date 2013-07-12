//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cpptoken.h
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
#ifndef __cpptoken__
#define __cpptoken__

#include <string>
#include <list>
#include <map>
#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/wxsqlite3.h>

class WXDLLIMPEXP_CL CppToken
{
    int      m_id;
    wxString name;    // the name of the token
    size_t   offset;  // file offset
    wxString filename;
    size_t   lineNumber;
public:
    typedef std::list<CppToken> List_t;
    
public:
    CppToken();
    CppToken(wxSQLite3ResultSet &res);
    ~CppToken();

    void reset();
    void append(wxChar ch);
    
    int store(wxSQLite3Database* db) const;
    /**
     * @brief load tokens from the cache by file/name
     * @param db
     * @param name
     * @param file_name
     * @return 
     */
    static CppToken::List_t loadByNameAndFile(wxSQLite3Database* db, const wxString& name, const wxString& filename);
    /**
     * @brief 
     * @param db
     * @param name
     * @return 
     */
    static CppToken::List_t loadByName(wxSQLite3Database* db, const wxString& name);
    
    void setName(const wxString& name) {
        this->name = name;
    }

    void setOffset(const size_t& offset) {
        this->offset = offset;
    }

    const wxString& getName() const {
        return name;
    }
    const size_t& getOffset() const {
        return offset;
    }

    void setFilename(const wxString& filename) {
        this->filename = filename;
    }

    const wxString& getFilename() const {
        return filename;
    }

    void setId(const int& id) {
        this->m_id = id;
    }

    const int& getId() const {
        return m_id;
    }

    size_t getLineNumber() const {
        return lineNumber;
    }

    void setLineNumber(size_t lineNo) {
        lineNumber = lineNo;
    }

    void print();

    // Provide lower-than operator so we can use std::list::sort method
    bool operator < (const CppToken& rhs) {
        return filename < rhs.filename;
    }
};

class WXDLLIMPEXP_CL CppTokensMap
{
    std::map<wxString, std::list<CppToken>* > m_tokens;

public:
    CppTokensMap();
    ~CppTokensMap();

    /**
     * @brief return true if any token with given name exists in the map
     * @param name token's name to search
     */
    bool contains(const wxString &name);
    /**
     * @brief return list of tokens with given name
     * @param name token name
     * @param tokens [output]
     */
    void findTokens(const wxString &name, std::list<CppToken> &tokens);
    /**
     * @brief add token to the map. if token with same name already exists, it will be appended so multiple tokens with same name is allowed
     * @param token token to add
     */
    void addToken(const CppToken &token);
    
    /**
     * @brief add list of tokens for a given name to the map
     */
    void addToken(const wxString& name, const CppToken::List_t &list);

    /**
     * @brief clear all token
     */
    void clear();

    /**
     * @brief return true if no tokens were found
     */
    bool is_empty();
};

#endif // __cpptoken__
