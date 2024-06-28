#include "CxxExpression.hpp"

#include "CxxLexerAPI.h"
#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"

CxxExpression::CxxExpression() {}

CxxExpression::~CxxExpression() {}

std::vector<CxxExpression> CxxExpression::from_expression(const wxString& expression, CxxRemainder* remainder)
{
    CxxTokenizer tokenizer;
    CxxLexerToken tk;
    wxString cast_type;
    tokenizer.Reset(expression);

    if(handle_cxx_casting(tokenizer, &cast_type)) {
        tokenizer.Reset(cast_type);
    }

    std::vector<CxxExpression> arr;
    CxxExpression curexpr;

    while(tokenizer.NextToken(tk)) {
        switch(tk.GetType()) {
        case T_IDENTIFIER:
            if(curexpr.m_type_name.empty()) {
                curexpr.m_type_name = tk.GetWXString();
            }
            break;
        case T_THIS:
            curexpr.m_type_name = tk.GetWXString();
            curexpr.m_flags |= FLAGS_THIS;
            break;
        case T_DOUBLE_COLONS:
        case T_ARROW:
        case '.':
            curexpr.set_operand(tk.GetType());
            arr.push_back(curexpr);
            curexpr = {};
            break;
        case '<':
            parse_template(tokenizer, &curexpr.m_template_init_list);
            break;
        case '(':
            if(curexpr.m_type_name.empty()) {
                int delim_found = 0;
                wxString consumed;
                tokenizer.read_until_find(tk, ')', 0, &delim_found, &consumed);
                if(delim_found == 0) {
                    return {}; // unbalacned parenthesis
                }
                consumed += ".";
                auto parts = from_expression(consumed, nullptr);
                if(!parts.empty()) {
                    arr.insert(arr.end(), parts.begin(), parts.end() - 1);
                    parts.back().set_operand(0); // remove the dummy operand
                    curexpr = parts.back();
                }
            } else {
                parse_func_call(tokenizer, &curexpr.m_func_call_params);
            }
            break;
        case '[':
            curexpr.m_subscript_params.emplace_back();
            parse_subscript(tokenizer, &curexpr.m_subscript_params.back());
            break;
        case T_AUTO:
            curexpr.m_flags |= FLAGS_AUTO;
            break;
        }
    }

    if(!curexpr.m_type_name.empty() && remainder) {
        // build the remainder
        remainder->filter = curexpr.type_name();
    }

    // always copy the operand
    if(remainder && !arr.empty()) {
        // copy the operand string from the last expression in the chain
        remainder->operand_string = arr.back().operand_string();
    }
    return arr;
}

bool CxxExpression::parse_list(CxxTokenizer& tokenizer, wxArrayString* params, int open_char, int close_char)
{
#define ADD_IF_NOT_EMPTY(param)   \
    param.Trim().Trim(false);     \
    if(!param.empty()) {          \
        params->push_back(param); \
    }

    CxxLexerToken tk;
    int depth = 0;
    wxString curparam;
    while(tokenizer.NextToken(tk)) {
        if(close_char == tk.GetType()) {
            if(depth == 0) {
                ADD_IF_NOT_EMPTY(curparam);
                return true;
            } else {
                depth--;
                curparam << tk.GetWXString();
            }
            continue;

        } else if(open_char == tk.GetType()) {
            depth++;
            curparam << tk.GetWXString();
            continue;

        } else if(tk.is_keyword()) {
            continue;
        } else {
            switch(tk.GetType()) {
            case T_DOUBLE_COLONS:
            case T_IDENTIFIER:
                curparam << tk.GetWXString();
                break;
            case T_AND_AND:
            case '*':
            case '&':
                // ignore these
                break;
            case ',':
                if(depth == 0) {
                    ADD_IF_NOT_EMPTY(curparam);
                    curparam.clear();
                } else {
                    curparam << ",";
                }
                break;

            default:
                if(tk.is_builtin_type() || tk.GetType() == T_IDENTIFIER) {
                    curparam << " " << tk.GetWXString();

                } else {
                    curparam << tk.GetWXString();
                }
                break;
            }
        }
    }
    return false;
#undef ADD_IF_NOT_EMPTY
}

bool CxxExpression::parse_template(CxxTokenizer& tokenizer, wxArrayString* init_list)
{
    return parse_list(tokenizer, init_list, '<', '>');
}

bool CxxExpression::parse_subscript(CxxTokenizer& tokenizer, wxArrayString* subscript_param)
{
    return parse_list(tokenizer, subscript_param, '[', ']');
}

bool CxxExpression::parse_func_call(CxxTokenizer& tokenizer, wxArrayString* func_call_params)
{
    return parse_list(tokenizer, func_call_params, '(', ')');
}

bool CxxExpression::handle_cxx_casting(CxxTokenizer& tokenizer, wxString* cast_type)
{
    CxxLexerToken t;
    if(!tokenizer.NextToken(t)) {
        return false;
    }

    constexpr int STATE_NORMAL = 0;
    constexpr int STATE_CAST = 1;
    constexpr int STATE_C_CAST = 2;

    int state = STATE_NORMAL;
    switch(t.GetType()) {
    case T_CONST_CAST:
    case T_DYNAMIC_CAST:
    case T_REINTERPRET_CAST:
    case T_STATIC_CAST:
        state = STATE_CAST;
        break;
    default:
        break;
    }

    // did not find cast epxression
    if(state == STATE_NORMAL) {
        tokenizer.UngetToken();
        return false;
    } else if(state == STATE_C_CAST) {
        // append the remainder
        while(tokenizer.NextToken(t)) {
            cast_type->Append(t.GetWXString() + " ");
        }
        return true;
    }

    // we are expecting '<'
    if(!tokenizer.NextToken(t) || t.GetType() != '<') {
        return false;
    }

    // read the cast type
    int depth = 1;
    bool cast_found = false;
    while(!cast_found && tokenizer.NextToken(t)) {
        switch(t.GetType()) {
        case '<':
            depth++;
            cast_type->Append('<');
            break;
        case '>':
            depth--;
            if(depth == 0) {
                cast_found = true;
            } else {
                cast_type->Append('>');
            }
            break;
        default:
            cast_type->Append(t.GetWXString() + " ");
            break;
        }
    }
    if(!cast_found) {
        return false;
    }

    // next token should be `(`
    if(!tokenizer.NextToken(t) || t.GetType() != '(') {
        return false;
    }

    // read the castee
    wxArrayString dummy;
    if(!parse_func_call(tokenizer, &dummy)) {
        return false;
    }

    // append the remainder to the cast_type
    while(tokenizer.NextToken(t)) {
        cast_type->Append(t.GetWXString() + " ");
    }
    return true;
}

wxString CxxExpression::template_placeholder_to_type(const wxString& placeholder) const
{
    if(m_template_placeholder_list.empty()) {
        return wxEmptyString;
    }

    size_t index = 0;
    for(; index < m_template_placeholder_list.size(); ++index) {
        if(m_template_placeholder_list[index] == placeholder) {
            break;
        }
    }

    if(index == m_template_placeholder_list.size()) {
        return wxEmptyString;
    }

    if(index >= m_template_init_list.size()) {
        return wxEmptyString;
    }
    return m_template_init_list[index];
}

void CxxExpression::parse_template_placeholders(const wxString& expr)
{
#define CHECK_TYPE(Type)     \
    if(tk.GetType() != Type) \
    return
#define ADD_PLACHOLDER()            \
    placeholder.Trim().Trim(false); \
    if(!placeholder.empty()) {      \
        arr.Add(placeholder);       \
        placeholder.clear();        \
    }

    CxxTokenizer tokenizer;
    CxxLexerToken tk;

    tokenizer.Reset(expr);
    tokenizer.NextToken(tk);

    // optionally, we expect `template` (depending of the source of the `expr`)
    if(tk.GetType() != T_TEMPLATE) {
        tokenizer.UngetToken();
    }

    tokenizer.NextToken(tk);
    CHECK_TYPE('<');

    int depth = 1;

    wxString placeholder;
    wxArrayString arr;
    bool cont = true;
    constexpr int STATE_NORMAL = 0;
    constexpr int STATE_DEFAULT_VALUE = 1;
    int state = STATE_NORMAL;

    while(cont && tokenizer.NextToken(tk)) {
        if(tk.is_pp_keyword() || tk.is_keyword()) {
            continue;
        }
        switch(state) {
        case STATE_NORMAL:
            switch(tk.GetType()) {
            case ',':
                if(depth == 1) {
                    ADD_PLACHOLDER();
                } else {
                    placeholder << ",";
                }
                break;
            case '<':
                depth++;
                placeholder << "<";
                break;
            case '>':
                depth--;
                if(depth == 0) {
                    // we are done
                    ADD_PLACHOLDER();
                    cont = false;
                } else {
                    placeholder << ">";
                }
                break;
            case '=':
                if(depth == 1) {
                    // default value -> skip it
                    state = STATE_DEFAULT_VALUE;

                } else {
                    placeholder << "=";
                }
                break;

            default:
                placeholder << tk.GetWXString();
                if(tk.is_builtin_type() || tk.GetType() == T_IDENTIFIER) {
                    placeholder << " ";
                }
                break;
            }
            break;
        case STATE_DEFAULT_VALUE:
            // read until we find "," or end of template definition
            switch(tk.GetType()) {
            case '<':
                depth++;
                break;
            case '>':
                depth--;
                if(depth == 0) {
                    state = STATE_NORMAL;
                    // let the default state handle this case
                    tokenizer.UngetToken();
                }
                break;
            case ',':
                if(depth == 0) {
                    state = STATE_NORMAL;
                    // let the default state handle this case
                    tokenizer.UngetToken();
                }
                break;
            default:
                break;
            }
            break;
        }
    }
    m_template_placeholder_list.insert(m_template_placeholder_list.end(), arr.begin(), arr.end());

#undef CHECK_TYPE
#undef ADD_PLACHOLDER
}

wxStringMap_t CxxExpression::get_template_placeholders_map() const
{
    wxStringMap_t M;
    size_t count = std::min(m_template_placeholder_list.size(), m_template_init_list.size());
    for(size_t i = 0; i < count; i++) {
        M.insert({ m_template_placeholder_list[i], m_template_init_list[i] });
    }
    return M;
}

const wxString& CxxExpression::operand_string() const { return m_operand_string; }

void CxxExpression::set_operand(int op)
{
    m_operand = op;
    switch(m_operand) {
    case T_DOUBLE_COLONS:
        m_operand_string = "::";
        break;
    case T_ARROW:
        m_operand_string = "->";
        break;
    case '.':
        m_operand_string = ".";
        break;
    default:
        break;
    }
}

// class ContextManager : public Singleton<ContextManager>, OtherClass, SecondClass<wxString, wxArrayString> {
std::vector<wxString> CxxExpression::split_subclass_expression(const wxString& subclass_pattern)
{
    CxxLexerToken token;
    CxxTokenizer tokenizer;

    tokenizer.Reset(subclass_pattern);

    // consume everything until the `:`
    while(tokenizer.NextToken(token)) {
        if(token.GetType() == ':') {
            break;
        }
    }

    // Check for possible
    std::vector<wxString> result;
    wxString curexpr;

    int depth = 0;
    bool cont = true;
    while(cont && tokenizer.NextToken(token)) {
        if(token.is_keyword()) {
            continue;
        }
        switch(token.GetType()) {
        case ',':
            if(depth == 0) {
                result.push_back(curexpr);
                curexpr.clear();
            } else {
                curexpr << ",";
            }
            break;
        case '{':
            if(depth == 0) {
                cont = false;
            } else {
                depth++;
                curexpr << "{";
            }
            break;
        case '[':
        case '<':
        case '(':
            depth++;
            curexpr << token.GetWXString();
            break;
        case '}':
        case ']':
        case '>':
        case ')':
            depth--;
            curexpr << token.GetWXString();
            break;
        default:
            curexpr << token.GetWXString() << " ";
            break;
        }
    }

    if(!curexpr.empty()) {
        result.push_back(curexpr);
    }
    return result;
}

void CxxExpression::set_subscript_params(const std::vector<wxArrayString>& params) { m_subscript_params = params; }
