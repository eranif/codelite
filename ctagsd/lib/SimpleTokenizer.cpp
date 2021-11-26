#include "SimpleTokenizer.hpp"

SimpleTokenizer::SimpleTokenizer(const wxString& str)
    : m_str(str)
{
}

SimpleTokenizer::~SimpleTokenizer() {}

#define RETURN_TOKEN_IF_POSSIBLE()                \
    if(m_token.ok()) {                            \
        if(!m_token.is_valid_identifier(m_str)) { \
            m_token.clear();                      \
        } else {                                  \
            *token = m_token;                     \
            m_token.clear();                      \
            return true;                          \
        }                                         \
    }

#define IS_NEXT_CHAR_EQUAL(what) (((m_pos + 1) < m_str.length()) && (what == m_str[m_pos + 1]))

#define INCREMENT_LINE() \
    m_line++;            \
    m_line_start_pos = m_pos

bool SimpleTokenizer::next(SimpleTokenizer::Token* token)
{
    SimpleTokenizerState escape_return_state = SimpleTokenizerState::DQUOTE_STRING;
    for(; m_pos < m_str.length(); ++m_pos) {
        wxChar ch = m_str[m_pos];
        switch(m_state) {
        case SimpleTokenizerState::PREPROCESSOR:
            switch(ch) {
            case '\n':
                INCREMENT_LINE();
                m_state = SimpleTokenizerState::NORMAL;
                break;
            default:
                // increments the column
                break;
            }
            break;
        case SimpleTokenizerState::NORMAL:
            switch(ch) {
            case '#':
                m_state = SimpleTokenizerState::PREPROCESSOR;
                RETURN_TOKEN_IF_POSSIBLE();
                break;
            case '/':
                if(IS_NEXT_CHAR_EQUAL('*')) {
                    ++m_pos;
                    m_state = SimpleTokenizerState::MULTILINE_COMMENT;
                } else if(IS_NEXT_CHAR_EQUAL('/')) {
                    ++m_pos;
                    m_state = SimpleTokenizerState::LINE_COMMENT;
                }
                RETURN_TOKEN_IF_POSSIBLE();
                break;
            case '\n':
                RETURN_TOKEN_IF_POSSIBLE();
                INCREMENT_LINE();
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
                    m_token = Token(m_pos, m_line, m_pos - m_line_start_pos - 1, 1);
                }
                break;
            case '"':
                m_state = SimpleTokenizerState::DQUOTE_STRING;
                RETURN_TOKEN_IF_POSSIBLE();
                break;
            case '\'':
                m_state = SimpleTokenizerState::SINGLE_STRING;
                RETURN_TOKEN_IF_POSSIBLE();
                break;
            default:
                // whitespace
                RETURN_TOKEN_IF_POSSIBLE();
                break;
            }
            break;
        case SimpleTokenizerState::DQUOTE_STRING:
            switch(ch) {
            case '"':
                m_state = SimpleTokenizerState::NORMAL;
                break;
            case '\\':
                escape_return_state = SimpleTokenizerState::DQUOTE_STRING;
                m_state = SimpleTokenizerState::ESCAPE;
                break;
            }
            break;
        case SimpleTokenizerState::SINGLE_STRING:
            switch(ch) {
            case '\'':
                m_state = SimpleTokenizerState::NORMAL;
                break;
            case '\\':
                escape_return_state = SimpleTokenizerState::SINGLE_STRING;
                m_state = SimpleTokenizerState::ESCAPE;
                break;
            }
            break;
        case SimpleTokenizerState::ESCAPE:
            m_state = escape_return_state;
            break;
        case SimpleTokenizerState::MULTILINE_COMMENT:
            switch(ch) {
            case '*':
                if(IS_NEXT_CHAR_EQUAL('/')) {
                    ++m_pos;
                    m_state = SimpleTokenizerState::NORMAL;
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
        case SimpleTokenizerState::LINE_COMMENT:
            switch(ch) {
            case '\n':
                INCREMENT_LINE();
                m_state = SimpleTokenizerState::NORMAL;
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