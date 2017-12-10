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
    switch(token.GetType()) {
    case T_PP_INCLUDE_FILENAME: {
        // we found an include statement, recurse into it
        wxFileName include;
        if(m_preProcessor->CanGoDeeper() && m_preProcessor->ExpandInclude(m_filename, token.GetText(), include)) {
            CxxUsingNamespaceCollector* scanner = new CxxUsingNamespaceCollector(m_preProcessor, include);
            m_preProcessor->IncDepth();
            scanner->Parse();
            m_preProcessor->DecDepth();

            // Append the matched results to the current parser results
            m_usingNamespaces.insert(
                m_usingNamespaces.end(), scanner->GetUsingNamespaces().begin(), scanner->GetUsingNamespaces().end());
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
    if(token.GetType() != T_NAMESPACE) return;

    // Read everything until we find the ';'
    wxString usingNamespace;
    while(::LexerNext(m_scanner, token)) {
        if(token.GetType() == ';') {
            break;
        }
        usingNamespace << token.GetText();
    }
    if(!usingNamespace.IsEmpty()) {
        m_usingNamespaces.Add(usingNamespace);
    }
}
