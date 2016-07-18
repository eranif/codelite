#include "CxxVariableScanner.h"
#include "CxxScannerTokens.h"

CxxVariableScanner::CxxVariableScanner(const wxString& buffer) { m_scanner = ::LexerNew(buffer); }

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
    CxxLexerToken::List_t vartype;
    if(!ReadType(vartype)) return vars;
    ;

    // Get the variable(s) name
    wxString varname, pointerOrRef;
    while(ReadName(varname, pointerOrRef)) {
        CxxVariable::Ptr_t var(new CxxVariable());
        var->SetName(varname);
        var->SetType(vartype);
        vars.push_back(var);
    }
    return vars;
}

bool CxxVariableScanner::ReadType(CxxLexerToken::List_t& vartype)
{
    int depth = 0;
    CxxLexerToken token;
    while(::LexerNext(m_scanner, token)) {
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
                case T_WCHAR_T:
                    vartype.push_back(token);
                    break;
                default:
                    // Not a type definition
                    return false;
                }
            } else {
                const CxxLexerToken& lastToken = vartype.back();
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
                case T_WCHAR_T:
                    vartype.push_back(token);
                    break;
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

bool CxxVariableScanner::ReadName(wxString& varname, wxString& pointerOrRef)
{
    CxxLexerToken token;
    while(::LexerNext(m_scanner, token)) {
        if(token.type == T_IDENTIFIER) {
            varname = token.text;
            ConsumeInitialization();

            // Now that we got the name, check if have more variables to expect
            if(!::LexerNext(m_scanner, token)) {
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

void CxxVariableScanner::ConsumeInitialization()
{
    CxxLexerToken token;
    if(!::LexerNext(m_scanner, token)) return;
    if(token.type == '(') {
        ReadUntil(')', ')', token);
    }

    int type = ReadUntil(';', ',', token);
    if(type == ',') {
        ::LexerUnget(m_scanner);
    }
}

int CxxVariableScanner::ReadUntil(int delim1, int delim2, CxxLexerToken& token)
{
    // loop until we find the open brace
    int depth = 0;
    while(::LexerNext(m_scanner, token)) {
        if(depth == 0) {
            if((token.type == delim1) || (token.type == delim2)) {
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
