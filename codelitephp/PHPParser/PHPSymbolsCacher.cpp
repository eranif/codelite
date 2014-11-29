#include "PHPSymbolsCacher.h"
#include <wx/ffile.h>
#include <wx/stopwatch.h>
#include "php_code_completion.h"

PHPSymbolsCacher::PHPSymbolsCacher(PHPCodeCompletion* owner, const wxString& dbfile)
    : m_owner(owner)
    , m_filename(dbfile)
{
}

PHPSymbolsCacher::~PHPSymbolsCacher() {}

void PHPSymbolsCacher::Process(wxThread* thread)
{
    wxStopWatch sw;
    sw.Start();

    wxFFile dbFile(m_filename, "rb");
    if(dbFile.IsOpened()) {
        wxString fileContent;
        wxCSConv fontEncConv(wxFONTENCODING_ISO8859_1);
        dbFile.ReadAll(&fileContent, fontEncConv);
        dbFile.Close();
        m_owner->CallAfter(&PHPCodeCompletion::OnSymbolsCached);
    } else {
        m_owner->CallAfter(&PHPCodeCompletion::OnSymbolsCacheError);
    }
}
