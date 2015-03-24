#include "JSExpressionParser.h"
#include <stack>
#include "JSLexerTokens.h"
#include "JSSourceFile.h"

JSExpressionParser::JSExpressionParser(const wxString& fulltext)
    : m_text(fulltext)
{
    m_expression = CreateExpression(m_text);
}

JSExpressionParser::~JSExpressionParser() {}

JSLexerToken::Vec_t JSExpressionParser::CreateExpression(const wxString& text)
{
    // Extract the expression at the end of the input text
    std::stack<JSLexerToken::Vec_t> stack;
    JSLexerToken::Vec_t tokens;
    stack.push(tokens);

    JSLexerToken::Vec_t* current = &stack.top();

    // strip the text from any comments
    JSScanner_t scanner = ::jsLexerNew(text);
    JSLexerToken token, lastToken;
    while(::jsLexerNext(scanner, token)) {
        lastToken = token;
        switch(token.type) {
        // the following are tokens that once seen
        // we should start a new expression:
        case kJS_NEW:
        case kJS_FUNCTION:
        case kJS_VAR:
        case kJS_CATCH:
        case kJS_THROW:
        case kJS_SWITCH:
        case kJS_CASE:
        case kJS_FOR:
        case kJS_STRING:
        case kJS_RETURN:
        case kJS_DEC_NUMBER:
        case kJS_OCTAL_NUMBER:
        case kJS_HEX_NUMBER:
        case kJS_FLOAT_NUMBER:
        case kJS_PLUS_PLUS:
        case kJS_MINUS_MINUS:
        case kJS_LS:
        case kJS_RS:
        case kJS_LE:
        case kJS_GE:
        case kJS_EQUAL:
        case kJS_NOT_EQUAL:
        case kJS_AND_AND:
        case kJS_OR_OR:
        case kJS_STAR_EQUAL:
        case kJS_SLASH_EQUAL:
        case kJS_DIV_EQUAL:
        case kJS_PLUS_EQUAL:
        case kJS_MINUS_EQUAL:
        case kJS_LS_ASSIGN:
        case kJS_RS_ASSIGN:
        case kJS_AND_EQUAL:
        case kJS_POW_EQUAL:
        case kJS_OR_EQUAL:
        case ';':
        case '{':
        case '}':
        case '=':
        case ':':
        case ',':
        case '!':
            if(current) current->clear();
            break;
        case '(':
        case '[':
            if(current) current->push_back(token);
            stack.push(JSLexerToken::Vec_t());
            current = &stack.top();
            break;
        case ')':
        case ']':
            if(stack.size() < 2) {
                // parse error...
                return JSLexerToken::Vec_t();
            }
            // switch back to the previous set of tokens
            stack.pop();
            current = &stack.top();
            if(current) current->push_back(token);
            break;
        default:
            if(current) current->push_back(token);
            break;
        }
    }

    ::jsLexerDestroy(&scanner);
    JSLexerToken::Vec_t result;

    if(current) {
        result.swap(*current);
    }
    return result;
}

JSObject::Ptr_t JSExpressionParser::Resolve(JSLookUpTable::Ptr_t lookup, const wxString& filename)
{
    if(m_expression.empty()) return NULL;

    // Reparse the current file
    JSSourceFile sourceFile(lookup, m_text, wxFileName(filename));
    sourceFile.Parse();

    JSObject::Ptr_t resolved(NULL);
    for(size_t i = 0; i < m_expression.size(); ++i) {
        JSLexerToken &token = m_expression.at(i);
        if(token.type == kJS_DOT) {
            if(resolved) {
                // locate the type in the lookup table
                resolved = lookup->FindObject(resolved->GetType());
                if(!resolved) return NULL;
            }
            continue;
        }
        
        if(i == 0) {
            // check to see if token is class name or a variable
            resolved = lookup->FindObject(token.text);
            if(!resolved) {
                // Check to see if this is a variable
                JSObject::Map_t localVars = lookup->GetVisibleVariables();
                JSObject::Map_t::const_iterator iter = localVars.find(token.text);
                if(iter == localVars.end()) {
                    return NULL;
                }
                resolved = iter->second;
                if(resolved->GetType().IsEmpty()) {
                    return NULL;
                }
            }
            if(!resolved) return NULL;
        } else {
            const JSObject::Map_t& properties = resolved->GetProperties();
            JSObject::Map_t::const_iterator iter = properties.find(token.text);
            if(iter == properties.end()) {
                return NULL;
            }
            resolved = iter->second;
        }
    }
    return resolved;
}
