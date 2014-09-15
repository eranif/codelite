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
    switch(token.type) {
    case T_PP_INCLUDE_FILENAME: {
        // we found an include statement, recurse into it
        wxFileName include;
        if(m_preProcessor->ExpandInclude(m_filename, token.text, include)) {
            CIncludeStatementCollector* scanner = new CIncludeStatementCollector(m_preProcessor, include);
            scanner->Parse();
            wxDELETE(scanner);
            DEBUGMSG("<== Resuming parser on file: %s\n", m_filename.GetFullPath());
        }
        break;
    }
    default:
        break;
    }
}
