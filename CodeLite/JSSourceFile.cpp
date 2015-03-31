#include "JSSourceFile.h"
#include "fileutils.h"
#include "JSLexerTokens.h"
#include "JSLexerAPI.h"
#include "file_logger.h"
#include "JSDocComment.h"
#include "JSObjectParser.h"

#define JS_NEXT_TOKEN() NextToken(token)
#define JS_NEXT_TOKEN_RETURN() \
    if(!NextToken(token)) return

#define JS_NEXT_TOKEN_RETURN_NULL() \
    if(!NextToken(token)) return NULL

#define JS_NEXT_TOKEN_RETURN_FALSE() \
    if(!NextToken(token)) return false

#define JS_CHECK_TYPE_RET_FALSE(t) \
    if(token.type != t) return false

#define JS_CHECK_TYPE_RETURN(t) \
    if(token.type != t) return

#define JS_CHECK_TYPE_RETURN_NULL(t) \
    if(token.type != t) return NULL;

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
    m_scanner = ::jsLexerNew(fileContent, kJSLexerOpt_ReturnComments);
}

JSSourceFile::~JSSourceFile()
{
    if(m_scanner) {
        ::jsLexerDestroy(&m_scanner);
    }
}

//========================================
// The main loop
//========================================
void JSSourceFile::Parse(int exitDepth)
{
    JSLexerToken token;
    while(NextToken(token)) {
        switch(token.type) {
        case kJS_RETURN:
            if(m_lookup->CurrentScope()->IsFunction()) {
                // Current scope is a function
                JSObject::Ptr_t ret = OnReturnValue();
                if(ret) {
                    m_lookup->CurrentScope()->AddType(ret->GetType(), false);
                }
            }
            break;
        case '}':
            if(m_depth == exitDepth) return;
            break;
        case '{':
            break;
        case kJS_FUNCTION:
            OnFunction();
            break;
        case kJS_VAR:
            // variable
            OnVariable();
            break;
        case kJS_IDENTIFIER: {
            JSObject::Ptr_t klass = m_lookup->FindClass(token.text);
            if(klass && klass->IsFunction()) {
                OnPropertyOrFunction(klass);
            }
        } break;
        case kJS_THIS:
            // try to process a :
            // this.foo =
            // or
            // this.prototype.foo =
            OnFunctionThisProperty();
            break;
        default:
            break;
        }
    }
}

// function Foo (
JSObject::Ptr_t JSSourceFile::OnFunction()
{
    JSLexerToken token;
    if(!NextToken(token)) return NULL;

    // Named function
    if(token.type == kJS_IDENTIFIER) {
        JSObject::Ptr_t func = m_lookup->NewFunction();
        m_lookup->CurrentScope()->AddProperty(func);
        func->SetName(token.text);
        // func->SetType(token.text);
        func->SetFile(m_filename);
        func->SetLine(token.lineNumber);
        AssociateComment(func);

        ParseFunction(func);

        // Check the function type, if it contains "|" i.e. return statement was found
        // remove the function name from the type itself

        // Now, parse the comment
        JSDocComment commenter;
        commenter.Process(func);
        return func;

    } else if(token.type == '(') {
        // Unget the current token (ParseFunction requires the open brace)
        UngetToken(token);

        // Anon function
        static int anonCounter = 0;
        JSObject::Ptr_t func = m_lookup->NewFunction();
        // m_lookup->CurrentScope()->AddProperty(func);
        func->SetName(wxString::Format("__anon%d", ++anonCounter));
        func->SetFile(m_filename);
        // func->SetType(token.text);
        func->SetLine(token.lineNumber);

        AssociateComment(func);
        ParseFunction(func);

        // Now, parse the comment
        JSDocComment commenter;
        commenter.Process(func);
        return func;
    }
    return NULL;
}

void JSSourceFile::ParseFunction(JSObject::Ptr_t func)
{
    JSLexerToken token;
    // read the signature of the function
    if(ReadSignature(func)) {
        // Read the open curly brace
        JS_NEXT_TOKEN();
        JS_CHECK_TYPE_RETURN('{');

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

        if(!curarg.IsEmpty() && (token.type == ',' || token.type == ')')) {
            JSObject::Ptr_t obj = m_lookup->NewObject();
            obj->SetLine(token.lineNumber);
            obj->SetName(curarg);
            obj->SetFile(m_filename);
            scope->AddVariable(obj);
            curarg.Clear();
        }

        // breaking condition
        if((token.type == ')') && (m_depth == startDepth)) {
            break;
        } else if(m_depth < startDepth) {
            return false;
        }
    }

    scope->SetSignature(sig);
    return true;
}

bool JSSourceFile::ReadUntil2(int until, int until2, wxString& content)
{
    JSLexerToken token;
    int startDepth = m_depth;
    while(JS_NEXT_TOKEN()) {
        content << token.text;
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
        content << token.text << " ";

        // breaking condition
        if(token.type == until && m_depth == startDepth) {
            return true;
        } else if(m_depth < startDepth) {
            return false;
        }
    }
    return false;
}

bool JSSourceFile::ReadUntilExcluding(int until, wxString& content)
{
    JSLexerToken token;
    int startDepth = m_depth;
    while(JS_NEXT_TOKEN()) {
        if(!content.IsEmpty()) {
            content << " ";
        }
        content << token.text;

        // breaking condition
        if(token.type == until && m_depth == startDepth) {
            UngetToken(token);
            content = content.BeforeLast(' ');
            return true;
        } else if(m_depth < startDepth) {
            return false;
        }
    }
    return false;
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

    JS_CHECK_TYPE_RETURN(kJS_IDENTIFIER);
    name = token.text;

    if(!JS_NEXT_TOKEN()) return;
    JS_CHECK_TYPE_RETURN('=');

    // When a function has a property, it becomes a class
    JSObject::Ptr_t curScope = m_lookup->CurrentScope();
    curScope->SetClass();

    if(!JS_NEXT_TOKEN()) return;
    if(token.type == kJS_FUNCTION) {
        // Allocate new function obj and add it
        JSObject::Ptr_t func = m_lookup->NewFunction();
        func->SetName(name);
        func->SetType(m_lookup->MakePath(func->GetName()));
        func->SetFile(m_filename);
        func->SetLine(token.lineNumber);
        AssociateComment(func);
        curScope->AddProperty(func);

        // From here on, its the same as normal function
        ParseFunction(func);

        // Now, parse the comment
        JSDocComment commenter;
        commenter.Process(func);

    } else {
        UngetToken(token);
        JSObjectParser op(*this, m_lookup);
        if(op.Parse(NULL)) {
            JSObject::Ptr_t obj = op.GetResult()->NewInstance(name);
            obj->SetFile(m_filename);
            obj->SetLine(token.lineNumber);
            AssociateComment(obj);
            curScope->AddProperty(obj);
        }
    }
}

void JSSourceFile::OnPropertyOrFunction(JSObject::Ptr_t This)
{
    JSLexerToken token;
    JS_NEXT_TOKEN_RETURN();
    JS_CHECK_TYPE_RETURN(kJS_DOT);

    if(!JS_NEXT_TOKEN()) return;
    if(token.type == kJS_PROTOTYPE) {
        JS_NEXT_TOKEN_RETURN();

        if(token.type == '=') {
            JS_NEXT_TOKEN_RETURN();
            JS_CHECK_TYPE_RETURN('{');

            // TypeClass.prototype = { ... }
            JSObjectParser op(*this, m_lookup);
            if(op.Parse(This)) {
                This->SetClass();
            }

        } else if(token.type == kJS_DOT) {
            // direct assignment
            JS_NEXT_TOKEN_RETURN();
            JS_CHECK_TYPE_RETURN(kJS_IDENTIFIER);

            wxString prototypeName = token.text;

            JS_NEXT_TOKEN_RETURN();
            JS_CHECK_TYPE_RETURN('=');

            // TypeClass.prototype.fooname =
            JSObjectParser op(*this, m_lookup);
            if(op.Parse(NULL)) {
                JSObject::Ptr_t obj = op.GetResult()->NewInstance(prototypeName);
                obj->SetFile(m_filename);
                obj->SetLine(token.lineNumber);
                AssociateComment(obj);
                This->AddProperty(obj);
                This->SetClass();
            }
        }

    } else {
        wxString prototypeName = token.text;
        JS_NEXT_TOKEN_RETURN();
        if(token.type == '=') {
            // "Static methods / properties"
            // TypeClass.fooname =
            JSObjectParser op(*this, m_lookup);
            if(op.Parse(NULL)) {
                JSObject::Ptr_t obj = op.GetResult()->NewInstance(prototypeName);
                obj->SetFile(m_filename);
                obj->SetLine(token.lineNumber);
                AssociateComment(obj);
                This->AddProperty(obj);
                This->SetClass();
            }
        }
    }
}

JSObject::Ptr_t JSSourceFile::OnVariable()
{
    JSLexerToken token;
    JS_NEXT_TOKEN_RETURN_NULL();
    JS_CHECK_TYPE_RETURN_NULL(kJS_IDENTIFIER);

    wxString name = token.text;
    if(!NextToken(token)) return NULL;

    JSObject::Ptr_t obj = m_lookup->NewObject();
    obj->SetName(name);
    obj->SetLine(token.lineNumber);
    obj->SetFile(m_filename);
    AssociateComment(obj);
    m_lookup->CurrentScope()->AddVariable(obj);
    if(token.type != '=') {
        return obj;
    }

    JSObjectParser parser(*this, m_lookup);
    if(parser.Parse(NULL)) {
        obj->SetType(parser.GetResult()->GetType());
    }
    return obj;
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
        } else if(res && token.type == '{') {
            ++m_depth;
        } else if(res && token.type == '}') {
            --m_depth;
        }
        return res;
    }
}

void JSSourceFile::UngetToken(const JSLexerToken& token) { m_lastToken = token; }

void JSSourceFile::AssociateComment(JSObject::Ptr_t obj)
{
    if((obj->GetLine() - 1) == m_lastCommentBlock.lineNumber) {
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

JSObject::Ptr_t JSSourceFile::ParseJSONObject(const wxString& content) { return NULL; }

JSObject::Ptr_t JSSourceFile::OnReturnValue()
{
    // void return
    JSObjectParser parser(*this, m_lookup);
    if(parser.Parse(NULL)) {
        return parser.GetResult();
    }
    return NULL;
}
