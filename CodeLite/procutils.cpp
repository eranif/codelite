//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : procutils.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifdef __FreeBSD__
#include <fcntl.h>
#include <kvm.h>
#include <paths.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#endif

#include "asyncprocess.h"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "file_logger.h"
#include "fileutils.h"
#include "processreaderthread.h"
#include "procutils.h"
#include "winprocess.h"
#include "wx/tokenzr.h"

#include <memory>
#include <stdio.h>
#ifdef __WXMSW__
#include "wx/msw/private.h"
#include "wx/textbuf.h"
#ifndef pclose
#define pclose _pclose
#endif

#ifndef popen
#define popen _popen
#endif

#endif

//-------------------------------------------------------------------
// clShellProcessEvent
//-------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_SHELL_ASYNC_PROCESS_TERMINATED, clShellProcessEvent);
clShellProcessEvent::clShellProcessEvent(const clShellProcessEvent& event) { *this = event; }

clShellProcessEvent::clShellProcessEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clShellProcessEvent::~clShellProcessEvent() {}

clShellProcessEvent& clShellProcessEvent::operator=(const clShellProcessEvent& src)
{
    clCommandEvent::operator=(src);
    m_pid = src.m_pid;
    m_exitCode = src.m_exitCode;
    m_output = src.m_output;
    return *this;
}

//-------------------------------------------------------------------
// clShellProcessEvent
//-------------------------------------------------------------------

ProcUtils::ProcUtils() {}

ProcUtils::~ProcUtils() {}

void ProcUtils::GetProcTree(std::map<unsigned long, bool>& parentsMap, long pid)
{
#ifdef __WXMSW__
    OSVERSIONINFO osver;

    // Check to see if were running under Windows95 or
    // Windows NT.
    osver.dwOSVersionInfoSize = sizeof(osver);
    if(!GetVersionEx(&osver)) {
        return;
    }

    if(osver.dwPlatformId != VER_PLATFORM_WIN32_NT) {
        return;
    }

    // get child processes of this node
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(!hProcessSnap) {
        return;
    }

    // Fill in the size of the structure before using it.
    PROCESSENTRY32 pe;
    memset(&pe, 0, sizeof(pe));
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Walk the snapshot of the processes, and for each process,
    // kill it if its parent is pid.
    if(!Process32First(hProcessSnap, &pe)) {
        // Can't get first process.
        CloseHandle(hProcessSnap);
        return;
    }

    parentsMap[pid] = true;

    do {
        if(parentsMap.find(pe.th32ParentProcessID) != parentsMap.end()) {
            // get the process handle
            HANDLE hProcess = ::OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION,
                                            FALSE, // not inheritable
                                            (DWORD)pid);
            if(hProcess != NULL) {
                CloseHandle(hProcess);
                // dont kill the process, just keep its ID
                parentsMap[pe.th32ProcessID] = true;
            }
        }
    } while(Process32Next(hProcessSnap, &pe));
    CloseHandle(hProcessSnap);
#else
    parentsMap[pid] = true;
#endif
}

PidVec_t ProcUtils::PS(const wxString& name)
{
    PidVec_t V;
    wxString command;
    size_t IMGNAME_COL = 0;
    size_t PID_COL = 0;
    size_t MIN_COLUMNS_NUMBER = 0;

#ifdef __WXMSW__
    command << "tasklist";
    IMGNAME_COL = 0;
    PID_COL = 1;
    MIN_COLUMNS_NUMBER = 2;
#else
    command << "ps ax";
    IMGNAME_COL = 4;
    PID_COL = 0;
    MIN_COLUMNS_NUMBER = 5;
#endif
    command = WrapInShell(command);

    wxString processOutput;
    IProcess::Ptr_t p(::CreateSyncProcess(command, IProcessCreateDefault | IProcessCreateWithHiddenConsole));
    if(p) {
        p->WaitForTerminate(processOutput);
    }

    // Search for a match

    // tasklist example output:
    //
    // Image Name                     PID Session Name        Session#    Mem Usage
    //========================= ======== ================ =========== ============
    // wininit.exe                   1000 Services                   0      2,320 K
    // csrss.exe                     1008 Console                    1      3,860 K

    // ps ax example output:
    //
    //   PID    TTY      STAT   TIME COMMAND
    //    52    ?        S      0:00 dbus-launch --autolaunch 811d5cdefd9d461891b6596cca7a6233 --binary-syntax
    //  2920    ?        Ss     0:00 ssh-agent

    wxArrayString lines = ::wxStringTokenize(processOutput, "\n", wxTOKEN_STRTOK);
    for(wxString& line : lines) {
        line.Trim().Trim(false);
        wxArrayString parts = ::wxStringTokenize(line, " \t", wxTOKEN_STRTOK);
        if(parts.size() < MIN_COLUMNS_NUMBER) {
            continue;
        }
        wxString& imageName = parts.Item(IMGNAME_COL);
        wxString& pid = parts.Item(PID_COL);
        if(FileUtils::FuzzyMatch(name, imageName)) {
            long nPid = -1;
            if(pid.ToCLong(&nPid)) {
                V.push_back({ imageName, nPid });
            }
        }
    }
    return V;
}

wxString ProcUtils::GetProcessNameByPid(long pid)
{
#ifdef __WXMSW__
    // go over the process modules and get the full path of
    // the executeable
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    //  Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, (DWORD)pid);
    if(hModuleSnap == INVALID_HANDLE_VALUE) {
        return wxEmptyString;
    }

    //  Set the size of the structure before using it.
    me32.dwSize = sizeof(MODULEENTRY32);

    //  Retrieve information about the first module,
    //  and exit if unsuccessful
    if(!Module32First(hModuleSnap, &me32)) {
        CloseHandle(hModuleSnap); // Must clean up the
        // snapshot object!
        return wxEmptyString;
    }

    // get the name of the process (it is located in the first entry)
    CloseHandle(hModuleSnap);
    return me32.szExePath;

#elif defined(__FreeBSD__)
    kvm_t* kvd;
    struct kinfo_proc* ki;
    int nof_procs;
    wxString cmd;

    if(!(kvd = kvm_openfiles(_PATH_DEVNULL, _PATH_DEVNULL, NULL, O_RDONLY, NULL)))
        return wxEmptyString;

    if(!(ki = kvm_getprocs(kvd, KERN_PROC_PID, pid, &nof_procs))) {
        kvm_close(kvd);
        return wxEmptyString;
    }

    cmd = wxString(ki->ki_ocomm, wxConvUTF8);
    kvm_close(kvd);

    return (cmd);
#else
    wxArrayString output;
    ExecuteCommand(wxT("ps -A -o pid,command --no-heading"), output);
    // parse the output and search for our process ID
    for(size_t i = 0; i < output.GetCount(); i++) {
        wxString line = output.Item(i);
        // remove whitespaces
        line = line.Trim();
        line = line.Trim(false);
        // get the process ID
        wxString spid = line.BeforeFirst(wxT(' '));
        long cpid(0);
        spid.ToLong(&cpid);
        if(cpid == pid) {
            // we got a match, extract the command, it is in the second column
            wxString command = line.AfterFirst(wxT(' '));
            return command;
        }
    }
    return wxEmptyString; // Not implemented yet
#endif
}

void ProcUtils::ExecuteCommand(const wxString& command, wxArrayString& output, long flags)
{
#ifdef __WXMSW__
    wxExecute(command, output, flags);
#else
    FILE* fp;
    char line[512];
    memset(line, 0, sizeof(line));
    fp = popen(command.mb_str(wxConvUTF8), "r");
    if(fp) {
        while(fgets(line, sizeof(line), fp)) {
            output.Add(wxString(line, wxConvUTF8));
            memset(line, 0, sizeof(line));
        }
        pclose(fp);
    }
#endif
}

void ProcUtils::ExecuteInteractiveCommand(const wxString& command) { wxShell(command); }

void ProcUtils::GetProcessList(std::vector<ProcessEntry>& proclist)
{
#ifdef __WXMSW__
    OSVERSIONINFO osver;

    // Check to see if were running under Windows95 or
    // Windows NT.
    osver.dwOSVersionInfoSize = sizeof(osver);
    if(!GetVersionEx(&osver)) {
        return;
    }

    if(osver.dwPlatformId != VER_PLATFORM_WIN32_NT) {
        return;
    }

    // get child processes of this node
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(!hProcessSnap) {
        return;
    }

    // Fill in the size of the structure before using it.
    PROCESSENTRY32 pe;
    memset(&pe, 0, sizeof(pe));
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Walk the snapshot of the processes, and for each process,
    // kill it if its parent is pid.
    if(!Process32First(hProcessSnap, &pe)) {
        // Can't get first process.
        CloseHandle(hProcessSnap);
        return;
    }

    do {
        ProcessEntry entry;
        entry.name = pe.szExeFile;
        entry.pid = (long)pe.th32ProcessID;
        proclist.push_back(entry);
    } while(Process32Next(hProcessSnap, &pe));
    CloseHandle(hProcessSnap);

#elif defined(__FreeBSD__)
    kvm_t* kvd;
    struct kinfo_proc* ki;
    int nof_procs, i;

    if(!(kvd = kvm_openfiles(_PATH_DEVNULL, _PATH_DEVNULL, NULL, O_RDONLY, NULL)))
        return;

    if(!(ki = kvm_getprocs(kvd, KERN_PROC_PROC, 0, &nof_procs))) {
        kvm_close(kvd);
        return;
    }

    for(i = 0; i < nof_procs; i++) {
        ProcessEntry entry;
        entry.pid = ki[i].ki_pid;
        entry.name = wxString(ki[i].ki_ocomm, wxConvUTF8);
        proclist.push_back(entry);
    }
    kvm_close(kvd);

#else
    // GTK and other
    wxArrayString output;
#if defined(__WXGTK__)
    ExecuteCommand(wxT("ps -A -o pid,command  --no-heading"), output);
#elif defined(__WXMAC__)
    // Mac does not like the --no-heading...
    ExecuteCommand(wxT("ps -A -o pid,command "), output);
#endif
    for(size_t i = 0; i < output.GetCount(); i++) {
        wxString line = output.Item(i);
        // remove whitespaces
        line = line.Trim().Trim(false);

        // get the process ID
        ProcessEntry entry;
        wxString spid = line.BeforeFirst(wxT(' '));
        spid.ToLong(&entry.pid);
        entry.name = line.AfterFirst(wxT(' '));

        if(entry.pid == 0 && i > 0) {
            // probably this line belongs to the provious one
            ProcessEntry e = proclist.back();
            proclist.pop_back();
            e.name << entry.name;
            proclist.push_back(e);
        } else {
            proclist.push_back(entry);
        }
    }
#endif
}

void ProcUtils::GetChildren(long pid, std::vector<long>& proclist)
{
#ifdef __WXMSW__
    OSVERSIONINFO osver;

    // Check to see if were running under Windows95 or
    // Windows NT.
    osver.dwOSVersionInfoSize = sizeof(osver);
    if(!GetVersionEx(&osver)) {
        return;
    }

    if(osver.dwPlatformId != VER_PLATFORM_WIN32_NT) {
        return;
    }

    // get child processes of this node
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(!hProcessSnap) {
        return;
    }

    // Fill in the size of the structure before using it.
    PROCESSENTRY32 pe;
    memset(&pe, 0, sizeof(pe));
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Walk the snapshot of the processes, and for each process,
    // kill it if its parent is pid.
    if(!Process32First(hProcessSnap, &pe)) {
        // Can't get first process.
        CloseHandle(hProcessSnap);
        return;
    }

    // loop over all processes and collect all the processes their parent
    // pid matches PID
    do {
        if((long)pe.th32ParentProcessID == pid) {
            proclist.push_back((long)pe.th32ProcessID);
        }
    } while(Process32Next(hProcessSnap, &pe));
    CloseHandle(hProcessSnap);

#elif defined(__FreeBSD__)
    kvm_t* kvd;
    struct kinfo_proc* ki;
    int nof_procs, i;

    if(!(kvd = kvm_openfiles(_PATH_DEVNULL, _PATH_DEVNULL, NULL, O_RDONLY, NULL)))
        return;

    if(!(ki = kvm_getprocs(kvd, KERN_PROC_PROC, pid, &nof_procs))) {
        kvm_close(kvd);
        return;
    }

    for(i = 0; i < nof_procs; i++) {
        if(ki[i].ki_ppid == pid)
            proclist.push_back(ki[i].ki_pid);
    }

    kvm_close(kvd);

#else
    // GTK and other
    wxArrayString output;
#ifdef __WXGTK__
    ExecuteCommand(wxT("ps -A -o pid,ppid  --no-heading"), output);
#else
    ExecuteCommand(wxT("ps -A -o pid,ppid "), output);
#endif
    // parse the output and search for our process ID
    for(size_t i = 0; i < output.GetCount(); i++) {
        long lpid(0);
        long lppid(0);
        wxString line = output.Item(i);

        // remove whitespaces
        line = line.Trim().Trim(false);

        // get the process ID
        wxString spid = line.BeforeFirst(wxT(' '));
        spid.ToLong(&lpid);

        // get the process Parent ID
        wxString sppid = line.AfterFirst(wxT(' '));
        sppid.ToLong(&lppid);
        if(lppid == pid) {
            proclist.push_back(lpid);
        }
    }
#endif
}

bool ProcUtils::Shell(const wxString& programConsoleCommand)
{
    wxString cmd;
#ifdef __WXMSW__
    wxChar* shell = wxGetenv(wxT("COMSPEC"));
    if(!shell) {
        shell = (wxChar*)wxT("CMD.EXE");
    }

    // just the shell
    cmd = shell;
#elif defined(__WXMAC__)
    wxString path = wxGetCwd();
    cmd = wxString(wxT("osascript -e 'tell application \"Terminal\"' -e 'activate' -e 'do script with command \"cd ") +
                   path + wxT("\"' -e 'end tell'"));
#else // non-windows
    // try to locate the configured terminal
    wxString terminal;
    wxString where;
    wxArrayString tokens;
    wxArrayString configuredTerminal;

    terminal = wxT("xterm");
    if(!programConsoleCommand.IsEmpty()) {
        tokens = wxStringTokenize(programConsoleCommand, wxT(" "), wxTOKEN_STRTOK);
        if(!tokens.IsEmpty()) {
            configuredTerminal = wxStringTokenize(tokens.Item(0), wxT("/"), wxTOKEN_STRTOK);
            if(!configuredTerminal.IsEmpty()) {
                terminal = configuredTerminal.Last();
                tokens.Clear();
                configuredTerminal.Clear();
            }
        }
    }
    if(Locate(terminal, where)) {
        if(terminal == wxT("konsole")) {
            wxString path = wxGetCwd();
            terminal.Clear();
            terminal << where << wxT(" --workdir \"") << path << wxT("\"");
        } else {
            terminal = where;
        }
    } else {
        return false;
    }
    cmd = terminal;
    terminal.Clear();
#endif
    return wxExecute(cmd, wxEXEC_ASYNC) != 0;
}

bool ProcUtils::Locate(const wxString& name, wxString& where)
{
    wxString command;
    wxArrayString output;
    command << wxT("which \"") << name << wxT("\"");
    ProcUtils::ExecuteCommand(command, output);

    if(output.IsEmpty() == false) {
        wxString interstingLine = output.Item(0);

        if(interstingLine.Trim().Trim(false).IsEmpty()) {
            return false;
        }

        if(!interstingLine.StartsWith(wxT("which: no "))) {
            where = output.Item(0);
            where = where.Trim().Trim(false);
            return true;
        }
    }
    return false;
}

void ProcUtils::SafeExecuteCommand(const wxString& command, wxArrayString& output)
{
#ifdef __WXMSW__
    wxString errMsg;
    LOG_IF_TRACE { clDEBUG1() << "executing process:" << command << endl; }
    std::unique_ptr<WinProcess> proc{ WinProcess::Execute(command, errMsg) };
    if(!proc) {
        return;
    }

    // wait for the process to terminate
    wxString tmpbuf;
    wxString buff;

    LOG_IF_TRACE { clDEBUG1() << "reading process output..." << endl; }
    while(proc->IsAlive()) {
        tmpbuf.Clear();
        if(proc->Read(tmpbuf)) {
            // as long as we read something, dont sleep...
            buff << tmpbuf;
        } else {
            wxThread::Sleep(1);
        }
    }
    tmpbuf.Clear();
    LOG_IF_TRACE
    {
        clDEBUG1() << "process terminated" << endl;
        // Read any unread output
        clDEBUG1() << "reading process output remainder..." << endl;
    }
    proc->Read(tmpbuf);
    while(!tmpbuf.IsEmpty()) {
        buff << tmpbuf;
        tmpbuf.Clear();
        proc->Read(tmpbuf);
    }
    proc->Cleanup();
    LOG_IF_TRACE { clDEBUG1() << "reading process output remainder...done" << endl; }

    // Convert buff into wxArrayString
    output = ::wxStringTokenize(buff, "\n", wxTOKEN_STRTOK);
#else
    ProcUtils::ExecuteCommand(command, output);
#endif
}

wxString ProcUtils::SafeExecuteCommand(const wxString& command)
{
    wxString strOut;
    wxArrayString arr;
    SafeExecuteCommand(command, arr);

    for(size_t i = 0; i < arr.GetCount(); ++i) {
        strOut << arr.Item(i) << "\n";
    }

    if(!strOut.IsEmpty()) {
        strOut.RemoveLast();
    }
    return strOut;
}

wxString ProcUtils::GrepCommandOutput(const std::vector<wxString>& cmd, const wxString& find_what)
{
    IProcess::Ptr_t proc(::CreateAsyncProcess(nullptr, cmd, IProcessCreateDefault | IProcessCreateSync));
    if(!proc) {
        return wxEmptyString;
    }

    wxString output;
    proc->WaitForTerminate(output);
    auto lines = ::wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
    for(wxString& line : lines) {
        line.Trim();
        if(line.Contains(find_what)) {
            return line;
        }
    }
    return wxEmptyString;
}

namespace
{
class ProcessHelper : public wxProcess
{
    wxEvtHandler* m_handler = nullptr;
    wxString m_output_file;
    wxString m_process_output;

private:
    void ReadOutput()
    {
        FileUtils::Deleter d{ m_output_file };
        FileUtils::ReadFileContent(m_output_file, m_process_output);
    }

public:
    ProcessHelper(wxEvtHandler* sink, const wxString& output_file)
        : m_handler(sink)
        , m_output_file(output_file)
    {
    }
    virtual ~ProcessHelper() {}

    // Notify about the process termination
    void OnTerminate(int pid, int status) override
    {
        if(status == 0) {
            ReadOutput();
        }

        clShellProcessEvent event_terminated{ wxEVT_SHELL_ASYNC_PROCESS_TERMINATED };
        event_terminated.SetPid(pid);
        event_terminated.SetExitCode(status);
        event_terminated.SetOutput(m_process_output);
        m_handler->QueueEvent(event_terminated.Clone());
        delete this;
    }
};
} // namespace

bool ProcUtils::ShellExecAsync(const wxString& command, long* pid, wxEvtHandler* sink)
{
    wxString filename = wxFileName::CreateTempFileName("clTempFile");
    wxString theCommand = wxString::Format("%s > \"%s\" 2>&1", command, filename);
    WrapInShell(theCommand);
    long rc = ::wxExecute(theCommand, wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE, new ProcessHelper(sink, filename), nullptr);
    if(rc > 0) {
        *pid = rc;
    }
    return rc > 0;
}

int ProcUtils::ShellExecSync(const wxString& command, wxString* output)
{
    wxString filename = wxFileName::CreateTempFileName("clTempFile");
    wxString theCommand = wxString::Format("%s > \"%s\" 2>&1", command, filename);
    WrapInShell(theCommand);

    wxArrayString out;
    wxArrayString err;
    int exit_code = ::wxExecute(theCommand, out, err, wxEXEC_SYNC | wxEXEC_HIDE_CONSOLE);

    // read the process output and return it
    FileUtils::Deleter d{ filename };
    FileUtils::ReadFileContent(filename, *output);
    return exit_code;
}

wxString& ProcUtils::WrapInShell(wxString& cmd)
{
    wxString command;
#ifdef __WXMSW__
    wxString shell = wxGetenv("COMSPEC");
    if(shell.IsEmpty()) {
        shell = "CMD.EXE";
    }
    command << shell << " /C ";
    if(cmd.StartsWith("\"") && !cmd.EndsWith("\"")) {
        command << "\"" << cmd << "\"";
    } else {
        command << cmd;
    }
#else
    command << "/bin/sh -c '";
    // escape any single quoutes
    cmd.Replace("'", "\\'");
    command << cmd << "'";
#endif
    cmd.swap(command);
    return cmd;
}