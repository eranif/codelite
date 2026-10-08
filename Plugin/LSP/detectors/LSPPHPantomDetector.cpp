#include "LSP/detectors/LSPPHPantomDetector.hpp"

#include "Platform/Platform.hpp"
#include "StringUtils.h"

LSPPHPantomDetector::LSPPHPantomDetector()
    : LSPDetector("PHPantom")
{
}

bool LSPPHPantomDetector::DoLocate()
{
    wxString name = "phpantom_lsp";
    const auto fullpath = ThePlatform->Which(name);
    if (!fullpath) {
        return false;
    }

    ConfigureFile(*fullpath);
    return true;
}

void LSPPHPantomDetector::ConfigureFile(const wxFileName& phpantom_exe)
{
    LSP_DEBUG() << "==> Found" << phpantom_exe << endl;
    wxString command = StringUtils::WrapWithDoubleQuotes(phpantom_exe.GetFullPath());

    SetCommand(command);
    // Add support for the languages
    GetLanguages().Add("php");
    SetConnectionString("stdio");
    SetEnabled(true);
}
