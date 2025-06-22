#include "DapLocator.hpp"

#include "Platform/Platform.hpp"
#include "StdToWX.h"
#include "file_logger.h"
#include "globals.h"
#include "procutils.h"

DapLocator::DapLocator() {}

DapLocator::~DapLocator() {}

size_t DapLocator::Locate(std::vector<DapEntry>* entries)
{
    find_lldb_dap(entries);
    find_debugpy(entries);
    find_gdb(entries);
    return entries->size();
}

namespace
{
wxString wrap_string(const wxString& str)
{
    wxString s = str;
    if (s.Contains(" ")) {
        s.Prepend("\"").Append("\"");
    }
    return s;
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
    for (const wxString& c : cmd) {
        command << wrap_string(c) << " ";
    }
    command.RemoveLast();
    entry.SetCommand(command);
    entry.SetLaunchType(launch_type);
    entry.SetEnvFormat(dap::EnvFormat::DICTIONARY);
    return entry;
}

DapEntry create_entry_stdio(const wxString& name, const std::vector<wxString>& cmd, DapLaunchType launch_type)
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
    entry.SetConnectionString("stdio");

    wxString command;
    for (const wxString& c : cmd) {
        command << wrap_string(c) << " ";
    }
    command.RemoveLast();
    entry.SetCommand(command);
    entry.SetLaunchType(launch_type);
    entry.SetEnvFormat(dap::EnvFormat::DICTIONARY);
    return entry;
}
} // namespace

void DapLocator::find_lldb_dap(std::vector<DapEntry>* entries)
{
    // Since LLVM-18, lldb-vscode was renamed to lldb-dap, try to find it as well
    const wxArrayString names = StdToWX::ToArrayString({ "lldb-dap", "lldb-vscode" });

    const auto lldb_debugger = ThePlatform->AnyWhich(names);
    if (!lldb_debugger) {
        return;
    }

    const wxString entry_name = wxFileName(*lldb_debugger).GetName();
#ifdef __WXMAC__
    auto entry = create_entry(entry_name, 12345, { *lldb_debugger, "--port", "12345" }, DapLaunchType::LAUNCH);
    entry.SetEnvFormat(dap::EnvFormat::LIST);
    entries->push_back(entry);
#else
    auto entry = create_entry_stdio(entry_name, { *lldb_debugger }, DapLaunchType::LAUNCH);
    entry.SetEnvFormat(dap::EnvFormat::LIST);
    entries->push_back(entry);
#endif
}

void DapLocator::find_debugpy(std::vector<DapEntry>* entries)
{
    // locate pip first
    wxArrayString paths;
    wxString python;

    // locate python3
    if (!ThePlatform->Which("python", &python) && !ThePlatform->Which("python3", &python)) {
        return;
    }

    // we got pip
    wxString line = ProcUtils::GrepCommandOutput({ python, "-m", "pip", "list" }, "debugpy");
    if (line.empty())
        return;

    // we have a match
    auto entry =
        create_entry("debugpy", 12345,
                     { python, "-m", "debugpy", "--listen", "12345", "--wait-for-client", "$(CurrentFileFullPath)" },
                     DapLaunchType::ATTACH);
    entry.SetUseNativePath();
    entries->push_back(entry);
}

void DapLocator::find_gdb(std::vector<DapEntry>* entries)
{
    // gdb, since version 14.0, supports the dap protocol
    wxArrayString paths;
    wxString gdb;

    // locate python3
    if (!ThePlatform->Which("gdb", &gdb)) {
        return;
    }

    clDEBUG() << "Found gdb at:" << gdb << endl;

    // Check the version. An example version:
    // GNU gdb (Ubuntu 12.1-0ubuntu1~22.04.2) 12.1
    // GNU gdb (GDB) 15.2
    static wxRegEx re_version(R"(GNU gdb \(.*?\) ([0-9\.]+))");
    unsigned long major_version = 0;
    ProcUtils::GrepCommandOutputWithCallback({ gdb, "-v" }, [&](const wxString& line) {
        clDEBUG() << "Checking line..." << line << endl;
        if (re_version.IsValid() && re_version.Matches(line)) {
            // Got the version line, extract its major version
            auto major_version_string = re_version.GetMatch(line, 1).BeforeFirst('.');
            clDEBUG() << "Found gdb line version:" << line << endl;
            clDEBUG() << "Version:" << major_version_string << endl;
            major_version_string.ToCULong(&major_version);
            // We can stop processing lines
            return true;
        }
        return false;
    });

    if (major_version >= 14) {
        // we have a match
        auto entry = create_entry_stdio("gdb-dap", { gdb, "-i=dap" }, DapLaunchType::LAUNCH);
        entry.SetUseForwardSlash(true);
        entry.SetUseVolume(true);
        entries->push_back(entry);
    }
}
