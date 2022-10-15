#include "DapLocator.hpp"

#include "Platform.hpp"
#include "asyncprocess.h"
#include "file_logger.h"
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
    wxArrayString paths;
    wxString lldb_vscode;
    if(!ThePlatform->Which("lldb-vscode", &lldb_vscode)) {
        return;
    }

    auto entry = create_entry("lldb-vscode", 12345, { lldb_vscode, "--port", "12345" }, DapLaunchType::LAUNCH);
    entry.SetEnvFormat(dap::EnvFormat::LIST);
    entries->push_back(entry);
}

void DapLocator::find_debugpy(std::vector<DapEntry>* entries)
{
    // locate pip first
    wxArrayString paths;
    wxString python;

    // locate python3
    if(!ThePlatform->Which("python", &python) && !ThePlatform->Which("python3", &python)) {
        return;
    }

    // we got pip
    wxString line = ProcUtils::GrepCommandOutput({ python, "-m", "pip", "list" }, "debugpy");
    if(line.empty())
        return;

    // we have a match
    auto entry =
        create_entry("debugpy", 12345,
                     { python, "-m", "debugpy", "--listen", "12345", "--wait-for-client", "$(CurrentFileFullPath)" },
                     DapLaunchType::ATTACH);
    entry.SetUseNativePath();
    entries->push_back(entry);
}
