#include "LSPGoplsDetector.hpp"

#include "Platform/Platform.hpp"
#include "StringUtils.h"

LSPGoplsDetector::LSPGoplsDetector()
    : LSPDetector("gopls")
{
}

bool LSPGoplsDetector::DoLocate()
{
    const auto path = ThePlatform->Which("gopls");
    if (!path) {
        return false;
    }

    LSP_DEBUG() << "Found gopls ==>" << *path << endl;
    ConfigureFile(*path);
    return true;
}
void LSPGoplsDetector::ConfigureFile(const wxFileName& gopls)
{
    LSP_DEBUG() << "==> Found" << gopls;
    wxString command;
    command << gopls.GetFullPath();
    StringUtils::WrapWithQuotes(command);

    SetCommand(command);
    SetEnabled(true);

    // Need this enabled to have semantic tokens
    SetInitialiseOptions(R"({ "ui.semanticTokens": true })");

    // Add support for the languages
    GetLanguages().Add("go");
    SetConnectionString("stdio");
}
