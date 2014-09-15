#include "CxxUsingNamespaceCollector.h"
#include "CxxScannerTokens.h"
#include "CxxPreProcessor.h"

CxxUsingNamespaceCollector::CxxUsingNamespaceCollector(CxxPreProcessor* preProcessor, const wxFileName& filename)
    : CxxScannerBase(preProcessor, filename)
{
}

CxxUsingNamespaceCollector::~CxxUsingNamespaceCollector() {}

void CxxUsingNamespaceCollector::OnToken(CxxLexerToken& token)
{
    switch(token.type) {
    case T_PP_INCLUDE_FILENAME: {
        // we found an include statement, recurse into it
        wxFileName include;
        if(m_preProcessor->ExpandInclude(m_filename, token.text, include)) {
            CxxUsingNamespaceCollector* scanner = new CxxUsingNamespaceCollector(m_preProcessor, include);
            scanner->Parse();
            wxDELETE(scanner);
            DEBUGMSG("<== Resuming parser on file: %s\n", m_filename.GetFullPath());
        }
        break;
    }
    case T_USING:
        ParseUsingNamespace();
        break;
    default:
        break;
    }
}

void CxxUsingNamespaceCollector::ParseUsingNamespace()
{
    // Get the next token
    CxxLexerToken token;
    if(!::LexerNext(m_scanner, token)) return;
    if(token.type != T_NAMESPACE) return;

    if(!::LexerNext(m_scanner, token)) return;

    // Read everything until we find the ';'
    wxString usingNamespace;
    while(::LexerNext(m_scanner, token)) {
        if(token.type == ';') {
            break;
        }
        usingNamespace << token.text;
    }
    m_usingNamespaces.Add(usingNamespace);
}
