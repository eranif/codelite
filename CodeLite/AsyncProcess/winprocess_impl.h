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

#include <thread>
#include <unordered_set>
#include <wx/msw/wrapwin.h> // includes windows.h
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
    bool DoReadFromPipe(HANDLE pipe, wxString& buff, std::string& raw_buff);

public:
    WinProcessImpl(wxEvtHandler* parent);
    ~WinProcessImpl() override;

    // Create process asynchronously and return a process object
    static IProcess* Execute(wxEvtHandler* parent, const wxString& cmd, size_t flags = IProcessCreateDefault,
                             const wxString& workingDir = wxEmptyString, IProcessCallback* cb = NULL);

    // Create process asynchronously and return a process object
    static IProcess* Execute(wxEvtHandler* parent, const wxArrayString& args, size_t flags = IProcessCreateDefault,
                             const wxString& workingDir = wxEmptyString, IProcessCallback* cb = NULL);

    // Create process asynchronously and return a process object
    static IProcess* ExecuteConPTY(wxEvtHandler* parent, const std::vector<wxString>& args,
                                   size_t flags = IProcessCreateWithHiddenConsole,
                                   const wxString& workingDir = wxEmptyString);

    // Create process asynchronously and return a process object
    static IProcess* ExecuteConPTY(wxEvtHandler* parent, const wxString& cmd,
                                   size_t flags = IProcessCreateWithHiddenConsole,
                                   const wxString& workingDir = wxEmptyString);

    /**
     * @brief read data from stdout and error
     * @param buff check the buffer when true is returned
     * @return return true on success or timeout, flase otherwise, incase of false the reader thread will terminate
     */
    bool Read(wxString& buff, wxString& buffErr, std::string& raw_buff, std::string& raw_buff_err) override;

    // Write to the process stdin
    bool Write(const wxString& buff) override;
    bool Write(const std::string& buff) override;
    bool WriteRaw(const wxString& buff) override;
    bool WriteRaw(const std::string& buff) override;
    bool WriteToConsole(const wxString& buff) override;

    // Return true if the process is still alive
    bool IsAlive() override;

    // Clean the process resources and kill the process if it is
    // still alive
    void Cleanup() override;
    void Terminate() override;
    void Detach() override;
    void Signal(wxSignal sig) override;

private:
    // Creating process related handles
    HANDLE hChildStdinRd = INVALID_HANDLE_VALUE, hChildStdinWr = INVALID_HANDLE_VALUE,
           hChildStdinWrDup = INVALID_HANDLE_VALUE, hChildStdoutRd = INVALID_HANDLE_VALUE,
           hChildStdoutWr = INVALID_HANDLE_VALUE, hChildStdoutRdDup = INVALID_HANDLE_VALUE,
           hChildStderrRd = INVALID_HANDLE_VALUE, hChildStderrWr = INVALID_HANDLE_VALUE,
           hChildStderrRdDup = INVALID_HANDLE_VALUE, hSaveStdin = INVALID_HANDLE_VALUE,
           hSaveStdout = INVALID_HANDLE_VALUE, hSaveStderr = INVALID_HANDLE_VALUE;
    
    VOID* m_hPseudoConsole = nullptr;
    // Child process id & information
    DWORD dwProcessId = wxNOT_FOUND;
    PROCESS_INFORMATION piProcInfo = {};
};

#endif

#endif // __winprocess__
