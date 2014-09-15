#include "CxxScannerBase.h"
#include "CxxPreProcessor.h"

CxxScannerBase::CxxScannerBase(CxxPreProcessor* preProcessor, const wxFileName& filename)
    : m_scanner(NULL)
    , m_filename(filename)
    , m_preProcessor(preProcessor)
{
    m_scanner = ::LexerNew(m_filename.GetFullPath(), m_preProcessor->GetOptions());
}

CxxScannerBase::~CxxScannerBase()
{
    if(m_scanner) {
        ::LexerDestroy(&m_scanner);
    }
}

void CxxScannerBase::Parse()
{
    CxxLexerToken token;
    while(::LexerNext(m_scanner, token)) {
        OnToken(token);
    }
}
