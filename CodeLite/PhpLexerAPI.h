//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PhpLexerAPI.h
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

#ifndef PhpLexerAPI_H__
#define PhpLexerAPI_H__

#include <string>
#include <vector>
#include <wx/filename.h>

#include "codelite_exports.h"
#include "PHPScannerTokens.h"

enum ePhpLexerOptions {
    kPhpLexerOpt_None = 0x00000000,
    kPhpLexerOpt_ReturnComments = 0x00000001,
    kPhpLexerOpt_ReturnWhitespace = 0x00000002,
    kPhpLexerOpt_ReturnAllNonPhp = 0x00000004,
};

struct WXDLLIMPEXP_CL phpLexerToken {
private:
    std::string text;
    wxString wtext;

public:
    int type;
    int lineNumber;
    int endLineNumber; // Usually, this is the same as lineNumber. Unless a multiple line token is found (heredoc,
                       // c-style comment etc)
public:
    phpLexerToken()
        : type(-1)
        , lineNumber(-1)
        , endLineNumber(-1)
    {
    }

    /**
     * @brief clear the token, i.e. IsNull() return true
     */
    void Clear()
    {
        type = -1;
        lineNumber = -1;
        endLineNumber = -1;
        text.clear();
        wtext.clear();
    }

    void ClearText()
    {
        text.clear();
        wtext.clear();
    }

    void SetText(const std::string& t)
    {
        text = t;
        wtext = wxString(text.c_str(), wxConvUTF8);
    }

    const wxString& Text() const { return wtext; }
    bool IsNull() const { return type == -1; }
    /**
     * @brief is the current token a comment? (c++ or c comment)
     */
    bool IsAnyComment() const { return type == kPHP_T_C_COMMENT || type == kPHP_T_CXX_COMMENT; }
    /**
     * @brief is the current token a PHP Doc comment?
     */
    bool IsDocComment() const { return type == kPHP_T_C_COMMENT; }
    typedef std::vector<phpLexerToken> Vet_t;
};

/**
 * @class phpLexerUserData
 */
struct WXDLLIMPEXP_CL phpLexerUserData {
private:
    size_t m_flags;
    std::string m_comment;
    std::string m_rawStringLabel;
    std::string m_string;
    int m_commentStartLine;
    int m_commentEndLine;
    bool m_insidePhp;
    FILE* m_fp;

public:
    void Clear()
    {
        if(m_fp) {
            ::fclose(m_fp);
        }
        m_fp = NULL;
        m_insidePhp = false;
        ClearComment();
        m_rawStringLabel.clear();
        m_string.clear();
    }

    phpLexerUserData(size_t options)
        : m_flags(options)
        , m_commentStartLine(wxNOT_FOUND)
        , m_commentEndLine(wxNOT_FOUND)
        , m_insidePhp(false)
        , m_fp(NULL)
    {
    }

    ~phpLexerUserData() { Clear(); }
    void SetFp(FILE* fp) { this->m_fp = fp; }
    /**
     * @brief do we collect comments?
     */
    bool IsCollectingComments() const { return m_flags & kPhpLexerOpt_ReturnComments; }
    bool IsCollectingWhitespace() const { return m_flags & kPhpLexerOpt_ReturnWhitespace; }
    bool IsCollectingAllNonPhp() const { return m_flags & kPhpLexerOpt_ReturnAllNonPhp; }
    void SetCollectingWhitespace(bool b)
    {
        if(b) {
            m_flags |= kPhpLexerOpt_ReturnWhitespace;
        } else {
            m_flags &= ~kPhpLexerOpt_ReturnWhitespace;
        }
    }
    void SetInsidePhp(bool insidePhp) { this->m_insidePhp = insidePhp; }
    bool IsInsidePhp() const { return m_insidePhp; }

    void SetString(const std::string& string) { this->m_string = string; }
    std::string& GetString() { return m_string; }

    void SetRawStringLabel(const std::string& rawStringLabel) { this->m_rawStringLabel = rawStringLabel; }
    const std::string& GetRawStringLabel() const { return m_rawStringLabel; }
    //==--------------------
    // Comment management
    //==--------------------
    void AppendToComment(const std::string& str) { m_comment.append(str); }
    void AppendToComment(char c) { m_comment.append(std::string(1, c)); }
    void SetCommentEndLine(int commentEndLine) { this->m_commentEndLine = commentEndLine; }
    void SetCommentStartLine(int commentStartLine) { this->m_commentStartLine = commentStartLine; }
    int GetCommentEndLine() const { return m_commentEndLine; }
    int GetCommentStartLine() const { return m_commentStartLine; }
    const std::string& GetComment() const { return m_comment; }
    bool HasComment() const { return !m_comment.empty(); }
    /**
     * @brief clear all info collected for the last comment
     */
    void ClearComment()
    {
        m_comment.clear();
        m_commentStartLine = wxNOT_FOUND;
        m_commentEndLine = wxNOT_FOUND;
    }
};

typedef void* PHPScanner_t;

/**
 * @brief create a new Lexer for a given file name
 */
WXDLLIMPEXP_CL PHPScanner_t phpLexerNew(const wxFileName& filename, size_t options = kPhpLexerOpt_None);

/**
 * @brief return the user data associated with this scanner
 */
WXDLLIMPEXP_CL phpLexerUserData* phpLexerGetUserData(PHPScanner_t scanner);

/**
 * @brief create a new Lexer for a given file content
 */
WXDLLIMPEXP_CL PHPScanner_t phpLexerNew(const wxString& content, size_t options = kPhpLexerOpt_None);

/**
 * @brief destroy the current lexer and perform cleanup
 */
WXDLLIMPEXP_CL void phpLexerDestroy(PHPScanner_t* scanner);

/**
 * @brief return the next token, its type, line number and columns
 */
WXDLLIMPEXP_CL bool phpLexerNext(PHPScanner_t scanner, phpLexerToken& token);

/**
 * @brief return true if we are currently inside a PHP block
 */
WXDLLIMPEXP_CL bool phpLexerIsPHPCode(PHPScanner_t scanner);

/**
 * @brief peek at the next token
 */
WXDLLIMPEXP_CL void phpLexerUnget(PHPScanner_t scanner);

/**
 * @class PHPScannerLocker
 * @brief a wrapper around the C API for PHPScanner
 */
struct WXDLLIMPEXP_CL PHPScannerLocker {
    PHPScanner_t scanner;
    PHPScannerLocker(const wxString& content, size_t options = kPhpLexerOpt_None)
    {
        scanner = ::phpLexerNew(content, options);
    }

    PHPScannerLocker(const wxFileName& filename, size_t options = kPhpLexerOpt_None)
    {
        scanner = ::phpLexerNew(filename, options);
    }

    PHPScannerLocker(PHPScanner_t phpScanner)
        : scanner(phpScanner)
    {
    }

    ~PHPScannerLocker()
    {
        if(scanner) {
            ::phpLexerDestroy(&scanner);
        }
    }
};

#endif
