#include "CxxPreProcessorScannerBase.h"
#include "CxxScannerTokens.h"
#include "CxxPreProcessor.h"
#include "file_logger.h"

CxxPreProcessorScannerBase::CxxPreProcessorScannerBase(const wxFileName& filename, size_t options)
    : m_scanner(NULL)
    , m_filename(filename)
    , m_options(options)
{
    m_scanner = ::LexerNew(m_filename.GetFullPath(), m_options);
}

CxxPreProcessorScannerBase::~CxxPreProcessorScannerBase()
{
    if(m_scanner) {
        ::LexerDestroy(&m_scanner);
    }
}

void CxxPreProcessorScannerBase::Parse(CxxPreProcessor* pp)
{
    CxxLexerToken token;
    while(::LexerNext(m_scanner, token)) {
        // Pre Processor state
        switch(token.type) {
        case T_PP_INCLUDE_FILENAME: {
            // we found an include statement, recurse into it
            wxFileName include;
            if(pp->ExpandInclude(m_filename, token.text, include)) {
                CxxPreProcessorScannerBase* scanner = new CxxPreProcessorScannerBase(include, pp->GetOptions());
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
