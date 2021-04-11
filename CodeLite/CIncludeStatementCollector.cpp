#include "CIncludeStatementCollector.h"
#include "CxxPreProcessor.h"
#include "CxxScannerTokens.h"
#include "file_logger.h"

CIncludeStatementCollector::CIncludeStatementCollector(CxxPreProcessor* pp, const wxFileName& filename,
                                                       std::unordered_set<wxString>& visitedFiles)
    : CxxScannerBase(pp, filename)
    , m_visitedFiles(visitedFiles)
{
}

CIncludeStatementCollector::~CIncludeStatementCollector()
{
}

void CIncludeStatementCollector::OnToken(CxxLexerToken& token)
{
    // Pre Processor state
    switch(token.GetType()) {
    case T_PP_INCLUDE_FILENAME: {
        // we found an include statement, recurse into it
        wxFileName include;
        if(m_preProcessor->ExpandInclude(m_filename, token.GetWXString(), include)) {
            CIncludeStatementCollector scanner(m_preProcessor, include, m_visitedFiles);
            scanner.Parse();
            clDEBUG1() << "<== Resuming parser on file:" << m_filename << clEndl;
        }
        break;
    }
    default:
        break;
    }
}
