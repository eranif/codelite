#include "LSPRustAnalyzerDetector.hpp"

#include "AsyncProcess/asyncprocess.h"
#include "Platform/Platform.hpp"
#include "Rust/clRustup.hpp"
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
    GetLanguages().Add("rust");
    SetConnectionString("stdio");
    return true;
}
