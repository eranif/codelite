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

wxString CompletionHelper::get_expression(const wxString& file_content)
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
        case T_NEW:
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
    }

    // build the expression from the vector
    wxString expression_string;
    for(const auto& t : expression) {
        expression_string << t;
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
