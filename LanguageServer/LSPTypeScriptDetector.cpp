#include "LSPTypeScriptDetector.hpp"
#include "NodeJSLocator.h"
#include "clNodeJS.h"
#include "globals.h"

LSPTypeScriptDetector::LSPTypeScriptDetector()
    : LSPDetector("TypeScript")
{
}

LSPTypeScriptDetector::~LSPTypeScriptDetector() {}

bool LSPTypeScriptDetector::DoLocate()
{
    NodeJSLocator node;
    node.Locate();

    if(node.GetNpm().empty()) {
        return false;
    }

    wxFileName typescript_lsp;
    wxArrayString hints;

    wxString fullname = "typescript-language-server";
#ifdef __WXMSW__
    hints.Add(clStandardPaths::Get().GetUserDataDir());
    fullname << ".cmd";
#else
    hints.Add(wxFileName(node.GetNpm()).GetPath());
#endif

    if(!FileUtils::FindExe(fullname, typescript_lsp, hints)) {
        return false;
    }

    if(!typescript_lsp.FileExists()) {
        return true;
    }

    wxString command;
    command << typescript_lsp.GetFullPath();
    ::WrapWithQuotes(command);

    command << " --stdio";
    SetCommand(command);
    wxArrayString langs;
    langs.Add("javascript");
    langs.Add("typescript");
    SetLangugaes(langs);
    SetConnectionString("stdio");
    SetPriority(100);
    return true;
}
