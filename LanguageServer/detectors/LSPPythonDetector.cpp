#include "LSPPythonDetector.hpp"

#include "Platform/Platform.hpp"
#include "StdToWX.h"
#include "StringUtils.h"
#include "procutils.h"

LSPPythonDetector::LSPPythonDetector()
    : LSPDetector("python")
{
}

LSPPythonDetector::~LSPPythonDetector() {}

bool LSPPythonDetector::DoLocate()
{
    // locate python3
    auto python = ThePlatform->AnyWhich(StdToWX::ToArrayString({ "python", "python3" }));
    if (!python) {
        return false;
    }

#if defined(__WXMAC__)
    // On macOS it is common to install pylsp using brew
    // which will not be shown via the `pip list` command
    // so try it first
    if (auto pylsp = ThePlatform->Which("pylsp")) {
        StringUtils::WrapWithQuotes(*pylsp);
        ConfigurePylsp(*pylsp);
        return true;
    }
#endif

    // Check if python-language-server is installed
    wxString output = ProcUtils::GrepCommandOutput({ *python, "-m", "pip", "list" }, "python-lsp-server");
    if (output.empty()) {
        // Not installed
        return false;
    }

    // We have it installed
    wxString command;
    StringUtils::WrapWithQuotes(*python);

    command << *python << " -m pylsp";
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
