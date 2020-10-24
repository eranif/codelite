
#include "PHPDocVisitor.h"
#include "PHPEntityClass.h"
#include "PHPEntityFunction.h"
#include "PHPEntityFunctionAlias.h"
#include "PHPEntityNamespace.h"
#include "PHPEntityVariable.h"
#include "PHPLookupTable.h"
#include "PHPScannerTokens.h"
#include "PHPSourceFile.h"
#include "fileutils.h"
#include <unordered_set>
#include <wx/arrstr.h>
#include <wx/ffile.h>

#define NEXT_TOKEN_BREAK_IF_NOT(t, action) \
    {                                      \
        if(!NextToken(token))              \
            break;                         \
        if(token.type != t) {              \
            action;                        \
            break;                         \
        }                                  \
    }

PHPSourceFile::PHPSourceFile(const wxString& content, PHPLookupTable* lookup)
    : m_text(content)
    , m_parseFunctionBody(false)
    , m_depth(0)
    , m_reachedEOF(false)
    , m_converter(NULL)
    , m_lookup(lookup)
{
    m_scanner = ::phpLexerNew(content, kPhpLexerOpt_ReturnComments);
}

PHPSourceFile::PHPSourceFile(const wxFileName& filename, PHPLookupTable* lookup)
    : m_filename(filename)
    , m_parseFunctionBody(false)
    , m_depth(0)
    , m_reachedEOF(false)
    , m_converter(NULL)
    , m_lookup(lookup)
{
    // Filename is kept in absolute path
    m_filename.MakeAbsolute();

    wxString content;
    if(FileUtils::ReadFileContent(filename, content, wxConvISO8859_1)) {
        m_text.swap(content);
    }
    m_scanner = ::phpLexerNew(m_text, kPhpLexerOpt_ReturnComments);
}

PHPSourceFile::~PHPSourceFile()
{
    if(m_scanner) {
        ::phpLexerDestroy(&m_scanner);
    }
}

bool PHPSourceFile::IsInPHPSection(const wxString& buffer)
{
    PHPScanner_t scanner = ::phpLexerNew(buffer);
    if(!scanner)
        return false;
    phpLexerToken tok;
    bool inPhp = false;
    while(::phpLexerNext(scanner, tok)) {
        if(::phpLexerIsPHPCode(scanner)) {
            inPhp = true;
        } else {
            inPhp = false;
        }
    }
    ::phpLexerDestroy(&scanner);
    return inPhp;
}

void PHPSourceFile::Parse(int exitDepth)
{
    int retDepth = exitDepth;
    phpLexerToken token;
    while(NextToken(token)) {
        switch(token.type) {
        case '=':
            m_lookBackTokens.clear();
            break;
        case '{':
            m_lookBackTokens.clear();
            break;
        case '}':
            m_lookBackTokens.clear();
            if(m_depth == retDepth) {
                return;
            }
            break;
        case ';':
            m_lookBackTokens.clear();
            break;
        case kPHP_T_VARIABLE:
            if(!CurrentScope()->Is(kEntityTypeClass)) {
                // A global variable
                OnVariable(token);
            }
            break;
        case kPHP_T_CATCH:
            // found 'catch (...)'
            OnCatch();
            break;
        case kPHP_T_PUBLIC:
        case kPHP_T_PRIVATE:
        case kPHP_T_PROTECTED: {
            int visibility = token.type;
            PHPEntityClass* cls = CurrentScope()->Cast<PHPEntityClass>();
            if(cls) {
                /// keep the current token
                m_lookBackTokens.push_back(token);

                // Now we have a small problem here:
                // public can be a start for a member or a function
                // we let the lexer run forward until it finds kPHP_T_VARIABLE (for variable)
                // or kPHP_T_IDENTIFIER
                int what = ReadUntilFoundOneOf(kPHP_T_VARIABLE, kPHP_T_FUNCTION, token);
                if(what == kPHP_T_VARIABLE) {
                    // A variable
                    PHPEntityBase::Ptr_t member(new PHPEntityVariable());
                    member->SetFilename(m_filename.GetFullPath());
                    member->Cast<PHPEntityVariable>()->SetVisibility(visibility);
                    member->Cast<PHPEntityVariable>()->SetFullName(token.Text());
                    size_t flags = LookBackForVariablesFlags();
                    member->Cast<PHPEntityVariable>()->SetFlag(kVar_Member);
                    member->Cast<PHPEntityVariable>()->SetFlag(kVar_Const, flags & kVar_Const);
                    member->Cast<PHPEntityVariable>()->SetFlag(kVar_Static, flags & kVar_Static);
                    member->Cast<PHPEntityVariable>()->SetLine(token.lineNumber);
                    CurrentScope()->AddChild(member);

                    // Handle member assignment
                    // public $memberVar = new Something();
                    // for such cases, assign $memberVar type of Something()
                    phpLexerToken t;
                    if(!NextToken(t)) {
                        // EOF
                        return;
                    }

                    if(t.type == '=') {
                        // assignment
                        wxString expr;
                        if(!ReadExpression(expr)) {
                            return;
                        }

                        // Optimize 'new ClassName(..)' expression
                        if(expr.StartsWith("new")) {
                            expr = expr.Mid(3);
                            expr.Trim().Trim(false);
                            expr = expr.BeforeFirst('(');
                            expr.Trim().Trim(false);
                            member->Cast<PHPEntityVariable>()->SetTypeHint(MakeIdentifierAbsolute(expr));

                        } else {
                            // keep the expression
                            member->Cast<PHPEntityVariable>()->SetExpressionHint(expr);
                        }

                    } else {
                        // restore the token
                        UngetToken(t);
                        if(!ConsumeUntil(';'))
                            return;
                    }

                } else if(what == kPHP_T_FUNCTION) {
                    // A function...
                    OnFunction();
                    m_lookBackTokens.clear();
                }
            }
            break;
        }
        case kPHP_T_DEFINE:
            // Define statement
            OnDefine(token);
            break;
        case kPHP_T_CONST:
            OnConstant(token);
            break;
        case kPHP_T_REQUIRE:
        case kPHP_T_REQUIRE_ONCE:
        case kPHP_T_INCLUDE:
        case kPHP_T_INCLUDE_ONCE:
            // Handle include files
            m_lookBackTokens.clear();
            break;
        case kPHP_T_FOREACH:
            // found "foreach" statement
            OnForEach();
            m_lookBackTokens.clear();
            break;
        case kPHP_T_USE:
            // Found outer 'use' statement - construct the alias table
            if(Class()) {
                // inside a class, this means that this is a 'use <trait>;'
                OnUseTrait();
            } else {
                // alias table
                OnUse();
            }
            m_lookBackTokens.clear();
            break;
        case kPHP_T_CLASS:
        case kPHP_T_INTERFACE:
        case kPHP_T_TRAIT:
            // Found class
            OnClass(token);
            m_lookBackTokens.clear();
            break;
        case kPHP_T_NAMESPACE:
            // Found a namespace
            OnNamespace();
            m_lookBackTokens.clear();
            break;
        case kPHP_T_FUNCTION:
            // Found function
            OnFunction();
            m_lookBackTokens.clear();
            break;
        default:
            // Keep the token
            break;
        }
    }
    PhaseTwo();
}

void PHPSourceFile::OnUse()
{
    wxString fullname, alias, temp;
    phpLexerToken token;
    bool cont = true;
    while(cont && NextToken(token)) {
        switch(token.type) {
        case ',':
        case ';': {
            if(fullname.IsEmpty()) {
                // no full name yet
                fullname.swap(temp);

            } else if(alias.IsEmpty()) {
                alias.swap(temp);
            }

            if(alias.IsEmpty()) {
                // no alias provided, use the last part of the fullname
                alias = fullname.AfterLast('\\');
            }

            if(!fullname.IsEmpty() && !alias.IsEmpty()) {
                // Use namespace is alway refered as fullpath namespace
                // So writing:
                // use Zend\Mvc\Controll\Action;
                // is equal for writing:
                // use \Zend\Mvc\Controll\Action;
                // For simplicitiy, we change it to fully qualified path
                // so parsing is easier
                if(!fullname.StartsWith("\\")) {
                    fullname.Prepend("\\");
                }
                m_aliases.insert(std::make_pair(alias, MakeIdentifierAbsolute(fullname)));
            }
            temp.clear();
            fullname.clear();
            alias.clear();
            if(token.type == ';') {
                cont = false;
            }
        } break;
        case kPHP_T_AS: {
            fullname.swap(temp);
            temp.clear();
        } break;
        default:
            temp << token.Text();
            break;
        }
    }
}

void PHPSourceFile::OnNamespace()
{
    // Read until we find the line delimiter ';' or EOF found
    wxString path;
    phpLexerToken token;
    while(NextToken(token)) {
        if(token.type == ';') {
            break;
        }

        // Make sure that the namespace path is alway set in absolute path
        // i.e. starts with kPHP_T_NS_SEPARATOR
        if(path.IsEmpty() && token.type != kPHP_T_NS_SEPARATOR) {
            path << "\\";
        }
        path << token.Text();
    }

    if(m_scopes.empty()) {
        // no scope is set, push the global scope
        m_scopes.push_back(PHPEntityBase::Ptr_t(new PHPEntityNamespace()));
        PHPEntityNamespace* ns = CurrentScope()->Cast<PHPEntityNamespace>();
        if(ns) {
            ns->SetFullName(path); // Global namespace
        }
    } else {
        // PHP parsing error... (namespace must be the first thing on the file)
    }
}

void PHPSourceFile::OnFunction()
{
    // read the next token
    phpLexerToken token;
    if(!NextToken(token)) {
        return;
    }

    bool funcReturnRef = false;
    if(token.type == '&') {
        funcReturnRef = true;
        if(!NextToken(token)) {
            return;
        }
    }

    PHPEntityFunction* func(NULL);
    int funcDepth(0);
    if(token.type == kPHP_T_IDENTIFIER) {
        // the function name
        func = new PHPEntityFunction();
        func->SetFullName(token.Text());
        func->SetLine(token.lineNumber);

    } else if(token.type == '(') {
        funcDepth = 1; // Since we already consumed the open brace
        // anonymous function
        func = new PHPEntityFunction();
        func->SetLine(token.lineNumber);
    }

    if(!func)
        return;
    PHPEntityBase::Ptr_t funcPtr(func);
    if(funcReturnRef) {
        funcPtr->SetFlag(kFunc_ReturnReference);
    }

    // add the function to the current scope
    CurrentScope()->AddChild(funcPtr);

    // Set the function as the current scope
    m_scopes.push_back(funcPtr);

    // update function attributes
    ParseFunctionSignature(funcDepth);
    func->SetFlags(LookBackForFunctionFlags());
    if(LookBackTokensContains(kPHP_T_ABSTRACT) || // The 'abstract modifier was found for this this function
       (funcPtr->Parent() && funcPtr->Parent()->Is(kEntityTypeClass) &&
        funcPtr->Parent()->Cast<PHPEntityClass>()->IsInterface())) // We are inside an interface
    {
        // Mark this function as an abstract function
        func->SetFlags(func->GetFlags() | kFunc_Abstract);
    }

    if(func->HasFlag(kFunc_Abstract)) {
        // an abstract function - it has no body
        if(!ConsumeUntil(';')) {
            // could not locate the function delimiter, remove it from the stack
            // we probably reached EOF here
            m_scopes.pop_back();
        }

    } else {
        if(!NextToken(token))
            return;
        if(token.type == ':') {
            // PHP 7 signature type
            // function foobar(...) : RETURN_TYPE

            if(!NextToken(token))
                return;
            if(token.type == '?') {
                // PHP 7.1 nullable return
                funcPtr->SetFlag(kFunc_ReturnNullable);
            } else {
                UngetToken(token);
            }

            wxString returnValuetype = ReadFunctionReturnValueFromSignature();
            if(returnValuetype.IsEmpty())
                return; // parse error
            func->SetReturnValue(returnValuetype);

        } else {
            // untake the token and place it back on the "unget" list
            UngetToken(token);
        }

        if(ReadUntilFound('{', token)) {
            // found the function body starting point
            if(IsParseFunctionBody()) {
                ParseFunctionBody();
            } else {
                // Consume the function body
                ConsumeFunctionBody();
            }
        } else {
            // could not locate the open brace!
            // remove this function from the stack
            m_scopes.pop_back();
        }
    }

    // Remove the current function from the scope list
    if(!m_reachedEOF) {
        m_scopes.pop_back();
    }
    m_lookBackTokens.clear();
}

PHPEntityBase::Ptr_t PHPSourceFile::CurrentScope()
{
    if(m_scopes.empty()) {
        // no scope is set, push the global scope
        m_scopes.push_back(PHPEntityBase::Ptr_t(new PHPEntityNamespace()));
        CurrentScope()->SetFullName("\\"); // Global namespace
    }
    return m_scopes.back();
}

size_t PHPSourceFile::LookBackForFunctionFlags()
{
    size_t flags(0);
    for(size_t i = 0; i < m_lookBackTokens.size(); ++i) {
        const phpLexerToken& tok = m_lookBackTokens.at(i);
        if(tok.type == kPHP_T_ABSTRACT) {
            flags |= kFunc_Abstract;

        } else if(tok.type == kPHP_T_FINAL) {
            flags |= kFunc_Final;
        } else if(tok.type == kPHP_T_STATIC) {
            flags |= kFunc_Static;

        } else if(tok.type == kPHP_T_PUBLIC) {
            flags |= kFunc_Public;
            flags &= ~kFunc_Private;
            flags &= ~kFunc_Protected;

        } else if(tok.type == kPHP_T_PRIVATE) {
            flags |= kFunc_Private;
            flags &= ~kFunc_Public;
            flags &= ~kFunc_Protected;

        } else if(tok.type == kPHP_T_PROTECTED) {
            flags |= kFunc_Protected;
            flags &= ~kFunc_Public;
            flags &= ~kFunc_Private;
        }
    }
    return flags;
}

void PHPSourceFile::ParseFunctionSignature(int startingDepth)
{
    phpLexerToken token;
    if(startingDepth == 0) {
        // loop until we find the open brace
        while(NextToken(token)) {
            if(token.type == '(') {
                ++startingDepth;
                break;
            }
        }
        if(startingDepth == 0)
            return;
    }

    // at this point the 'depth' is 1, as we already read the open brace
    int depth = 1;
    wxString typeHint;
    wxString defaultValue;
    wxString name;
    PHPEntityVariable* var(NULL);
    bool collectingDefaultValue = false;
    while(NextToken(token)) {
        switch(token.type) {
        case kPHP_T_VARIABLE:
            if(!var) {
                // var can be non null if we are parsing PHP-7 function arguments
                // with type-hinting
                var = new PHPEntityVariable();
            }

            name = token.Text();
            var->SetLine(token.lineNumber);
            var->SetFilename(m_filename);
            // Mark this variable as function argument
            var->SetFlag(kVar_FunctionArg);
            if(name.StartsWith("&")) {
                var->SetIsReference(true);
                name.Remove(0, 1);
            } else if(typeHint.EndsWith("&")) {
                var->SetIsReference(true);
                typeHint.RemoveLast();
            }
            var->SetFullName(name);
            var->SetTypeHint(MakeIdentifierAbsolute(typeHint));
            break;
        case '(':
            depth++;
            if(collectingDefaultValue) {
                defaultValue << "(";
            }
            break;
        case ')':
            depth--;
            // if the depth goes under 1 - we are done
            if(depth < 1) {
                if(var) {
                    var->SetDefaultValue(defaultValue);
                    CurrentScope()->AddChild(PHPEntityBase::Ptr_t(var));
                }
                return;

            } else if(depth) {
                defaultValue << token.Text();
            }
            break;
        case '=':
            // default value
            collectingDefaultValue = true;
            break;
        case ',':
            if(var) {
                var->SetDefaultValue(defaultValue);
                CurrentScope()->AddChild(PHPEntityBase::Ptr_t(var));
            }
            var = NULL;
            typeHint.Clear();
            defaultValue.Clear();
            collectingDefaultValue = false;
            break;
        case '?':
            if(!var) {
                var = new PHPEntityVariable();
            }
            var->SetIsNullable(true);
            break;
        case kPHP_T_NS_SEPARATOR:
        case kPHP_T_IDENTIFIER:
            if(!var) {
                // PHP-7 type hinting function arguments
                var = new PHPEntityVariable();
                UngetToken(token);
                typeHint = ReadType();
                if(!typeHint.IsEmpty()) {
                    break;
                }
            }
        // all "else" cases simply fall into the default case
        default:
            if(collectingDefaultValue) {
                defaultValue << token.Text();
            } else {
                typeHint << token.Text();
            }
            break;
        }
    }
}

void PHPSourceFile::PrintStdout()
{
    // print the alias table
    wxPrintf("Alias table:\n");
    wxPrintf("===========\n");
    std::map<wxString, wxString>::iterator iter = m_aliases.begin();
    for(; iter != m_aliases.end(); ++iter) {
        wxPrintf("%s => %s\n", iter->first, iter->second);
    }
    wxPrintf("===========\n");
    if(m_scopes.empty())
        return;
    m_scopes.front()->PrintStdout(0);
}

bool PHPSourceFile::ReadUntilFound(int delim, phpLexerToken& token)
{
    // loop until we find the open brace
    while(NextToken(token)) {
        if(token.type == delim) {
            return true;
        }
    }
    return false;
}

void PHPSourceFile::ConsumeFunctionBody()
{
    int depth = m_depth;
    phpLexerToken token;
    while(NextToken(token)) {
        switch(token.type) {
        case '}':
            if(m_depth < depth) {
                return;
            }
            break;
        default:
            break;
        }
    }
}

void PHPSourceFile::ParseFunctionBody()
{
    m_lookBackTokens.clear();

    // when we reach the current depth-1 -> leave
    int exitDepth = m_depth - 1;
    phpLexerToken token;
    PHPEntityBase::Ptr_t var(NULL);
    while(NextToken(token)) {
        switch(token.type) {
        case '{':
            m_lookBackTokens.clear();
            break;
        case '}':
            m_lookBackTokens.clear();
            if(m_depth == exitDepth) {
                return;
            }
            break;
        case ';':
            m_lookBackTokens.clear();
            break;
        case kPHP_T_CATCH:
            OnCatch();
            break;
        case kPHP_T_VARIABLE: {
            OnVariable(token);
            break;
        } break;
        default:
            break;
        }
    }
}

wxString PHPSourceFile::ReadType()
{
    bool cont = true;
    wxString type;
    phpLexerToken token;

    while(cont && NextToken(token)) {
        switch(token.type) {
        case kPHP_T_IDENTIFIER:
            type << token.Text();
            break;

        case kPHP_T_NS_SEPARATOR:
            type << token.Text();
            break;
        default:
            // restore the token so next call to NextToken
            // will pick it up again
            UngetToken(token);
            cont = false;
            break;
        }
    }

    type = MakeTypehintAbsolute(type);
    return type;
}

PHPEntityBase::Ptr_t PHPSourceFile::Namespace()
{
    if(m_scopes.empty()) {
        return CurrentScope();
    }
    return *m_scopes.begin();
}

wxString PHPSourceFile::LookBackForTypeHint()
{
    if(m_lookBackTokens.empty())
        return wxEmptyString;
    wxArrayString tokens;

    for(int i = (int)m_lookBackTokens.size() - 1; i >= 0; --i) {
        if(m_lookBackTokens.at(i).type == kPHP_T_IDENTIFIER || m_lookBackTokens.at(i).type == kPHP_T_NS_SEPARATOR) {
            tokens.Insert(m_lookBackTokens.at(i).Text(), 0);
        } else {
            break;
        }
    }

    wxString type;
    for(size_t i = 0; i < tokens.GetCount(); ++i) {
        type << tokens.Item(i);
    }
    return type;
}

void PHPSourceFile::PhaseTwo()
{
    // Visit each entity found during the parsing stage
    // and try to match it with its phpdoc comment block (we do this by line number)
    // the visitor also makes sure that each entity is properly assigned with the current file name
    PHPDocVisitor visitor(*this, m_comments);
    visitor.Visit(Namespace());
}

bool PHPSourceFile::NextToken(phpLexerToken& token)
{
    bool res = ::phpLexerNext(m_scanner, token);
    if(res && (token.type == kPHP_T_C_COMMENT)) {
        m_comments.push_back(token);
        // We keep comments made in the class body
        if(!m_scopes.empty() && CurrentScope()->Is(kEntityTypeClass)) {
            PHPDocVar::Ptr_t var(new PHPDocVar(*this, token.Text()));
            if(var->IsOk()) {
                var->SetLineNumber(token.lineNumber);
                // this comment is a @var comment
                CurrentScope()->Cast<PHPEntityClass>()->AddVarPhpDoc(var);
            }
        }
    }

    if(token.type == '{') {
        m_depth++;
    } else if(token.type == '}') {
        m_depth--;
    } else if(token.type == ';') {
        m_lookBackTokens.clear();
    }
    if(!res)
        m_reachedEOF = true;
    if(res)
        m_lookBackTokens.push_back(token);
    return res;
}

wxString PHPSourceFile::MakeIdentifierAbsolute(const wxString& type) { return DoMakeIdentifierAbsolute(type, false); }

void PHPSourceFile::OnClass(const phpLexerToken& tok)
{
    wxString classDoc;
    const phpLexerToken& prevToken = GetPreviousToken();
    if(!prevToken.IsNull() && prevToken.IsDocComment()) {
        classDoc = prevToken.Text();
    }

    // A "complex" example: class A extends BaseClass implements C, D {}
    bool isAbstractClass = LookBackTokensContains(kPHP_T_ABSTRACT);

    // Read until we get the class name
    phpLexerToken token;
    while(NextToken(token)) {
        if(token.IsAnyComment())
            continue;
        if(token.type != kPHP_T_IDENTIFIER) {
            // expecting the class name
            return;
        }
        break;
    }

    // create new class entity
    PHPEntityBase::Ptr_t klass(new PHPEntityClass());
    klass->SetFilename(m_filename.GetFullPath());
    klass->SetDocComment(classDoc);

    PHPEntityClass* pClass = klass->Cast<PHPEntityClass>();

    // Is the class an interface?
    pClass->SetIsInterface(tok.type == kPHP_T_INTERFACE);
    pClass->SetIsAbstractClass(isAbstractClass);
    pClass->SetIsTrait(tok.type == kPHP_T_TRAIT);
    pClass->SetFullName(PrependCurrentScope(token.Text()));
    pClass->SetLine(token.lineNumber);

    while(NextToken(token)) {
        if(token.IsAnyComment())
            continue;
        switch(token.type) {
        case kPHP_T_EXTENDS: {
            // inheritance
            wxString extends = ReadExtends();
            if(extends.IsEmpty())
                return;
            // No need to call 'MakeIdentifierAbsolute' it was called internally by
            pClass->SetExtends(extends);
        } break;
        case kPHP_T_IMPLEMENTS: {
            wxArrayString implements;
            ReadImplements(implements);
            pClass->SetImplements(implements);
        } break;
        case '{': {
            // entering the class body
            // add the current class to the current scope
            CurrentScope()->AddChild(klass);
            m_scopes.push_back(klass);
            Parse(m_depth - 1);
            if(!m_reachedEOF) {
                m_scopes.pop_back();
            }
            return;
        }
        default:
            break;
        }
    }
}

bool PHPSourceFile::ReadCommaSeparatedIdentifiers(int delim, wxArrayString& list)
{
    phpLexerToken token;
    wxString temp;
    while(NextToken(token)) {
        if(token.IsAnyComment())
            continue;
        if(token.type == delim) {
            if(!temp.IsEmpty() && list.Index(temp) == wxNOT_FOUND) {
                list.Add(MakeIdentifierAbsolute(temp));
            }
            UngetToken(token);
            return true;
        }

        switch(token.type) {
        case ',':
            if(list.Index(temp) == wxNOT_FOUND) {
                list.Add(MakeIdentifierAbsolute(temp));
            }
            temp.clear();
            break;
        default:
            temp << token.Text();
            break;
        }
    }
    return false;
}

bool PHPSourceFile::ConsumeUntil(int delim)
{
    phpLexerToken token;
    while(NextToken(token)) {
        if(token.type == delim) {
            return true;
        }
    }
    return false;
}

bool PHPSourceFile::ReadExpression(wxString& expression)
{
    expression.clear();
    phpLexerToken token;
    int depth(0);
    while(NextToken(token)) {
        if(token.type == ';') {
            return true;

        } else if(token.type == '{') {
            UngetToken(token);
            return true;
        }

        switch(token.type) {
        case kPHP_T_FUNCTION:
            OnFunction();
            break;
        case kPHP_T_REQUIRE:
        case kPHP_T_REQUIRE_ONCE:
            expression.clear();
            return false;

        case kPHP_T_STRING_CAST:
        case kPHP_T_CONSTANT_ENCAPSED_STRING:
        case kPHP_T_C_COMMENT:
        case kPHP_T_CXX_COMMENT:
            // skip comments and strings
            break;
        case '(':
            depth++;
            expression << "(";
            break;
        case ')':
            depth--;
            if(depth == 0) {
                expression << ")";
            }
            break;
        case kPHP_T_NEW:
            if(depth == 0) {
                expression << token.Text() << " ";
            }
            break;
        default:
            if(depth == 0) {
                expression << token.Text();
            }
            break;
        }
    }
    // reached EOF
    return false;
}

void PHPSourceFile::UngetToken(const phpLexerToken& token)
{
    ::phpLexerUnget(m_scanner);
    // undo any depth / comments
    if(token.type == '{') {
        m_depth--;
    } else if(token.type == '}') {
        m_depth++;
    } else if(token.type == kPHP_T_C_COMMENT && !m_comments.empty()) {
        m_comments.erase(m_comments.begin() + m_comments.size() - 1);
    }
}

const PHPEntityBase* PHPSourceFile::Class()
{
    PHPEntityBase::Ptr_t curScope = CurrentScope();
    PHPEntityBase* pScope = curScope.Get();
    while(pScope) {
        PHPEntityClass* cls = pScope->Cast<PHPEntityClass>();
        if(cls) {
            // this scope is a class
            return pScope;
        }
        pScope = pScope->Parent();
    }
    return NULL;
}

int PHPSourceFile::ReadUntilFoundOneOf(int delim1, int delim2, phpLexerToken& token)
{
    // loop until we find the open brace
    while(NextToken(token)) {
        if(token.type == delim1) {
            return delim1;
        } else if(token.type == delim2) {
            return delim2;
        }
    }
    return wxNOT_FOUND;
}

bool PHPSourceFile::LookBackTokensContains(int type) const
{
    for(size_t i = 0; i < m_lookBackTokens.size(); ++i) {
        if(m_lookBackTokens.at(i).type == type)
            return true;
    }
    return false;
}

size_t PHPSourceFile::LookBackForVariablesFlags()
{
    size_t flags(kVar_Public);
    for(size_t i = 0; i < m_lookBackTokens.size(); ++i) {
        const phpLexerToken& tok = m_lookBackTokens.at(i);
        if(tok.type == kPHP_T_STATIC) {
            flags |= kVar_Static;

        } else if(tok.type == kPHP_T_CONST) {
            flags |= kVar_Const;

        } else if(tok.type == kPHP_T_PUBLIC) {
            flags |= kVar_Public;
            flags &= ~kVar_Private;
            flags &= ~kVar_Protected;

        } else if(tok.type == kPHP_T_PRIVATE) {
            flags |= kVar_Private;
            flags &= ~kVar_Public;
            flags &= ~kVar_Protected;

        } else if(tok.type == kPHP_T_PROTECTED) {
            flags |= kVar_Protected;
            flags &= ~kVar_Private;
            flags &= ~kVar_Public;
        }
    }
    return flags;
}

void PHPSourceFile::OnVariable(const phpLexerToken& tok)
{
    PHPEntityBase::Ptr_t var(new PHPEntityVariable());
    var->SetFullName(tok.Text());
    var->SetFilename(m_filename.GetFullPath());
    var->SetLine(tok.lineNumber);
    if(!CurrentScope()->FindChild(var->GetFullName(), true)) {
        CurrentScope()->AddChild(var);
    }

    if(!ReadVariableInitialization(var)) {
        m_lookBackTokens.clear();
        return;
    }
}

bool PHPSourceFile::ReadVariableInitialization(PHPEntityBase::Ptr_t var)
{
    phpLexerToken token;
    if(!NextToken(token)) {
        return false;
    }

    if(token.type != '=') {
        // restore the token
        UngetToken(token);
        return false;
    }

    wxString expr;
    if(!ReadExpression(expr)) {
        return false; // EOF
    }

    // Optimize 'new ClassName(..)' expression
    if(expr.StartsWith("new")) {
        expr = expr.Mid(3);
        expr.Trim().Trim(false);
        expr = expr.BeforeFirst('(');
        expr.Trim().Trim(false);
        var->Cast<PHPEntityVariable>()->SetTypeHint(MakeIdentifierAbsolute(expr));

    } else {
        // keep the expression
        var->Cast<PHPEntityVariable>()->SetExpressionHint(expr);
    }
    return true;
}

PHPEntityBase::List_t PHPSourceFile::GetAliases() const
{
    PHPEntityBase::List_t aliases;
    std::map<wxString, wxString>::const_iterator iter = m_aliases.begin();
    for(; iter != m_aliases.end(); ++iter) {
        // wrap each alias with class entity
        PHPEntityBase::Ptr_t klass(new PHPEntityClass());
        klass->SetFullName(iter->second);
        klass->SetShortName(iter->first);
        klass->SetFilename(GetFilename());
        aliases.push_back(klass);
    }
    return aliases;
}

void PHPSourceFile::OnDefine(const phpLexerToken& tok)
{
    phpLexerToken token;
    if(!NextToken(token))
        return; // EOF
    if(token.type != '(') {
        ConsumeUntil(';');
        return;
    }
    if(!NextToken(token))
        return; // EOF
    if(token.type != kPHP_T_CONSTANT_ENCAPSED_STRING) {
        ConsumeUntil(';');
        return;
    }
    // Remove the quotes
    wxString varName = token.Text();
    if((varName.StartsWith("\"") && varName.EndsWith("\"")) || (varName.StartsWith("'") && varName.EndsWith("'"))) {
        varName.Remove(0, 1);
        varName.RemoveLast();
        // define() defines constants exactly as it was instructed
        // i.e. it does not take the current namespace into consideration
        PHPEntityBase::Ptr_t var(new PHPEntityVariable());

        // Convert the variable into fullpath + relative name
        if(!varName.StartsWith("\\")) {
            varName.Prepend("\\");
        }
        wxString shortName = varName.AfterLast('\\');
        var->SetFullName(varName);
        var->SetShortName(shortName);
        var->SetFlag(kVar_Define);
        var->SetFilename(GetFilename());
        var->SetLine(tok.lineNumber);

        // We keep the defines in a special list
        // this is because 'define' does not obay to the current scope
        m_defines.push_back(var);
    }
    // Always consume the 'define' statement
    ConsumeUntil(';');
}

void PHPSourceFile::OnUseTrait()
{
    PHPEntityBase::Ptr_t clas = CurrentScope();
    if(!clas)
        return;

    // Collect the identifiers followed the 'use' statement
    wxArrayString identifiers;
    wxString tempname;
    phpLexerToken token;
    while(NextToken(token)) {
        switch(token.type) {
        case ',': {
            if(!tempname.IsEmpty()) {
                identifiers.Add(MakeIdentifierAbsolute(tempname));
            }
            tempname.clear();
        } break;
        case '{': {
            // we are looking at a case like:
            // use A, B { ... }
            if(!tempname.IsEmpty()) {
                identifiers.Add(MakeIdentifierAbsolute(tempname));
                ParseUseTraitsBody();
            }
            tempname.clear();
            // add the traits as list of 'extends'
            clas->Cast<PHPEntityClass>()->SetTraits(identifiers);
            return;
        } break;
        case ';': {
            if(!tempname.IsEmpty()) {
                identifiers.Add(MakeIdentifierAbsolute(tempname));
            }
            tempname.clear();

            // add the traits as list of 'extends'
            clas->Cast<PHPEntityClass>()->SetTraits(identifiers);
            return;
        } break;
        default:
            tempname << token.Text();
            break;
        }
    }
}

void PHPSourceFile::OnCatch()
{
    // Read until we find the kPHP_T_VARIABLE
    bool cont(true);
    phpLexerToken token;
    wxString typehint;
    wxString varname;
    while(cont && NextToken(token)) {
        switch(token.type) {
        case kPHP_T_VARIABLE:
            cont = false;
            varname = token.Text();
            break;
        case kPHP_T_IDENTIFIER:
        case kPHP_T_NS_SEPARATOR:
            typehint << token.Text();
            break;
        default:
            break;
        }
    }

    if(!varname.IsEmpty()) {
        // we found the variable
        PHPEntityBase::Ptr_t var(new PHPEntityVariable());
        var->SetFullName(varname);
        var->SetFilename(m_filename.GetFullPath());
        var->SetLine(token.lineNumber);
        var->Cast<PHPEntityVariable>()->SetTypeHint(MakeIdentifierAbsolute(typehint));

        // add the variable to the current scope
        if(!CurrentScope()->FindChild(var->GetFullName(), true)) {
            CurrentScope()->AddChild(var);
        }
    }
}

wxString PHPSourceFile::ReadExtends()
{
    wxString type;
    phpLexerToken token;
    while(NextToken(token)) {
        if(token.type == kPHP_T_IDENTIFIER || token.type == kPHP_T_NS_SEPARATOR) {
            type << token.Text();
        } else {
            UngetToken(token);
            break;
        }
    }
    type = MakeIdentifierAbsolute(type);
    return type;
}

void PHPSourceFile::ReadImplements(wxArrayString& impls)
{
    wxString type;
    phpLexerToken token;
    while(NextToken(token)) {
        switch(token.type) {
        case kPHP_T_IDENTIFIER:
        case kPHP_T_NS_SEPARATOR:
            type << token.Text();
            break;
        case ',':
            // More to come
            if(!type.IsEmpty()) {
                wxString fullyQualifiedType = MakeIdentifierAbsolute(type);
                if(impls.Index(fullyQualifiedType) == wxNOT_FOUND) {
                    impls.Add(fullyQualifiedType);
                }
                type.clear();
            }
            break;
        default:
            // unexpected token
            if(!type.IsEmpty()) {
                wxString fullyQualifiedType = MakeIdentifierAbsolute(type);
                if(impls.Index(fullyQualifiedType) == wxNOT_FOUND) {
                    impls.Add(fullyQualifiedType);
                }
                type.clear();
            }
            UngetToken(token);
            return;
        }
    }
}

/*foreach (array_expression as $value)
    statement
foreach (array_expression as $key => $value)
    statement*/
void PHPSourceFile::OnForEach()
{
    // read until the "as" keyword
    phpLexerToken token;
    if(!ReadUntilFound(kPHP_T_AS, token))
        return;

    // Found the "as" key word and consumed it
    if(!NextToken(token))
        return;

    phpLexerToken peekToken;
    if(!NextToken(peekToken))
        return;

    // Ensure we got a variable
    if(token.type != kPHP_T_VARIABLE)
        return;

    // Add the key
    {
        PHPEntityBase::Ptr_t var(new PHPEntityVariable());
        var->SetFullName(token.Text());
        var->SetFilename(m_filename.GetFullPath());
        var->SetLine(token.lineNumber);
        if(!CurrentScope()->FindChild(var->GetFullName(), true)) {
            CurrentScope()->AddChild(var);
        }
    }
    // Check to see if we are using the syntax of:
    // foreach (array_expression as $key => $value)
    if(peekToken.type == kPHP_T_DOUBLE_ARROW) {
        if(!NextToken(token) || token.type != kPHP_T_VARIABLE) {
            return;
        }

        // Add the value as well
        // Create a new variable
        PHPEntityBase::Ptr_t var(new PHPEntityVariable());
        var->SetFullName(token.Text());
        var->SetFilename(m_filename.GetFullPath());
        var->SetLine(token.lineNumber);

        if(!CurrentScope()->FindChild(var->GetFullName(), true)) {
            CurrentScope()->AddChild(var);
        }

    } else {
        UngetToken(peekToken);
    }
}

void PHPSourceFile::ParseUseTraitsBody()
{
    wxString fullname, alias, temp;
    phpLexerToken token;
    bool cont = true;
    while(cont && NextToken(token)) {
        switch(token.type) {
        case '}': {
            cont = false;
        } break;
        case ',':
        case ';': {
            if(fullname.IsEmpty()) {
                // no full name yet
                fullname.swap(temp);

            } else if(alias.IsEmpty()) {
                alias.swap(temp);
            }

            if(alias.IsEmpty()) {
                // no alias provided, use the last part of the fullname
                alias = fullname.AfterLast('\\');
            }

            if(!fullname.IsEmpty() && !alias.IsEmpty()) {
                // Use namespace is alway refered as fullpath namespace
                // So writing:
                // use Zend\Mvc\Controll\Action;
                // is equal for writing:
                // use \Zend\Mvc\Controll\Action;
                // For simplicitiy, we change it to fully qualified path
                // so parsing is easier
                if(!fullname.StartsWith("\\")) {
                    fullname.Prepend("\\");
                }
                PHPEntityBase::Ptr_t funcAlias(new PHPEntityFunctionAlias());
                funcAlias->Cast<PHPEntityFunctionAlias>()->SetRealname(MakeIdentifierAbsolute(fullname));
                funcAlias->Cast<PHPEntityFunctionAlias>()->SetScope(CurrentScope()->GetFullName());
                funcAlias->SetShortName(alias);
                funcAlias->SetFullName(CurrentScope()->GetFullName() + "\\" + alias);
                funcAlias->SetFilename(GetFilename());
                funcAlias->SetLine(token.lineNumber);
                CurrentScope()->AddChild(funcAlias);
            }

            temp.clear();
            fullname.clear();
            alias.clear();
        } break;
        case kPHP_T_PAAMAYIM_NEKUDOTAYIM: {
            // Convert "::" into "\\"
            temp << "\\";
        } break;
        case kPHP_T_AS: {
            fullname.swap(temp);
            temp.clear();
        } break;
        case kPHP_T_INSTEADOF: {
            // For now, we are not interested in
            // A insteadof b; statements, so just clear the collected data so far
            fullname.clear();
            temp.clear();
            alias.clear();
            if(!ConsumeUntil(';'))
                return;
        } break;
        default:
            temp << token.Text();
            break;
        }
    }
}

void PHPSourceFile::OnConstant(const phpLexerToken& tok)
{
    // Parse constant line (possibly multiple constants)
    phpLexerToken token;
    PHPEntityBase::Ptr_t member;
    while(NextToken(token)) {
        if(token.type == '=') {

            // The next value should contain the constant value
            wxString constantValue;
            while(NextToken(token)) {
                if(token.type == ';') {
                    UngetToken(token);
                    break;
                } else if(token.type == ',') {
                    break;
                }
                constantValue << token.Text();
            }

            if(member && !constantValue.IsEmpty()) {
                // Keep the constant value, we will be using it later for tooltip
                member->Cast<PHPEntityVariable>()->SetDefaultValue(constantValue);
            }
        }
        if(token.type == ';') {
            if(member) {
                CurrentScope()->AddChild(member);
                break;
            }
        } else if(token.type == ',') {
            if(member) {
                CurrentScope()->AddChild(member);
                member.Reset(NULL);
            }
        } else if(token.type == kPHP_T_IDENTIFIER) {
            // found the constant name
            member.Reset(new PHPEntityVariable());
            member->Cast<PHPEntityVariable>()->SetFlag(kVar_Const);
            member->Cast<PHPEntityVariable>()->SetFlag(kVar_Member);
            member->SetFullName(token.Text());
            member->SetLine(token.lineNumber);
            member->SetFilename(m_filename.GetFullPath());
        } else {
            // do nothing
        }
    }
}

phpLexerToken& PHPSourceFile::GetPreviousToken()
{
    static phpLexerToken NullToken;
    if(m_lookBackTokens.size() >= 2) {
        // The last token in the list is the current one. We want the previous one
        return m_lookBackTokens.at(m_lookBackTokens.size() - 2);
    }
    return NullToken;
}

wxString PHPSourceFile::ReadFunctionReturnValueFromSignature()
{
    bool cont = true;
    wxString type;
    phpLexerToken token;

    while(cont && NextToken(token)) {
        switch(token.type) {
        case kPHP_T_SELF:
        case kPHP_T_ARRAY:
            return token.Text();

        case kPHP_T_IDENTIFIER:
            type << token.Text();
            break;

        case kPHP_T_NS_SEPARATOR:
            type << token.Text();
            break;
        default:
            // restore the token so next call to NextToken
            // will pick it up again
            UngetToken(token);
            cont = false;
            break;
        }
    }

    type = MakeIdentifierAbsolute(type);
    return type;
}

wxString PHPSourceFile::PrependCurrentScope(const wxString& className)
{
    wxString currentScope = Namespace()->GetFullName();
    if(!currentScope.EndsWith("\\")) {
        currentScope << "\\";
    }
    return currentScope + className;
}

wxString PHPSourceFile::MakeTypehintAbsolute(const wxString& type) { return DoMakeIdentifierAbsolute(type, true); }

wxString PHPSourceFile::DoMakeIdentifierAbsolute(const wxString& type, bool exactMatch)
{
    if(m_converter) {
        return m_converter->MakeIdentifierAbsolute(type);
    }

    static std::unordered_set<std::string> phpKeywords;
    if(phpKeywords.empty()) {
        // List taken from https://www.php.net/manual/en/language.types.intro.php
        // Native types
        phpKeywords.insert("bool");
        phpKeywords.insert("int");
        phpKeywords.insert("float");
        phpKeywords.insert("string");
        phpKeywords.insert("array");
        phpKeywords.insert("object");
        phpKeywords.insert("iterable");
        phpKeywords.insert("callable");
        phpKeywords.insert("null");
        phpKeywords.insert("mixed");
        phpKeywords.insert("void");
        // Types that are common in documentation
        phpKeywords.insert("boolean");
        phpKeywords.insert("integer");
        phpKeywords.insert("double");
        phpKeywords.insert("real");
        phpKeywords.insert("binery");
        phpKeywords.insert("resource");
        phpKeywords.insert("number");
        phpKeywords.insert("callback");
    }
    wxString typeWithNS(type);
    typeWithNS.Trim().Trim(false);

    if(phpKeywords.count(type.ToStdString())) {
        // primitives, don't bother...
        return typeWithNS;
    }

    if(typeWithNS.IsEmpty())
        return "";

    // A fully qualified type? don't touch it
    if(typeWithNS.StartsWith("\\")) {
        return typeWithNS;
    }

    // Handle 'use' cases:
    // use Zend\Form; // create an alias entry: Form => Zend\Form
    // class A extends Form\Form {}
    // The extends should be expanded to Zend\Form\Form
    if(typeWithNS.Contains("\\")) {
        wxString scopePart = typeWithNS.BeforeLast('\\');
        wxString className = typeWithNS.AfterLast('\\');
        if(m_aliases.find(scopePart) != m_aliases.end()) {
            typeWithNS.clear();
            typeWithNS << m_aliases.find(scopePart)->second << "\\" << className;
            // Remove duplicate NS separators
            typeWithNS.Replace("\\\\", "\\");
            if(!typeWithNS.StartsWith("\\")) {
                typeWithNS << "\\";
            }
            return typeWithNS;
        }
    }

    // Use the alias table first
    if(m_aliases.find(type) != m_aliases.end()) {
        return m_aliases.find(type)->second;
    }

    wxString ns = Namespace()->GetFullName();
    if(!ns.EndsWith("\\")) {
        ns << "\\";
    }

    if(exactMatch && m_lookup && !typeWithNS.Contains("\\") && !m_lookup->ClassExists(ns + typeWithNS)) {
        // Only when "exactMatch" apply this logic, otherwise, we might be getting a partialy typed string
        // which we will not find by calling FindChild()
        typeWithNS.Prepend("\\"); // Use the global NS
    } else {
        typeWithNS.Prepend(ns);
    }
    return typeWithNS;
}

const PHPEntityBase::List_t& PHPSourceFile::GetAllMatchesInOrder()
{
    if(m_allMatchesInorder.empty()) {
        auto ns = Namespace();
        if(!ns) {
            return m_allMatchesInorder;
        }
        if(ns->GetChildren().empty()) {
            return m_allMatchesInorder;
        }

        PHPEntityBase::List_t Q;
        Q.insert(Q.end(), ns->GetChildren().begin(), ns->GetChildren().end());

        while(!Q.empty()) {
            auto cur = Q.front();
            Q.erase(Q.begin());
            m_allMatchesInorder.push_back(cur);
            if(!cur->GetChildren().empty()) {
                // we want to process these children on the next iteration
                // so we prepend them to the queue
                Q.reserve(Q.size() + cur->GetChildren().size());
                Q.insert(Q.begin(), cur->GetChildren().begin(), cur->GetChildren().end());
            }
        }
    }
    return m_allMatchesInorder;
}
