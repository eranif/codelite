#include "LSPRustAnalyzerDetector.hpp"

#include "MSYS2.hpp"
#include "asyncprocess.h"
#include "clRustup.hpp"
#include "file_logger.h"
#include "globals.h"
#include "macros.h"

#include <wx/tokenzr.h>

LSPRustAnalyzerDetector::LSPRustAnalyzerDetector()
    : LSPDetector("rust-analyzer")
{
}

LSPRustAnalyzerDetector::~LSPRustAnalyzerDetector() {}

bool LSPRustAnalyzerDetector::DoLocate()
{
    wxString analyzer_path;
#ifdef __WXMSW__
    wxString homedir;
    if(!MSYS2::FindHomeDir(&homedir)) {
        return false;
    }

    wxFileName ra{ homedir, "rust-analyzer.exe" };
    ra.AppendDir(".cargo");
    ra.AppendDir("bin");
    if(!ra.FileExists()) {
        return false;
    }
    analyzer_path = ra.GetFullPath();

#else
    clRustup rustup;
    if(!rustup.FindExecutable("rust-analyzer", &analyzer_path)) {
        return false;
    }
#endif

    // we check for the binary in both
    wxString command;
    command << analyzer_path;
    ::WrapWithQuotes(command);
    SetCommand(command);

    // Add support for the languages
    GetLangugaes().Add("rust");
    SetConnectionString("stdio");
    SetPriority(100); // Give it a higher priority than the rls
    return true;
}
