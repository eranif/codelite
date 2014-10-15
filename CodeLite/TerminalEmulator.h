#ifndef TERMINALEMULATOR_H
#define TERMINALEMULATOR_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/event.h>
#include "cl_command_event.h"
#include <wx/process.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_TERMINAL_COMMAND_EXIT, clCommandEvent);

class WXDLLIMPEXP_CL TerminalEmulator : public wxEvtHandler
{
    wxProcess* m_process;
    int m_pid;
protected:
    void OnProcessTerminated(wxProcessEvent &event);
    
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
    bool IsRunning() const ;
    
    /**
     * @brief Execute a command in the OS terminal.
     * @param command the command to execute
     * @param workingDirectory working directory for the new process
     * @param waitOnExit when set to true, pause the terminal with the message
     * "Hit any key to continue"
     * @param title set the terminal title. If an empty string is provided, use the command as the title
     */
    bool Execute(const wxString& command,
                 const wxString& workingDirectory,
                 bool waitOnExit,
                 const wxString& title = wxEmptyString);
};

#endif // TERMINALEMULATOR_H
