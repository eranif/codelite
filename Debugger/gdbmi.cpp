#include "gdbmi.hpp"
#include <cctype>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace
{
std::unordered_map<std::string, gdbmi::eToken> words = {
    { "done", gdbmi::T_DONE },   { "running", gdbmi::T_RUNNING }, { "connected", gdbmi::T_CONNECTED },
    { "error", gdbmi::T_ERROR }, { "exit", gdbmi::T_EXIT },       { "stopped", gdbmi::T_STOPPED },
};

void trim_both(std::string& str)
{
    static std::string trimString(" \r\n\t\v");
    str.erase(0, str.find_first_not_of(trimString));
    str.erase(str.find_last_not_of(trimString) + 1);
}

void strip_double_backslashes(std::string& str)
{
    std::string fixed_str;
    fixed_str.reserve(str.length());

    char last_char = 0;
    for(size_t i = 0; i < str.length(); ++i) {
        char ch = str[i];
        if(ch == '\\' && last_char == '\\') {
            // do nothing
        } else {
            fixed_str.append(1, ch);
        }
        last_char = ch;
    }
    str.swap(fixed_str);
    trim_both(str);
}

} // namespace

gdbmi::Node::ptr_t gdbmi::Node::add_child(std::string name, std::string value)
{
    auto c = do_add_child(name);
    c->value = std::move(value);
    strip_double_backslashes(c->value);
    return c;
}

#define CHECK_EOF()                      \
    {                                    \
        if(m_buffer.length() == m_pos) { \
            *type = T_EOF;               \
            return {};                   \
        }                                \
    }

#define RETURN_TYPE(ret_type)                              \
    {                                                      \
        *type = ret_type;                                  \
        ++m_pos;                                           \
        return StringView(m_buffer.data() + m_pos - 1, 1); \
    }

gdbmi::Tokenizer::Tokenizer(StringView buffer)
    : m_buffer(buffer)
{
}

gdbmi::StringView gdbmi::Tokenizer::next_token(eToken* type)
{
    *type = T_EOF;
    StringView curbuf;

    // skip leading whitespaces
    for(; m_pos < m_buffer.length(); ++m_pos) {
        if(m_buffer[m_pos] == ' ' || m_buffer[m_pos] == '\t') {
            continue;
        }
        break;
    }

    CHECK_EOF();
    switch(m_buffer[m_pos]) {
    case '{':
        RETURN_TYPE(T_TUPLE_OPEN);
    case '}':
        RETURN_TYPE(T_TUPLE_CLOSE);
    case '[':
        RETURN_TYPE(T_LIST_OPEN);
    case ']':
        RETURN_TYPE(T_LIST_CLOSE);
    case '=':
        RETURN_TYPE(T_EQUAL);
    case '^':
        RETURN_TYPE(T_POW);
    case '*':
        RETURN_TYPE(T_STAR);
    case '+':
        RETURN_TYPE(T_PLUS);
    case '@':
        RETURN_TYPE(T_TARGET_OUTPUT);
    case '&':
        RETURN_TYPE(T_LOG_OUTPUT);
    case '~':
        RETURN_TYPE(T_STREAM_OUTPUT);
    case ',':
        RETURN_TYPE(T_COMMA);
    default:
        break;
    }

    if(m_buffer[m_pos] == '"') {
        // c-string
        ++m_pos;
        return read_string(type);
    } else {

        auto w = read_word(type);
        std::string as_str = w.to_string();
        if(words.count(as_str)) {
            *type = words[as_str];
            return w;
        } else {
            *type = T_WORD;
            return w;
        }
    }
}

gdbmi::StringView gdbmi::Tokenizer::read_string(eToken* type)
{
    constexpr int STATE_NORMAL = 0;
    constexpr int STATE_IN_ESCAPE = 1;

    int state = STATE_NORMAL;
    size_t start_pos = m_pos;
    for(; m_pos < m_buffer.length(); ++m_pos) {
        char ch = m_buffer[m_pos];
        switch(state) {
        case STATE_NORMAL:
            switch(ch) {
            case '"': {
                *type = T_CSTRING;
                auto cstr = StringView(m_buffer.data() + start_pos, m_pos - start_pos);
                // now move the position
                m_pos++;
                return cstr;
            }
            case '\\':
                state = STATE_IN_ESCAPE;
                break;
            default:
                break; // let the m_pos progress
            }
            break;
        case STATE_IN_ESCAPE:
        default:
            // we have nothing to do in this state, but only skip the escaped char
            // and return to the normal state
            state = STATE_NORMAL;
            break;
        }
    }

    // if we reached here, it means that the buffer is in complete
    *type = T_EOF;
    return {};
}

gdbmi::StringView gdbmi::Tokenizer::read_word(eToken* type)
{
    size_t start_pos = m_pos;
    while(std::isalnum(m_buffer[m_pos]) || m_buffer[m_pos] == '-' || m_buffer[m_pos] == '_') {
        ++m_pos;
    }
    *type = T_WORD;
    return StringView(m_buffer.data() + start_pos, m_pos - start_pos);
}

void gdbmi::Parser::parse(const std::string& buffer, ParsedResult* result)
{
    gdbmi::Tokenizer tokenizer(buffer);
    gdbmi::eToken token;

    constexpr int STATE_START = 0;
    constexpr int STATE_RESULT_CLASS = 1;
    constexpr int STATE_POW = 3;
    constexpr int STATE_BREAK = 4;
    int state = STATE_START; // initial state
    while(true) {
        auto s = tokenizer.next_token(&token);
        if(token == T_EOF || state == STATE_BREAK) {
            break;
        }
        switch(state) {
        case STATE_START:
            // TODO: handle ~ & @ cases here
            switch(token) {
            case T_WORD:
                // the token
                result->txid = s;
                state = STATE_POW;
                break;
            case T_POW:
                state = STATE_RESULT_CLASS;
                break;
            default:
                break;
            }
            break;
        case STATE_POW:
            if(token == T_POW) {
                state = STATE_RESULT_CLASS;
            }
            break;
        case STATE_RESULT_CLASS:
            switch(token) {
            case T_DONE:
            case T_RUNNING:
            case T_CONNECTED:
            case T_ERROR:
            case T_EXIT:
                result->result_class = s;
                state = STATE_BREAK;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    parse_properties(&tokenizer, result->tree);
}

void gdbmi::Parser::parse_properties(Tokenizer* tokenizer, Node::ptr_t parent)
{
    gdbmi::eToken token;

#define RESET_PROP() \
    name = {};       \
    value = {};

    constexpr int STATE_NAME = 0;
    constexpr int STATE_EQUAL = 1;
    constexpr int STATE_VALUE = 2;

    int state = STATE_NAME; // initial state
    StringView name;
    StringView value;
    while(true) {
        auto s = tokenizer->next_token(&token);
        if(token == T_EOF) {
            break;
        }
        if(token == T_COMMA) {
            state = STATE_NAME;
            continue;
        }

        switch(state) {
        case STATE_NAME:
            switch(token) {
            case T_CSTRING: {
                // an array look-a-like
                // create a fake entry id
                parent->add_child("", s.to_string());
                break;
            }
            case T_TUPLE_CLOSE:
            case T_LIST_CLOSE:
                return;
            case T_TUPLE_OPEN:
            case T_LIST_OPEN: {
                parse_properties(tokenizer, parent->add_child());
                state = STATE_NAME;
                RESET_PROP();
                break;
            }
            case T_WORD:
                // the name
                name = s;
                state = STATE_EQUAL; // expecting the =
                break;
            default:
                break;
            }
            break;
        case STATE_EQUAL:
            switch(token) {
            case T_EQUAL:
                state = STATE_VALUE;
                break;
            default:
                // we expect "=", if dont get one, clear the parser state
                RESET_PROP();
                state = STATE_NAME;
                break;
            }
            break;
        case STATE_VALUE:
            switch(token) {
            case T_TUPLE_CLOSE:
            case T_LIST_CLOSE:
                return;
            case T_TUPLE_OPEN:
            case T_LIST_OPEN: {
                parse_properties(tokenizer, parent->add_child(name.to_string()));
                state = STATE_NAME;
                RESET_PROP();
                break;
            }
            case T_CSTRING: {
                state = STATE_NAME;
                value = s;
                parent->add_child(name.to_string(), value.to_string());
                RESET_PROP();
                break;
            }
            default:
                break;
            }
            break;
        }
    }
#undef RESET_PROP
}

void gdbmi::Parser::print(Node::ptr_t node, int depth)
{
    if(!node->name.empty()) {
        std::cout << std::string(depth, ' ') << node->name;
    }

    if(!node->value.empty()) {
        std::cout << " -> " << node->value;
    }
    std::cout << std::endl;

    for(auto child : node->children) {
        print(child, depth + 4);
    }
}
