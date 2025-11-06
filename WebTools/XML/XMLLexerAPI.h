//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XMLLexerAPI.h
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

#ifndef XMLLexerAPI_H__
#define XMLLexerAPI_H__

#include <cstdio>
#include <vector>
#include <wx/filename.h>
#include <wx/string.h>

struct XMLLexerToken {
    int lineNumber;
    int column;
    wxString text;
    int type;
    wxString comment;
    XMLLexerToken()
        : lineNumber(0)
        , column(0)
        , type(0)
    {
    }

    XMLLexerToken(int tokenType)
        : lineNumber(0)
        , column(0)
        , type(tokenType)
    {
    }

    void Clear()
    {
        lineNumber = 0;
        column = 0;
        type = 0;
        text.Clear();
    }
    using Vec_t = std::vector<XMLLexerToken>;
};

/**
 * @class XMLLexerUserData
 */
struct XMLLexerUserData {
public:
    FILE* m_currentPF;
    void* parserData;

public:
    void Clear()
    {
        if(m_currentPF) {
            ::fclose(m_currentPF);
            m_currentPF = NULL;
        }
        parserData = NULL;
    }

    XMLLexerUserData()
        : m_currentPF(NULL)
        , parserData(NULL)
    {
    }

    ~XMLLexerUserData() { Clear(); }
    void SetCurrentPF(FILE* currentPF) { this->m_currentPF = currentPF; }
};

using XMLScanner_t = void*;

/**
 * @brief create a new Lexer for a file content
 */
XMLScanner_t xmlLexerNew(const wxString& content);

/**
 * @brief create a scanner for a given file name
 */
XMLScanner_t xmlLexerNew(const wxFileName& filename);

/**
 * @brief destroy the current lexer and perform cleanup
 */
void xmlLexerDestroy(XMLScanner_t* scanner);

/**
 * @brief return the next token, its type, line number and columns
 */
bool xmlLexerNext(XMLScanner_t scanner, XMLLexerToken& token);

/**
 * @brief unget the last token
 */
void xmlLexerUnget(XMLScanner_t scanner);

/**
 * @brief return the current lexer token
 */
wxString xmlLexerCurrentToken(XMLScanner_t scanner);

/**
 * @brief return the associated data with this scanner
 */
XMLLexerUserData* xmlLexerGetUserData(XMLScanner_t scanner);

#endif
