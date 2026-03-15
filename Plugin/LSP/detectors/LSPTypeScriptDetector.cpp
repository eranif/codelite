#include "LSPTypeScriptDetector.hpp"

#include "StringUtils.h"
#include "fileutils.h"

LSPTypeScriptDetector::LSPTypeScriptDetector()
    : LSPDetector("TypeScript")
{
}

bool LSPTypeScriptDetector::DoLocate()
{
    wxFileName typescript_lsp;
    wxArrayString hints;

    wxString fullname = "typescript-language-server";
#ifdef __WXMSW__
    wxFileName roaming(clStandardPaths::Get().GetUserDataDir(), wxEmptyString);
    roaming.RemoveLastDir();
    roaming.AppendDir("npm");
    hints.Add(roaming.GetPath());
    fullname << ".cmd";
#endif

    if(!FileUtils::FindExe(fullname, typescript_lsp, hints)) {
        return false;
    }

    wxString command;
    command << typescript_lsp.GetFullPath();
    StringUtils::WrapWithQuotes(command);

    command << " --stdio";
    SetCommand(command);
    SetLanguages({"javascript", "typescript"});
    SetConnectionString("stdio");
    return true;
}
