#include "LSPPythonDetector.hpp"

#include "AsyncProcess/asyncprocess.h"
#include "Platform/Platform.hpp"
#include "file_logger.h"
#include "globals.h"
#include "procutils.h"

#include <cstdlib>
#include <wx/filename.h>

LSPPythonDetector::LSPPythonDetector()
    : LSPDetector("python")
{
}

LSPPythonDetector::~LSPPythonDetector() {}

bool LSPPythonDetector::DoLocate()
{
    wxString python;

    // locate python3
    if (!ThePlatform->Which("python", &python) && !ThePlatform->Which("python3", &python)) {
        return false;
    }

#if defined(__WXMAC__)
    // On macOS it is common to install pylsp using brew
    // which will not be shown via the `pip list` command
    // so try it first
    wxString pylsp;
    if (ThePlatform->Which("pylsp", &pylsp)) {
        ::WrapWithQuotes(pylsp);
        ConfigurePylsp(pylsp);
        return true;
    }
#endif

    // Check if python-language-server is installed
    wxString output = ProcUtils::GrepCommandOutput({ python, "-m", "pip", "list" }, "python-lsp-server");
    if (output.empty()) {
        // Not installed
        return false;
    }

    // We have it installed
    wxString command;
    ::WrapWithQuotes(python);

    command << python << " -m pylsp";
    ConfigurePylsp(command);
    return true;
}

void LSPPythonDetector::ConfigurePylsp(const wxString& pylsp)
{
    SetCommand(pylsp);
    // Add support for the languages
    GetLanguages().Add("python");
    SetConnectionString("stdio");
}
