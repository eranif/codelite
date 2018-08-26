#include "CxxScannerTokens.h"
#include "CxxVariableScanner.h"
#include "file_logger.h"
#include <algorithm>
#include <unordered_set>

CxxVariableScanner::CxxVariableScanner(const wxString& buffer, eCxxStandard standard, const wxStringTable_t& macros,
                                       bool isFuncSignature)
    : m_scanner(NULL)
    , m_buffer(buffer)
    , m_eof(false)
    , m_parenthesisDepth(0)
    , m_standard(standard)
    , m_macros(macros)
    , m_isFuncSignature(isFuncSignature)
{
    if(!m_buffer.IsEmpty() && m_buffer[0] == '(') { m_buffer.Remove(0, 1); }
    m_nativeTypes.insert(T_AUTO);
    m_nativeTypes.insert(T_BOOL);
    m_nativeTypes.insert(T_CHAR);
    m_nativeTypes.insert(T_CHAR16_T);
    m_nativeTypes.insert(T_CHAR32_T);
    m_nativeTypes.insert(T_DOUBLE);
    m_nativeTypes.insert(T_FLOAT);
    m_nativeTypes.insert(T_INT);
    m_nativeTypes.insert(T_LONG);
    m_nativeTypes.insert(T_SHORT);
    m_nativeTypes.insert(T_SIGNED);
    m_nativeTypes.insert(T_UNSIGNED);
    m_nativeTypes.insert(T_VOID);
    m_nativeTypes.insert(T_WCHAR_T);
}

CxxVariableScanner::~CxxVariableScanner() {}

CxxVariable::Vec_t CxxVariableScanner::GetVariables(bool sort)
{
    wxString strippedBuffer;
    OptimizeBuffer(m_buffer, strippedBuffer);
    CxxVariable::Vec_t vars = DoGetVariables(strippedBuffer, sort);
    if(sort) {
        std::sort(vars.begin(), vars.end(),
                  [&](CxxVariable::Ptr_t a, CxxVariable::Ptr_t b) { return a->GetName() < b->GetName(); });
    }
    return vars;
}

bool CxxVariableScanner::ReadType(CxxVariable::LexerToken::Vec_t& vartype, bool& isAuto)
{
    isAuto = false;
    int depth = 0;
    CxxLexerToken token;
    while(GetNextToken(token)) {
        if(depth == 0) {
            if(vartype.empty()) {
                // a type can start the following tokens
                switch(token.GetType()) {
                case T_AUTO:
                    isAuto = true;
                // fall
                case T_CLASS:
                case T_STRUCT:
                case T_IDENTIFIER:
                case T_DOUBLE_COLONS:
                case T_BOOL:
                case T_CHAR:
                case T_CHAR16_T:
                case T_CHAR32_T:
                case T_CONST:
                case T_CONSTEXPR:
                case T_DOUBLE:
                case T_FLOAT:
                case T_INT:
                case T_LONG:
                case T_MUTABLE:
                case T_REGISTER:
                case T_SHORT:
                case T_SIGNED:
                case T_STATIC:
                case T_UNSIGNED:
                case T_VOLATILE:
                case T_VOID:
                case T_USING:
                case T_WCHAR_T: {
                    vartype.push_back(CxxVariable::LexerToken(token, depth));
                    break;
                }
                default:
                    // Not a type definition
                    return false;
                }
            } else {
                const CxxVariable::LexerToken& lastToken = vartype.back();
                switch(token.GetType()) {
                case T_IDENTIFIER: {
                    if(TypeHasIdentifier(vartype) && (vartype.back().type != T_DOUBLE_COLONS)) {
                        // We already found the identifier for this type, its probably part of the name
                        UngetToken(token);
                        return true;
                    } else if(HasNativeTypeInList(vartype) && (vartype.back().type != T_DOUBLE_COLONS)) {
                        UngetToken(token);
                        return true;
                    }
                    // Found an identifier
                    // We consider this part of the type only if the previous token was "::" or
                    // if it was T_CONST
                    switch(lastToken.type) {
                    case T_CLASS:
                    case T_STRUCT:
                    case T_DOUBLE_COLONS:
                    case T_CONST:
                    case T_CONSTEXPR:
                    case T_REGISTER:
                    case T_MUTABLE:
                    case T_VOLATILE:
                    case T_STATIC:
                        vartype.push_back(CxxVariable::LexerToken(token, depth));
                        break;
                    default:
                        UngetToken(token);
                        return true;
                    }
                    break;
                case T_AUTO:
                    isAuto = true;
                // fall
                case T_DOUBLE_COLONS:
                case T_BOOL:
                case T_CHAR:
                case T_CHAR16_T:
                case T_CHAR32_T:
                case T_CONST:
                case T_CONSTEXPR:
                case T_DOUBLE:
                case T_FLOAT:
                case T_INT:
                case T_LONG:
                case T_SHORT:
                case T_SIGNED:
                case T_UNSIGNED:
                case T_VOID:
                case T_WCHAR_T: {
                    vartype.push_back(CxxVariable::LexerToken(token, depth));
                    break;
                }
                }
                case '<':
                case '[':
                    vartype.push_back(CxxVariable::LexerToken(token, depth));
                    depth++;
                    break;
                case '*':
                case '&':
                case '@': // AngelScript
                    // Part of the name
                    UngetToken(token);
                    return true;
                default:
                    return false;
                }
            }
        } else {
            // Depth > 0
            vartype.push_back(CxxVariable::LexerToken(token, depth));
            if(token.GetType() == '>' || token.GetType() == ']') {
                --depth;
            } else if(token.GetType() == '<' || token.GetType() == '[') {
                ++depth;
            }
        }
    }
    return false;
}

bool CxxVariableScanner::ReadName(wxString& varname, wxString& pointerOrRef, wxString& varInitialization)
{
    CxxLexerToken token;
    while(GetNextToken(token)) {
        if(token.GetType() == '@') {
            // AngelScript. @ is similar to * in C/C++
            // @see https://github.com/eranif/codelite/issues/1839
            if(!GetNextToken(token) || token.GetType() != T_IDENTIFIER) {
                varname.Clear();
                return false;
            }
            varname = token.GetWXString();
            varInitialization.Clear();
            pointerOrRef = "@";
            return true;
        } else if(token.GetType() == T_IDENTIFIER) {
            varname = token.GetWXString();
            
            // When parsing function signature, we don't have multiple arguments 
            // tied to the same TYPE
            if(m_isFuncSignature) { return false; }
            
            // Peek at the next token
            // We can expect "=", "," "(", ";" or ")"
            // Examples:
            // TYPE name = 1;
            // TYPE name, secondVariable;
            // TYPE name(10);
            // TYPE name;
            static std::unordered_set<int> s_validLocalTerminators;
            if(s_validLocalTerminators.empty()) {
                s_validLocalTerminators.insert((int)',');
                s_validLocalTerminators.insert((int)'=');
                s_validLocalTerminators.insert((int)';');
                s_validLocalTerminators.insert((int)')');
                s_validLocalTerminators.insert((int)'(');
                s_validLocalTerminators.insert((int)'{'); // C++11 initialization, e.g: vector<int> v {1,2,3};
                s_validLocalTerminators.insert((int)'['); // Array
            }

            // Now that we got the name, check if have more variables to expect
            if(!GetNextToken(token)) {
                // We reached EOF, but we do got the variable name
                // se we return false ("don't continue") but we dont clear the name
                return false;
            }

            // Always return the token
            UngetToken(token);

            if(s_validLocalTerminators.count(token.GetType()) == 0) {
                varname.Clear();
                return false;
            }

            ConsumeInitialization(varInitialization);

            // Now that we got the name, check if have more variables to expect
            if(!GetNextToken(token)) { return false; }

            if((token.GetType() == '{') && !varInitialization.IsEmpty()) {
                // Don't collect functions and consider them as variables
                UngetToken(token);
                varname.clear();
                return false;
            }

            if(!varInitialization.empty()) { varInitialization.RemoveLast(); }

            // If we found comma, return true. Unless we are parsing a function signature
            if(!m_isFuncSignature && (token.GetType() == ',')) {
                return true;
            } else {
                UngetToken(token);
                return false;
            }
        } else if(token.GetType() == '*' || token.GetType() == '&') {
            pointerOrRef << token.GetWXString();
        } else {
            return false;
        }
    }
    return false;
}

void CxxVariableScanner::ConsumeInitialization(wxString& consumed)
{
    CxxLexerToken token;
    wxString dummy;
    if(!GetNextToken(token)) return;
    int type = wxNOT_FOUND;
    int tokType = token.GetType();
    if(tokType == '(') {
        // Read the initialization
        std::unordered_set<int> delims;
        delims.insert(')');
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) { return; }
        consumed.Prepend("(");
        // Now read until the delimiter
        delims.clear();
        delims.insert(';');
        delims.insert(',');
        delims.insert('{');
        type = ReadUntil(delims, token, dummy);

    } else if(tokType == '[') {
        // Array
        std::unordered_set<int> delims;
        delims.insert(']');
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) { return; }
        consumed.Prepend("[");
        // Now read until the delimiter
        delims.clear();
        delims.insert(';');
        delims.insert(',');
        type = ReadUntil(delims, token, dummy);

    } else if(tokType == '{') {
        // Read the initialization
        std::unordered_set<int> delims;
        delims.insert('}');
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) { return; }
        consumed.Prepend("{");
        // Now read until the delimiter
        delims.clear();
        delims.insert(';');
        delims.insert(',');
        type = ReadUntil(delims, token, dummy);

    } else if(tokType == '=') {
        std::unordered_set<int> delims;
        delims.insert(';');
        delims.insert(',');
        type = ReadUntil(delims, token, consumed);
    } else {
        UngetToken(token);
        consumed.clear();
        std::unordered_set<int> delims;
        delims.insert(';');
        delims.insert(',');
        delims.insert('{');
        type = ReadUntil(delims, token, dummy);
    }

    if(type == ',' || type == (int)'{' || type == ';') { UngetToken(token); }
}

int CxxVariableScanner::ReadUntil(const std::unordered_set<int>& delims, CxxLexerToken& token, wxString& consumed)
{
    // loop until we find the open brace
    CxxVariable::LexerToken::Vec_t v;
    int depth = 0;
    while(GetNextToken(token)) {
        v.push_back(CxxVariable::LexerToken(token, depth));
        if(depth == 0) {
            if(delims.count(token.GetType())) {
                consumed = CxxVariable::PackType(v, m_standard);
                return token.GetType();
            } else {
                switch(token.GetType()) {
                case '<':
                case '{':
                case '[':
                case '(':
                    depth++;
                    break;
                default:
                    // ignore it
                    break;
                }
            }
        } else {
            switch(token.GetType()) {
            case '>':
            case '}':
            case ']':
            case ')':
                depth--;
                break;
            default:
                // ignore it
                break;
            }
        }
    }
    return wxNOT_FOUND;
}

bool CxxVariableScanner::GetNextToken(CxxLexerToken& token)
{
    bool res = false;

    while(true) {
        res = ::LexerNext(m_scanner, token);
        if(!res) break;

        // Ignore any T_IDENTIFIER which is declared as macro
        if((token.GetType() == T_IDENTIFIER) && m_macros.count(token.GetWXString())) { continue; }
        break;
    }

    m_eof = !res;
    switch(token.GetType()) {
    case '(':
        ++m_parenthesisDepth;
        break;
    case ')':
        --m_parenthesisDepth;
        break;
    default:
        break;
    }
    return res;
}

void CxxVariableScanner::OptimizeBuffer(const wxString& buffer, wxString& stripped_buffer)
{
    stripped_buffer.Clear();
    Scanner_t sc = ::LexerNew(buffer);
    if(!sc) {
        clWARNING() << "CxxVariableScanner::OptimizeBuffer(): failed to create Scanner_t" << clEndl;
        return; // Failed to allocate scanner
    }

    CppLexerUserData* userData = ::LexerGetUserData(sc);
    CxxLexerToken tok;
    CxxLexerToken lastToken;

    // Cleanup
    m_buffers.clear();
    PushBuffer();
    int parenthesisDepth = 0;
    while(::LexerNext(sc, tok)) {
        // Skip prep processing state
        if(userData && userData->IsInPreProcessorSection()) { continue; }

        // Return the working buffer, which depends on the current state
        wxString& buffer = Buffer();

        // Outer switch: state based
        switch(tok.GetType()) {
        case T_PP_STATE_EXIT:
            break;
        case T_FOR:
            OnForLoop(sc);
            break;
        case T_CATCH:
            OnCatch(sc);
            break;
        case T_DECLTYPE:
            OnDeclType(sc);
            break;
        case T_WHILE:
            OnWhile(sc);
            break;
        case '(':
            buffer << tok.GetWXString();
            if(lastToken.GetType() == ']') {
                OnLambda(sc);
            } else {
                ++parenthesisDepth;
                PushBuffer();
            }
            break;
        case '{':
            buffer << tok.GetWXString();
            PushBuffer();
            break;
        case '}':
            buffer = PopBuffer();
            // The closing curly bracket is added *after* we switch buffers
            buffer << tok.GetWXString();
            break;
        case ')':
            --parenthesisDepth;
            buffer = PopBuffer();
            buffer << ")";
            // The closing curly bracket is added *after* we switch buffers
            // if(parenthesisDepth == 0) {
            //     buffer << tok.GetWXString();
            // } else {
            // }
            break;
        default:
            buffer << tok.GetWXString() << " ";
            break;
        }
        lastToken = tok;
    }
    ::LexerDestroy(&sc);

    // Merge the buffers
    stripped_buffer.Clear();
    std::for_each(m_buffers.rbegin(), m_buffers.rend(), [&](const wxString& buffer) { stripped_buffer << buffer; });
}

CxxVariable::Vec_t CxxVariableScanner::DoGetVariables(const wxString& buffer, bool sort)
{
    // First, we strip all parenthesis content from the buffer
    m_scanner = ::LexerNew(buffer);
    m_eof = false;
    m_parenthesisDepth = 0;
    if(!m_scanner) return CxxVariable::Vec_t(); // Empty list

    CxxVariable::Vec_t vars;

    // Read the variable type
    while(!IsEof()) {
        bool isAuto;
        CxxVariable::LexerToken::Vec_t vartype;
        if(!ReadType(vartype, isAuto)) continue;

        // Get the variable(s) name
        wxString varname, pointerOrRef, varInitialization;
        bool cont = false;
        do {
            cont = ReadName(varname, pointerOrRef, varInitialization);
            CxxVariable::Ptr_t var(new CxxVariable(m_standard));
            var->SetName(varname);
            var->SetType(vartype);
            var->SetDefaultValue(varInitialization);
            var->SetPointerOrReference(pointerOrRef);
            var->SetIsAuto(isAuto);
            if(var->IsOk()) {
                vars.push_back(var);
            } else if(!varInitialization.IsEmpty()) {
                // This means that the above was a function call
                // Parse the siganture which is placed inside the varInitialization
                CxxVariableScanner scanner(varInitialization, m_standard, m_macros, true);
                CxxVariable::Vec_t args = scanner.GetVariables(sort);
                vars.insert(vars.end(), args.begin(), args.end());
                break;
            }
        } while(cont && (m_parenthesisDepth == 0) /* not inside a function */);
    }

    ::LexerDestroy(&m_scanner);
    return vars;
}

bool CxxVariableScanner::TypeHasIdentifier(const CxxVariable::LexerToken::Vec_t& type)
{
    // do we have an identifier in the type?
    CxxVariable::LexerToken::Vec_t::const_iterator iter =
        std::find_if(type.begin(), type.end(),
                     [&](const CxxVariable::LexerToken& token) { return (token.GetType() == T_IDENTIFIER); });
    return (iter != type.end());
}

CxxVariable::Map_t CxxVariableScanner::GetVariablesMap()
{
    CxxVariable::Vec_t l = GetVariables(true);
    CxxVariable::Map_t m;
    std::for_each(l.begin(), l.end(), [&](CxxVariable::Ptr_t v) {
        if(m.count(v->GetName()) == 0) { m.insert(std::make_pair(v->GetName(), v)); }
    });
    return m;
}

bool CxxVariableScanner::HasNativeTypeInList(const CxxVariable::LexerToken::Vec_t& type) const
{
    CxxVariable::LexerToken::Vec_t::const_iterator iter =
        std::find_if(type.begin(), type.end(), [&](const CxxVariable::LexerToken& token) {
            return ((token._depth == 0) && (m_nativeTypes.count(token.GetType()) != 0));
        });
    return (iter != type.end());
}

CxxVariable::Vec_t CxxVariableScanner::DoParseFunctionArguments(const wxString& buffer)
{
    m_scanner = ::LexerNew(buffer);
    m_eof = false;
    m_parenthesisDepth = 0;
    if(!m_scanner) return CxxVariable::Vec_t(); // Empty list

    CxxVariable::Vec_t vars;

    // Read the variable type
    while(!IsEof()) {
        bool isAuto;
        CxxVariable::LexerToken::Vec_t vartype;
        if(!ReadType(vartype, isAuto)) continue;

        // Get the variable(s) name
        wxString varname, pointerOrRef, varInitialization;
        ReadName(varname, pointerOrRef, varInitialization);
        CxxVariable::Ptr_t var(new CxxVariable(m_standard));
        var->SetName(varname);
        var->SetType(vartype);
        var->SetDefaultValue(varInitialization);
        var->SetPointerOrReference(pointerOrRef);
        var->SetIsAuto(isAuto);
        vars.push_back(var);
    }
    ::LexerDestroy(&m_scanner);
    return vars;
}

CxxVariable::Vec_t CxxVariableScanner::ParseFunctionArguments() { return DoParseFunctionArguments(m_buffer); }

void CxxVariableScanner::UngetToken(const CxxLexerToken& token)
{
    ::LexerUnget(m_scanner);

    // Fix the depth if needed
    if(token.GetType() == '(') {
        --m_parenthesisDepth;
    } else if(token.GetType() == ')') {
        ++m_parenthesisDepth;
    }
}

wxString& CxxVariableScanner::Buffer() { return m_buffers[0]; }

bool CxxVariableScanner::OnForLoop(Scanner_t scanner)
{
    CxxLexerToken tok;

    // The next token must be '('
    if(!::LexerNext(scanner, tok)) return false;

    // Parser error
    if(tok.GetType() != '(') return false;

    int depth(1);
    wxString& buffer = Buffer();
    bool lookingForFirstSemiColon = true;
    while(::LexerNext(scanner, tok)) {
        // Skip prep processing state
        // 'for' and 'catch' parenthesis content is kept in the buffer
        switch(tok.GetType()) {
        case '(':
            depth++;
            if(lookingForFirstSemiColon) { buffer << "("; }
            break;
        case ')':
            depth--;
            if(lookingForFirstSemiColon) { buffer << ")"; }
            if(depth == 0) return true;
            break;
        case ';':
        case ':': // C++11 ranged for
            if(lookingForFirstSemiColon) { buffer << ";"; }
            lookingForFirstSemiColon = false;
            break;
        default:
            if(lookingForFirstSemiColon) { buffer << tok.GetWXString() << " "; }
            break;
        }
    }
    return false;
}

bool CxxVariableScanner::OnCatch(Scanner_t scanner)
{
    CxxLexerToken tok;

    // The next token must be '('
    if(!::LexerNext(scanner, tok)) return false;

    // Parser error
    if(tok.GetType() != '(') return false;
    int depth(1);
    wxString& buffer = Buffer();
    buffer << ";"; // Help the parser
    while(::LexerNext(scanner, tok)) {
        switch(tok.GetType()) {
        case '(':
            ++depth;
            buffer << tok.GetWXString();
            break;
        case ')':
            --depth;
            buffer << tok.GetWXString();
            if(depth == 0) { return true; }
            break;
        default:
            buffer << tok.GetWXString() << " ";
            break;
        }
    }
    return false;
}

bool CxxVariableScanner::OnWhile(Scanner_t scanner)
{

    CxxLexerToken tok;

    // The next token must be '('
    if(!::LexerNext(scanner, tok)) return false;

    // Parser error
    if(tok.GetType() != '(') return false;
    int depth(1);
    while(::LexerNext(scanner, tok)) {
        switch(tok.GetType()) {
        case '(':
            ++depth;
            break;
        case ')':
            --depth;
            if(depth == 0) return true;
            break;
        default:
            break;
        }
    }
    return false;
}

bool CxxVariableScanner::OnDeclType(Scanner_t scanner)
{
    CxxLexerToken tok;
    wxString& buffer = Buffer();

    // The next token must be '('
    if(!::LexerNext(scanner, tok)) return false;

    // Parser error
    if(tok.GetType() != '(') return false;
    int depth(1);
    buffer << "decltype(";
    while(::LexerNext(scanner, tok)) {
        switch(tok.GetType()) {
        case '(':
            ++depth;
            buffer << tok.GetWXString();
            break;
        case ')':
            --depth;
            buffer << ")";
            if(depth == 0) { return true; }
            break;
        default:
            break;
        }
    }
    return false;
}

wxString& CxxVariableScanner::PushBuffer()
{
    wxString buffer;
    m_buffers.insert(m_buffers.begin(), buffer);
    return m_buffers[0];
}

wxString& CxxVariableScanner::PopBuffer()
{
    if(m_buffers.size() > 1) { m_buffers.erase(m_buffers.begin()); }
    return m_buffers[0];
}

bool CxxVariableScanner::OnLambda(Scanner_t scanner)
{
    CxxLexerToken tok;
    int depth(1);
    wxString& buffer = Buffer();
    while(::LexerNext(scanner, tok)) {
        switch(tok.GetType()) {
        case '(':
            ++depth;
            buffer << tok.GetWXString();
            break;
        case ')':
            --depth;
            buffer << tok.GetWXString();
            if(depth == 0) { return true; }
            break;
        default:
            buffer << tok.GetWXString() << " ";
            break;
        }
    }
    return false;
}
