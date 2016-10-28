//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : CxxLexerAPI.h
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

#ifndef CppLexerAPI_H__
#define CppLexerAPI_H__

#include <wx/filename.h>
#include <wx/string.h>
#include <wx/variant.h>
#include <map>
#include <list>
#include <vector>
#include <codelite_exports.h>

#if 0
#define DEBUGMSG wxPrintf
#define CL_DEBUG wxPrintf
#define CL_DEBUG1 wxPrintf
#else

#define DEBUGMSG(...) \
    do {              \
    } while(false)

#if 0
#define CL_DEBUG(...) \
    do {              \
    } while(false)

#define CL_DEBUG1(...) \
    do {               \
    } while(false)

#endif

#endif

enum eLexerOptions {
    kLexerOpt_None = 0x00000000,
    kLexerOpt_ReturnComments = 0x00000001,
    kLexerOpt_ReturnWhitespace = 0x00000002,
    kLexerOpt_CollectMacroValueNumbers = 0x00000004,
    kLexerOpt_DontCollectMacrosDefinedInThisFile = 0x00000008,
};

struct WXDLLIMPEXP_CL CxxLexerException
{
    wxString message;
    CxxLexerException(const wxString& msg)
        : message(msg)
    {
    }
};

struct WXDLLIMPEXP_CL CxxLexerToken
{
    int lineNumber;
    int column;
    char* text;
    int type;
    wxString comment;
    CxxLexerToken()
        : lineNumber(0)
        , column(0)
        , text(NULL)
        , type(0)
    {
    }

    CxxLexerToken(int tokenType)
        : lineNumber(0)
        , column(0)
        , text(NULL)
        , type(tokenType)
    {
    }
    typedef std::vector<CxxLexerToken> Vect_t;
    typedef std::list<CxxLexerToken> List_t;
};

struct WXDLLIMPEXP_CL CxxPreProcessorToken
{
    wxString name;
    wxString value;
    bool deleteOnExit;
    CxxPreProcessorToken()
        : deleteOnExit(false)
    {
    }
    typedef std::map<wxString, CxxPreProcessorToken> Map_t;
};
/**
 * @class CppLexerUserData
 */
struct WXDLLIMPEXP_CL CppLexerUserData
{
private:
    size_t m_flags;
    wxString m_comment;
    wxString m_rawStringLabel;
    int m_commentStartLine;
    int m_commentEndLine;
    FILE* m_currentPF;

public:
    void Clear()
    {
        if(m_currentPF) {
            ::fclose(m_currentPF);
            m_currentPF = NULL;
        }

        ClearComment();
        m_rawStringLabel.Clear();
    }

    CppLexerUserData(size_t options)
        : m_flags(options)
        , m_commentStartLine(wxNOT_FOUND)
        , m_commentEndLine(wxNOT_FOUND)
        , m_currentPF(NULL)
    {
    }

    ~CppLexerUserData() { Clear(); }

    void SetCurrentPF(FILE* currentPF) { this->m_currentPF = currentPF; }
    /**
     * @brief do we collect comments?
     */
    bool IsCollectingComments() const { return m_flags & kLexerOpt_ReturnComments; }
    bool IsCollectingWhitespace() const { return m_flags & kLexerOpt_ReturnWhitespace; }
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

typedef void* Scanner_t;
/**
 * @brief create a new Lexer for a buffer
 */
WXDLLIMPEXP_CL Scanner_t LexerNew(const wxString& buffer, size_t options = kLexerOpt_None);

/**
 * @brief create a scanner for a given file name
 */
WXDLLIMPEXP_CL Scanner_t LexerNew(const wxFileName& filename, size_t options);
/**
 * @brief destroy the current lexer and perform cleanup
 */
WXDLLIMPEXP_CL void LexerDestroy(Scanner_t* scanner);

/**
 * @brief return the next token, its type, line number and columns
 */
WXDLLIMPEXP_CL bool LexerNext(Scanner_t scanner, CxxLexerToken& token);

/**
 * @brief unget the last token
 */
WXDLLIMPEXP_CL void LexerUnget(Scanner_t scanner);

/**
 * @brief return the current lexer token
 */
WXDLLIMPEXP_CL wxString LexerCurrentToken(Scanner_t scanner);

/**
 * @brief return the associated data with this scanner
 */
WXDLLIMPEXP_CL CppLexerUserData* LexerGetUserData(Scanner_t scanner);
#endif
