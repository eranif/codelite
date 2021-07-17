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

#include "asyncprocess.h"
#include "codelite_exports.h"
#include "wx/msw/wrapwin.h" // includes windows.h
#include <thread>
#include <unordered_set>
#include <wx/string.h>

class ProcessReaderThread;
class WinWriterThread;

#define BUFFER_SIZE 16 * 1024

class WXDLLIMPEXP_CL WinProcessImpl : public IProcess
{
    char m_buffer[BUFFER_SIZE];
    WinWriterThread* m_writerThread = nullptr;
    std::unordered_set<long> m_initialChildren;

protected:
    void StartReaderThread();
    bool DoReadFromPipe(HANDLE pipe, wxString& buff);

public:
    WinProcessImpl(wxEvtHandler* parent);
    virtual ~WinProcessImpl();

    // Create process asynchronously and return a process object
    static IProcess* Execute(wxEvtHandler* parent, const wxString& cmd, size_t flags = IProcessCreateDefault,
                             const wxString& workingDir = wxEmptyString, IProcessCallback* cb = NULL);

    // Create process asynchronously and return a process object
    static IProcess* Execute(wxEvtHandler* parent, const wxArrayString& args, size_t flags = IProcessCreateDefault,
                             const wxString& workingDir = wxEmptyString, IProcessCallback* cb = NULL);

    /**
     * @brief read data from stdout and error
     * @param buff check the buffer when true is returned
     * @return return true on success or timeout, flase otherwise, incase of false the reader thread will terminate
     */
    virtual bool Read(wxString& buff, wxString& buffErr);

    // Write to the process stdin
    virtual bool Write(const wxString& buff);
    virtual bool Write(const std::string& buff);
    virtual bool WriteRaw(const wxString& buff);
    virtual bool WriteRaw(const std::string& buff);
    virtual bool WriteToConsole(const wxString& buff);

    // Return true if the process is still alive
    virtual bool IsAlive();

    // Clean the process resources and kill the process if it is
    // still alive
    virtual void Cleanup();
    virtual void Terminate();
    virtual void Detach();
    virtual void Signal(wxSignal sig);

private:
    // Creating process related handles
    HANDLE hChildStdinRd, hChildStdinWr, hChildStdinWrDup, hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup,
        hChildStderrRd, hChildStderrWr, hChildStderrRdDup, hSaveStdin, hSaveStdout, hSaveStderr;

    // Child process id & information
    DWORD dwProcessId;
    PROCESS_INFORMATION piProcInfo;
};

#endif

#endif // __winprocess__
