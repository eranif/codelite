//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : asyncprocess.h
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

#ifndef I_PROCESS_H
#define I_PROCESS_H

#include "codelite_exports.h"
#include <map>
#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wx/string.h>

enum IProcessCreateFlags {
    IProcessCreateDefault = (1 << 0),           // Default: create process with no console window
    IProcessCreateConsole = (1 << 1),           // Create with console window shown
    IProcessCreateWithHiddenConsole = (1 << 2), // Create process with a hidden console
    IProcessCreateSync =
        (1 << 3), // Create a synchronous process (i.e. there is no background thread that performs the reads)
    IProcessCreateAsSuperuser = (1 << 4), // On platforms that support it, start the process as superuser
    IProcessNoRedirect = (1 << 5),
};

class WXDLLIMPEXP_CL IProcess;
class WXDLLIMPEXP_CL IProcessCallback : public wxEvtHandler
{
public:
    virtual void OnProcessOutput(const wxString& str) { wxUnusedVar(str); }
    virtual void OnProcessTerminated() {}
};

/**
 * @class IProcess
 * @author eran
 * @date 10/09/09
 * @file i_process.h
 * @brief
 */
class WXDLLIMPEXP_CL IProcess
{
protected:
    wxEvtHandler* m_parent;
    int m_pid;
    bool m_hardKill;
    IProcessCallback* m_callback;
    size_t m_flags; // The creation flags
    
public:
    typedef wxSharedPtr<IProcess> Ptr_t;

public:
    IProcess(wxEvtHandler* parent)
        : m_parent(parent)
        , m_pid(-1)
        , m_hardKill(false)
        , m_callback(NULL)
        , m_flags(0)
    {
    }
    virtual ~IProcess() {}

public:
    // Handle process exit code. This is done this way this
    // under Linux / Mac the exit code is returned only after the signal child has been
    // handled by codelite
    static void SetProcessExitCode(int pid, int exitCode);
    static bool GetProcessExitCode(int pid, int& exitCode);

    // Stop notifying the parent window about input/output from the process
    // this is useful when we wish to terminate the process onExit but we don't want
    // to know about its termination
    virtual void Detach() = 0;

    // Read from process stdout - return immediately if no data is available
    virtual bool Read(wxString& buff) = 0;

    // Write to the process stdin
    virtual bool Write(const wxString& buff) = 0;

    /**
     * @brief wait for process to terminate and return all its output to the caller
     * Note that this function is blocking
     */
    virtual void WaitForTerminate(wxString& output);

    /**
     * @brief this method is mostly needed on MSW where writing a password
     * is done directly on the console buffer rather than its stdin
     */
    virtual bool WriteToConsole(const wxString& buff) = 0;

    // Return true if the process is still alive
    virtual bool IsAlive() = 0;

    // Clean the process resources and kill the process if it is
    // still alive
    virtual void Cleanup() = 0;

    // Terminate the process. It is recommended to use this method
    // so it will invoke the 'Cleaup' procedure and the process
    // termination event will be sent out
    virtual void Terminate() = 0;

    void SetPid(int pid) { this->m_pid = pid; }

    int GetPid() const { return m_pid; }

    void SetHardKill(bool hardKill) { this->m_hardKill = hardKill; }
    bool GetHardKill() const { return m_hardKill; }
    IProcessCallback* GetCallback() { return m_callback; }
    
    /**
     * @brief do we have process redirect enabled?
     */
    bool IsRedirect() const { return !(m_flags & IProcessNoRedirect); }
};

// Help method
/**
 * @brief start process
 * @param parent the parent. all events will be sent to this object
 * @param cmd command line to execute
 * @param flags possible creation flag
 * @param workingDir set the working directory of the executed process
 * @return
 */
WXDLLIMPEXP_CL IProcess* CreateAsyncProcess(wxEvtHandler* parent, const wxString& cmd,
                                            size_t flags = IProcessCreateDefault,
                                            const wxString& workingDir = wxEmptyString);

/**
 * @brief create synchronus process
 * @param cmd command to execute
 * @param flags process creation flags
 * @param workingDir working directory for the new process
 * @return IPorcess handle on succcess
 */
WXDLLIMPEXP_CL IProcess* CreateSyncProcess(const wxString& cmd, size_t flags = IProcessCreateDefault,
                                           const wxString& workingDir = wxEmptyString);

/**
 * @brief start process
 * @brief cb callback object. Instead of events, OnProcessOutput and OnProcessTerminated will be called respectively
 * @param parent the parent. all events will be sent to this object
 * @param cmd command line to execute
 * @param flags possible creation flag
 * @param workingDir set the working directory of the executed process
 * @return
 */
WXDLLIMPEXP_CL IProcess* CreateAsyncProcessCB(wxEvtHandler* parent, IProcessCallback* cb, const wxString& cmd,
                                              size_t flags = IProcessCreateDefault,
                                              const wxString& workingDir = wxEmptyString);

#endif // I_PROCESS_H
