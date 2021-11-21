#include "CompletionHelper.hpp"
#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"
#include <vector>

using namespace std;
namespace
{
#define PREPEND_STRING(tokn) expression.insert(expression.begin(), text)
} // namespace

CompletionHelper::CompletionHelper() {}

CompletionHelper::~CompletionHelper() {}

wxString CompletionHelper::get_expression(const wxString& file_content, wxString* last_word)
{
    // tokenize the text
    CxxTokenizer tokenizer;
    tokenizer.Reset(file_content);

    CxxLexerToken token;
    vector<pair<wxString, int>> tokens;
    tokens.reserve(10000);

    while(tokenizer.NextToken(token)) {
        tokens.push_back({ token.GetWXString(), token.GetType() });
    }

    vector<wxString> expression;
    int depth = 0;
    bool cont = true;
    std::vector<int> types;
    for(int i = static_cast<int>(tokens.size() - 1); (i >= 0) && cont; --i) {
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
        case ']':
        case '}':
        case ')':
            PREPEND_STRING(t);
            depth++;
            break;
        case ';':
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
            if(!types.empty() && types.back() == T_IDENTIFIER) {
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
}

wxString CompletionHelper::truncate_file_to_location(const wxString& file_content, size_t line, size_t column)
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

    if(offset + column < file_content.size()) {
        return file_content.Mid(0, offset + column);
    }
    return wxEmptyString;
}
