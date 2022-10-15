#include "LSPClangdDetector.hpp"

#include "CompilerLocatorCLANG.h"
#include "MSYS2.hpp"
#include "Platform.hpp"
#include "asyncprocess.h"
#include "clFilesCollector.h"
#include "file_logger.h"
#include "globals.h"
#include "procutils.h"

#include <wx/filename.h>
#include <wx/regex.h>

LSPClangdDetector::LSPClangdDetector()
    : LSPDetector("clangd")
{
}

LSPClangdDetector::~LSPClangdDetector() {}

bool LSPClangdDetector::DoLocate()
{
    wxString path;
    if(!ThePlatform->WhichWithVersion("clangd", { 12, 13, 14, 15, 16, 17, 18, 19, 20 }, &path)) {
        return false;
    }

    clDEBUG() << "Found clangd ==>" << path << endl;
    ConfigureFile(path);
    return true;
}

void LSPClangdDetector::ConfigureFile(const wxFileName& clangdExe)
{
    clDEBUG() << "==> Found" << clangdExe;
    wxString command;
    command << clangdExe.GetFullPath();
    ::WrapWithQuotes(command);

    command << " -limit-results=250 -header-insertion-decorators=0 --compile-commands-dir=$(WorkspacePath)";
    SetCommand(command);
    // Add support for the languages
    GetLangugaes().Add("c");
    GetLangugaes().Add("cpp");
    SetConnectionString("stdio");
    SetPriority(90); // clangd should override the default cc engine
}
