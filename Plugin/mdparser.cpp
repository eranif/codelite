#include "mdparser.hpp"

#include "wx/string.h"

#include <cctype>
#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <wx/crt.h>

wxChar mdparser::Tokenizer::safe_get_char(size_t pos) const
{
    if(pos >= m_text.length()) {
        return 0;
    }
    return m_text[pos];
}

#define WORD_WRAP_AT_COL 100
#define RETURN_TYPE(TYPE, STR, INCR_POS)                                                                       \
    if(TYPE == Type::T_TEXT) {                                                                                 \
        m_text_sequence++;                                                                                     \
    } else {                                                                                                   \
        m_text_sequence = 0;                                                                                   \
    }                                                                                                          \
    /* implement word wrap */                                                                                  \
    if(m_text_sequence >= WORD_WRAP_AT_COL && (wxStrncmp(STR, " ", 1) == 0 || wxStrncmp(STR, "\t", 1) == 0)) { \
        m_text_sequence = 0;                                                                                   \
        --m_pos;                                                                                               \
        return { T_EOL, "" };                                                                                  \
    } else {                                                                                                   \
        m_pos += INCR_POS;                                                                                     \
        return { TYPE, STR };                                                                                  \
    }

#define IS_ONE_OF2(c, C1, C2) (c == C1 || c == C2)
#define IS_ONE_OF3(c, C1, C2, C3) (c == C1 || c == C2 || c == C3)

std::pair<mdparser::Type, wxString> mdparser::Tokenizer::next()
{
    while(m_pos < m_text.length()) {
        wxChar ch0 = m_text[m_pos];
        wxChar ch1 = safe_get_char(m_pos + 1);
        wxChar ch2 = safe_get_char(m_pos + 2);
        wxChar ch3 = safe_get_char(m_pos + 3);
        wxChar before_ch = safe_get_char(m_pos - 1);
        ++m_pos;
        switch(ch0) {
        case '\n':
            RETURN_TYPE(T_EOL, "\n", 0);
        case '*':
            // bold & italic
            if(ch1 == '*') {
                RETURN_TYPE(T_BOLD, "**", 1);
            } else {
                RETURN_TYPE(T_ITALIC, "*", 0);
            }
            break;
        case '~':
            // strikethrough
            if(ch1 == '~') {
                RETURN_TYPE(T_STRIKE, "~~", 1);
            } else {
                RETURN_TYPE(T_TEXT, ch0, 0);
            }
            break;
        case '`':
            // code blocks ``` & `
            if(ch1 == '`' && ch2 == '`') {
                RETURN_TYPE(T_CODEBLOCK, "```", 2);
            } else {
                RETURN_TYPE(T_CODE, "`", 0);
            }
            break;
        case '#':
            // Heading, up to 3 x #
            if(ch1 == '#' && ch2 == '#') {
                RETURN_TYPE(T_H3, "###", 2);
            } else if(ch1 == '#') {
                RETURN_TYPE(T_H2, "##", 1);
            } else {
                RETURN_TYPE(T_H1, "#", 0);
            }
            break;
        case '=':
            if(IS_ONE_OF2(before_ch, '\n', 0) && ch1 == '=' && ch2 == '=' && IS_ONE_OF3(ch3, '\n', '\r', 0)) {
                RETURN_TYPE(T_HR, "===", 2);
            } else {
                RETURN_TYPE(T_TEXT, "=", 0);
            }
            break;
        case '-':
            if(IS_ONE_OF2(before_ch, '\n', 0) && ch1 == '-' && ch2 == '-' && IS_ONE_OF3(ch3, '\n', '\r', 0)) {
                RETURN_TYPE(T_HR, "---", 2);
            } else if(ch1 != '-') {
                RETURN_TYPE(T_LI, "-", 0);
            } else {
                RETURN_TYPE(T_TEXT, ch0, 0);
            }
            break;
        case '\\':
            if(m_enable_backslash_esc) {
                RETURN_TYPE(T_TEXT, ch1, 1);
            } else {
                RETURN_TYPE(T_TEXT, ch0, 0);
            }
            break;
        default:
            RETURN_TYPE(T_TEXT, ch0, 0);
        }
    }
    RETURN_TYPE(T_EOF, "", 0);
}

void mdparser::Tokenizer::consume_until(wxChar ch)
{
    for(; m_pos < m_text.length(); ++m_pos) {
        if(m_text[m_pos] == ch) {
            ++m_pos;
            break;
        }
    }
}

void mdparser::Parser::parse(const wxString& input_str, write_callback_t on_write)
{
    constexpr int STATE_NORMAL = 0;
    constexpr int STATE_CODE = 1;
    constexpr int STATE_CODEBLOCK_HEADER = 2;
    constexpr int STATE_CODEBLOCK = 3;
    int state = STATE_NORMAL;

    write_cb = std::move(on_write);
    Tokenizer tokenizer(input_str);
    Style style;
    Type last_state = T_EOF;
    wxString buffer;
    while(true) {
        auto tok = tokenizer.next();
        if(tok.first == T_EOF) {
            flush_buffer(buffer, style, false);
            break;
        }

        switch(state) {
        case STATE_NORMAL:
            switch(tok.first) {
            case T_LI:
                if(last_state == T_EOL || last_state == T_EOF) {
                    buffer << L"\u2022"; // bullet
                } else {
                    buffer << "-";
                }
                break;
            // below are style styles
            case T_BOLD:
            case T_ITALIC:
            case T_STRIKE:
                flush_buffer(buffer, style, false);
                style.toggle_property(tok.first);
                break;
            case T_H1:
            case T_H2:
            case T_H3:
                if(last_state == T_EOL || last_state == T_EOF) {
                    flush_buffer(buffer, style, false);
                    style.toggle_property(tok.first);
                } else {
                    // handle it as text
                    buffer << tok.second;
                }
                break;
            case T_CODE:
            case T_CODEBLOCK:
                state = (tok.first == T_CODE ? STATE_CODE : STATE_CODEBLOCK_HEADER);
                flush_buffer(buffer, style, false);
                style.toggle_property(tok.first);
                tokenizer.enable_backslash_esc(false);
                break;
            case T_EOL:
                // clear heading
                flush_buffer(buffer, style, true);
                style.clear_property(T_H1);
                style.clear_property(T_H2);
                style.clear_property(T_H3);
                break;
            case T_TEXT:
                buffer << tok.second;
                break;
            case T_HR:
                flush_buffer(buffer, style, false);
                notify_hr();
                tokenizer.consume_until('\n');
                break;
            case T_EOF:
                break;
            }
            break;
        case STATE_CODEBLOCK_HEADER:
            // we are looking for the first EOL
            switch(tok.first) {
            case T_EOL:
                state = STATE_CODEBLOCK;
                break;
            default:
                // ignore anything else
                break;
            }
            break;
        case STATE_CODE:
        case STATE_CODEBLOCK:
            if((state == STATE_CODE && tok.first == T_CODE) || (state == STATE_CODEBLOCK && tok.first == T_CODEBLOCK)) {
                state = STATE_NORMAL;
                flush_buffer(buffer, style, false);
                style.toggle_property(tok.first);
                tokenizer.enable_backslash_esc(true);
            } else if(tok.first == T_EOL) {
                flush_buffer(buffer, style, true);
            } else {
                if(buffer.empty() && tok.second == '\n') {
                    // skip it
                } else {
                    buffer << tok.second;
                }
            }
            break;
        }

        if(tok.first == T_TEXT && (tok.second == ' ' || tok.second == '\t')) {
            // dont change the lasta seen state for whitespace
            continue;
        }
        last_state = tok.first;
    }
}

void mdparser::Parser::flush_buffer(wxString& buffer, const Style& style, bool is_eol)
{
    if(!buffer.empty() || is_eol) {
        write_cb(buffer, style, is_eol);
    }
    buffer.clear();
}

void mdparser::Parser::notify_hr()
{
    Style style;
    style.toggle_property(T_HR);
    write_cb("", style, true);
}
