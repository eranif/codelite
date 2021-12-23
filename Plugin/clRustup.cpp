#include "clRustup.hpp"

#include "asyncprocess.h"
#include "globals.h"

#include <wx/tokenzr.h>
#include <wx/utils.h>

clRustup::clRustup() {}

clRustup::~clRustup() {}

bool clRustup::FindExecutable(const wxString& name, wxString* opath) const
{
    // define the *known* targets
    wxString homedir;
#ifdef __WXMSW__
    ::wxGetEnv("USERPROFILE", &homedir);
    wxFileName rustup(homedir, "rustup.exe");
#else
    ::wxGetEnv("HOME", &homedir);
    wxFileName rustup(homedir, "rustup");
#endif
    rustup.AppendDir(".cargo");
    rustup.AppendDir("bin");

    if(!rustup.FileExists()) {
        return false;
    }

    wxString cmd;
    wxString cmd_output;
    cmd << rustup.GetFullPath();
    ::WrapWithQuotes(cmd);

    cmd << " target list";
    IProcess::Ptr_t p(::CreateSyncProcess(cmd, IProcessCreateDefault | IProcessWrapInShell));
    p->WaitForTerminate(cmd_output);

    wxString target;
    auto lines = ::wxStringTokenize(cmd_output, "\r\n", wxTOKEN_STRTOK);
    for(auto& line : lines) {
        line.Trim().Trim(false);
        if(line.Contains("(installed")) {
            target = line.BeforeFirst(' ');
            target.Trim().Trim(false);
            break;
        }
    }

    if(target.empty()) {
        return false;
    }

    // check the stable channel
    wxString stable;
    wxString nightly;

    stable << homedir << wxFileName::GetPathSeparator() << ".rustup" << wxFileName::GetPathSeparator() << "toolchains"
           << wxFileName::GetPathSeparator() << "stable-" << target << wxFileName::GetPathSeparator() << "bin";
    nightly << homedir << wxFileName::GetPathSeparator() << ".rustup" << wxFileName::GetPathSeparator() << "toolchains"
            << wxFileName::GetPathSeparator() << "nightly-" << target << wxFileName::GetPathSeparator() << "bin";

    wxString exe_name = name;
#ifdef __WXMSW__
    exe_name << ".exe";
#endif

    if(wxFileName::DirExists(stable)) {
        // check for the executable here
        wxFileName fn(stable, exe_name);
        if(fn.FileExists()) {
            *opath = fn.GetFullPath();
            return true;
        }
    }

    if(wxFileName::DirExists(nightly)) {
        // check for the executable here
        wxFileName fn(nightly, exe_name);
        if(fn.FileExists()) {
            *opath = fn.GetFullPath();
            return true;
        }
    }
    return false;
}
