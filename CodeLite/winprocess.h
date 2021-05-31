//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : winprocess.h
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
#ifndef __winprocess__
#define __winprocess__

#ifdef __WXMSW__

#include "wx/msw/wrapwin.h" // includes windows.h
#include <wx/string.h>

class WinProcess
{
public:
    WinProcess();
    virtual ~WinProcess();

    // Create process asynchronously and return a process object
    static WinProcess* Execute(const wxString& cmd, wxString& errMsg, const wxString& workingDir = wxEmptyString);

    // Read from process stdout - return immediately if no data is available
    bool Read(wxString& buff);

    // Write to the process stdin
    bool Write(const wxString& buff);

    // Return true if the process is still alive
    bool IsAlive();

    // Clean the process resources and kill the process if it is
    // still alive
    void Cleanup();

private:
    // WINDOWS implementation
    // Creating process related handles
    HANDLE hChildStdinRd, hChildStdinWr, hChildStdinWrDup, hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup,
        hSaveStdin, hSaveStdout;

    // Child process id & information
    DWORD dwProcessId;
    PROCESS_INFORMATION piProcInfo;
};

#endif

#endif // __winprocess__
