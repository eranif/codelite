#include "CompletionHelper.hpp"
#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"
#include <vector>

using namespace std;
namespace
{
#define PREPEND_STRING(tokn) expression.insert(expression.begin(), text)

bool is_word_char(wxChar ch)
{
    return (ch >= 65 && ch <= 90)     // uppercase A-Z
           || (ch >= 97 && ch <= 122) // lowercase a-z
           || (ch >= 48 && ch <= 57)  // 0-9
           || (ch == '_');
}
} // namespace

CompletionHelper::CompletionHelper() {}

CompletionHelper::~CompletionHelper() {}

wxString CompletionHelper::get_expression(const wxString& file_content, bool for_calltip, wxString* last_word) const
{
#define LAST_TOKEN_IS(token_type) (!types.empty() && (types[types.size() - 1] == token_type))
    // tokenize the text
    CxxTokenizer tokenizer;
    tokenizer.Reset(file_content);

    CxxLexerToken token;
    vector<pair<wxString, int>> tokens;
    tokens.reserve(10000);

    while(tokenizer.NextToken(token)) {
        tokens.push_back({ token.GetWXString(), token.GetType() });
    }

    int i = static_cast<int>(tokens.size() - 1);
    bool cont = true;
    if(for_calltip) {
        // read backwards until we find the first open parentheses
        for(; (i >= 0) && cont; --i) {
            switch(tokens[i].second) {
            case '(':
                cont = false;
                break;
            default:
                break;
            }
        }
    }

    vector<wxString> expression;
    std::vector<int> types;
    int depth = 0;
    cont = true;
    for(; (i >= 0) && cont; --i) {
        const wxString& text = tokens[i].first;
        int type = tokens[i].second;
        switch(type) {
        case '[':
        case '{':
        case '(':
        case '<':
            if(depth == 0) {
                cont = false;
            } else {
                PREPEND_STRING(t);
                depth--;
            }
            break;
        case '>':
            if(depth == 0 && LAST_TOKEN_IS(T_IDENTIFIER)) {
                cont = false;
            } else {
                PREPEND_STRING(t);
                depth++;
            }
            break;
        case ']':
        case ')':
            PREPEND_STRING(t);
            depth++;
            break;
        case '}':
            if(depth == 0) {
                cont = false;
            } else {
                PREPEND_STRING(t);
                depth++;
            }
            break;
        case ';':
        case T_PP_DEFINE:
        case T_PP_DEFINED:
        case T_PP_IF:
        case T_PP_IFNDEF:
        case T_PP_IFDEF:
        case T_PP_ELSE:
        case T_PP_ELIF:
        case T_PP_LINE:
        case T_PP_PRAGMA:
        case T_PP_UNDEF:
        case T_PP_ERROR:
        case T_PP_ENDIF:
        case T_PP_IDENTIFIER:
        case T_PP_DEC_NUMBER:
        case T_PP_OCTAL_NUMBER:
        case T_PP_HEX_NUMBER:
        case T_PP_FLOAT_NUMBER:
        case T_PP_STRING:
        case T_PP_AND:
        case T_PP_OR:
        case T_PP_STATE_EXIT:
        case T_PP_INCLUDE_FILENAME:
        case T_PP_INCLUDE:
        case T_PP_GT:
        case T_PP_GTEQ:
        case T_PP_LT:
        case T_PP_LTEQ:
            cont = false;
            break;
        case '=':
            if(depth == 0) {
                cont = false;
            } else {
                PREPEND_STRING(t);
            }
            break;
        case T_IDENTIFIER:
            if(LAST_TOKEN_IS(T_IDENTIFIER)) {
                // we do not allow two consecutive T_IDENTIFIER
                cont = false;
                break;
            } else {
                PREPEND_STRING(t);
            }
            break;
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
        case ',':
        case '*':
        case '&':
        case '!':
        case '~':
        case '+':
        case '^':
        case '|':
        case '%':
        case '?':
        case '/':
        case ':':
            if(depth <= 0) {
                cont = false;
            } else {
                PREPEND_STRING(t);
            }
            break;
        default:
            PREPEND_STRING(t);
            break;
        }
        types.push_back(type);
    }

    // build the expression from the vector
    wxString expression_string;
    for(const auto& t : expression) {
        expression_string << t;
    }
    if(last_word && !expression.empty()) {
        *last_word = expression[expression.size() - 1];
    }
    return expression_string;
#undef LAST_TOKEN_IS
}

wxString CompletionHelper::truncate_file_to_location(const wxString& file_content, size_t line, size_t column,
                                                     bool only_complete_words) const
{
    size_t curline = 0;
    size_t offset = 0;

    // locate the line
    for(const auto& ch : file_content) {
        if(curline == line) {
            break;
        }
        switch(ch.GetValue()) {
        case '\n':
            ++curline;
            ++offset;
            break;
        default:
            ++offset;
            break;
        }
    }

    if(curline != line) {
        return wxEmptyString;
    }

    // columns
    offset += column;

    if(offset < file_content.size()) {
        if(only_complete_words) {
            while(true) {
                size_t next_pos = offset;

                if(next_pos < file_content.size()) {
                    wxChar next_char = file_content[next_pos];
                    if(is_word_char(next_char)) {
                        offset += 1;
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
        }
        return file_content.Mid(0, offset);
    }
    return wxEmptyString;
}

thread_local std::unordered_set<wxString> words;
bool CompletionHelper::is_cxx_keyword(const wxString& word)
{
    if(words.empty()) {
        words.insert("auto");
        words.insert("break");
        words.insert("case");
        words.insert("char");
        words.insert("const");
        words.insert("continue");
        words.insert("default");
        words.insert("define");
        words.insert("defined");
        words.insert("do");
        words.insert("double");
        words.insert("elif");
        words.insert("else");
        words.insert("endif");
        words.insert("enum");
        words.insert("error");
        words.insert("extern");
        words.insert("float");
        words.insert("for");
        words.insert("goto");
        words.insert("if");
        words.insert("ifdef");
        words.insert("ifndef");
        words.insert("include");
        words.insert("int");
        words.insert("line");
        words.insert("long");
        words.insert("bool");
        words.insert("pragma");
        words.insert("register");
        words.insert("return");
        words.insert("short");
        words.insert("signed");
        words.insert("sizeof");
        words.insert("static");
        words.insert("struct");
        words.insert("switch");
        words.insert("typedef");
        words.insert("undef");
        words.insert("union");
        words.insert("unsigned");
        words.insert("void");
        words.insert("volatile");
        words.insert("while");
        words.insert("class");
        words.insert("namespace");
        words.insert("delete");
        words.insert("friend");
        words.insert("inline");
        words.insert("new");
        words.insert("operator");
        words.insert("overload");
        words.insert("protected");
        words.insert("private");
        words.insert("public");
        words.insert("this");
        words.insert("virtual");
        words.insert("template");
        words.insert("typename");
        words.insert("dynamic_cast");
        words.insert("static_cast");
        words.insert("const_cast");
        words.insert("reinterpret_cast");
        words.insert("using");
        words.insert("throw");
        words.insert("catch");
        words.insert("nullptr");
        words.insert("noexcept");
        words.insert("override");
    }
    return words.count(word) != 0;
}

vector<wxString> CompletionHelper::split_function_signature(const wxString& signature, wxString* return_value) const
{
    // ---------------------------------------------------------------------------------------------
    // ----------------macros start-------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------
#define ADD_CURRENT_PARAM(current_param) \
    if(!current_param->empty()) {        \
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
#define REMOVE_TRAILING_SPACE()                                                         \
    if(!current_param->empty() && (*current_param)[current_param->size() - 1] == ' ') { \
        current_param->RemoveLast();                                                    \
    }

    // ---------------------------------------------------------------------------------------------
    // ----------------macros end-------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------
    CxxTokenizer tokenizer;
    tokenizer.Reset(signature);

    CxxLexerToken token;

    wxString cur_func_param;
    wxString* current_param = &cur_func_param;
    vector<wxString> args;
    vector<wxString> func_args;
    int depth = 0;

    vector<int> types;
    // search for the first opening brace
    while(tokenizer.NextToken(token)) {
        if(token.GetType() == '(') {
            depth = 1;
            break;
        }
    }

    bool done_collecting_args = false;
    while(tokenizer.NextToken(token)) {
        switch(token.GetType()) {
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
            current_param->Append(token.GetWXString()).Append(" ");
            break;
        case '*':
            if(LAST_TOKEN_IS('*')) {
                REMOVE_TRAILING_SPACE();
            }
            current_param->Append(token.GetWXString()).Append(" ");
            break;
        case T_IDENTIFIER:
            if(LAST_TOKEN_IS_CLOSING_PARENTHESES() || LAST_TOKEN_IS(T_IDENTIFIER)) {
                current_param->Append(" ");
            } else if(LAST_TOKEN_IS('*')) {
                REMOVE_TRAILING_SPACE();
            }
            current_param->Append(token.GetWXString());
            break;
        case T_ARROW:
            if(done_collecting_args) {
                // we are collecting function return value now, disregard it
            } else {
                current_param->Append(token.GetWXString());
            }
            break;
        case ',':
            if(depth == 1) {
                ADD_CURRENT_PARAM(current_param);
            } else {
                current_param->Append(", ");
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
            if(!done_collecting_args && depth == 0) {
                // reached signature end
                ADD_CURRENT_PARAM(current_param);
                func_args.swap(args);
                done_collecting_args = true;
            } else {
                if(LAST_TOKEN_IS_CLOSING_PARENTHESES() || LAST_TOKEN_IS(T_IDENTIFIER)) {
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
            if(!current_param->empty() && current_param->Last() != ' ') {
                current_param->Append(" ");
            }
            current_param->Append("= ");
            break;
        default:
            current_param->Append(token.GetWXString());
            break;
        }
        types.push_back(token.GetType());
    }

    if(!done_collecting_args) {
        // we did not complete
        func_args.swap(args);
    } else {
        // check if we have a return value
        ADD_CURRENT_PARAM(current_param);
        if(!args.empty()) {
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
