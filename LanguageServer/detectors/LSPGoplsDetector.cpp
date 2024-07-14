#include "LSPGoplsDetector.hpp"

#include "Platform/Platform.hpp"
#include "globals.h"

LSPGoplsDetector::LSPGoplsDetector()
    : LSPDetector("gopls")
{
}

LSPGoplsDetector::~LSPGoplsDetector() {}

bool LSPGoplsDetector::DoLocate()
{
    wxString path;
    if (!ThePlatform->Which("gopls", &path)) {
        return false;
    }

    LSP_DEBUG() << "Found gopls ==>" << path << endl;
    ConfigureFile(path);
    return true;
}
void LSPGoplsDetector::ConfigureFile(const wxFileName& gopls)
{
    LSP_DEBUG() << "==> Found" << gopls;
    wxString command;
    command << gopls.GetFullPath();
    ::WrapWithQuotes(command);

    SetCommand(command);
    SetEnabled(true);

    // Add support for the languages
    GetLangugaes().Add("go");
    SetConnectionString("stdio");
}
