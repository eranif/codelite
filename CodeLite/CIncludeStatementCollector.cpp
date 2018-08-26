#include "CIncludeStatementCollector.h"
#include "CxxScannerTokens.h"
#include "CxxPreProcessor.h"
#include "file_logger.h"

CIncludeStatementCollector::CIncludeStatementCollector(CxxPreProcessor* pp, const wxFileName& filename)
    : CxxScannerBase(pp, filename)
{
}

CIncludeStatementCollector::~CIncludeStatementCollector() {}

void CIncludeStatementCollector::OnToken(CxxLexerToken& token)
{
    // Pre Processor state
    switch(token.GetType()) {
    case T_PP_INCLUDE_FILENAME: {
        // we found an include statement, recurse into it
        wxFileName include;
        if(m_preProcessor->ExpandInclude(m_filename, token.GetWXString(), include)) {
            CIncludeStatementCollector* scanner = new CIncludeStatementCollector(m_preProcessor, include);
            scanner->Parse();
            wxDELETE(scanner);
            clDEBUG1() << "<== Resuming parser on file:" << m_filename << clEndl;
        }
        break;
    }
    default:
        break;
    }
}
