//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : JSLexerAPI.h
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

#ifndef JSLexerAPI_H__
#define JSLexerAPI_H__

#include <wx/filename.h>
#include <wx/string.h>
#include <wx/variant.h>
#include <map>
#include <vector>
#include <codelite_exports.h>

#define DEBUGMSG(...) \
    do {              \
    } while(false)

enum eJSLexerOptions {
    kJSLexerOpt_None = 0x00000000,
    kJSLexerOpt_ReturnComments = 0x00000001,
};

struct WXDLLIMPEXP_CL JSLexerException
{
    wxString message;
    JSLexerException(const wxString& msg)
        : message(msg)
    {
    }
};

struct WXDLLIMPEXP_CL JSLexerToken
{
    int lineNumber;
    int column;
    wxString text;
    int type;
    wxString comment;
    JSLexerToken()
        : lineNumber(0)
        , column(0)
        , type(0)
    {
    }
    
    JSLexerToken(int tokenType)
        : lineNumber(0)
        , column(0)
        , type(tokenType)
    {
    }
    
    void Clear() {
        lineNumber = 0;
        column = 0;
        type = 0;
        text.Clear();
    }
    typedef std::vector<JSLexerToken> Vec_t;
};

/**
 * @class JSLexerUserData
 */
struct WXDLLIMPEXP_CL JSLexerUserData
{
private:
    size_t m_flags;
    wxString m_comment;
    int m_commentStartLine;
    int m_commentEndLine;
    FILE* m_currentPF;
    
public:
    void* parserData;

public:
    void Clear()
    {
        if(m_currentPF) {
            ::fclose(m_currentPF);
            m_currentPF = NULL;
        }

        ClearComment();
        parserData = NULL;
    }

    JSLexerUserData(size_t options)
        : m_flags(options)
        , m_commentStartLine(wxNOT_FOUND)
        , m_commentEndLine(wxNOT_FOUND)
        , m_currentPF(NULL)
        , parserData(NULL)
    {
    }

    ~JSLexerUserData() { Clear(); }

    void SetCurrentPF(FILE* currentPF) { this->m_currentPF = currentPF; }
    /**
     * @brief do we collect comments?
     */
    bool IsCollectingComments() const { return m_flags & kJSLexerOpt_ReturnComments; }
    //==--------------------
    // Comment management
    //==--------------------
    void AppendToComment(const wxString& str) { m_comment << str; }
    void SetCommentEndLine(int commentEndLine) { this->m_commentEndLine = commentEndLine; }
    void SetCommentStartLine(int commentStartLine) { this->m_commentStartLine = commentStartLine; }
    int GetCommentEndLine() const { return m_commentEndLine; }
    int GetCommentStartLine() const { return m_commentStartLine; }
    const wxString& GetComment() const { return m_comment; }
    bool HasComment() const { return !m_comment.IsEmpty(); }
    /**
     * @brief clear all info collected for the last comment
     */
    void ClearComment()
    {
        m_comment.Clear();
        m_commentStartLine = wxNOT_FOUND;
        m_commentEndLine = wxNOT_FOUND;
    }
};

typedef void* JSScanner_t;
/**
 * @brief create a new Lexer for a file content
 */
WXDLLIMPEXP_CL JSScanner_t jsLexerNew(const wxString& content, size_t options = kJSLexerOpt_None);

/**
 * @brief create a scanner for a given file name
 */
WXDLLIMPEXP_CL JSScanner_t jsLexerNew(const wxFileName& filename, size_t options);
/**
 * @brief destroy the current lexer and perform cleanup
 */
WXDLLIMPEXP_CL void jsLexerDestroy(JSScanner_t* scanner);

/**
 * @brief return the next token, its type, line number and columns
 */
WXDLLIMPEXP_CL bool jsLexerNext(JSScanner_t scanner, JSLexerToken& token);

/**
 * @brief unget the last token
 */
WXDLLIMPEXP_CL void jsLexerUnget(JSScanner_t scanner);

/**
 * @brief return the current lexer token
 */
WXDLLIMPEXP_CL wxString jsLexerCurrentToken(JSScanner_t scanner);

/**
 * @brief return the associated data with this scanner
 */
WXDLLIMPEXP_CL JSLexerUserData* jsLexerGetUserData(JSScanner_t scanner);
 
#endif
