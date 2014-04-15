#ifndef LLDBCONNECTOR_H
#define LLDBCONNECTOR_H

#include <wx/event.h>
#include "LLDBCommand.h"
#include "clSocketClient.h"
#include "cl_socket_server.h"
#include "LLDBEnums.h"
#include "LLDBBreakpoint.h"
#include "asyncprocess.h"
#include "LLDBEvent.h"

class LLDBConnector;
class LLDBNetworkListenerThread;
class LLDBTerminalCallback : public IProcessCallback
{
    LLDBConnector* m_connector;
    IProcess* m_process;

public:
    LLDBTerminalCallback(LLDBConnector* connector) : m_connector(connector), m_process(NULL) {}
    virtual void OnProcessOutput(const wxString &str);
    virtual void OnProcessTerminated();
    void SetProcess(IProcess* process) {
        this->m_process = process;
    }
};

class LLDBConnector : public wxEvtHandler
{
public:
    struct TerminalInfo {
        int terminalProcessID;
        wxString tty;

        TerminalInfo() : terminalProcessID(wxNOT_FOUND) {}
    };

protected:
    clSocketClient::Ptr_t       m_socket;
    LLDBNetworkListenerThread * m_thread;
    LLDBBreakpoint::Vec_t       m_breakpoints;
    LLDBBreakpoint::Vec_t       m_pendingDeletionBreakpoints;
    IProcess*                   m_process;
    bool                        m_isRunning;
    bool                        m_canInteract;
    LLDBCommand                 m_runCommand;
    TerminalInfo                m_terminalInfo;

    wxDECLARE_EVENT_TABLE();
    void OnProcessOutput(wxCommandEvent &event);
    void OnProcessTerminated(wxCommandEvent &event);

protected:
    bool IsBreakpointExists(LLDBBreakpoint::Ptr_t bp) const;
    LLDBBreakpoint::Vec_t::const_iterator FindBreakpoint(LLDBBreakpoint::Ptr_t bp) const;
    LLDBBreakpoint::Vec_t::iterator FindBreakpoint(LLDBBreakpoint::Ptr_t bp);
    LLDBBreakpoint::Vec_t GetUnappliedBreakpoints();

    void OnLLDBStarted(LLDBEvent &event);
    void OnLLDBExited(LLDBEvent &event);
    wxString GetDebugServerPath() const;

public:
    LLDBConnector();
    virtual ~LLDBConnector();

    TerminalInfo& GetTerminalInfo() {
        return m_terminalInfo;
    }

    bool IsCanInteract() const {
        return m_canInteract;
    }
    void SetCanInteract(bool canInteract) {
        this->m_canInteract = canInteract;
    }
    void SetIsRunning(bool isRunning) {
        this->m_isRunning = isRunning;
    }
    bool IsRunning() const {
        return m_isRunning;
    }
    const LLDBBreakpoint::Vec_t& GetAllBreakpoints() const;

    /**
     * @brief invalidate all breakpoints (remove their internal id)
     */
    void InvalidateBreakpoints();

    /**
     * @brief establish connection to codelite-lldb server
     * @param timeout number of seconds to wait until successfull connect
     * @return
     */
    bool ConnectToLocalDebugger(int timeout = 10);

    /**
     * @brief start codelite-lldb if not running
     */
    bool LaunchDebugServer();

    /**
     * @brief terminate the debug server
     */
    void StopDebugServer();
    /**
     * @brief apply the breakpoints
     * Note this does not assure that the breakpoints will actually be added
     * codelite-lldb will might need to interrupt the process in order to apply them
     * so a LLDB_STOPPED event will be sent with interrupt-reason set to
     */
    void ApplyBreakpoints();

    /**
     * @brief add list of breakpoints ( do not add apply them just yet)
     * @param breakpoints
     */
    void AddBreakpoints(const LLDBBreakpoint::Vec_t& breakpoints);

    /**
     * @brief add list of breakpoints ( do not add apply them just yet)
     * @param breakpoints
     */
    void AddBreakpoints(const BreakpointInfo::Vec_t& breakpoints);

    /**
     * @brief add a breakpoint to the list (do not apply them yet)
     */
    void AddBreakpoint(LLDBBreakpoint::Ptr_t breakpoint, bool notify = true);

    /**
     * @brief replace all the breakpoints with new list. This also clear the 'pending deletion' breakpoints
     */
    void UpdateAppliedBreakpoints(const LLDBBreakpoint::Vec_t& breakpoints);

    // aliases
    void AddBreakpoint(const wxString &location) {
        LLDBBreakpoint::Ptr_t bp( new LLDBBreakpoint(location) );
        AddBreakpoint( bp );
    }
    void AddBreakpoint(const wxFileName& filename, int line) {
        LLDBBreakpoint::Ptr_t bp( new LLDBBreakpoint(filename, line) );
        AddBreakpoint( bp );
    }

    /**
     * @brief delete a single breakpoint
     */
    void MarkBreakpointForDeletion(LLDBBreakpoint::Ptr_t bp);

    /**
     * @brief delete all breakpoints which are marked for deletion
     */
    void DeleteBreakpoints();

    /**
     * @brief delete all breakpoints
     */
    void DeleteAllBreakpoints();

    /**
     * @brief clear the breakpoint pending deletion queue
     */
    void ClearBreakpointDeletionQueue();

    /**
     * @brief send command to codelite-lldb
     */
    void SendCommand(const LLDBCommand& command);

    /**
     * @brief issue a 'Continue' command
     */
    void Continue();

    /**
     * @brief send request to the debugger to request the local varibles
     */
    void RequestLocals();

    /**
     * @brief request lldb to expand a variable and return its children
     * @param lldbId the unique identifier that identifies this variable
     * at the debug server side
     */
    void RequestVariableChildren(int lldbId);
    /**
     * @brief stop the debugger
     */
    void Stop();

    /**
     * @brief send a next command
     */
    void Next();

    /**
     * @brief send a step-out command (gdb's "finish")
     */
    void StepOut();

    /**
     * @brief send a step-into-function command (gdb's "step")
     */
    void StepIn();

    /**
     * @brief cleanup this object and make re-usable for next debug session
     */
    void Cleanup();

    /**
     * @brief Start the debugger
     * The debugger starts in 2 phases:
     * 1. A "Start" command is sent over to the debugger
     * 2. When the "Start" command is successfully executed on the codelite-lldb side, the caller should call 'Run()'. Run() will use the arguments passed to this command
     * @param runCommand
     */
    void Start(const LLDBCommand& runCommand);

    /**
     * @brief instruct the debugger to 'run'
     * using the arguments passed in the Start() command
     */
    void Run(const wxString &tty);

    /**
     * @brief
     * @param reason
     */
    void Interrupt(eInterruptReason reason);
};

#endif // LLDBCONNECTOR_H
