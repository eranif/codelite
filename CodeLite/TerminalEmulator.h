#ifndef TERMINALEMULATOR_H
#define TERMINALEMULATOR_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/event.h>
#include "cl_command_event.h"
#include <wx/process.h>
#include "asyncprocess.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_TERMINAL_COMMAND_EXIT, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_TERMINAL_COMMAND_OUTPUT, clCommandEvent);

class MyProcess;
class WXDLLIMPEXP_CL TerminalEmulator : public wxEvtHandler
{
    IProcess* m_process;
    long m_pid;
    friend class MyProcess;
    
protected:
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    
public:
    TerminalEmulator();
    virtual ~TerminalEmulator();

    wxString PrepareCommand(const wxString& str, const wxString& title, bool waitOnExit);

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
     * @param workingDirectory working directory for the new process
     * @param waitOnExit when set to true, pause the terminal with the message
     * "Hit any key to continue"
     * @param title set the terminal title. If an empty string is provided, use the command as the title
     */
    bool ExecuteConsole(const wxString& command,
                        const wxString& workingDirectory,
                        bool waitOnExit,
                        const wxString& title = wxEmptyString);
    /**
     * @brief execute a command without attaching console
     */
    bool ExecuteNoConsole(const wxString& command, const wxString& workingDirectory);
};

#endif // TERMINALEMULATOR_H
