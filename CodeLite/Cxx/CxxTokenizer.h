#ifndef CXXTOKENIZER_H
#define CXXTOKENIZER_H

#include "CxxLexerAPI.h"

class WXDLLIMPEXP_CL CxxTokenizer
{
    Scanner_t m_scanner;
    wxString m_buffer;
    CxxLexerToken m_lastToken;

protected:
    CppLexerUserData* GetUserData() const;

public:
    CxxTokenizer();
    virtual ~CxxTokenizer();

    /**
     * @brief get the next token from the toknizer
     * @reutrn false when reached EOF
     */
    bool NextToken(CxxLexerToken& token);

    /**
     * @brief return the last token back to the scanner
     */
    bool UngetToken();

    /**
     * @brief reset the lexer with a new text buffer
     */
    void Reset(const wxString& buffer);

    /**
     * @brief read until 'delim' is found. Return true if 'delim' found
     * also, return the data read
     */
    bool ReadUntilClosingBracket(int delim, wxString& bufferRead);

    const CxxLexerToken& GetLastToken() const { return m_lastToken; }
    /**
     * @brief peek at the next token and return its type
     */
    int PeekToken(wxString& text);

    /**
     * @brief given an input string, return the visible scope at the end of the buffer
     * e.g.
     *
     * Input string:
     *
     * void foo() {
     *     int a;
     *     int b;
     * }
     *
     * void bar() {
     *     |
     *
     * Output:
     * void foo(){} void bar() {
     *
     */
    wxString GetVisibleScope(const wxString& inputString);

    /**
     * @brief return true if the current scanner position is placed inside a PP section
     */
    bool IsInPreProcessorSection() const;

    /**
     * @brief read until we find `type_1` or `type_2`. Return the string consumed. This function
     * consumes the delimiter found (`type_1` or `type_2`)
     * @param what_was_found return value
     * @return string consumed until delimiter found
     */
    void read_until_find(CxxLexerToken& token, int type_1, int type_2, int* what_was_found, wxString* consumed);
};

#endif // CXXTOKENIZER_H
