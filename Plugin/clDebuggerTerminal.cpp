//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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
    ::LaunchTerminalForDebugger(m_title, m_tty, m_pid);
    if ( IsValid() ) {
        CL_DEBUG("clDebuggerTerminalPOSIX successfully started. Process %d", (int)m_pid);
    }
}

bool clDebuggerTerminalPOSIX::IsValid() const
{
    return m_pid != wxNOT_FOUND && !m_tty.IsEmpty();
}

void clDebuggerTerminalPOSIX::Clear()
{
#ifndef __WXMSW__
    if ( m_pid != wxNOT_FOUND ) {
        // terminate the process
        ::wxKill(m_pid, wxSIGKILL);
    }
#endif
    m_pid = wxNOT_FOUND;
    m_tty.Clear();
    m_title.Clear();
}
