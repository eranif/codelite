#include "CxxVariableScanner.h"
#include "CxxScannerTokens.h"
#include <algorithm>
#include <unordered_set>
#include "file_logger.h"

CxxVariableScanner::CxxVariableScanner(const wxString& buffer, eCxxStandard standard, const wxStringTable_t& macros)
    : m_scanner(NULL)
    , m_buffer(buffer)
    , m_eof(false)
    , m_parenthesisDepth(0)
    , m_standard(standard)
    , m_macros(macros)
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

CxxVariable::Vec_t CxxVariableScanner::GetVariables(bool sort)
{
    wxString strippedBuffer, parenthesisBuffer;
    OptimizeBuffer(strippedBuffer, parenthesisBuffer);
    CxxVariable::Vec_t vars = DoGetVariables(strippedBuffer, sort);
    CxxVariable::Vec_t args = DoGetVariables(parenthesisBuffer, sort);
    vars.insert(vars.end(), args.begin(), args.end());
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
                switch(token.type) {
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
                switch(token.type) {
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
            if(token.type == '>' || token.type == ']') {
                --depth;
            } else if(token.type == '<' || token.type == '[') {
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
        if(token.type == T_IDENTIFIER) {
            varname = token.text;

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
            }

            // Now that we got the name, check if have more variables to expect
            if(!GetNextToken(token)) {
                varname.Clear();
                return false;
            }

            // Always return the token
            UngetToken(token);

            if(s_validLocalTerminators.count(token.type) == 0) {
                varname.Clear();
                return false;
            }

            ConsumeInitialization(varInitialization);

            // Now that we got the name, check if have more variables to expect
            if(!GetNextToken(token)) {
                return false;
            }

            if((token.type == '{') && !varInitialization.IsEmpty()) {
                // Don't collect functions and consider them as variables
                UngetToken(token);
                varname.clear();
                return false;
            }

            if(!varInitialization.empty()) {
                varInitialization.RemoveLast();
            }

            // If we found comma, return true
            if(token.type == ',') {
                return true;
            } else {
                UngetToken(token);
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
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) {
            return;
        }
        consumed.Prepend("(");
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
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) {
            return;
        }
        consumed.Prepend("{");
        // Now read until the delimiter
        delims.clear();
        delims.insert(';');
        delims.insert(',');
        type = ReadUntil(delims, token, dummy);

    } else if(tokType == '=') {
        std::set<int> delims;
        delims.insert(';');
        delims.insert(',');
        type = ReadUntil(delims, token, consumed);
    } else {
        UngetToken(token);
        consumed.clear();
        std::set<int> delims;
        delims.insert(';');
        delims.insert(',');
        delims.insert('{');
        type = ReadUntil(delims, token, dummy);
    }

    if(type == ',' || type == (int)'{') {
        UngetToken(token);
    }
}

int CxxVariableScanner::ReadUntil(const std::set<int>& delims, CxxLexerToken& token, wxString& consumed)
{
    // loop until we find the open brace
    CxxVariable::LexerToken::Vec_t v;
    int depth = 0;
    while(GetNextToken(token)) {
        v.push_back(CxxVariable::LexerToken(token, depth));
        if(depth == 0) {
            if(delims.count(token.type)) {
                consumed = CxxVariable::PackType(v, m_standard);
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
    bool res = false;

    while(true) {
        res = ::LexerNext(m_scanner, token);
        if(!res) break;

        // Ignore any T_IDENTIFIER which is declared as macro
        if((token.type == T_IDENTIFIER) && m_macros.count(token.text)) {
            continue;
        }
        break;
    }

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
    if(!sc) {
        clWARNING() << "CxxVariableScanner::OptimizeBuffer(): failed to create Scanner_t" << clEndl;
        return; // Failed to allocate scanner
    }
    int depth = 0;
    CxxLexerToken tok;
    CxxLexerToken lastToken;

    bool lookingForFirstSemiColon = true;
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
                    if((lastToken.type == T_FOR)) {
                        state = kInForLoop;
                        lookingForFirstSemiColon = true;
                    } else if(lastToken.type == T_CATCH) {
                        state = kInCatch;
                    } else if(lastToken.type == T_DECLTYPE) {
                        state = kInDecltype;
                    } else if(lastToken.type == T_WHILE) {
                        state = kInWhile;
                        parenthesisBuffer << "();";
                    } else if(lastToken.type == ']') {
                        // Lambda?
                        state = kInCatch; // similar as "catch" - collect everything
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
        } else if(state == kInWhile) {
            switch(tok.type) {
            case '(':
                depth++;
                break;
            case ')':
                depth--;
                if(depth == 0) {
                    state = kNormal;
                }
                break;
            default:
                break;
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
        } else if(state == kInDecltype) {
            strippedBuffer << tok.text;
            switch(tok.type) {
            case '(':
                ++depth;
                break;
            case ')':
                --depth;
                if(depth == 0) {
                    state = kNormal;
                }
                break;
            default:
                break;
            }
        } else if(state == kInCatch) {
            // lambda and 'catch' parenthesis content is kept in the strippedBuffer
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
        } else if(state == kInForLoop) {
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
            case ';':
                lookingForFirstSemiColon = false;
                strippedBuffer << ";";
                break;
            default:
                if(lookingForFirstSemiColon) {
                    strippedBuffer << tok.text << " ";
                }
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
                CxxVariableScanner scanner(varInitialization, m_standard, m_macros);
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
    CxxVariable::LexerToken::Vec_t::const_iterator iter = std::find_if(
        type.begin(), type.end(), [&](const CxxVariable::LexerToken& token) { return (token.type == T_IDENTIFIER); });
    return (iter != type.end());
}

CxxVariable::Map_t CxxVariableScanner::GetVariablesMap()
{
    CxxVariable::Vec_t l = GetVariables(true);
    CxxVariable::Map_t m;
    std::for_each(l.begin(), l.end(), [&](CxxVariable::Ptr_t v) {
        if(m.count(v->GetName()) == 0) {
            m.insert(std::make_pair(v->GetName(), v));
        }
    });
    return m;
}

bool CxxVariableScanner::HasNativeTypeInList(const CxxVariable::LexerToken::Vec_t& type) const
{
    CxxVariable::LexerToken::Vec_t::const_iterator iter =
        std::find_if(type.begin(), type.end(), [&](const CxxVariable::LexerToken& token) {
            return ((token._depth == 0) && (m_nativeTypes.count(token.type) != 0));
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
    if(token.type == '(') {
        --m_parenthesisDepth;
    } else if(token.type == ')') {
        ++m_parenthesisDepth;
    }
}
