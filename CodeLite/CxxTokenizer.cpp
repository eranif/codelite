#include "CxxTokenizer.h"
#include <stack>
#include "CxxScannerTokens.h"

#define SCP_STATE_NORMAL 0
#define SCP_STATE_IN_IF 1
#define SCP_STATE_IN_WHILE 2
#define SCP_STATE_IN_FOR 3
#define SCP_STATE_IN_CATCH 4
#define SCP_STATE_IN_FOR_NO_SEMICOLON 5

CxxTokenizer::CxxTokenizer()
    : m_scanner(nullptr)
{
}

CxxTokenizer::~CxxTokenizer()
{
    if(m_scanner) {
        ::LexerDestroy(&m_scanner);
    }
}

bool CxxTokenizer::NextToken(CxxLexerToken& token)
{
    if(!m_scanner) return false;
    m_lastToken = token;
    return ::LexerNext(m_scanner, token);
}

void CxxTokenizer::Reset(const wxString& buffer)
{
    if(m_scanner) {
        ::LexerDestroy(&m_scanner);
    }
    if(!buffer.empty()) {
        m_buffer = buffer;
        m_scanner = ::LexerNew(buffer, 0);
    }
}

bool CxxTokenizer::ReadUntilClosingBracket(int delim, wxString& bufferRead)
{
    CxxLexerToken tok;
    int depth = 0;
    while(NextToken(tok)) {
        if(IsInPreProcessorSection()) {
            // pre-processor tokens
            continue;
        }
        switch(tok.GetType()) {
        case '<':
        case '(':
        case '[':
        case '{':
            depth++;
            bufferRead << tok.GetWXString() << " ";
            break;
        case '>':
        case ')':
        case ']':
        case '}':
            depth--;
            bufferRead << tok.GetWXString() << " ";
            if((tok.GetType() == delim) && (depth == 0)) {
                ::LexerUnget(m_scanner);
                return true;
            }
            break;
        default:
            bufferRead << tok.GetWXString() << " ";
            break;
        }
    }
    return false;
}

int CxxTokenizer::PeekToken(wxString& text)
{
    CxxLexerToken tok;
    if(!NextToken(tok)) {
        return false;
    }
    text = tok.GetWXString();
    int type = tok.GetType();
    ::LexerUnget(m_scanner);
    return type;
}

wxString CxxTokenizer::GetVisibleScope(const wxString& inputString)
{
    std::stack<wxString> scopes;
    Reset(inputString);

    CxxLexerToken token;
    wxString currentScope;
    int parenthesisDepth = 0;
    int state = SCP_STATE_NORMAL;
    CppLexerUserData *scannerData = GetUserData();
    while(NextToken(token)) {
        // Skip pre-processor block
        if(scannerData && scannerData->IsInPreProcessorSection()) {
            continue;
        }
        switch(state) {
        case SCP_STATE_NORMAL:
            switch(token.GetType()) {
            case T_PP_STATE_EXIT:
                break;
            case '{':
                currentScope << "{";
                scopes.push(currentScope);
                currentScope.clear();
                break;
            case '}':
                if(scopes.empty()) return ""; // Invalid braces count
                currentScope = scopes.top();
                scopes.pop();
                currentScope << "} ";
                break;
            case T_IF:
                state = SCP_STATE_IN_IF;
                currentScope << " if ";
                break;
            case T_WHILE:
                state = SCP_STATE_IN_WHILE;
                currentScope << " while ";
                break;
            case T_FOR:
                state = SCP_STATE_IN_FOR_NO_SEMICOLON;
                currentScope << ";";
                break;
            case T_CATCH:
                state = SCP_STATE_IN_CATCH;
                currentScope << ";";
                break;
            case '(':
                parenthesisDepth++;
                currentScope << "(";
                // Handle lambda
                // If we are enterting lamda function defenition, collect the locals
                // this is exactly what we in 'catch' hence the state change to SCP_STATE_IN_CATCH
                if(GetLastToken().GetType() == ']') {
                    state = SCP_STATE_IN_CATCH;
                }
                break;
            case ')':
                parenthesisDepth--;
                currentScope << ")";
                break;
            default:
                if(parenthesisDepth == 0) {
                    currentScope << " " << token.GetWXString();
                }
                break;
            }
            break;
        case SCP_STATE_IN_WHILE:
        case SCP_STATE_IN_IF:
            switch(token.GetType()) {
            case '(':
                parenthesisDepth++;
                currentScope << "(";
                break;
            case ')':
                parenthesisDepth--;
                currentScope << ")";
                if(parenthesisDepth == 0) {
                    state = SCP_STATE_NORMAL;
                }
                break;
            }
            break;
        case SCP_STATE_IN_FOR_NO_SEMICOLON:
            switch(token.GetType()) {
            case '(':
                parenthesisDepth++;
                currentScope << "(";
                break;
            case ')':
                parenthesisDepth--;
                currentScope << ")";
                if(parenthesisDepth == 0) {
                    state = SCP_STATE_NORMAL;
                }
                break;
            case ';':
                currentScope << ";";
                state = SCP_STATE_IN_FOR;
                break;
            default:
                currentScope << " " << token.GetWXString();
                break;
            }
            break;
        case SCP_STATE_IN_FOR:
            switch(token.GetType()) {
            case '(':
                parenthesisDepth++;
                currentScope << "(";
                break;
            case ')':
                parenthesisDepth--;
                currentScope << ")";
                if(parenthesisDepth == 0) {
                    state = SCP_STATE_NORMAL;
                }
                break;
            default:
                break;
            }
            break;
        case SCP_STATE_IN_CATCH:
            switch(token.GetType()) {
            case '(':
                currentScope << "(";
                parenthesisDepth++;
                break;
            case ')':
                parenthesisDepth--;
                currentScope << ")";
                if(parenthesisDepth == 0) {
                    state = SCP_STATE_NORMAL;
                }
                break;
            default:
                currentScope << " " << token.GetWXString();
                break;
            }
            break;
        default:
            break;
        }
    }

    wxString s;
    while(!scopes.empty()) {
        s.Prepend(scopes.top());
        scopes.pop();
    }
    s << currentScope;
    return s;
}

CppLexerUserData* CxxTokenizer::GetUserData() const
{
    if(!m_scanner) return NULL;
    return ::LexerGetUserData(m_scanner);
}

bool CxxTokenizer::IsInPreProcessorSection() const
{
    if(!GetUserData()) return false;
    return GetUserData()->IsInPreProcessorSection();
}
