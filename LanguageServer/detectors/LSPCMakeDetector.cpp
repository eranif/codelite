#include "LSPCMakeDetector.hpp"

#include "Platform/Platform.hpp"
#include "StringUtils.h"

LSPCMakeDetector::LSPCMakeDetector()
    : LSPDetector("cmake")
{
}

LSPCMakeDetector::~LSPCMakeDetector() {}

bool LSPCMakeDetector::DoLocate()
{
    wxString name = "cmake-language-server";
    wxString fullpath;
    if(!ThePlatform->Which(name, &fullpath)) {
        return false;
    }

    ConfigureFile(fullpath);
    return true;
}

void LSPCMakeDetector::ConfigureFile(const wxFileName& cmakelsp)
{
    LSP_DEBUG() << "==> Found" << cmakelsp << endl;
    wxString command = StringUtils::WrapWithDoubleQuotes(cmakelsp.GetFullPath());

    SetCommand(command);
    // Add support for the languages
    GetLanguages().Add("cmake");
    SetConnectionString("stdio");
    SetEnabled(true);
}
