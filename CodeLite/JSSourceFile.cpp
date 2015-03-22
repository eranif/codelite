#include "JSSourceFile.h"
#include "fileutils.h"
#include "JSLexerTokens.h"
#include "JSLexerAPI.h"
#include "file_logger.h"
#include "JSFunction.h"

JSSourceFile::JSSourceFile(const wxFileName& filename)
    : m_filename(filename)
{
    wxString fileContent;
    FileUtils::ReadFileContent(filename, fileContent);
    m_scanner = ::jsLexerNew(fileContent, kJSLexerOpt_ReturnComments);
}

JSSourceFile::JSSourceFile(const wxString& fileContent, const wxFileName& filename)
    : m_filename(filename)
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
void JSSourceFile::OnGlobalFunction()
{
    //CL_DEBUG("OnFunction called");
    //JSObject::Ptr_t scope(new JSFunction());
    //scope->SetName(statement.at(1).text);
    //
    //// read the signature of the function
    //ReadSignature(scope);
    //
    //// read the "{"
    //JSLexerToken token;
    //if(!NextToken(token)) return;
    //
    //if(token.type == '{') {
    //    m_scopes.push_back(scope);
    //    CL_DEBUG("Current scope is: %s", scope->GetName());
    //    Parse(m_depth - 1);
    //    m_scopes.pop_back();
    //}
    //skipToNextStatement();
}

void JSSourceFile::ReadSignature(JSObject::Ptr_t scope)
{
    wxString sig;
    while(true) {
        JSLexerToken token;
        if(!::jsLexerNext(m_scanner, token)) return;
        if(token.type == ')') {
            sig << ")";
            break;
        }
        sig << token.type << " ";
    }
    scope->As<JSFunction>()->SetSignature(sig);
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
            OnGlobalFunction();
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

void JSSourceFile::skipToNextStatement()
{
    JSLexerToken token;
    token = JSLexerToken();
    while(NextToken(token)) {
        if(token.type == ';') {
            break;
        }
    }
}

bool JSSourceFile::NextToken(JSLexerToken& token) { return ::jsLexerNext(m_scanner, token); }
