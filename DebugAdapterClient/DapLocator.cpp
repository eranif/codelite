#include "DapLocator.hpp"

#include "asyncprocess.h"
#include "globals.h"
#include "processreaderthread.h"
#include "procutils.h"

#include <wx/filefn.h>

DapLocator::DapLocator() {}

DapLocator::~DapLocator() {}

size_t DapLocator::Locate(std::vector<DapEntry>* entries)
{
    find_lldb_vscode(entries);
    find_debugpy(entries);
    return entries->size();
}

namespace
{
wxString wrap_string(const wxString& str)
{
    wxString s = str;
    if(s.Contains(" ")) {
        s.Prepend("\"").Append("\"");
    }
    return s;
}

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

DapEntry create_entry(const wxString& name, int port, const std::vector<wxString>& cmd, DapLaunchType launch_type)
{
    DapEntry entry;
    entry.SetName(name);
#ifdef __WXMSW__
    entry.SetUseForwardSlash(true);
    entry.SetUseVolume(false);
    entry.SetUseRelativePath(false);
#else
    entry.SetUseNativePath();
#endif
    wxString connection_string;
    connection_string << "tcp://127.0.0.1:" << port;
    entry.SetConnectionString(connection_string);

    wxString command;
    for(const wxString& c : cmd) {
        command << wrap_string(c) << " ";
    }
    command.RemoveLast();
    entry.SetCommand(command);
    entry.SetLaunchType(launch_type);
    entry.SetEnvFormat(dap::EnvFormat::DICTIONARY);
    return entry;
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
    auto entry = create_entry("lldb-vscode", 12345, { path, "--port", "12345" }, DapLaunchType::LAUNCH);
    entry.SetEnvFormat(dap::EnvFormat::LIST);
    entries->push_back(entry);
}

void DapLocator::find_debugpy(std::vector<DapEntry>* entries)
{
    // locate pip first
    wxPathList paths;

#ifdef __WXMSW__
    // set this paths before the PATH env
    paths.Add(R"(C:\msys64\mingw64\bin")");
#endif
    paths.AddEnvList("PATH");

    // add homebrew after PATH
#if defined(__WXMAC__)
    paths.Add("/opt/homebrew/bin");
#endif

    wxArrayString names;
    names.Add(get_exe_name("pip3"));
    names.Add(get_exe_name("pip"));

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

    // we got pip
    std::vector<wxString> cmd = { path, "list" };
    IProcess::Ptr_t proc(::CreateAsyncProcess(nullptr, cmd, IProcessCreateDefault | IProcessCreateSync));
    if(!proc) {
        return;
    }

    wxString output;
    proc->WaitForTerminate(output);
    auto lines = ::wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
    for(auto& line : lines) {
        line.Trim().Trim(false);
        if(line.Contains("debugpy")) {
            wxFileName python_exe = path;
            python_exe.SetFullName("python");
            auto entry = create_entry("debugpy", 12345,
                                      { python_exe.GetFullPath(), "-m", "debugpy", "--listen", "12345",
                                        "--wait-for-client", "$(CurrentFileFullPath)" },
                                      DapLaunchType::ATTACH);
            entry.SetUseNativePath();
            entries->push_back(entry);
            return;
        }
    }
}
