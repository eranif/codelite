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
#include "wx/tokenzr.h"
#include "procutils.h"
#include "winprocess.h"

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

#ifdef __FreeBSD__
#include <kvm.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <paths.h>
#endif

ProcUtils::ProcUtils()
{
}

ProcUtils::~ProcUtils()
{
}

void ProcUtils::GetProcTree(std::map<unsigned long, bool> &parentsMap, long pid)
{
#ifdef __WXMSW__
    OSVERSIONINFO osver ;

    // Check to see if were running under Windows95 or
    // Windows NT.
    osver.dwOSVersionInfoSize = sizeof( osver ) ;
    if ( !GetVersionEx( &osver ) ) {
        return ;
    }

    if ( osver.dwPlatformId != VER_PLATFORM_WIN32_NT ) {
        return;
    }

    //get child processes of this node
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!hProcessSnap) {
        return;
    }

    //Fill in the size of the structure before using it.
    PROCESSENTRY32 pe;
    memset(&pe, 0, sizeof(pe));
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Walk the snapshot of the processes, and for each process,
    // kill it if its parent is pid.
    if (!Process32First(hProcessSnap, &pe)) {
        // Can't get first process.
        CloseHandle (hProcessSnap);
        return;
    }

    parentsMap[pid] = true;

    do {
        if (parentsMap.find(pe.th32ParentProcessID) != parentsMap.end()) {
            //get the process handle
            HANDLE hProcess = ::OpenProcess(SYNCHRONIZE |
                                            PROCESS_TERMINATE |
                                            PROCESS_QUERY_INFORMATION,
                                            FALSE, // not inheritable
                                            (DWORD)pid);
            if (hProcess != NULL) {
                CloseHandle(hProcess);
                //dont kill the process, just keep its ID
                parentsMap[pe.th32ProcessID] = true;
            }
        }
    } while (Process32Next (hProcessSnap, &pe));
    CloseHandle (hProcessSnap);
#else
    parentsMap[pid] = true;
#endif
}

wxString ProcUtils::GetProcessNameByPid(long pid)
{
#ifdef __WXMSW__
    //go over the process modules and get the full path of
    //the executeable
    MODULEENTRY32 me32;

    //  Take a snapshot of all modules in the specified process.
    HANDLE hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, (DWORD)pid );
    if ( hModuleSnap == INVALID_HANDLE_VALUE ) {
        return wxEmptyString;
    }

    //  Set the size of the structure before using it.
    me32.dwSize = sizeof( MODULEENTRY32 );

    //  Retrieve information about the first module,
    //  and exit if unsuccessful
    if (!Module32First( hModuleSnap, &me32 )) {
        CloseHandle( hModuleSnap );    // Must clean up the
        // snapshot object!
        return wxEmptyString;
    }

    //get the name of the process (it is located in the first entry)
    CloseHandle( hModuleSnap );
    return me32.szExePath;

#elif defined(__FreeBSD__)
    kvm_t *kvd;
    struct kinfo_proc *ki;
    int nof_procs;
    wxString cmd;

    if (!(kvd = kvm_openfiles(_PATH_DEVNULL, _PATH_DEVNULL, NULL, O_RDONLY, NULL)))
        return wxEmptyString;

    if (!(ki = kvm_getprocs(kvd, KERN_PROC_PID, pid, &nof_procs))) {
        kvm_close(kvd);
        return wxEmptyString;
    }

    cmd = wxString(ki->ki_ocomm, wxConvUTF8);
    kvm_close(kvd);

    return (cmd);
#else
    wxArrayString output;
    ExecuteCommand(wxT("ps -A -o pid,command --no-heading"), output);
    //parse the output and search for our process ID
    for (size_t i=0; i< output.GetCount(); i++) {
        wxString line = output.Item(i);
        //remove whitespaces
        line = line.Trim();
        line = line.Trim(false);
        //get the process ID
        wxString spid = line.BeforeFirst(wxT(' '));
        long cpid(0);
        spid.ToLong( &cpid );
        if (cpid == pid) {
            //we got a match, extract the command, it is in the second column
            wxString command = line.AfterFirst(wxT(' '));
            return command;
        }
    }
    return wxEmptyString;	//Not implemented yet
#endif
}

void ProcUtils::ExecuteCommand(const wxString &command, wxArrayString &output, long flags)
{
#ifdef __WXMSW__
    wxExecute(command, output, flags);
#else
    FILE *fp;
    char line[512];
    memset(line, 0, sizeof(line));
    fp = popen(command.mb_str(wxConvUTF8), "r");
    while ( fgets( line, sizeof line, fp)) {
        output.Add(wxString(line, wxConvUTF8));
        memset(line, 0, sizeof(line));
    }

    pclose(fp);
#endif
}

void ProcUtils::ExecuteInteractiveCommand(const wxString &command)
{
    wxShell(command);
}

void ProcUtils::GetProcessList(std::vector<ProcessEntry> &proclist)
{
#ifdef __WXMSW__
    OSVERSIONINFO osver ;

    // Check to see if were running under Windows95 or
    // Windows NT.
    osver.dwOSVersionInfoSize = sizeof( osver ) ;
    if ( !GetVersionEx( &osver ) ) {
        return;
    }

    if ( osver.dwPlatformId != VER_PLATFORM_WIN32_NT ) {
        return;
    }

    //get child processes of this node
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!hProcessSnap) {
        return;
    }

    //Fill in the size of the structure before using it.
    PROCESSENTRY32 pe;
    memset(&pe, 0, sizeof(pe));
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Walk the snapshot of the processes, and for each process,
    // kill it if its parent is pid.
    if (!Process32First(hProcessSnap, &pe)) {
        // Can't get first process.
        CloseHandle (hProcessSnap);
        return;
    }

    do {
        ProcessEntry entry;
        entry.name = pe.szExeFile;
        entry.pid = (long)pe.th32ProcessID;
        proclist.push_back(entry);
    } while (Process32Next (hProcessSnap, &pe));
    CloseHandle (hProcessSnap);

#elif defined(__FreeBSD__)
    kvm_t *kvd;
    struct kinfo_proc *ki;
    int nof_procs, i;

    if (!(kvd = kvm_openfiles(_PATH_DEVNULL, _PATH_DEVNULL, NULL, O_RDONLY, NULL)))
        return;

    if (!(ki = kvm_getprocs(kvd, KERN_PROC_PROC, 0, &nof_procs))) {
        kvm_close(kvd);
        return;
    }

    for (i=0; i<nof_procs; i++) {
        ProcessEntry entry;
        entry.pid = ki[i].ki_pid;
        entry.name = wxString(ki[i].ki_ocomm, wxConvUTF8);
        proclist.push_back(entry);
    }
    kvm_close(kvd);

#else
    //GTK and other
    wxArrayString output;
#if defined (__WXGTK__)
    ExecuteCommand(wxT("ps -A -o pid,command  --no-heading"), output);
#elif defined (__WXMAC__)
    // Mac does not like the --no-heading...
    ExecuteCommand(wxT("ps -A -o pid,command "), output);
#endif
    for (size_t i=0; i< output.GetCount(); i++) {
        wxString line = output.Item(i);
        //remove whitespaces
        line = line.Trim().Trim(false);

        //get the process ID
        ProcessEntry entry;
        wxString spid = line.BeforeFirst(wxT(' '));
        spid.ToLong( &entry.pid );
        entry.name = line.AfterFirst(wxT(' '));

        if (entry.pid == 0 && i > 0) {
            //probably this line belongs to the provious one
            ProcessEntry e = proclist.back();
            proclist.pop_back();
            e.name << entry.name;
            proclist.push_back( e );
        } else {
            proclist.push_back( entry );
        }
    }
#endif
}

void ProcUtils::GetChildren(long pid, std::vector<long> &proclist)
{
#ifdef __WXMSW__
    OSVERSIONINFO osver ;

    // Check to see if were running under Windows95 or
    // Windows NT.
    osver.dwOSVersionInfoSize = sizeof( osver ) ;
    if ( !GetVersionEx( &osver ) ) {
        return;
    }

    if ( osver.dwPlatformId != VER_PLATFORM_WIN32_NT ) {
        return;
    }

    //get child processes of this node
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!hProcessSnap) {
        return;
    }

    //Fill in the size of the structure before using it.
    PROCESSENTRY32 pe;
    memset(&pe, 0, sizeof(pe));
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Walk the snapshot of the processes, and for each process,
    // kill it if its parent is pid.
    if (!Process32First(hProcessSnap, &pe)) {
        // Can't get first process.
        CloseHandle (hProcessSnap);
        return;
    }

    //loop over all processes and collect all the processes their parent
    //pid matches PID
    do {
        if ((long)pe.th32ParentProcessID == pid) {
            proclist.push_back((long)pe.th32ProcessID);
        }
    } while (Process32Next (hProcessSnap, &pe));
    CloseHandle (hProcessSnap);

#elif defined(__FreeBSD__)
    kvm_t *kvd;
    struct kinfo_proc *ki;
    int nof_procs, i;

    if (!(kvd = kvm_openfiles(_PATH_DEVNULL, _PATH_DEVNULL, NULL, O_RDONLY, NULL)))
        return;

    if (!(ki = kvm_getprocs(kvd, KERN_PROC_PROC, pid, &nof_procs))) {
        kvm_close(kvd);
        return;
    }

    for (i=0; i<nof_procs; i++) {
        if (ki[i].ki_ppid == pid)
            proclist.push_back(ki[i].ki_pid);
    }

    kvm_close(kvd);

#else
    //GTK and other
    wxArrayString output;
#ifdef __WXGTK__
    ExecuteCommand(wxT("ps -A -o pid,ppid  --no-heading"), output);
#else
    ExecuteCommand(wxT("ps -A -o pid,ppid "), output);
#endif
    //parse the output and search for our process ID
    for (size_t i=0; i< output.GetCount(); i++) {
        long lpid(0);
        long lppid(0);
        wxString line = output.Item(i);

        //remove whitespaces
        line = line.Trim().Trim(false);

        //get the process ID
        wxString spid  = line.BeforeFirst(wxT(' '));
        spid.ToLong( &lpid );

        //get the process Parent ID
        wxString sppid = line.AfterFirst(wxT(' '));
        sppid.ToLong( &lppid );
        if (lppid == pid) {
            proclist.push_back(lpid);
        }
    }
#endif
}

bool ProcUtils::Shell(const wxString &programConsoleCommand)
{
    wxString cmd;
#ifdef __WXMSW__
    wxChar *shell = wxGetenv(wxT("COMSPEC"));
    if ( !shell ) {
        shell = (wxChar*) wxT("\\COMMAND.COM");
    }

    // just the shell
    cmd = shell;
#elif defined(__WXMAC__)
    wxString path = wxGetCwd();
    cmd = wxString( wxT("osascript -e 'tell application \"Terminal\"' -e 'activate' -e 'do script with command \"cd ") + path + wxT("\"' -e 'end tell'") );
#else //non-windows
    //try to locate the configured terminal
    wxString terminal;
    wxString where;
    wxArrayString tokens;
    wxArrayString configuredTerminal;
    /*if (Locate(wxT("gnome-terminal"), where)) {
    	terminal = where;
    } else if (Locate(wxT("konsole"), where)) {
    	wxString path = wxGetCwd();
    	terminal << where << wxT(" --workdir \"") << path << wxT("\"");
    } else if (Locate(wxT("terminal"), where)) {
    	terminal = where;
    } else if (Locate(wxT("lxterminal"), where)) {
    	terminal = where;
    } else if (Locate(wxT("xterm"), where)) {
    	terminal = where;
    }
    cmd = terminal;*/
    terminal = wxT ( "xterm" );
    if ( !programConsoleCommand.IsEmpty() ) {
        tokens = wxStringTokenize ( programConsoleCommand, wxT ( " " ), wxTOKEN_STRTOK );
        if ( !tokens.IsEmpty() ) {
            configuredTerminal = wxStringTokenize ( tokens.Item(0), wxT ( "/" ), wxTOKEN_STRTOK );
            if ( !configuredTerminal.IsEmpty() ) {
                terminal = configuredTerminal.Last();
                tokens.Clear();
                configuredTerminal.Clear();
            }
        }
    }
    if ( Locate ( terminal, where ) ) {
        if ( terminal == wxT("konsole") ) {
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

bool ProcUtils::Locate(const wxString &name, wxString &where)
{
    wxString command;
    wxArrayString output;
    command << wxT("which \"") << name << wxT("\"");
    ProcUtils::ExecuteCommand(command, output);

    if (output.IsEmpty() == false) {
        wxString interstingLine = output.Item(0);

        if (interstingLine.Trim().Trim(false).IsEmpty()) {
            return false;
        }

        if (!interstingLine.StartsWith(wxT("which: no "))) {
            where = output.Item(0);
            where = where.Trim().Trim(false);
            return true;
        }
    }
    return false;
}

void ProcUtils::SafeExecuteCommand(const wxString &command, wxArrayString &output)
{
#ifdef __WXMSW__
    wxString errMsg;
    WinProcess *proc = WinProcess::Execute(command, errMsg);
    if (!proc) {
        return;
    }

    // wait for the process to terminate
    wxString tmpbuf;
    wxString buff;

    while (proc->IsAlive()) {
        tmpbuf.Clear();
        proc->Read(tmpbuf);
        buff << tmpbuf;
        wxThread::Sleep(100);
    }
    tmpbuf.Clear();

    // Read any unread output
    proc->Read(tmpbuf);
    while( !tmpbuf.IsEmpty() ) {
        buff << tmpbuf;
        tmpbuf.Clear();
        proc->Read(tmpbuf);
    }

    // Convert buff into wxArrayString
    buff.Trim().Trim(false);
    wxString s;
    int where = buff.Find(wxT("\n"));
    while( where != wxNOT_FOUND ) {
        // use c_str() to make sure we create a unique copy
        s = buff.Mid(0, where).c_str();
        s.Trim().Trim(false);
        output.Add(s.c_str());
        buff.Remove(0, where+1);

        where = buff.Find(wxT("\n"));
    }

    if(buff.empty() == false) {
        s = buff.Trim().Trim(false);
        output.Add(s.c_str());
    }

    proc->Cleanup();
    delete proc;

#else
    ProcUtils::ExecuteCommand(command, output);
#endif
}
