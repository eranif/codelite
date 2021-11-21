#include "CompletionHelper.hpp"
#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"
#include <vector>

using namespace std;
namespace
{
#define PREPEND_STRING(tokn) expression.insert(expression.begin(), tokn.GetWXString())
} // namespace

CompletionHelper::CompletionHelper() {}

CompletionHelper::~CompletionHelper() {}

wxString CompletionHelper::get_expression_from_location(const wxString& file_content, size_t line, size_t character,
                                                        ITagsStoragePtr db)
{
    // tokenize the text
    CxxTokenizer tokenizer;
    tokenizer.Reset(file_content);

    CxxLexerToken token;
    vector<CxxLexerToken> tokens;
    tokens.reserve(10000);

    while(tokenizer.NextToken(token)) {
        tokens.push_back(token);
    }

    vector<wxString> expression;
    int depth = 0;
    bool cont = true;
    for(int i = static_cast<int>(tokens.size() - 1); (i >= 0) && cont; --i) {
        const auto& t = tokens[i];
        switch(t.GetType()) {
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
