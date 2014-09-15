#include "CxxPreProcessorIncludeStatementCollector.h"
#include "CxxScannerTokens.h"
#include "CxxPreProcessor.h"
#include "file_logger.h"

CIncludeStatementCollector::CIncludeStatementCollector(const wxFileName& filename, size_t options)
    : m_scanner(NULL)
    , m_filename(filename)
    , m_options(options)
{
    m_scanner = ::LexerNew(m_filename.GetFullPath(), m_options);
}

CIncludeStatementCollector::~CIncludeStatementCollector()
{
    if(m_scanner) {
        ::LexerDestroy(&m_scanner);
    }
}

void CIncludeStatementCollector::Parse(CxxPreProcessor* pp)
{
    CxxLexerToken token;
    while(::LexerNext(m_scanner, token)) {
        // Pre Processor state
        switch(token.type) {
        case T_PP_INCLUDE_FILENAME: {
            // we found an include statement, recurse into it
            wxFileName include;
            if(pp->ExpandInclude(m_filename, token.text, include)) {
                CIncludeStatementCollector* scanner = new CIncludeStatementCollector(include, pp->GetOptions());
                scanner->Parse(pp);
                // make sure we always delete the scanner
                wxDELETE(scanner);
                DEBUGMSG("<== Resuming parser on file: %s\n", m_filename.GetFullPath());
            }
            break;
        }
        default:
            break;
        }
    }
}
