#include "LSPRustAnalyzerDetector.hpp"
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
    clRustup rustup;
    wxString anaylzer_path;
    if(!rustup.FindExecutable("rust-analyzer", &anaylzer_path)) {
        return false;
    }

    // we check for the binary in both
    wxString command;
    command << anaylzer_path;
    ::WrapWithQuotes(command);
    SetCommand(command);

    // Add support for the languages
    GetLangugaes().Add("rust");
    SetConnectionString("stdio");
    SetPriority(100); // Give it a higher priority than the rls
    return true;
}
