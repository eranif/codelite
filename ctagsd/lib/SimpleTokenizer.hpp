#ifndef SIMPLETOKENIZER_HPP
#define SIMPLETOKENIZER_HPP

#include <wx/string.h>

enum class SimpleTokenizerState {
    NORMAL = 0,
    SINGLE_STRING = 1,
    DQUOTE_STRING = 2,
    ESCAPE = 3,
    LINE_COMMENT = 4,
    MULTILINE_COMMENT = 5,
    PREPROCESSOR = 6,
};

class SimpleTokenizer
{
public:
    struct Token {
    private:
        // token properties
        long m_token_position = wxNOT_FOUND;
        long m_token_line = wxNOT_FOUND;
        long m_token_column = wxNOT_FOUND;
        long m_token_length = wxNOT_FOUND;

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
            if(!ok()) {
                return wxEmptyString;
            }
            return source_string.Mid(m_token_position, m_token_length);
        }
    };

private:
    const wxString& m_str;
    size_t m_pos = 0;
    long m_line = 0;
    long m_line_start_pos = 0;
    SimpleTokenizerState m_state = SimpleTokenizerState::NORMAL;

    Token m_token;

public:
    SimpleTokenizer(const wxString& str);
    ~SimpleTokenizer();

    /**
     * @brief return next token details
     */
    bool next(Token* token);
};

#endif // SIMPLETOKENIZER_HPP
