#ifndef CppLexerAPI_H__
#define CppLexerAPI_H__

#include <wx/filename.h>
#include <wx/string.h>
#include "codelite_exports.h"

enum eLexerOptions {
    kLexerOpt_None = 0x00000000,
    kLexerOpt_ReturnComments = 0x00000001,
    kLexerOpt_ReturnWhitespace = 0x00000002,
    kLexerOpt_ReturnAllNonPhp = 0x00000004,
};

struct WXDLLIMPEXP_CL phpLexerToken {
    int type;
    wxString text;
    int lineNumber;
    int endLineNumber; // Usually, this is the same as lineNumber. Unless a multiple line token is found (heredoc,
                       // c-style comment etc)

    phpLexerToken()
        : type(-1)
        , lineNumber(-1)
        , endLineNumber(-1)
    {
    }

    bool IsNull() const { return type == -1; }
};

/**
 * @class phpLexerUserData
 */
struct WXDLLIMPEXP_CL phpLexerUserData {
private:
    size_t m_flags;
    wxString m_comment;
    wxString m_rawStringLabel;
    wxString m_string;
    int m_commentStartLine;
    int m_commentEndLine;
    bool m_insidePhp;

public:
    void Clear()
    {
        m_insidePhp = false;
        ClearComment();
        m_rawStringLabel.Clear();
        m_string.Clear();
    }

    phpLexerUserData(size_t options)
        : m_flags(options)
        , m_commentStartLine(wxNOT_FOUND)
        , m_commentEndLine(wxNOT_FOUND)
        , m_insidePhp(false)
    {
    }

    /**
     * @brief do we collect comments?
     */
    bool IsCollectingComments() const { return m_flags & kLexerOpt_ReturnComments; }
    bool IsCollectingWhitespace() const { return m_flags & kLexerOpt_ReturnWhitespace; }
    bool IsCollectingAllNonPhp() const { return m_flags & kLexerOpt_ReturnAllNonPhp; }

    void SetInsidePhp(bool insidePhp) { this->m_insidePhp = insidePhp; }
    bool IsInsidePhp() const { return m_insidePhp; }

    void SetString(const wxString& string) { this->m_string = string; }
    wxString& GetString() { return m_string; }

    void SetRawStringLabel(const wxString& rawStringLabel) { this->m_rawStringLabel = rawStringLabel; }
    const wxString& GetRawStringLabel() const { return m_rawStringLabel; }
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
        m_commentEndLine = wxNOT_FOUND;
        m_commentEndLine = wxNOT_FOUND;
    }
};

typedef void* PHPScanner_t;

/**
 * @brief create a new Lexer for a given file name
 */
WXDLLIMPEXP_CL PHPScanner_t phpLexerNew(const wxString& filename, size_t options = kLexerOpt_None);

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

#endif
