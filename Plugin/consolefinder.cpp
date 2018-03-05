//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : consolefinder.cpp
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
#include "consolefinder.h"
#include "exelocator.h"
#include "macros.h"
#include "procutils.h"
#include "file_logger.h"

ConsoleFinder::ConsoleFinder()
    : m_nConsolePid(0)
    , m_consoleCommand(TERMINAL_CMD)
{
}

ConsoleFinder::~ConsoleFinder() { FreeConsole(); }

void ConsoleFinder::FreeConsole()
{
    if(m_nConsolePid) {
        wxKill(m_nConsolePid, wxSIGKILL, NULL, wxKILL_CHILDREN);
        m_nConsolePid = 0;
    }
}

int ConsoleFinder::RunConsole(const wxString& title)
{
// start the xterm and put the shell to sleep with -e sleep 80000
// fetch the xterm tty so we can issue to gdb a "tty /dev/pts/#"
// redirecting program stdin/stdout/stderr to the xterm console.

#ifndef __WXMSW__
    wxString cmd;

    cmd = GetConsoleCommand();
    cmd.Replace(wxT("$(TITLE)"), title);
    cmd.Replace(wxT("$(CMD)"), wxString::Format(wxT("sleep %lu"), 80000 + wxGetProcessId()));

    clDEBUG() << "Launching console:" << cmd;

    m_nConsolePid = wxExecute(cmd, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER);
    if(m_nConsolePid <= 0) {
        return -1;
    }

    // Issue the PS command to get the /dev/tty device name
    // First, wait for the xterm to settle down, else PS won't see the sleep task
    wxSleep(1);
    m_ConsoleTty = GetConsoleTty(m_nConsolePid);
    if(m_ConsoleTty.IsEmpty()) {
        FreeConsole();
        return -1;
    }
    return m_nConsolePid;

#else //__WXMSW__
    wxUnusedVar(title);
    return -1;
#endif
}

wxString ConsoleFinder::GetConsoleTty(int ConsolePid)
{
#ifndef __WXMSW__
    // execute the ps x -o command  and read PS output to get the /dev/tty field
    unsigned long ConsPid = ConsolePid;
    wxString psCmd;
    wxArrayString psOutput;
    wxArrayString psErrors;

    psCmd << wxT("ps x -o tty,pid,command");
    ProcUtils::ExecuteCommand(psCmd, psOutput);

    wxString ConsTtyStr;
    wxString ConsPidStr;
    ConsPidStr << ConsPid;
    // find task with our unique sleep time
    wxString uniqueSleepTimeStr;
    uniqueSleepTimeStr << wxT("sleep ") << wxString::Format(wxT("%lu"), 80000 + ::wxGetProcessId());
    // search the output of "ps pid" command
    int knt = psOutput.GetCount();
    for(int i = knt - 1; i > -1; --i) {
        psCmd = psOutput.Item(i);
        // find the pts/# or tty/# or whatever it's called
        // by seaching the output of "ps x -o tty,pid,command" command.
        // The output of ps looks like:
        // TT       PID   COMMAND
        // pts/0    13342 /bin/sh ./run.sh
        // pts/0    13343 /home/pecanpecan/devel/trunk/src/devel/codeblocks
        // pts/0    13361 /usr/bin/gdb -nx -fullname -quiet -args ./conio
        // pts/0    13362 xterm -font -*-*-*-*-*-*-20-*-*-*-*-*-*-* -T Program Console -e sleep 93343
        // pts/2    13363 sleep 93343
        // ?        13365 /home/pecan/proj/conio/conio
        // pts/1    13370 ps x -o tty,pid,command

        if(psCmd.Contains(uniqueSleepTimeStr)) do {  // check for correct "sleep" line
                if(psCmd.Contains(wxT("-T"))) break; // error;wrong sleep line.
                // found "sleep 93343" string, extract tty field
                ConsTtyStr = wxT("/dev/") + psCmd.BeforeFirst(' ');
                return ConsTtyStr;
            } while(0); // if do
    }                   // for
    return wxEmptyString;
#else
    wxUnusedVar(ConsolePid);
    return wxEmptyString;
#endif
}

bool ConsoleFinder::FindConsole(const wxString& title, wxString& consoleName)
{
    int pid = RunConsole(title);
    if(pid > 0) {
        consoleName = m_ConsoleTty;
        return true;
    }
    return false;
}

wxString ConsoleFinder::GetConsoleName()
{
    wxString cmd;
#ifdef __WXMSW__
    cmd = wxGetenv(wxT("COMSPEC"));
    if(cmd.IsEmpty()) {
        cmd = wxT("CMD.EXE");
    }
#else // non-windows
    // try to locate the default terminal
    wxString terminal;
    wxString where;
    if(ExeLocator::Locate(wxT("gnome-terminal"), where)) {
        terminal = wxT("gnome-terminal -e ");
    } else if(ExeLocator::Locate(wxT("konsole"), where)) {
        terminal = wxT("konsole");
    } else if(ExeLocator::Locate(wxT("terminal"), where)) {
        terminal = wxT("terminal -e");
    } else if(ExeLocator::Locate(wxT("lxterminal"), where)) {
        terminal = wxT("lxterminal -e");
    } else if(ExeLocator::Locate(wxT("xterm"), where)) {
        terminal = wxT("xterm -e ");
    }

    if(cmd.IsEmpty()) {
        cmd = wxT("xterm -e ");
    }

    cmd = terminal;
#endif
    return cmd;
}
