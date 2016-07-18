#include "CxxVariableScanner.h"
#include "CxxScannerTokens.h"

CxxVariableScanner::CxxVariableScanner(const wxString& buffer)
    : m_eof(false)
{
    m_scanner = ::LexerNew(buffer);
}

CxxVariableScanner::~CxxVariableScanner()
{
    if(m_scanner) {
        ::LexerDestroy(&m_scanner);
    }
}

CxxVariable::List_t CxxVariableScanner::GetVariables()
{
    CxxVariable::List_t vars;
    if(!m_scanner) return CxxVariable::List_t(); // Empty list

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
            }
        } while(cont);
    }
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
                case T_IDENTIFIER:
                case T_DOUBLE_COLONS:
                case T_AUTO:
                case T_BOOL:
                case T_CHAR:
                case T_CHAR16_T:
                case T_CHAR32_T:
                case T_CONST:
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
                    // Found an identifier
                    // We consider this part of the type only if the previous token was "::"
                    if(lastToken.type == T_DOUBLE_COLONS) {
                        vartype.push_back(token);
                    } else {
                        // End of type
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
                case T_DOUBLE:
                case T_FLOAT:
                case T_INT:
                case T_LONG:
                case T_SHORT:
                case T_SIGNED:
                case T_UNSIGNED:
                case T_VOID:
                case T_WCHAR_T: {
                    vartype.push_back(CxxVariable::LexerToken(token));
                    break;
                }
                }
                case '<':
                case '[':
                    depth++;
                    break;
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
        consumed << token.text;
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
    return res;
}
