#include "CompletionHelper.hpp"

#include "Cxx/CxxScannerTokens.h"
#include "Cxx/CxxTokenizer.h"

#include <wx/regex.h>

std::vector<wxString>
CompletionHelper::split_function_signature(const wxString& signature, wxString* return_value, size_t flags) const
{
    // ---------------------------------------------------------------------------------------------
    // ----------------macros start-------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------
#define ADD_CURRENT_PARAM(current_param) \
    current_param->Trim().Trim(false);   \
    if (!current_param->empty()) {       \
        args.push_back(*current_param);  \
    }                                    \
    current_param->clear();
#define LAST_TOKEN_IS(token_type) (!types.empty() && (types[types.size() - 1] == token_type))
#define LAST_TOKEN_IS_ONE_OF_2(t1, t2) (LAST_TOKEN_IS(t1) || LAST_TOKEN_IS(t2))
#define LAST_TOKEN_IS_ONE_OF_3(t1, t2, t3) (LAST_TOKEN_IS_ONE_OF_2(t1, t2) || LAST_TOKEN_IS(t3))
#define LAST_TOKEN_IS_ONE_OF_4(t1, t2, t3, t4) (LAST_TOKEN_IS_ONE_OF_3(t1, t2, t3) || LAST_TOKEN_IS(t4))
#define LAST_TOKEN_IS_ONE_OF_5(t1, t2, t3, t4, t5) (LAST_TOKEN_IS_ONE_OF_4(t1, t2, t3, t4) || LAST_TOKEN_IS(t5))
#define LAST_TOKEN_IS_CLOSING_PARENTHESES() LAST_TOKEN_IS_ONE_OF_4('}', ']', '>', ')')
#define LAST_TOKEN_IS_OPEN_PARENTHESES() LAST_TOKEN_IS_ONE_OF_4('{', '[', '<', '(')
#define REMOVE_TRAILING_SPACE()                                                          \
    if (!current_param->empty() && (*current_param)[current_param->size() - 1] == ' ') { \
        current_param->RemoveLast();                                                     \
    }
#define APPEND_SPACE_IF_MISSING()                                                          \
    if (!current_param->empty() && ((*current_param)[current_param->size() - 1] != ' ')) { \
        current_param->Append(" ");                                                        \
    }

    // ---------------------------------------------------------------------------------------------
    // ----------------macros end-------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------
    CxxTokenizer tokenizer;
    tokenizer.Reset(signature);

    CxxLexerToken token;

    wxString cur_func_param;
    wxString* current_param = &cur_func_param;
    std::vector<wxString> args;
    std::vector<wxString> func_args;
    int depth = 0;

    std::vector<int> types;
    // search for the first opening brace
    while (tokenizer.NextToken(token)) {
        if (token.GetType() == '(') {
            depth = 1;
            break;
        }
    }

    bool done_collecting_args = false;
    constexpr int STATE_NORMAL = 0;
    constexpr int STATE_DEFAULT_VALUE = 1;
    int state = STATE_NORMAL;
    while (tokenizer.NextToken(token)) {
        switch (state) {
        case STATE_DEFAULT_VALUE:
            // consume everything until we reach signature end
            // or until we hit a "," (where depth==1)
            switch (token.GetType()) {
            case '<':
            case '{':
            case '(':
            case '[':
                depth++;
                break;
            case '>':
            case '}':
            case ']':
                depth--;
                break;
            case ')':
                depth--;
                if (depth == 0) {
                    // end of argument reading, switch back to the normal state
                    tokenizer.UngetToken();
                    // restore the depth
                    depth++;
                    state = STATE_NORMAL;
                }
                break;
            case ',':
                if (depth == 1) {
                    tokenizer.UngetToken();
                    state = STATE_NORMAL;
                }
                break;
            default:
                break;
            }
            break;
        case STATE_NORMAL:
            switch (token.GetType()) {
            case T_ALIGNAS:
            case T_ALIGNOF:
            case T_AND:
            case T_AND_EQ:
            case T_ASM:
            case T_AUTO:
            case T_BITAND:
            case T_BITOR:
            case T_BOOL:
            case T_BREAK:
            case T_CATCH:
            case T_CHAR:
            case T_CHAR16_T:
            case T_CHAR32_T:
            case T_CLASS:
            case T_COMPL:
            case T_CONST:
            case T_CONSTEXPR:
            case T_CONST_CAST:
            case T_CONTINUE:
            case T_DECLTYPE:
            case T_DEFAULT:
            case T_DELETE:
            case T_DO:
            case T_DOUBLE:
            case T_DYNAMIC_CAST:
            case T_ELSE:
            case T_ENUM:
            case T_EXPLICIT:
            case T_EXPORT:
            case T_EXTERN:
            case T_FALSE:
            case T_FINAL:
            case T_FLOAT:
            case T_FOR:
            case T_FRIEND:
            case T_GOTO:
            case T_IF:
            case T_INLINE:
            case T_INT:
            case T_LONG:
            case T_MUTABLE:
            case T_NAMESPACE:
            case T_NEW:
            case T_NOEXCEPT:
            case T_NOT:
            case T_NOT_EQ:
            case T_NULLPTR:
            case T_OPERATOR:
            case T_OR:
            case T_OR_EQ:
            case T_OVERRIDE:
            case T_PRIVATE:
            case T_PROTECTED:
            case T_PUBLIC:
            case T_REGISTER:
            case T_REINTERPRET_CAST:
            case T_CASE:
            case T_SHORT:
            case T_SIGNED:
            case T_SIZEOF:
            case T_STATIC:
            case T_STATIC_ASSERT:
            case T_STATIC_CAST:
            case T_STRUCT:
            case T_SWITCH:
            case T_TEMPLATE:
            case T_THREAD_LOCAL:
            case T_THROW:
            case T_TRUE:
            case T_TRY:
            case T_TYPEDEF:
            case T_TYPEID:
            case T_TYPENAME:
            case T_UNION:
            case T_UNSIGNED:
            case T_USING:
            case T_VIRTUAL:
            case T_VOID:
            case T_VOLATILE:
            case T_WCHAR_T:
            case T_WHILE:
            case T_XOR:
            case T_XOR_EQ:
            case T_STRING:
            case T_DOT_STAR:
            case T_ARROW_STAR:
            case T_PLUS_PLUS:
            case T_MINUS_MINUS:
            case T_LS:
            case T_LE:
            case T_GE:
            case T_EQUAL:
            case T_NOT_EQUAL:
            case T_AND_AND:
            case T_OR_OR:
            case T_STAR_EQUAL:
            case T_SLASH_EQUAL:
            case T_DIV_EQUAL:
            case T_PLUS_EQUAL:
            case T_MINUS_EQUAL:
            case T_LS_ASSIGN:
            case T_RS_ASSIGN:
            case T_AND_EQUAL:
            case T_POW_EQUAL:
            case T_OR_EQUAL:
            case T_3_DOTS:
            case '&':
            case ':':
                current_param->Append(token.GetWXString());
                APPEND_SPACE_IF_MISSING();
                break;
            case '*':
                if (LAST_TOKEN_IS('*')) {
                    REMOVE_TRAILING_SPACE();
                }
                current_param->Append(token.GetWXString());
                APPEND_SPACE_IF_MISSING();
                break;
            case T_IDENTIFIER: {
                wxString peeked_token_text;
                bool add_identifier = true;
                int next_token_type = tokenizer.PeekToken(peeked_token_text);
                // Check if we want to ignore the argument name
                if ((depth == 1) && (next_token_type == ',' || next_token_type == '=' || next_token_type == ')') &&
                    (flags & STRIP_NO_NAME)) {
                    // two consecutive T_IDENTIFIER, don't add it
                    add_identifier = false;
                } else if (LAST_TOKEN_IS_CLOSING_PARENTHESES() || LAST_TOKEN_IS_ONE_OF_2(T_IDENTIFIER, '*')) {
                    APPEND_SPACE_IF_MISSING();
                }

                if (add_identifier) {
                    current_param->Append(token.GetWXString());
                }
            } break;
            case T_ARROW:
                if (done_collecting_args) {
                    // we are collecting function return value now, disregard it
                } else {
                    current_param->Append(token.GetWXString());
                }
                break;
            case ',':
                if (depth == 1) {
                    ADD_CURRENT_PARAM(current_param);
                } else {
                    current_param->Append(",");
                    APPEND_SPACE_IF_MISSING();
                }
                break;
            case '>':
            case ']':
            case '}':
                depth--;
                REMOVE_TRAILING_SPACE();
                current_param->Append(token.GetWXString());
                break;
            case ')':
                depth--;
                if (!done_collecting_args && depth == 0) {
                    // reached signature end
                    ADD_CURRENT_PARAM(current_param);
                    func_args.swap(args);
                    done_collecting_args = true;
                } else {
                    if (LAST_TOKEN_IS_CLOSING_PARENTHESES() || LAST_TOKEN_IS(T_IDENTIFIER)) {
                        REMOVE_TRAILING_SPACE();
                    }
                    current_param->Append(token.GetWXString());
                }
                break;
            case '[':
            case '<':
            case '(':
            case '{':
                depth++;
                REMOVE_TRAILING_SPACE();
                current_param->Append(token.GetWXString());
                break;
            case '=':
                if ((depth == 1) && (flags & STRIP_NO_DEFAULT_VALUES)) {
                    state = STATE_DEFAULT_VALUE;
                } else {
                    APPEND_SPACE_IF_MISSING();
                    current_param->Append("= ");
                }
                break;
            default:
                current_param->Append(token.GetWXString());
                break;
            }

            break;
        }
        types.push_back(token.GetType());
    }

    if (!done_collecting_args) {
        // we did not complete
        func_args.swap(args);
    } else {
        // check if we have a return value
        ADD_CURRENT_PARAM(current_param);
        if (!args.empty() && return_value) {
            *return_value = args[0].Trim().Trim(false);
        }
    }
    return func_args;
#undef ADD_CURRENT_PARAM
#undef LAST_TOKEN_IS
#undef LAST_TOKEN_IS_ONE_OF_2
#undef LAST_TOKEN_IS_ONE_OF_3
#undef LAST_TOKEN_IS_ONE_OF_4
#undef LAST_TOKEN_IS_ONE_OF_5
#undef LAST_TOKEN_IS_CLOSING_PARENTHESES
#undef LAST_TOKEN_IS_OPEN_PARENTHESES
#undef REMOVE_TRAILING_SPACE
}

thread_local wxRegEx reIncludeFile("include *[\\\"\\<]{1}([a-zA-Z0-9_/\\.\\+\\-]*)");

bool CompletionHelper::is_line_include_statement(const wxString& line) const
{
    wxString tmp_line = line;

    tmp_line.Trim().Trim(false);
    tmp_line.Replace("\t", " ");

    // search for the "#"
    wxString remainder;
    if (!tmp_line.StartsWith("#", &remainder)) {
        return false;
    }

    if (!reIncludeFile.Matches(remainder)) {
        return false;
    }
    return true;
}

bool CompletionHelper::is_include_statement(const wxString& f_content) const
{
    // read backward until we find LF
    if (f_content.empty()) {
        return false;
    }

    int i = f_content.size() - 1;
    for (; i >= 0; --i) {
        if (f_content[i] == '\n') {
            break;
        }
    }

    wxString line = f_content.Mid(i);
    return is_line_include_statement(line);
}

wxString CompletionHelper::normalize_function(const TagEntry* tag, size_t flags)
{
    wxString return_value;
    wxString fullname;

    wxString name = tag->GetName();
    wxString signature = tag->GetSignature();
    fullname << name << "(";
    std::vector<wxString> args = split_function_signature(signature, &return_value, flags);
    wxString funcsig;
    for (const wxString& arg : args) {
        funcsig << arg << ", ";
    }

    if (funcsig.EndsWith(", ")) {
        funcsig.RemoveLast(2);
    }

    fullname << funcsig << ")";
    if (tag->is_const()) {
        fullname << " const";
    }
    return fullname;
}

wxString CompletionHelper::normalize_function(TagEntryPtr tag, size_t flags)
{
    return normalize_function(tag.get(), flags);
}
