#include "CxxExpression.hpp"

#include "CxxLexerAPI.h"
#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"

CxxExpression::CxxExpression() {}

CxxExpression::~CxxExpression() {}

vector<CxxExpression> CxxExpression::from_expression(const wxString& expression, CxxExpression* remainder)
{
    CxxTokenizer tokenizer;
    CxxLexerToken tk;
    wxString cast_type;

    tokenizer.Reset(expression);
    if(handle_casting(tokenizer, &cast_type)) {
        tokenizer.Reset(cast_type);
    }

    vector<CxxExpression> arr;
    CxxExpression curexpr;

    while(tokenizer.NextToken(tk)) {
        switch(tk.GetType()) {
        case T_IDENTIFIER:
            curexpr.m_type_name = tk.GetWXString();
            break;
        case T_DOUBLE_COLONS:
        case T_ARROW:
        case '.':
            curexpr.m_operand = tk.GetType();
            arr.push_back(curexpr);
            curexpr = {};
            break;
        case '<':
            parse_template(tokenizer, &curexpr.m_init_list);
            break;
        case '(':
            parse_func_call(tokenizer, &curexpr.m_func_call_params);
            break;
        case '[':
            parse_subscript(tokenizer, &curexpr.m_subscript_params);
            break;
        case T_AUTO:
            curexpr.m_flags |= FLAGS_AUTO;
            break;
        }
    }

    if(!curexpr.m_type_name.empty() && remainder) {
        *remainder = curexpr;
    }
    return arr;
}

bool CxxExpression::parse_list(CxxTokenizer& tokenizer, wxArrayString* params, int open_char, int close_char)
{
#define ADD_IF_NOT_EMPTY(param)   \
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

        } else {
            switch(tk.GetType()) {
            case T_IDENTIFIER:
                curparam << tk.GetWXString();
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
                curparam << " " << tk.GetWXString();
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

bool CxxExpression::handle_casting(CxxTokenizer& tokenizer, wxString* cast_type)
{
    CxxLexerToken t;
    if(!tokenizer.NextToken(t)) {
        return false;
    }

    constexpr int STATE_NORMAL = 0;
    constexpr int STATE_CAST = 1;

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
    }

    // we are expecting '<'
    if(!tokenizer.NextToken(t) || t.GetType() != '<') {
        return false;
    }

    // read the cast type
    int depth = 1;
    while(tokenizer.NextToken(t)) {
        switch(t.GetType()) {
        case '<':
            depth++;
            cast_type->Append('<');
            break;
        case '>':
            depth--;
            if(depth == 0) {
                return true;
            } else {
                cast_type->Append('>');
            }
            break;
        default:
            cast_type->Append(t.GetWXString() + " ");
            break;
        }
    }
    return false;
}
