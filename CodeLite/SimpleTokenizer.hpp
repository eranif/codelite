#ifndef SIMPLETOKENIZER_HPP
#define SIMPLETOKENIZER_HPP

#include "codelite_exports.h"

#include <wx/string.h>

enum eSimpleTokenizerState {
    TOKNZR_STATE_NORMAL = 0,
    TOKNZR_STATE_SINGLE_STRING = 1,
    TOKNZR_STATE_DQUOTE_STRING = 2,
    TOKNZR_STATE_ESCAPE = 3,
    TOKNZR_STATE_LINE_COMMENT = 4,
    TOKNZR_STATE_MULTILINE_COMMENT = 5,
    TOKNZR_STATE_PREPROCESSOR = 6,
};

enum eSimpleTokenizerMode {
    TOKNZR_MODE_NONE = -1,
    TOKNZR_MODE_NORMAL = 0,
    TOKNZR_MODE_COMMENTS = 1,
};

class WXDLLIMPEXP_CL SimpleTokenizer
{
public:
    struct Token {
    private:
        // token properties
        long m_token_position = wxNOT_FOUND;
        long m_token_line = wxNOT_FOUND;
        long m_token_column = wxNOT_FOUND;
        long m_token_length = wxNOT_FOUND;
        char m_following_char1 = 0; // the 1st following char after this token
        char m_following_char2 = 0; // the 2nd following char after this token

    public:
        Token(long pos, long line, long col, long len)
            : m_token_position(pos)
            , m_token_line(line)
            , m_token_column(col)
            , m_token_length(len)
        {
        }

        Token() {}

        bool ok() const
        {
            return m_token_column != wxNOT_FOUND && m_token_length != wxNOT_FOUND && m_token_line != wxNOT_FOUND;
        }

        bool following_char1_is(char c) const { return m_following_char1 == c; }
        bool following_char2_is(char c) const { return m_following_char2 == c; }
        void set_following_char1(char c) { m_following_char1 = c; }
        void set_following_char2(char c) { m_following_char2 = c; }

        bool is_valid_identifier(const wxString& input_string) const
        {
            return ok() && (input_string.length() > (size_t)m_token_position) &&
                   isalpha(input_string[m_token_position]);
        }

        void inc_length() { ++m_token_length; }

        long line() const { return m_token_line; }
        long column() const { return m_token_column; }
        long length() const { return m_token_length; }

        void clear() { m_token_length = m_token_column = m_token_line = wxNOT_FOUND; }
        wxString to_string(const wxString& source_string) const
        {
            if(!ok() || length() == 0) {
                return wxEmptyString;
            }
            return source_string.Mid(m_token_position, m_token_length);
        }
        void set_line(long line) { m_token_line = line; }
    };

private:
    const wxString& m_str;
    size_t m_pos = 0;
    long m_line = 0;
    long m_line_start_pos = 0;
    eSimpleTokenizerState m_state = TOKNZR_STATE_NORMAL;
    eSimpleTokenizerMode m_mode = TOKNZR_MODE_NONE;
    Token m_token;

public:
    SimpleTokenizer(const wxString& str);
    ~SimpleTokenizer();

    /**
     * @brief return next token details
     */
    bool next(Token* token);
    /**
     * @brief while in this mode, scan for comments only
     */
    bool next_comment(Token* token);

    /**
     * @brief remove `*` and other decorators
     * from a comment
     */
    void strip_comment(wxString& comment);
};

#endif // SIMPLETOKENIZER_HPP
