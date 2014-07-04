//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : asyncprocess.cpp
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


class wxEvtHandler;
class IProcess;
#include <wx/string.h>

#ifdef __WXMSW__
#   include "winprocess_impl.h"
#else
#   include "unixprocess_impl.h"
#endif

IProcess* CreateAsyncProcess(wxEvtHandler *parent, const wxString& cmd, size_t flags, const wxString &workingDir)
{
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(parent, cmd, errMsg, flags, workingDir);
#else
    return UnixProcessImpl::Execute(parent, cmd, flags, workingDir);
#endif
}

IProcess* CreateAsyncProcessCB(wxEvtHandler *parent, IProcessCallback* cb, const wxString& cmd, size_t flags, const wxString &workingDir)
{
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(parent, cmd, errMsg, flags, workingDir, cb);
#else
    return UnixProcessImpl::Execute(parent, cmd, flags, workingDir, cb);
#endif
}

IProcess* CreateSyncProcess(const wxString& cmd, size_t flags, const wxString& workingDir)
{
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(NULL, cmd, errMsg, flags|IProcessCreateSync, workingDir);
#else
    return UnixProcessImpl::Execute(NULL, cmd, flags|IProcessCreateSync, workingDir);
#endif
}

// Static methods:
bool IProcess::GetProcessExitCode(int pid, int &exitCode)
{
    wxUnusedVar(pid);
    wxUnusedVar(exitCode);

    exitCode = 0;
    return true;
}

void IProcess::SetProcessExitCode(int pid, int exitCode)
{
    wxUnusedVar(pid);
    wxUnusedVar(exitCode);
}

void IProcess::WaitForTerminate(wxString& output)
{
    wxString buff;
    while ( Read(buff) ) {
        output << buff;
    }
}
