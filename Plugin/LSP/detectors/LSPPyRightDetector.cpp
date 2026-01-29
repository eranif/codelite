#include "LSPPyRightDetector.hpp"

#include "Platform/MSYS2.hpp"
#include "Platform/Platform.hpp"
#include "StringUtils.h"

#include <wx/filename.h>

LSPPyRightDetector::LSPPyRightDetector()
    : LSPDetector("pyright")
{
}

bool LSPPyRightDetector::DoLocate()
{
    const auto path = ThePlatform->Which("pyright-langserver");
    if (!path) {
        return false;
    }

    LSP_DEBUG() << "Found pyright-langserver ==>" << *path << endl;
    ConfigureFile(*path);
    return true;
}

void LSPPyRightDetector::ConfigureFile(const wxFileName& tool_path)
{
    LSP_DEBUG() << "==> Found" << tool_path;
    wxString command;
    command << tool_path.GetFullPath();
    StringUtils::WrapWithQuotes(command);

    command << " --stdio";

    SetCommand(command);
    GetLanguages().Add("python");
    SetConnectionString("stdio");
}
