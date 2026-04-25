#include "LSPTypeScriptDetector.hpp"

#include "StringUtils.h"
#include "fileutils.h"

LSPTypeScriptDetector::LSPTypeScriptDetector()
    : LSPDetector("TypeScript")
{
}

bool LSPTypeScriptDetector::DoLocate()
{
    wxArrayString hints;

    wxString fullname = "typescript-language-server";
#ifdef __WXMSW__
    wxFileName roaming(clStandardPaths::Get().GetUserDataDir(), wxEmptyString);
    roaming.RemoveLastDir();
    roaming.AppendDir("npm");
    hints.Add(roaming.GetPath());
    fullname << ".cmd";
#endif

    auto typescript_lsp = FileUtils::FindExe(fullname, hints);
    if (!typescript_lsp) {
        return false;
    }

    wxString command;
    command << typescript_lsp->GetFullPath();
    StringUtils::WrapWithQuotes(command);

    command << " --stdio";
    SetCommand(command);
    SetLanguages({"javascript", "typescript"});
    SetConnectionString("stdio");
    return true;
}
