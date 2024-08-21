#include "LSPTypeScriptDetector.hpp"

#include "NodeJSLocator.h"
#include "StdToWX.h"
#include "clNodeJS.h"
#include "fileutils.h"
#include "globals.h"

LSPTypeScriptDetector::LSPTypeScriptDetector()
    : LSPDetector("TypeScript")
{
}

LSPTypeScriptDetector::~LSPTypeScriptDetector() {}

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
    WrapWithQuotes(command);

    command << " --stdio";
    SetCommand(command);
    SetLanguages(StdToWX::ToArrayString({ "javascript", "typescript" }));
    SetConnectionString("stdio");
    return true;
}
