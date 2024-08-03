#include "LSPCTagsdDetector.hpp"

#include "cl_standard_paths.h"
#include "file_logger.h"
#include "globals.h"

#include <wx/filename.h>

LSPCTagsdDetector::LSPCTagsdDetector()
    : LSPDetector("ctagsd")
{
}

LSPCTagsdDetector::~LSPCTagsdDetector() {}

bool LSPCTagsdDetector::DoLocate()
{
    wxFileName ctagsd(clStandardPaths::Get().GetBinaryFullPath("ctagsd"));
    if(ctagsd.FileExists()) {
        ConfigureFile(ctagsd);
        return true;
    }
    return false;
}

void LSPCTagsdDetector::ConfigureFile(const wxFileName& ctagsd_exe)
{
    LSP_DEBUG() << "==> Found" << ctagsd_exe << endl;
    wxString command;
    command << ctagsd_exe.GetFullPath();
    ::WrapWithQuotes(command);

    command << " --port 45634 --log-level ERR";
    SetCommand(command);
    // Add support for the languages
    GetLanguages().Add("c");
    GetLanguages().Add("cpp");
    SetConnectionString("tcp://127.0.0.1:45634");
    SetEnabled(false);
}
