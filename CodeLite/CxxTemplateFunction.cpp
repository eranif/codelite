#include "CxxTemplateFunction.h"
#include "CxxScannerTokens.h"

CxxTemplateFunction::CxxTemplateFunction(TagEntryPtr tag)
{
    m_scanner = ::LexerNew(tag->GetPatternClean(), kLexerOpt_None);
}

CxxTemplateFunction::~CxxTemplateFunction()
{
    if(m_scanner) {
        ::LexerDestroy(&m_scanner);
    }
}

void CxxTemplateFunction::ParseDefinitionList()
{
    m_list.Clear();
    int depth(0);
    CxxLexerToken token;
    
    // scan until we find 'template' keyword
    while(::LexerNext(m_scanner, token)) {
        if(token.type == T_TEMPLATE) {
            break;
        }
    }
    
    if(!token.type) return; // EOF
    
    // Loop until we found the open brace '<'
    while(::LexerNext(m_scanner, token)) {
        if(token.type == '<') {
            ++depth;
            break;
        }
    }
    
    // could not find it
    if(!depth) return;
    
    bool cont = true;
    wxString currentToken;
    while(cont && ::LexerNext(m_scanner, token)) {
        switch(token.type) {
        case T_TYPENAME:
        case T_CLASS:
            // ignore these keywords
            break;
        case ',':
            if(!currentToken.IsEmpty()) {
                m_list.Add(currentToken.Trim().Trim(false));
                currentToken.Clear();
            }
            break;
        case '>':
            --depth;
            if(depth == 0) {
                // done
                if(!currentToken.IsEmpty()) {
                    m_list.Add(currentToken.Trim().Trim(false));
                    currentToken.Clear();
                }
                cont = false;
            } else {
                currentToken << token.text << " ";
            }
            break;
        case '<':
            ++depth;
            currentToken << token.text << " ";
            break;
        default:
            currentToken << token.text << " ";
            break;
        }
    }
}
