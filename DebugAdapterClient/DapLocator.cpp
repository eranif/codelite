#include "DapLocator.hpp"

#include "globals.h"

#include <wx/filefn.h>

DapLocator::DapLocator() {}

DapLocator::~DapLocator() {}

size_t DapLocator::Locate(std::vector<DapEntry>* entries)
{
    find_lldb_vscode(entries);
    return entries->size();
}

namespace
{
wxString get_exe_name(const wxString& base, size_t counter = wxString::npos)
{
    wxString exename = base;
    if(counter != wxString::npos) {
        exename << "-" << counter;
    }
#ifdef __WXMSW__
    exename << ".exe";
#endif
    return exename;
}
} // namespace

void DapLocator::find_lldb_vscode(std::vector<DapEntry>* entries)
{
    wxPathList paths;
#ifdef __WXMSW__
    // set these paths before the PATH env
    paths.Add(R"(C:\msys64\clang64\bin")");
    paths.Add(R"(C:\msys64\mingw64\bin")");
#elif defined(__WXMAC__)
    paths.Add("/opt/homebrew/opt/llvm/bin");
#endif
    paths.AddEnvList("PATH");

    wxArrayString names;
    names.reserve(30);

    names.Add(get_exe_name("lldb-vscode")); // the obvious name
    for(size_t i = 30; i >= 10; --i) {
        names.Add(get_exe_name("lldb-vscode", i));
    }

    wxString path;
    for(const auto& name : names) {
        path = paths.FindAbsoluteValidPath(name);
        if(!path.empty()) {
            break;
        }
    }

    if(path.empty()) {
        return;
    }

    // construct DapEntry
    DapEntry entry;
    entry.SetName("lldb-vscode");
    entry.SetCommand(::WrapWithQuotes(path) + " --port 12345");
    entry.SetConnectionString("tcp://127.0.0.1:12345");

#ifdef __WXMSW__
    entry.SetUseForwardSlash(true);
    entry.SetUseVolume(false);
    entry.SetUseRelativePath(false);
#else
    entry.SetUseNativePath();
#endif
    entries->push_back(entry);
}
