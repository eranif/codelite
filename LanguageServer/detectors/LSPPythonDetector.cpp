#include "LSPPythonDetector.hpp"

#include "Platform.hpp"
#include "asyncprocess.h"
#include "file_logger.h"
#include "globals.h"
#include "procutils.h"
#include "wx/filename.h"

#include <cstdlib>

LSPPythonDetector::LSPPythonDetector()
    : LSPDetector("python")
{
}

LSPPythonDetector::~LSPPythonDetector() {}

bool LSPPythonDetector::DoLocate()
{
    wxString python;

    // locate python3
    if(!ThePlatform->Which("python", &python) && !ThePlatform->Which("python3", &python)) {
        return false;
    }

    // Check if python-language-server is installed
    wxString output = ProcUtils::GrepCommandOutput({ python, "-m", "pip", "list" }, "python-lsp-server");
    if(output.empty()) {
        // Not installed
        return false;
    }

    // We have it installed
    wxString command;
    ::WrapWithQuotes(python);

    command << python << " -m pylsp";
    SetCommand(command);
    // Add support for the languages
    GetLangugaes().Add("python");
    SetConnectionString("stdio");
    SetPriority(50);
    return true;
}
