//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : TerminalEmulator.h
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

#ifndef TERMINALEMULATOR_H
#define TERMINALEMULATOR_H

#include "asyncprocess.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include <list>
#include <wx/event.h>
#include <wx/process.h>
#include <wx/string.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_TERMINAL_COMMAND_EXIT, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_TERMINAL_COMMAND_OUTPUT, clCommandEvent);

class MyProcess;
class WXDLLIMPEXP_CL TerminalEmulator : public wxEvtHandler
{
    IProcess* m_process;
    long m_pid;
    friend class MyProcess;
    std::list<wxProcess*> m_myProcesses;

protected:
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);

public:
    TerminalEmulator();
    virtual ~TerminalEmulator();

    /**
     * @brief terminate the running process
     */
    void Terminate();

    /**
     * @brief is the process currently running?
     */
    bool IsRunning() const;

    /**
     * @brief Execute a command in the OS terminal.
     * @param command the command to execute
     * @param waitOnExit when set to true, pause the terminal with the message
     * @param command_args arguments to be appended to the command
     * @param workingDirectory working directory for the new process
     * "Hit any key to continue"
     * @param title set the terminal title. If an empty string is provided, use the command as the title
     */
    bool ExecuteConsole(const wxString& command, bool waitOnExit = false, const wxString& command_args = "",
                        const wxString& workingDirectory = "", const wxString& title = "");
    /**
     * @brief execute a command without attaching console
     */
    bool ExecuteNoConsole(const wxString& command, const wxString& workingDirectory);
};

#endif // TERMINALEMULATOR_H
