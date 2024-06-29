#include "CxxTemplateFunction.h"
#include "CxxScannerTokens.h"
#include <set>

CxxTemplateFunction::CxxTemplateFunction(TagEntryPtr tag)
{
    m_scanner = ::LexerNew(tag->GetPatternClean(), kLexerOpt_None);
    m_sigScanner = ::LexerNew(tag->GetSignature(), kLexerOpt_None);
}

CxxTemplateFunction::~CxxTemplateFunction()
{
    if(m_scanner) {
        ::LexerDestroy(&m_scanner);
    }
    if(m_sigScanner) {
        ::LexerDestroy(&m_sigScanner);
    }
}

void CxxTemplateFunction::ParseDefinitionList()
{
    m_list.Clear();
    int depth(0);
    CxxLexerToken token;
    
    // scan until we find 'template' keyword
    while(::LexerNext(m_scanner, token)) {
        if(token.GetType() == T_TEMPLATE) {
            break;
        }
    }
    
    if(!token.GetType()) return; // EOF
    
    // Loop until we found the open brace '<'
    while(::LexerNext(m_scanner, token)) {
        if(token.GetType() == '<') {
            ++depth;
            break;
        }
    }
    
    // could not find it
    if(!depth) return;
    
    bool cont = true;
    wxString currentToken;
    while(cont && ::LexerNext(m_scanner, token)) {
        switch(token.GetType()) {
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
                currentToken << token.GetWXString() << " ";
            }
            break;
        case '<':
            ++depth;
            currentToken << token.GetWXString() << " ";
            break;
        default:
            currentToken << token.GetWXString() << " ";
            break;
        }
    }
}

bool CxxTemplateFunction::CanTemplateArgsDeduced()
{
    ParseDefinitionList(); // Initializes the m_list array
    
    std::set<wxString> words;
    CxxLexerToken token;
    
    // Collect all the identifiers and keep them inside a set
    while(::LexerNext(m_sigScanner, token)) {
        if(token.GetType() == T_IDENTIFIER) {
            words.insert(token.GetWXString());
        }
    }
    
    // Loop over the function arguments list and check that all of them 
    // exists in the set we created from the function signature
    for(size_t i=0; i<m_list.GetCount(); ++i) {
        if(words.count(m_list.Item(i)) == 0) {
            // this template argument could not be found in the function signature...
            return false;
        }
    }
    return true;
}
