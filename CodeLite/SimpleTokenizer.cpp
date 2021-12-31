#include "SimpleTokenizer.hpp"

#include <wx/tokenzr.h>

SimpleTokenizer::SimpleTokenizer(const wxString& str)
    : m_str(str)
{
}

SimpleTokenizer::~SimpleTokenizer() {}

#define __PEEK_LOOKAHEAD_N(__N, c)       \
    if((m_pos + __N) < m_str.length()) { \
        c = m_str[m_pos + __N];          \
    } else {                             \
        c = 0;                           \
    }

#define PEEK_LOOKAHEAD_1(c) __PEEK_LOOKAHEAD_N(1, c)
#define PEEK_LOOKAHEAD_2(c) __PEEK_LOOKAHEAD_N(2, c)
#define PEEK_LOOKAHEAD_FIRST_NON_WHITESPACE(c)                                                                \
    {                                                                                                         \
                                                                                                              \
        for(size_t __cur = m_pos; __cur < m_str.length(); ++__cur) {                                          \
            if(m_str[__cur] != ' ' && m_str[__cur] != '\t' && m_str[__cur] != '\n' && m_str[__cur] != '\r') { \
                c = m_str[__cur];                                                                             \
                break;                                                                                        \
            }                                                                                                 \
        }                                                                                                     \
    }

#define PEEK_LOOKAHEAD_FIRST_2_NON_WHITESPACE(c1, c2)                                                         \
    {                                                                                                         \
        size_t __count_found = 0;                                                                             \
        for(size_t __cur = m_pos; __cur < m_str.length(); ++__cur) {                                          \
            if(m_str[__cur] != ' ' && m_str[__cur] != '\t' && m_str[__cur] != '\n' && m_str[__cur] != '\r') { \
                if(__count_found == 0) {                                                                      \
                    c1 = m_str[__cur];                                                                        \
                    __count_found = 1;                                                                        \
                } else {                                                                                      \
                    c2 = m_str[__cur];                                                                        \
                    break;                                                                                    \
                }                                                                                             \
            }                                                                                                 \
        }                                                                                                     \
    }

#define RETURN_TOKEN_IF_POSSIBLE()                                                   \
    if(m_token.ok()) {                                                               \
        if(!m_token.is_valid_identifier(m_str)) {                                    \
            m_token.clear();                                                         \
        } else {                                                                     \
            *token = m_token;                                                        \
            char following_char1 = 0;                                                \
            char following_char2 = 0;                                                \
            PEEK_LOOKAHEAD_FIRST_2_NON_WHITESPACE(following_char1, following_char2); \
            token->set_following_char1(following_char1);                             \
            token->set_following_char2(following_char2);                             \
            m_token.clear();                                                         \
            ++m_pos;                                                                 \
            return true;                                                             \
        }                                                                            \
    }

#define RETURN_COMMENT_TOKEN_IF_POSSIBLE() \
    if(m_token.ok()) {                     \
        *token = m_token;                  \
        m_token.clear();                   \
        ++m_pos;                           \
        return true;                       \
    }

#define LOOKAHEAD_1(what) (((m_pos + 1) < m_str.length()) && (what == m_str[m_pos + 1]))
#define LOOKAHEAD_2(what) (((m_pos + 2) < m_str.length()) && (what == m_str[m_pos + 2]))

#define INCREMENT_LINE() \
    m_line++;            \
    m_line_start_pos = m_pos

#define CHECK_TOKENIZER_MODE(expected_mode) \
    if(m_mode == TOKNZR_MODE_NONE) {        \
        m_mode = expected_mode;             \
    }                                       \
    if(m_mode != expected_mode) {           \
        return false;                       \
    }

bool SimpleTokenizer::next(SimpleTokenizer::Token* token)
{
    CHECK_TOKENIZER_MODE(TOKNZR_MODE_NORMAL);

    eSimpleTokenizerState escape_return_state = TOKNZR_STATE_DQUOTE_STRING;
    for(; m_pos < m_str.length(); ++m_pos) {
        wxChar ch = m_str[m_pos];
        switch(m_state) {
        case TOKNZR_STATE_PREPROCESSOR:
            switch(ch) {
            case '\n':
                INCREMENT_LINE();
                m_state = TOKNZR_STATE_NORMAL;
                break;
            default:
                // increments the column
                break;
            }
            break;
        case TOKNZR_STATE_NORMAL:
            switch(ch) {
            case '#':
                m_state = TOKNZR_STATE_PREPROCESSOR;
                RETURN_TOKEN_IF_POSSIBLE();
                break;
            case '/':
                if(LOOKAHEAD_1('*')) {
                    ++m_pos;
                    m_state = TOKNZR_STATE_MULTILINE_COMMENT;
                } else if(LOOKAHEAD_1('/')) {
                    ++m_pos;
                    m_state = TOKNZR_STATE_LINE_COMMENT;
                }
                RETURN_TOKEN_IF_POSSIBLE();
                break;
            case '\n':
                INCREMENT_LINE();
                RETURN_TOKEN_IF_POSSIBLE();
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case '_':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if(m_token.ok()) {
                    m_token.inc_length();
                } else {
                    // start a new token
                    m_token = Token(m_pos, m_line, m_pos == 0 ? 0 : (m_pos - m_line_start_pos - 1), 1);
                }
                break;
            case '"':
                m_state = TOKNZR_STATE_DQUOTE_STRING;
                RETURN_TOKEN_IF_POSSIBLE();
                break;
            case '\'':
                m_state = TOKNZR_STATE_SINGLE_STRING;
                RETURN_TOKEN_IF_POSSIBLE();
                break;
            default:
                // whitespace
                RETURN_TOKEN_IF_POSSIBLE();
                break;
            }
            break;
        case TOKNZR_STATE_DQUOTE_STRING:
            switch(ch) {
            case '"':
                m_state = TOKNZR_STATE_NORMAL;
                break;
            case '\\':
                escape_return_state = TOKNZR_STATE_DQUOTE_STRING;
                m_state = TOKNZR_STATE_ESCAPE;
                break;
            }
            break;
        case TOKNZR_STATE_SINGLE_STRING:
            switch(ch) {
            case '\'':
                m_state = TOKNZR_STATE_NORMAL;
                break;
            case '\\':
                escape_return_state = TOKNZR_STATE_SINGLE_STRING;
                m_state = TOKNZR_STATE_ESCAPE;
                break;
            }
            break;
        case TOKNZR_STATE_ESCAPE:
            m_state = escape_return_state;
            break;
        case TOKNZR_STATE_MULTILINE_COMMENT:
            switch(ch) {
            case '*':
                if(LOOKAHEAD_1('/')) {
                    ++m_pos;
                    m_state = TOKNZR_STATE_NORMAL;
                }
                break;
            case '\n':
                INCREMENT_LINE();
                break;
            default:
                // increments the column
                break;
            }
            break;
        case TOKNZR_STATE_LINE_COMMENT:
            switch(ch) {
            case '\n':
                INCREMENT_LINE();
                m_state = TOKNZR_STATE_NORMAL;
                break;
            default:
                // increments the column
                break;
            }
            break;
        }
    }
    // eof
    RETURN_TOKEN_IF_POSSIBLE();
    return false;
}

bool SimpleTokenizer::next_comment(Token* token)
{
    CHECK_TOKENIZER_MODE(TOKNZR_MODE_COMMENTS);
    eSimpleTokenizerState escape_return_state = TOKNZR_STATE_DQUOTE_STRING;
    for(; m_pos < m_str.length(); ++m_pos) {
        wxChar ch = m_str[m_pos];
        switch(m_state) {
        case TOKNZR_STATE_PREPROCESSOR:
            switch(ch) {
            case '\n':
                INCREMENT_LINE();
                m_state = TOKNZR_STATE_NORMAL;
                break;
            default:
                // increments the column
                break;
            }
            break;
        case TOKNZR_STATE_NORMAL:
            switch(ch) {
            case '#':
                m_state = TOKNZR_STATE_PREPROCESSOR;
                break;
            case '/':
                if(LOOKAHEAD_1('*')) {
                    // check for Javadoc or Qt style
                    if(LOOKAHEAD_2('*') || LOOKAHEAD_2('!')) {
                        ++m_pos;
                    }
                    ++m_pos;
                    m_state = TOKNZR_STATE_MULTILINE_COMMENT;
                    m_token = Token(m_pos + 1, m_line, m_pos - m_line_start_pos, 0);
                } else if(LOOKAHEAD_1('/')) {
                    if(LOOKAHEAD_2('/') || LOOKAHEAD_2('!')) {
                        ++m_pos;
                    }
                    ++m_pos;
                    m_state = TOKNZR_STATE_LINE_COMMENT;
                    m_token = Token(m_pos + 1, m_line, m_pos - m_line_start_pos, 0);
                }
                break;
            case '\n':
                INCREMENT_LINE();
                break;
            case '"':
                m_state = TOKNZR_STATE_DQUOTE_STRING;
                break;
            case '\'':
                m_state = TOKNZR_STATE_SINGLE_STRING;
                break;
            default:
                // whitespace
                break;
            }
            break;
        case TOKNZR_STATE_DQUOTE_STRING:
            switch(ch) {
            case '"':
                m_state = TOKNZR_STATE_NORMAL;
                break;
            case '\\':
                escape_return_state = TOKNZR_STATE_DQUOTE_STRING;
                m_state = TOKNZR_STATE_ESCAPE;
                break;
            }
            break;
        case TOKNZR_STATE_SINGLE_STRING:
            switch(ch) {
            case '\'':
                m_state = TOKNZR_STATE_NORMAL;
                break;
            case '\\':
                escape_return_state = TOKNZR_STATE_SINGLE_STRING;
                m_state = TOKNZR_STATE_ESCAPE;
                break;
            }
            break;
        case TOKNZR_STATE_ESCAPE:
            m_state = escape_return_state;
            break;
        case TOKNZR_STATE_MULTILINE_COMMENT:
            switch(ch) {
            case '*':
                if(LOOKAHEAD_1('*') && LOOKAHEAD_2('/')) {
                    m_pos += 2;
                    m_state = TOKNZR_STATE_NORMAL;
                    m_token.set_line(m_line);
                    RETURN_COMMENT_TOKEN_IF_POSSIBLE();
                } else if(LOOKAHEAD_1('/')) {
                    m_pos += 1;
                    m_state = TOKNZR_STATE_NORMAL;
                    m_token.set_line(m_line);
                    RETURN_COMMENT_TOKEN_IF_POSSIBLE();
                } else {
                    m_token.inc_length();
                }
                break;
            case '\n':
                INCREMENT_LINE();
                break;
            default:
                m_token.inc_length();
                break;
            }
            break;
        case TOKNZR_STATE_LINE_COMMENT:
            switch(ch) {
            case '\n':
                m_token.set_line(m_line);
                INCREMENT_LINE();
                m_state = TOKNZR_STATE_NORMAL;
                RETURN_COMMENT_TOKEN_IF_POSSIBLE();
                break;
            default:
                // increments the column
                m_token.inc_length();
                break;
            }
            break;
        }
    }
    // eof
    RETURN_COMMENT_TOKEN_IF_POSSIBLE();
    return false;
}

thread_local wxString LEFT_TRIM("*!<\r\n\t\v/ ");
thread_local wxString RIGHT_TRIM(" \n\t\v\r/");

void SimpleTokenizer::strip_comment(wxString& comment)
{
    wxArrayString lines = ::wxStringTokenize(comment, "\n", wxTOKEN_STRTOK);
    comment.clear();
    for(auto& line : lines) {
        line.erase(0, line.find_first_not_of(LEFT_TRIM));
        line.erase(line.find_last_not_of(RIGHT_TRIM) + 1);
        if(line.empty()) {
            continue;
        }
        comment << line << "\n";
    }

    if(!comment.empty()) {
        comment.RemoveLast();
    }
}
