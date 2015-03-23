#include "JSSourceFile.h"
#include "fileutils.h"
#include "JSLexerTokens.h"
#include "JSLexerAPI.h"
#include "file_logger.h"
#include "JSFunction.h"

#define JS_NEXT_TOKEN() ::jsLexerNext(m_scanner, token)
#define JS_CHECK_TYPE_RET_FALSE(t) \
    if(token.type != t) return false

#define JS_CHECK_TYPE_RET_VOID(t) \
    if(token.type != t) return

JSSourceFile::JSSourceFile(JSLookUpTable::Ptr_t lookup, const wxFileName& filename)
    : m_filename(filename)
    , m_lookup(lookup)
    , m_depth(0)
{
    wxString fileContent;
    FileUtils::ReadFileContent(filename, fileContent);
    m_scanner = ::jsLexerNew(fileContent, kJSLexerOpt_ReturnComments);
}

JSSourceFile::JSSourceFile(JSLookUpTable::Ptr_t lookup, const wxString& fileContent, const wxFileName& filename)
    : m_filename(filename)
    , m_lookup(lookup)
    , m_depth(0)
{
    m_scanner = ::jsLexerNew(fileContent);
}

JSSourceFile::~JSSourceFile()
{
    if(m_scanner) {
        ::jsLexerDestroy(&m_scanner);
    }
}

// function Foo (
void JSSourceFile::OnFunction()
{
    CL_DEBUG("OnFunction called");

    JSLexerToken token;
    if(!NextToken(token)) return;
    
    // Named function
    if(token.type == kJS_IDENTIFIER) {

        JSObject::Ptr_t scope(new JSFunction());
        scope->SetName(token.text);
        scope->SetPath(GetCurrentPath() + "." + scope->GetName());

        // read the signature of the function
        if(ReadSignature(scope)) {
            // Read the open curly brace
            JS_NEXT_TOKEN();
            JS_CHECK_TYPE_RET_VOID('{');
            ++m_depth;
            
            // Increase the scope
            m_scopes.push_back(scope);
            
            // store this match
            m_lookup->AddObject(scope);
            
            Parse(m_depth - 1);
            m_scopes.pop_back();
        }
    }
}

bool JSSourceFile::ReadSignature(JSObject::Ptr_t scope)
{
    wxString sig;
    JSLexerToken token;

    JS_NEXT_TOKEN();
    JS_CHECK_TYPE_RET_FALSE('(');

    sig << "(";
    // read until we find the closing brace
    if(!ReadUntil(')', sig)) return false;
    scope->As<JSFunction>()->SetSignature(sig);
    return true;
}

bool JSSourceFile::ReadUntil(int until, wxString& content)
{
    JSLexerToken token;
    while(JS_NEXT_TOKEN()) {
        content << token.text;
        if(token.type == until) {
            return true;
        }
    }
    return false;
}

// statement => Foo.bar =
void JSSourceFile::Parse(int exitDepth)
{
    JSLexerToken token;
    while(NextToken(token)) {
        switch(token.type) {
        case '}':
            m_depth--;
            if(m_depth == exitDepth) return;
            break;
        case '{':
            m_depth++;
            break;
        case kJS_FUNCTION:
            OnFunction();
            break;
        case kJS_IDENTIFIER:
        case kJS_THIS:
            if(!m_scopes.empty()) {
                // try to process a :
                // this.foo =
                // or
                // this.prototype.foo =
            }
            break;
        default:
            break;
        }
    }
}

bool JSSourceFile::NextToken(JSLexerToken& token) { return ::jsLexerNext(m_scanner, token); }

wxString JSSourceFile::GetCurrentPath()
{
    wxString path;
    if(m_scopes.empty()) return path;
    for(size_t i = 0; i < m_scopes.size(); ++i) {
        path << m_scopes.at(i)->GetName() << ".";
    }
    path.RemoveLast();
    return path;
}
