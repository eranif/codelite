#include "LSP/detectors/LSPPhPhantomDetector.hpp"

#include "Platform/Platform.hpp"
#include "StringUtils.h"

LSPPhPhantomDetector::LSPPhPhantomDetector()
    : LSPDetector("PHPhantom")
{
}

bool LSPPhPhantomDetector::DoLocate()
{
    wxString name = "phpantom_lsp";
    const auto fullpath = ThePlatform->Which(name);
    if (!fullpath) {
        return false;
    }

    ConfigureFile(*fullpath);
    return true;
}

void LSPPhPhantomDetector::ConfigureFile(const wxFileName& phphantom_exe)
{
    LSP_DEBUG() << "==> Found" << phphantom_exe << endl;
    wxString command = StringUtils::WrapWithDoubleQuotes(phphantom_exe.GetFullPath());

    SetCommand(command);
    // Add support for the languages
    GetLanguages().Add("php");
    SetConnectionString("stdio");
    SetEnabled(true);
}
