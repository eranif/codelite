#include "CxxVariableScanner.h"
#include "CxxScannerTokens.h"
#include <algorithm>

CxxVariableScanner::CxxVariableScanner(const wxString& buffer)
    : m_scanner(NULL)
    , m_buffer(buffer)
    , m_eof(false)
    , m_parenthesisDepth(0)
{
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

CxxVariable::List_t CxxVariableScanner::GetVariables()
{
    wxString strippedBuffer, parenthesisBuffer;
    OptimizeBuffer(strippedBuffer, parenthesisBuffer);
    CxxVariable::List_t vars = DoGetVariables(strippedBuffer);
    CxxVariable::List_t args = DoGetVariables(parenthesisBuffer);
    vars.insert(vars.end(), args.begin(), args.end());
    vars.sort([&](CxxVariable::Ptr_t a, CxxVariable::Ptr_t b) { return a->GetName().CmpNoCase(b->GetName()); });
    return vars;
}

bool CxxVariableScanner::ReadType(CxxVariable::LexerToken::List_t& vartype)
{
    int depth = 0;
    CxxLexerToken token;
    while(GetNextToken(token)) {
        if(depth == 0) {
            if(vartype.empty()) {
                // a type can start the following tokens
                switch(token.type) {
                case T_CLASS:
                case T_STRUCT: {
                    // Class or struct forward decl
                    std::set<int> delims;
                    delims.insert(';'); // Forward decl
                    delims.insert('{'); // Class body
                    wxString consumed;
                    ReadUntil(delims, token, consumed);
                    return false;
                }
                case T_IDENTIFIER:
                case T_DOUBLE_COLONS:
                case T_AUTO:
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
                    vartype.push_back(CxxVariable::LexerToken(token));
                    break;
                }
                default:
                    // Not a type definition
                    return false;
                }
            } else {
                const CxxVariable::LexerToken& lastToken = vartype.back();
                switch(token.type) {
                case T_IDENTIFIER: {
                    if(TypeHasIdentifier(vartype) && (vartype.back().type != T_DOUBLE_COLONS)) {
                        // We already found the identifier for this type, its probably part of the name
                        ::LexerUnget(m_scanner);
                        return true;
                    } else if(HasTypeInList(vartype)) {
                        ::LexerUnget(m_scanner);
                        return true;
                    }
                    // Found an identifier
                    // We consider this part of the type only if the previous token was "::" or
                    // if it was T_CONST
                    switch(lastToken.type) {
                    case T_DOUBLE_COLONS:
                    case T_CONST:
                    case T_CONSTEXPR:
                    case T_REGISTER:
                    case T_MUTABLE:
                    case T_VOLATILE:
                    case T_STATIC:
                        vartype.push_back(token);
                        break;
                    default:
                        ::LexerUnget(m_scanner);
                        return true;
                    }
                    break;
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
                case T_AUTO:
                case T_WCHAR_T: {
                    vartype.push_back(CxxVariable::LexerToken(token));
                    break;
                }
                }
                case '<':
                case '[':
                    vartype.push_back(CxxVariable::LexerToken(token));
                    depth++;
                    break;
                case '*':
                case '&':
                    // Part of the name
                    ::LexerUnget(m_scanner);
                    return true;
                default:
                    return false;
                }
            }
        } else {
            // Depth > 0
            vartype.push_back(token);
            if(token.type == '>' || token.type == ']') {
                --depth;
            }
        }
    }
    return false;
}

bool CxxVariableScanner::ReadName(wxString& varname, wxString& pointerOrRef, wxString& varInitialization)
{
    CxxLexerToken token;
    while(GetNextToken(token)) {
        if(token.type == T_IDENTIFIER) {
            varname = token.text;
            ConsumeInitialization(varInitialization);

            // Now that we got the name, check if have more variables to expect
            if(!GetNextToken(token)) {
                return false;
            }

            if((token.type == '{') && !varInitialization.IsEmpty()) {
                // Don't collect functions and consider them as variables
                ::LexerUnget(m_scanner);
                varname.clear();
                return false;
            }

            // If we found comma, return true
            if(token.type == ',') {
                return true;
            } else {
                ::LexerUnget(m_scanner);
                return false;
            }

        } else if(token.type == '*' || token.type == '&') {
            pointerOrRef << token.text;
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
    int tokType = token.type;
    if(tokType == '(') {
        // Read the initialization
        std::set<int> delims;
        delims.insert(')');
        consumed = "(";
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) {
            return;
        }
        // Now read until the delimiter
        delims.clear();
        delims.insert(';');
        delims.insert(',');
        delims.insert('{');
        type = ReadUntil(delims, token, dummy);

    } else if(tokType == '{') {
        // Read the initialization
        std::set<int> delims;
        delims.insert('}');
        consumed = "{";
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) {
            return;
        }
        // Now read until the delimiter
        delims.clear();
        delims.insert(';');
        delims.insert(',');
        delims.insert('{');
        type = ReadUntil(delims, token, dummy);

    } else if(tokType == '=') {
        std::set<int> delims;
        delims.insert(';');
        delims.insert(',');
        delims.insert('{');
        type = ReadUntil(delims, token, consumed);
    } else {
        ::LexerUnget(m_scanner);
        consumed.clear();
        std::set<int> delims;
        delims.insert(';');
        delims.insert(',');
        delims.insert('{');
        type = ReadUntil(delims, token, dummy);
    }

    if(type == ',' || type == '{') {
        ::LexerUnget(m_scanner);
    }
}

int CxxVariableScanner::ReadUntil(const std::set<int>& delims, CxxLexerToken& token, wxString& consumed)
{
    // loop until we find the open brace
    int depth = 0;
    while(GetNextToken(token)) {
        consumed << token.text << " ";
        if(depth == 0) {
            if(delims.count(token.type)) {
                return token.type;
            } else {
                switch(token.type) {
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
            switch(token.type) {
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
    bool res = ::LexerNext(m_scanner, token);
    m_eof = !res;
    switch(token.type) {
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

void CxxVariableScanner::OptimizeBuffer(wxString& strippedBuffer, wxString& parenthesisBuffer)
{
    Scanner_t sc = ::LexerNew(m_buffer);
    int depth = 0;
    CxxLexerToken tok;
    CxxLexerToken lastToken;

    eState state = kNormal;
    while(::LexerNext(sc, tok)) {
        if(state == kNormal) {
            if((tok.type >= T_PP_DEFINE) && (tok.type <= T_PP_LTEQ)) {
                // In PreProcessor state
                state = kPreProcessor;
            } else {
                switch(tok.type) {
                case '(':
                    depth++;
                    strippedBuffer << "(";
                    if((lastToken.type == T_FOR) && false) {
                        state = kInForLoop;
                    } else if(lastToken.type == T_CATCH) {
                        state = kInCatch;
                    } else {
                        state = kInParen;
                        parenthesisBuffer << "(";
                    }
                    break;
                default:
                    strippedBuffer << tok.text << " ";
                    break;
                }
            }
        } else if(state == kInParen) {
            switch(tok.type) {
            case '(':
                depth++;
                parenthesisBuffer << "(";
                break;
            case ')':
                depth--;
                parenthesisBuffer << ")";
                if(depth == 0) {
                    parenthesisBuffer << ";"; // Needed to help the parser
                    state = kNormal;
                    strippedBuffer << ")";
                }
                break;
            default:
                parenthesisBuffer << tok.text << " ";
                break;
            }
        } else if((state == kInForLoop) || (state == kInCatch)) {
            // 'for' and 'catch' parenthesis content is kept in the strippedBuffer
            switch(tok.type) {
            case '(':
                depth++;
                strippedBuffer << "(";
                break;
            case ')':
                depth--;
                strippedBuffer << ")";
                if(depth == 0) {
                    state = kNormal;
                }
                break;
            default:
                strippedBuffer << tok.text << " ";
                break;
            }
        } else if(state == kPreProcessor) {
            switch(tok.type) {
            case T_PP_STATE_EXIT:
                state = kNormal;
                break;
            default:
                break;
            }
        }
        lastToken = tok;
    }
    ::LexerDestroy(&sc);
}

CxxVariable::List_t CxxVariableScanner::DoGetVariables(const wxString& buffer)
{
    // First, we strip all parenthesis content from the buffer
    m_scanner = ::LexerNew(buffer);
    m_eof = false;
    m_parenthesisDepth = 0;
    if(!m_scanner) return CxxVariable::List_t(); // Empty list

    CxxVariable::List_t vars;

    // Read the variable type
    while(!IsEof()) {
        CxxVariable::LexerToken::List_t vartype;
        if(!ReadType(vartype)) continue;

        // Get the variable(s) name
        wxString varname, pointerOrRef, varInitialization;
        bool cont = false;
        do {
            cont = ReadName(varname, pointerOrRef, varInitialization);
            CxxVariable::Ptr_t var(new CxxVariable());
            var->SetName(varname);
            var->SetType(vartype);
            if(var->IsOk()) {
                vars.push_back(var);
            } else if(!varInitialization.IsEmpty()) {
                // This means that the above was a function call
                // Parse the siganture which is placed inside the varInitialization
                CxxVariableScanner scanner(varInitialization);
                CxxVariable::List_t args = scanner.GetVariables();
                vars.insert(vars.end(), args.begin(), args.end());
                break;
            }
        } while(cont && (m_parenthesisDepth == 0) /* not inside a function */);
    }

    // Sort the variables by name
    ::LexerDestroy(&m_scanner);
    return vars;
}

bool CxxVariableScanner::TypeHasIdentifier(const CxxVariable::LexerToken::List_t& type)
{
    // do we have an identifier in the type?
    CxxVariable::LexerToken::List_t::const_iterator iter = std::find_if(
        type.begin(), type.end(), [&](const CxxVariable::LexerToken& token) { return (token.type == T_IDENTIFIER); });
    return (iter != type.end());
}

CxxVariable::Map_t CxxVariableScanner::GetVariablesMap()
{
    CxxVariable::List_t l = GetVariables();
    CxxVariable::Map_t m;
    std::for_each(l.begin(), l.end(), [&](CxxVariable::Ptr_t v) {
        if(m.count(v->GetName()) == 0) {
            m.insert(std::make_pair(v->GetName(), v));
        }
    });
    return m;
}

bool CxxVariableScanner::HasTypeInList(const CxxVariable::LexerToken::List_t& type) const
{
    CxxVariable::LexerToken::List_t::const_iterator iter = std::find_if(type.begin(), type.end(),
        [&](const CxxVariable::LexerToken& token) { return m_nativeTypes.count(token.type) != 0; });
    return (iter != type.end());
}
