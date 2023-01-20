#include "LSPRustAnalyzerDetector.hpp"

#include "Platform.hpp"
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
    if(!ThePlatform->Which("rust-analyzer", &analyzer_path)) {
        return false;
    }

    // we check for the binary in both
    wxString command;
    command << analyzer_path;
    ::WrapWithQuotes(command);
    SetCommand(command);

    // Add support for the languages
    GetLangugaes().Add("rust");
    SetConnectionString("stdio");
    SetPriority(100); // Give it a higher priority over the rls
    return true;
}
