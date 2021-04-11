#include "CxxPreProcessor.h"
#include "CxxScannerTokens.h"
#include "CxxUsingNamespaceCollector.h"

CxxUsingNamespaceCollector::CxxUsingNamespaceCollector(CxxPreProcessor* preProcessor, const wxFileName& filename,
                                                       std::unordered_set<wxString>& visitedFiles)
    : CxxScannerBase(preProcessor, filename)
    , m_visitedFiles(visitedFiles)
{
}

CxxUsingNamespaceCollector::~CxxUsingNamespaceCollector()
{
}

void CxxUsingNamespaceCollector::OnToken(CxxLexerToken& token)
{
    switch(token.GetType()) {
    case T_PP_INCLUDE_FILENAME: {
        // we found an include statement, recurse into it
        wxFileName include;
        if(m_preProcessor->CanGoDeeper() && m_preProcessor->ExpandInclude(m_filename, token.GetText(), include) &&
           m_visitedFiles.count(include.GetFullPath()) == 0) {
            m_visitedFiles.insert(include.GetFullPath());
            CxxUsingNamespaceCollector scanner(m_preProcessor, include, m_visitedFiles);
            m_preProcessor->IncDepth();
            scanner.Parse();
            m_preProcessor->DecDepth();

            // Append the matched results to the current parser results
            m_usingNamespaces.insert(m_usingNamespaces.end(), scanner.GetUsingNamespaces().begin(),
                                     scanner.GetUsingNamespaces().end());
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
    if(!::LexerNext(m_scanner, token))
        return;
    if(token.GetType() != T_NAMESPACE)
        return;

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
