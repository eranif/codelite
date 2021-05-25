#include "LSPPythonDetector.hpp"
#include "asyncprocess.h"
#include "clPythonLocator.hpp"
#include "globals.h"
#include "wx/filename.h"
#include <cstdlib>

LSPPythonDetector::LSPPythonDetector()
    : LSPDetector("python")
{
}

LSPPythonDetector::~LSPPythonDetector() {}

bool LSPPythonDetector::DoLocate()
{
    clPythonLocator locator;
    if(!locator.Locate()) {
        return false;
    }

    const wxString& pythonExe = locator.GetPython();
    const wxString& pip = locator.GetPip();

    // Check if python-language-server is installed
    wxFileName fnPython(pip);
    wxString command;
    command << pip;
    ::WrapWithQuotes(command);

    command << " list";

    IProcess::Ptr_t proc(::CreateSyncProcess(command, IProcessCreateDefault, fnPython.GetPath()));
    if(!proc) {
        return false;
    }

    wxString output;
    proc->WaitForTerminate(output);

    if(!output.Contains("python-lsp-server")) {
        return false;
    }

    // We have it installed

    command.Clear();
    command << pythonExe;
    ::WrapWithQuotes(command);

    command << " -m pylsp";
    SetCommand(command);
    // Add support for the languages
    GetLangugaes().Add("python");
    SetConnectionString("stdio");
    SetPriority(50);
    return true;
}
