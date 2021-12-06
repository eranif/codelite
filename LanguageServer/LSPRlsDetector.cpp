#include "LSPRlsDetector.hpp"
#include "file_logger.h"
#include "globals.h"
#include <wx/filename.h>
#include <wx/utils.h>

LSPRlsDetector::LSPRlsDetector()
    : LSPDetector("rust")
{
}

LSPRlsDetector::~LSPRlsDetector() {}

bool LSPRlsDetector::DoLocate()
{
    // rls exists under ~/.cargo/bin/rls
    wxString homedir;
#ifdef __WXMSW__
    ::wxGetEnv("USERPROFILE", &homedir);
    wxFileName rls(homedir, "rls.exe");
#else
    ::wxGetEnv("HOME", &homedir);
    wxFileName rls(homedir, "rls");
#endif
    rls.AppendDir(".cargo");
    rls.AppendDir("bin");
    if(!rls.FileExists()) {
        return false;
    }

    clDEBUG() << "==> Found" << rls << endl;
    wxString command;
    command << rls.GetFullPath();
    ::WrapWithQuotes(command);
    SetCommand(command);

    // Add support for the languages
    GetLangugaes().Add("rust");
    SetConnectionString("stdio");
    SetPriority(75);   // rls is the second best choice after rust-analyzer
    SetEnabled(false); // by default disable this LSP
    return true;
}
