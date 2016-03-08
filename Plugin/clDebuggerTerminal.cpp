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
#include "globals.h"
#include "file_logger.h"
#include "procutils.h"

#ifndef __WXMSW__
#   include <sys/wait.h>
#endif

clDebuggerTerminalPOSIX::clDebuggerTerminalPOSIX()
    : m_pid(wxNOT_FOUND)
{
}

clDebuggerTerminalPOSIX::~clDebuggerTerminalPOSIX()
{
}

void clDebuggerTerminalPOSIX::Launch(const wxString &title)
{
    m_title = title;
    wxString symblink;
    ::LaunchTerminalForDebugger(m_title, symblink, m_tty, m_pid);
    wxUnusedVar(symblink);
    
    if ( IsValid() ) {
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
    if ( m_pid != wxNOT_FOUND ) {
        // konsole hangs on exit with the message  "Warning: Program '/bin/sleep' crashed."
        // I can't find any way to prevent this, so grab the terminal's pid so we can kill it too if it's konsole
        bool killParent(false); // There's no need to kill non-konsole terminals
        wxString command(wxString::Format("ps -o ppid= -p %i", (int)m_pid));
        wxString result = ProcUtils::SafeExecuteCommand(command);
        long parentID;
        if (result.Trim().ToLong(&parentID)) {
            wxString command(wxString::Format("ps -o command= -p %i", (int)parentID));
            wxString name = ProcUtils::SafeExecuteCommand(command);
            if (name.Contains("--separate")) { 
                killParent = true; //It _is_ konsole, which will have been launched with --separate as an option
            }
        }
        
        // terminate the process
        ::wxKill(m_pid, wxSIGTERM);
        
        if (killParent) {
            ::wxKill(parentID, wxSIGTERM);
        }
    }
#endif
    m_pid = wxNOT_FOUND;
    m_tty.Clear();
    m_title.Clear();
}
