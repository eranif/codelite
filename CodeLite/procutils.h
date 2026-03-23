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
#pragma once

#include "cl_command_event.h"
#include "codelite_exports.h"

#include <atomic>
#include <set>
#include <vector>
#include <wx/arrstr.h>
#include <wx/defs.h>
#include <wx/string.h>

#ifdef __WINDOWS__          // __WINDOWS__ defined by wx/defs.h
#include <wx/msw/wrapwin.h> // includes windows.h
// #include <devpropdef.h>
#include <Psapi.h>
#include <SetupAPI.h>
#include <tlhelp32.h>
#endif

struct ProcessEntry {
    wxString name;
    long pid;
};

using PidVec_t = std::vector<ProcessEntry>;

/// Event class used by ProcUtils::ShellExecAsync method
class WXDLLIMPEXP_CL clShellProcessEvent : public clCommandEvent
{
    wxString m_output;
    int m_pid = wxNOT_FOUND;
    int m_exitCode = wxNOT_FOUND;

public:
    clShellProcessEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clShellProcessEvent(const clShellProcessEvent&) = default;
    clShellProcessEvent& operator=(const clShellProcessEvent&) = delete;
    ~clShellProcessEvent() override = default;
    wxEvent* Clone() const override { return new clShellProcessEvent(*this); }

    void SetOutput(const wxString& output) { this->m_output = output; }
    const wxString& GetOutput() const { return m_output; }

    void SetPid(int pid) { this->m_pid = pid; }
    void SetExitCode(int exitCode) { this->m_exitCode = exitCode; }

    int GetPid() const { return m_pid; }
    int GetExitCode() const { return m_exitCode; }
};

using clShellProcessEventFunction = void (wxEvtHandler::*)(clShellProcessEvent&);
#define clShellProcessEventHandler(func) wxEVENT_HANDLER_CAST(clShellProcessEventFunction, func)

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_SHELL_ASYNC_PROCESS_TERMINATED, clShellProcessEvent);

class WXDLLIMPEXP_CL ProcUtils
{
public:
    ProcUtils() = default;
    ~ProcUtils() = default;

    /**
     * @brief wrap a given command in the shell command (e.g. cmd /c "command")
     */
    static wxString& WrapInShell(wxString& cmd);

    static std::set<unsigned long> GetProcTree(long pid);
    static void
    ExecuteCommand(const wxString& command, wxArrayString& output, long flags = wxEXEC_NODISABLE | wxEXEC_SYNC);
    static void ExecuteInteractiveCommand(const wxString& command);
    static wxString GetProcessNameByPid(long pid);
    static std::vector<ProcessEntry> GetProcessList();
    static bool Shell(const wxString& programConsoleCommand);
    static bool Locate(const wxString& name, wxString& where);

    /**
     * @brief the equivalent of 'ps ax|grep <name>'
     */
    static PidVec_t PS(const wxString& name);

    /**
     * Executes a command safely and collects its standard output.
     *
     * On Windows, this method launches the command using a process wrapper, polls for completion,
     * and optionally aborts early if the provided shutdown flag is set. Any captured output is split
     * into lines and stored in the output array. On non-Windows platforms, it delegates to the
     * platform-specific Popen implementation.
     *
     * @param command The command line to execute.
     * @param output wxArrayString reference that receives the captured output, split on newline
     *               characters.
     * @param shutdown_flag Shared atomic flag that can be set to request early termination; may be
     *                      null.
     *
     * @return The process exit code on successful completion, or wxNOT_FOUND if the process could not
     *         be started. If execution is interrupted by shutdown on Windows, returns -1.
     *
     * @throws No exceptions are thrown by this function.
     */
    static int SafeExecuteCommand(const wxString& command,
                                  wxArrayString& output,
                                  std::shared_ptr<std::atomic_bool> shutdown_flag = nullptr);

    /**
     * @brief Executes a shell command safely, optionally changing to a working directory first.
     *
     * This ProcUtils method builds a shell command string, prepends a "cd" into the specified
     * working directory when provided, wraps the result in the appropriate shell invocation, and
     * then executes it through the safer command execution path while collecting any output.
     *
     * @param command The shell command to execute.
     * @param working_directory The directory to switch to before running the command; if empty,
     * no directory change is performed.
     * @param output wxArrayString reference that receives the command output.
     * @param shutdown_flag Shared atomic flag used to request early termination of execution.
     *
     * @return int The result code returned by SafeExecuteCommand.
     */
    static int SafeExecuteShellCommand(const wxString& command,
                                       const wxString& working_directory,
                                       wxArrayString& output,
                                       std::shared_ptr<std::atomic_bool> shutdown_flag = nullptr);

    /**
     * @brief execute a command and return its output as plain string
     */
    static wxString SafeExecuteCommand(const wxString& command);

    /**
     * @brief execute command and execute the callback on each line until the callback returns true
     */
    static void GrepCommandOutputWithCallback(const std::vector<wxString>& cmd,
                                              std::function<bool(const wxString&)> callback);

    /**
     * @brief execute command and search the output for the first occurrence of `find_what`
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
