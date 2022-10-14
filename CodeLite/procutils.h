//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : procutils.h
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
#ifndef PROCUTILS_H
#define PROCUTILS_H

#include "codelite_exports.h"
#include "wx/defs.h"

#include <map>
#include <vector>
#include <wx/arrstr.h>
#include <wx/process.h>
#include <wx/string.h>
#ifdef __WINDOWS__          // __WINDOWS__ defined by wx/defs.h
#include "wx/msw/wrapwin.h" // includes windows.h
// #include <devpropdef.h>
#include <Psapi.h>
#include <SetupAPI.h>
#include <tlhelp32.h>
#endif

#include "cl_command_event.h"
#include "codelite_events.h"

struct ProcessEntry {
    wxString name;
    long pid;
};

typedef std::vector<ProcessEntry> PidVec_t;

/// Event class used by ProcUtils::ShellExecAsync method
class WXDLLIMPEXP_CL clShellProcessEvent : public clCommandEvent
{
    wxString m_output;
    int m_pid = wxNOT_FOUND;
    int m_exitCode = wxNOT_FOUND;

public:
    clShellProcessEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clShellProcessEvent(const clShellProcessEvent& event);
    clShellProcessEvent& operator=(const clShellProcessEvent& src);
    virtual ~clShellProcessEvent();
    virtual wxEvent* Clone() const { return new clShellProcessEvent(*this); }

    void SetOutput(const wxString& output) { this->m_output = output; }
    const wxString& GetOutput() const { return m_output; }

    void SetPid(int pid) { this->m_pid = pid; }
    void SetExitCode(int exitCode) { this->m_exitCode = exitCode; }

    int GetPid() const { return m_pid; }
    int GetExitCode() const { return m_exitCode; }
};

typedef void (wxEvtHandler::*clShellProcessEventFunction)(clShellProcessEvent&);
#define clShellProcessEventHandler(func) wxEVENT_HANDLER_CAST(clShellProcessEventFunction, func)

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SHELL_ASYNC_PROCESS_TERMINATED, clShellProcessEvent);

class WXDLLIMPEXP_CL ProcUtils
{
public:
    ProcUtils();
    ~ProcUtils();

    static wxString& WrapInShell(wxString& cmd);

    static void GetProcTree(std::map<unsigned long, bool>& parentsMap, long pid);
    static void ExecuteCommand(const wxString& command, wxArrayString& output,
                               long flags = wxEXEC_NODISABLE | wxEXEC_SYNC);
    static void ExecuteInteractiveCommand(const wxString& command);
    static wxString GetProcessNameByPid(long pid);
    static void GetProcessList(std::vector<ProcessEntry>& proclist);
    static void GetChildren(long pid, std::vector<long>& children);
    static bool Shell(const wxString& programConsoleCommand);
    static bool Locate(const wxString& name, wxString& where);

    /**
     * @brief the equivalent of 'ps ax|grep <name>'
     */
    static PidVec_t PS(const wxString& name);

    /**
     * \brief a safe function that executes 'command' and returns its output. This function
     * is safed to be called from secondary thread (hence, SafeExecuteCommand)
     * \param command
     * \param output
     */
    static void SafeExecuteCommand(const wxString& command, wxArrayString& output);

    /**
     * @brief execute a command and return its output as plain string
     */
    static wxString SafeExecuteCommand(const wxString& command);

    /**
     * @brief execute command and search the output for the first occurance of `find_what`
     */
    static wxString GrepCommandOutput(const std::vector<wxString>& cmd, const wxString& find_what);

    /**
     * @brief executes a command under the proper shell async
     * @param command the command the execute, e.g. `ls -l`
     * @param pid storage pointer for the executed process
     * @param sink handler class for the process events
     * @returns true on success, otherwise (i.e. failed to launch the process) return false
     */
    static bool ShellExecAsync(const wxString& command, long* pid, wxEvtHandler* sink);

    /**
     * @brief executes a command under the proper shell sync
     * @param command the command the execute, e.g. `ls -l`
     * @param std_out [output]
     * @param std_err [output]
     * @return process exit code
     */
    static int ShellExecSync(const wxString& command, wxString* output);
};

#endif // PROCUTILS_H
