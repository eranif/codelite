#include "CxxScannerBase.h"
#include "CxxPreProcessor.h"
#include "fileutils.h"

CxxScannerBase::CxxScannerBase(CxxPreProcessor* preProcessor, const wxFileName& filename)
    : m_scanner(NULL)
    , m_filename(filename)
    , m_preProcessor(preProcessor)
{
    wxString content;
    FileUtils::ReadFileContent(filename, content, wxConvISO8859_1);
    m_scanner = ::LexerNew(content, m_preProcessor->GetOptions());
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
