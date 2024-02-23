#include "LSPClangdDetector.hpp"

#include "CompilerLocatorCLANG.h"
#include "MSYS2.hpp"
#include "Platform.hpp"
#include "asyncprocess.h"
#include "clFilesCollector.h"
#include "file_logger.h"
#include "globals.h"
#include "procutils.h"
#include "tags_options_data.h"

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
    if (!ThePlatform->WhichWithVersion("clangd", { 12, 13, 14, 15, 16, 17, 18, 19, 20 }, &path)) {
        return false;
    }

    LSP_DEBUG() << "Found clangd ==>" << path << endl;
    ConfigureFile(path);
    return true;
}

void LSPClangdDetector::ConfigureFile(const wxFileName& clangdExe)
{
    LSP_DEBUG() << "==> Found" << clangdExe;
    wxString command;
    command << clangdExe.GetFullPath();
    ::WrapWithQuotes(command);

    clConfig ccConfig("code-completion.conf");
    TagsOptionsData tagsOptionsData;
    ccConfig.ReadItem(&tagsOptionsData);

    size_t limit_results = tagsOptionsData.GetCcNumberOfDisplayItems();
    command << " --limit-results=" << limit_results
            << " --header-insertion-decorators=0 --compile-commands-dir=$(WorkspacePath)";

    SetCommand(command);
    // Add support for the languages
    GetLangugaes().Add("c");
    GetLangugaes().Add("cpp");
    SetConnectionString("stdio");
}
