//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clDebuggerTerminal.cpp
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

#include "clDebuggerTerminal.h"

#include "file_logger.h"
#include "globals.h"
#include "procutils.h"

#ifndef __WXMSW__
#include <sys/wait.h>
#endif

clDebuggerTerminalPOSIX::clDebuggerTerminalPOSIX()
    : m_pid(wxNOT_FOUND)
{
}

clDebuggerTerminalPOSIX::~clDebuggerTerminalPOSIX() {}

void clDebuggerTerminalPOSIX::Launch(const wxString& title)
{
    m_title = title;
    wxString symblink;
    ::LaunchTerminalForDebugger(m_title, symblink, m_tty, m_pid);
    wxUnusedVar(symblink);

    if(IsValid()) {
        CL_DEBUG("clDebuggerTerminalPOSIX successfully started. Process %d. TTY: %s", (int)m_pid, m_tty);
    }
}

bool clDebuggerTerminalPOSIX::IsValid() const
{
#ifdef __WXMAC__
    return !m_tty.IsEmpty();
#else
    return m_pid != wxNOT_FOUND && !m_tty.IsEmpty();
#endif
}

void clDebuggerTerminalPOSIX::Clear()
{
#ifndef __WXMSW__
    if(m_pid != wxNOT_FOUND) {
        // konsole and and its descendent qterminal hang on exit
        // That's because we made them call /bin/sleep, which becomes the process stored in m_pid
        // Killing 'sleep' makes other terminals self-close, but not these two
        // (At least for konsole, it displays "Warning: Program '/bin/sleep' crashed" instead)
        // I can't find any way to prevent this, so grab the real terminal pid so in this situation we can kill it too
        bool killParent(false); // There's no need to kill most terminals
        wxString command(wxString::Format("ps -o ppid= -p %i", (int)m_pid));
        wxString result = ProcUtils::SafeExecuteCommand(command);
        long parentID;
        if(result.Trim().ToLong(&parentID)) {
            wxString command(wxString::Format("ps -o command= -p %i", (int)parentID));
            wxString name = ProcUtils::SafeExecuteCommand(command);
            if(name.Contains("--separate") || name.Contains("qterminal")) {
                killParent = true; // as it _is_ konsole, which will have been launched with --separate as an option, or
                                   // qterminal
            }
        }

        // terminate the process
        ::wxKill(m_pid, wxSIGTERM);

        if(killParent) {
            ::wxKill(parentID, wxSIGTERM);
        }
    }
#endif
    m_pid = wxNOT_FOUND;
    m_tty.Clear();
    m_title.Clear();
}

wxString clDebuggerTerminalPOSIX::MakeExeTitle(const wxString& exePath, const wxString& args)
{
    return wxString(wxT("Debugging: ")) << exePath << wxT(" ") << args;
}

wxString clDebuggerTerminalPOSIX::MakeCoreTitle(const wxString& coreFile)
{
    return wxString(wxT("Debugging core: ")) << coreFile;
}

wxString clDebuggerTerminalPOSIX::MakePidTitle(const int pid)
{
    return wxString(wxT("Debugging console pid: ")) << pid;
}
