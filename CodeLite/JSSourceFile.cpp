#include "JSSourceFile.h"
#include "fileutils.h"
#include "JSLexerTokens.h"
#include "JSLexerAPI.h"
#include "file_logger.h"
#include "JSFunction.h"
#include "JSDocComment.h"

#define JS_NEXT_TOKEN() NextToken(token)
#define JS_NEXT_TOKEN_RETURN() \
    if(!NextToken(token)) return
#define JS_NEXT_TOKEN_RETURN_FALSE() \
    if(!NextToken(token)) return false

#define JS_CHECK_TYPE_RET_FALSE(t) \
    if(token.type != t) return false

#define JS_CHECK_TYPE_RETURN(t) \
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
    JSLexerToken token;
    if(!NextToken(token)) return;

    // Named function
    if(token.type == kJS_IDENTIFIER) {
        JSObject::Ptr_t func(new JSFunction());
        func->SetName(token.text);
        func->SetPath(m_lookup->MakePath(func->GetName()));
        func->SetFile(m_filename);
        func->SetLine(token.lineNumber);
        AssociateComment(func);

        m_lookup->CurrentScope()->AddChild(func);
        ParseFunction(func);
        
        // Now, parse the comment
        JSDocComment commenter;
        commenter.Process(func);
        
    } else if(token.type == '(') {
        // Unget the current token (ParseFunction requires the open brace)
        UngetToken(token);
        
        // Anon function
        static int anonCounter = 0;
        JSObject::Ptr_t func(new JSFunction());
        func->SetName(wxString::Format("__anon%d", ++anonCounter));
        func->SetPath(func->GetName());
        func->SetFile(m_filename);
        func->SetLine(token.lineNumber);
        AssociateComment(func);
        m_lookup->CurrentScope()->AddChild(func);
        ParseFunction(func);
        
        // Now, parse the comment
        JSDocComment commenter;
        commenter.Process(func);

    }
}

void JSSourceFile::ParseFunction(JSObject::Ptr_t func)
{
    JSLexerToken token;
    // read the signature of the function
    if(ReadSignature(func)) {
        // Read the open curly brace
        JS_NEXT_TOKEN();
        JS_CHECK_TYPE_RETURN('{');
        ++m_depth;

        // store this match
        m_lookup->AddObject(func);

        // Increase the scope
        m_lookup->PushScope(func);

        int exitDepth = m_depth - 1;
        Parse(exitDepth);
        
        // FIXME: finalize the function by setting its type and param
        // based on the document comment block
        
        
        // if the current scope not closed properly, don't remove it from list of scopes
        if(exitDepth == m_depth) {
            m_lookup->PopScope();
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

    wxString curarg;
    int startDepth = m_depth;
    while(JS_NEXT_TOKEN()) {

        if(curarg.IsEmpty() && token.type == kJS_IDENTIFIER) {
            curarg << token.text;
        }
        sig << token.text;

        if(token.type == '{') {
            ++m_depth;
        } else if(token.type == '}') {
            --m_depth;
        } else if(!curarg.IsEmpty() && (token.type == ',' || token.type == ')')) {
            JSObject::Ptr_t arg(new JSObject());
            arg->SetName(curarg);
            arg->SetFile(m_filename);
            arg->SetLine(token.lineNumber);
            scope->As<JSFunction>()->AddVariable(arg);
            curarg.Clear();
        }

        // breaking condition
        if((token.type == ')') && (m_depth == startDepth)) {
            break;
        } else if(m_depth < startDepth) {
            return false;
        }
    }

    scope->As<JSFunction>()->SetSignature(sig);
    return true;
}

bool JSSourceFile::ReadUntil2(int until, int until2, wxString& content)
{
    JSLexerToken token;
    int startDepth = m_depth;
    while(JS_NEXT_TOKEN()) {
        content << token.text;

        if(token.type == '{') {
            ++m_depth;
        } else if(token.type == '}') {
            --m_depth;
        }

        // breaking condition
        if((token.type == until && m_depth == startDepth) || (token.type == until2 && m_depth == startDepth)) {
            return true;
        } else if(m_depth < startDepth) {
            return false;
        }
    }
    return false;
}

bool JSSourceFile::ReadUntil(int until, wxString& content)
{
    JSLexerToken token;
    int startDepth = m_depth;
    while(JS_NEXT_TOKEN()) {
        content << token.text;

        if(token.type == '{') {
            ++m_depth;
        } else if(token.type == '}') {
            --m_depth;
        }

        // breaking condition
        if(token.type == until && m_depth == startDepth) {
            return true;
        } else if(m_depth < startDepth) {
            return false;
        }
    }
    return false;
}

//========================================
// The main loop
//========================================
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
        case kJS_VAR:
            // variable
            OnVariable();
            break;
        case kJS_IDENTIFIER: {
            JSObject::Ptr_t klass = m_lookup->FindObject(token.text);
            if(klass && klass->IsFunction()) {
                m_lookup->SetTempScope(klass);
                OnPropertyOrFunction();
                m_lookup->SwapScopes();
            }/* else if(m_lookup->GetVisibleVariables().count(token.text)) {
                // we have a variable with this name, its probably an expression
                // skip it
            }*/
        } break;
        case kJS_THIS:
            if(!m_lookup->CurrentPath().IsEmpty()) {
                // try to process a :
                // this.foo =
                // or
                // this.prototype.foo =
                OnFunctionThisProperty();
            } else {
                wxString dummy;
                // Read until we hit the } or ; (taking the current depth into account)
                ReadUntil2(';', '}', dummy);
            }
            break;
        default:
            break;
        }
    }
}

void JSSourceFile::OnFunctionThisProperty()
{
    JSLexerToken token;
    // we are inside a function body and found "this"
    if(!JS_NEXT_TOKEN()) return;
    JS_CHECK_TYPE_RETURN(kJS_DOT);

    if(!JS_NEXT_TOKEN()) return;

    // Keep the name
    wxString name;

    // we found "prototype" - read the next dot
    if(token.type == kJS_PROTOTYPE) {
        if(!JS_NEXT_TOKEN()) return;
        JS_CHECK_TYPE_RETURN(kJS_DOT);

        // the property name
        if(!JS_NEXT_TOKEN()) return;
    }
    JS_CHECK_TYPE_RETURN(kJS_IDENTIFIER);
    name = token.text;

    if(!JS_NEXT_TOKEN()) return;
    JS_CHECK_TYPE_RETURN('=');

    if(!JS_NEXT_TOKEN()) return;
    if(token.type == kJS_FUNCTION) {
        // Allocate new function obj and add it
        JSObject::Ptr_t func(new JSFunction());
        func->SetName(name);
        func->SetPath(m_lookup->MakePath(func->GetName()));
        func->SetFile(m_filename);
        func->SetLine(token.lineNumber);
        AssociateComment(func);
        m_lookup->CurrentScope()->AddChild(func);

        // From here on, its the same as normal function
        ParseFunction(func);

        // Now, parse the comment
        JSDocComment commenter;
        commenter.Process(func);

    } else {
        wxString assigment;
        ReadUntil(';', assigment);
        JSObject::Ptr_t property(new JSObject());
        property->SetName(name);
        property->SetPath(m_lookup->MakePath(property->GetName()));
        property->SetFile(m_filename);
        property->SetLine(token.lineNumber);
        AssociateComment(property);
        m_lookup->CurrentScope()->AddChild(property);
    }
}

void JSSourceFile::OnPropertyOrFunction()
{
    JSLexerToken token;
    JS_NEXT_TOKEN_RETURN();
    JS_CHECK_TYPE_RETURN(kJS_DOT);

    if(!JS_NEXT_TOKEN()) return;
    if(token.type == kJS_PROTOTYPE) {
        JS_NEXT_TOKEN_RETURN();
        JS_CHECK_TYPE_RETURN(kJS_DOT);

        JS_NEXT_TOKEN_RETURN();
    }

    JS_CHECK_TYPE_RETURN(kJS_IDENTIFIER);
    wxString name = token.text;

    JS_NEXT_TOKEN_RETURN();
    JS_CHECK_TYPE_RETURN('=');

    JS_NEXT_TOKEN_RETURN();
    if(token.type == kJS_FUNCTION) {
        // Allocate new function obj and add it
        JSObject::Ptr_t func(new JSFunction());
        func->SetName(name);
        func->SetPath(m_lookup->MakePath(func->GetName()));
        func->SetFile(m_filename);
        func->SetLine(token.lineNumber);
        AssociateComment(func);
        
        m_lookup->CurrentScope()->AddChild(func);

        // From here on, its the same as normal function
        ParseFunction(func);
        
        // Now, parse the comment
        JSDocComment commenter;
        commenter.Process(func);

    } else {
        // variable
        wxString assigment;
        ReadUntil(';', assigment);
        JSObject::Ptr_t property(new JSObject());
        property->SetName(name);
        property->SetPath(m_lookup->MakePath(property->GetName()));
        property->SetFile(m_filename);
        property->SetLine(token.lineNumber);
        AssociateComment(property);
        
        m_lookup->CurrentScope()->AddChild(property);
    }
}

void JSSourceFile::OnVariable()
{
    JSLexerToken token;
    JS_NEXT_TOKEN_RETURN();
    JS_CHECK_TYPE_RETURN(kJS_IDENTIFIER);

    wxString name = token.text;
    if(!NextToken(token)) return;

    JSObject::Ptr_t var(new JSObject());
    var->SetName(name);
    var->SetLine(token.lineNumber);
    var->SetFile(m_filename);
    AssociateComment(var);
    m_lookup->CurrentScope()->As<JSFunction>()->AddVariable(var);
    if(token.type != '=') {
        return;
    }

    wxString content;
    if(ReadUntil(';', content)) {
        content.Trim().Trim(false);
        if(content.StartsWith("new")) {
            content = content.Mid(3);
            content = content.BeforeFirst('(');
            content.Trim().Trim(false);
            var->SetType(content);
        }
    }
}

bool JSSourceFile::NextToken(JSLexerToken& token)
{
    if(m_lastToken.type != 0) {
        // Reuse the last token
        token = m_lastToken;
        m_lastToken.Clear();
        return true;
    } else {
        bool res = ::jsLexerNext(m_scanner, token);
        if(res && token.type == kJS_C_COMMENT) {
            // doc comment
            m_lastCommentBlock = token;
        }
        return res;
    }
}

void JSSourceFile::UngetToken(const JSLexerToken& token) { m_lastToken = token; }

void JSSourceFile::AssociateComment(JSObject::Ptr_t obj)
{
    if((obj->GetLine() -1) == m_lastCommentBlock.lineNumber ) {
        obj->SetComment(m_lastCommentBlock.comment);
        
        // For objects, we can parse the comment now.
        // For function, we need to wait until the entire function is parsed
        // (we need the function signature, which is not avilable at this point)
        if(!obj->IsFunction()) {
            // parse the comment
            JSDocComment commenter;
            commenter.Process(obj);
        }
        
    }
}
