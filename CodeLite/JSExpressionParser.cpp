#include "JSExpressionParser.h"
#include <stack>
#include "JSLexerTokens.h"
#include "JSSourceFile.h"
#include <algorithm>
#include <wx/wxcrtvararg.h>
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
        case kJS_TRUE:
        case kJS_FALSE:
        case kJS_NULL:
        case kJS_NEW:
        case kJS_FUNCTION:
        case kJS_VAR:
        case kJS_CATCH:
        case kJS_THROW:
        case kJS_SWITCH:
        case kJS_CASE:
        case kJS_FOR:
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
            if(current) current->push_back(token);
            stack.push(JSLexerToken::Vec_t());
            current = &stack.top();
            break;
        case ')':
            if(stack.size() < 2) {
                // parse error...
                return JSLexerToken::Vec_t();
            }
            // switch back to the previous set of tokens
            stack.pop();
            current = &stack.top();

            // remove the open brace
            if(current && !current->empty() && current->back().type == '(') {
                current->pop_back();
            }

            break;
        case '[':
            if(current) current->push_back(token);
            stack.push(JSLexerToken::Vec_t());
            current = &stack.top();
            break;
        case ']':
            if(stack.size() < 2) {
                // parse error...
                return JSLexerToken::Vec_t();
            }
            // switch back to the previous set of tokens
            stack.pop();
            current = &stack.top();
            if(current) {
                current->push_back(token);
                // pop both the "[]" token from the current sequence and replace it with "Array"
                current->pop_back();                       // ]
                if(!current->empty()) current->pop_back(); // [
                JSLexerToken tok;
                tok.type = kJS_IDENTIFIER;
                tok.text = "Array";
                current->push_back(tok);
            }
            break;
        case kJS_STRING:
            token.text = "String";
            if(current) current->push_back(token);
            break;
        default:
            if(current) current->push_back(token);
            break;
        }
    }

    ::jsLexerDestroy(&scanner);
    JSLexerToken::Vec_t result;

    // special check: if the current sequence is empty and the previous one ends with "("
    // use the previous one
    // e.g. user typed: Fooname(
    if(current && current->empty() && (stack.size() > 1)) {
        stack.pop();
        current = &stack.top();
        // sanity
        if(current->empty()) return result;
        if(current->back().type != '(') return result;
    }
    
    if(current) {
        result.swap(*current);
    }
    return result;
}

JSObject::Ptr_t
JSExpressionParser::Resolve(JSLookUpTable::Ptr_t lookup, const wxString& filename, JSSourceFile* pSourceFile)
{
    if(m_expression.empty()) return NULL;


    // Determing the type of completion requested
    m_completeType = kCodeComplete;

    if(m_expression.at(m_expression.size() - 1).type == '(') {
        m_completeType = kFunctionTipComplete;
    } else if(m_expression.at(m_expression.size() - 1).type == kJS_IDENTIFIER) {
        m_completeType = kWordComplete;
    }

    // Reparse the current file
    JSSourceFile sourceFile(lookup, m_text, wxFileName(filename));
    if(!pSourceFile) {
        // Prepare the lookup for parsing a source file
        lookup->PrepareLookup();
        sourceFile.Parse();
        pSourceFile = &sourceFile;
    }

    if(IsWordComplete()) {
        // Remove the last token so it is now a "code-complete" expression
        m_wordCompleteFilter = m_expression.back().text;
        m_expression.pop_back();

        if(m_expression.empty()) {
            // after removing the last element, we have no more tokens in the expression
            // return the global scope
            return lookup->CurrentScope();
        }
    } else if(IsFunctionTipComplete()) {
        // Function calltip
        m_expression.pop_back();
        if(m_expression.empty()) return NULL;
    }

    JSObject::Ptr_t resolved(NULL);
    for(size_t i = 0; i < m_expression.size(); ++i) {
        JSLexerToken& token = m_expression.at(i);

        // ignore open paren
        if(token.type == '(') continue;

        // scope resolve operator
        if(token.type == kJS_DOT) {
            if(resolved) {
                // locate the type in the lookup table
                resolved = lookup->FindClass(resolved->GetType());
                if(!resolved) return NULL;
            }
            continue;
        }

        if(i == 0) {
            wxString classType = token.text;
            if(token.type == kJS_THIS) {
                classType = lookup->CurrentScope()->GetType();
            }
            
            // check to see if token is class name or a variable
            resolved = lookup->FindClass(classType);
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
            JSObject::Map_t properties = lookup->GetObjectProperties(resolved);
            JSObject::Map_t::const_iterator iter = properties.find(token.text);
            if(iter == properties.end()) {
                return NULL;
            }
            resolved = iter->second;
        }
    }
    return resolved;
}
