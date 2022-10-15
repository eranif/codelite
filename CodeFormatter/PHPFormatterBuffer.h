//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : PHPFormatterBuffer.h
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

#ifndef PHPFORMATTERBUFFER_H
#define PHPFORMATTERBUFFER_H

#include "PhpLexerAPI.h"

#include <stack>
#include <wx/string.h>

enum ePHPFormatterFlags {
    kPFF_UseTabs = (1 << 1),
    kPFF_BreakBeforeFunction = (1 << 2),
    kPFF_BreakBeforeWhile = (1 << 3),
    kPFF_BreakBeforeIf = (1 << 4),
    kPFF_BreakBeforeForeach = (1 << 5),
    kPFF_ElseOnSameLineAsClosingCurlyBrace = (1 << 6),
    kPFF_BreakAfterHeredoc = (1 << 7),
    kPFF_BreakBeforeClass = (1 << 8),
    kPFF_VerticalArrays = (1 << 9),
    kPFF_BreakAfterStringConcatentation = (1 << 10),
    kPFF_Defaults = kPFF_BreakBeforeFunction | kPFF_BreakBeforeWhile | kPFF_BreakBeforeIf | kPFF_BreakBeforeForeach |
                    kPFF_ElseOnSameLineAsClosingCurlyBrace | kPFF_VerticalArrays | kPFF_BreakAfterStringConcatentation |
                    kPFF_BreakBeforeClass,
};

struct PHPFormatterOptions {
    wxString eol;      // default: \n
    size_t indentSize; // default: 4
    size_t flags;
    PHPFormatterOptions()
        : eol("\n")
        , indentSize(4)
        , flags(kPFF_Defaults)
    {
    }
};

/**
 * @class PHPFormatterBuffer
 * A simple PHP formatter based on a PHP scanner
 */
class PHPFormatterBuffer
{
    enum eDepthCommand {
        kDepthNone,
        kDepthInc,
        kDepthDec,
        kDepthIncTemporarily,
    };

protected:
    PHPScanner_t m_scanner;
    PHPFormatterOptions m_options;
    wxString m_buffer;
    phpLexerToken m_lastToken;
    wxString m_indentString;
    bool m_openTagWithEcho;
    std::stack<phpLexerToken::Vet_t> m_stack;
    phpLexerToken::Vet_t* m_sequence;
    phpLexerToken::Vet_t m_tokensBuffer;

    // For statement
    int m_forDepth;
    bool m_insideForStatement;

    // Heredoc
    bool m_insideHereDoc;

    // Depth management
    int m_depth;

    // Is the last seen function has a comment associated with it?
    int m_lastCommentLine;

    // Paren depth "("
    int m_parenDepth;

protected:
    /**
     * @brief indent using tabs?
     */
    bool IsUseTabs() const { return m_options.flags & kPFF_UseTabs; }

    /**
     * @brief insert a new line before the current statement
     */
    void InsertSeparatorLine();

    /**
     * @brief remove everything from the buffer until we find
     * delim
     */
    void ReverseClearUntilFind(const wxString& delim);

    /**
     * @brief format doxygen comment (C-style comment)
     */
    wxString FormatDoxyComment(const wxString& comment);

    void HandleOpenCurlyBrace();
    /**
     * @brief return an indent string based on the current settings
     */
    wxString& GetIndent();

    /**
     * @brief remove one indent size from the current buffer
     */
    void UnIndent();

    /**
     * @brief remove last space character from the buffer
     */
    void RemoveLastSpace();

    void AppendEOL(eDepthCommand depth = kDepthNone);

    PHPFormatterBuffer& ProcessToken(const phpLexerToken& token);

    /**
     * @brief return the next token. This function wraps the standard ::phpLexerNext
     * but it also takes into considertation our internal buffer
     */
    bool NextToken(phpLexerToken& token);

    /**
     * @brief return the next token but place it back in the m_buffer so next call to NextToken() will
     * re-use it
     */
    bool PeekToken(phpLexerToken& token);

    /**
     * @brief process an array. We can assume that the token at this point is
     * "(" and the token previously found was "array"
     */
    void ProcessArray(int openParen, int closingChar);

    /**
     * @brief reverse find ch in the buffer and return a whitepace representing
     * the distance from the line start position and ch index
     * If 'ch' is not found, return the current line indentation string
     */
    wxString GetIndentationToLast(wxChar ch);

public:
    PHPFormatterBuffer(const wxString& buffer, const PHPFormatterOptions& options);
    virtual ~PHPFormatterBuffer();

    /**
     * @brief format the buffer (provided in the constructor)
     */
    void format();

    const wxString& GetBuffer() const { return m_buffer; }
};

#endif // PHPFORMATTERBUFFER_H
