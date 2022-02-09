#ifndef CLCONSOLEBASE_H
#define CLCONSOLEBASE_H

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "macros.h"

#include <wx/arrstr.h>
#include <wx/process.h>
#include <wx/sharedptr.h>
#include <wx/string.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_TERMINAL_EXIT, clProcessEvent);

class WXDLLIMPEXP_CL clConsoleEnvironment
{
    wxStringMap_t m_environment;
    wxStringMap_t m_oldEnvironment;

public:
    clConsoleEnvironment(const wxStringMap_t& env);
    clConsoleEnvironment();
    ~clConsoleEnvironment();

    void Add(const wxString& name, const wxString& value);
    void Apply();
    void UnApply();
};

class WXDLLIMPEXP_CL clConsoleBase
{
public:
    typedef wxSharedPtr<clConsoleBase> Ptr_t;

protected:
    wxStringMap_t m_environment;
    wxString m_workingDirectory;
    wxString m_command;
    wxString m_commandArgs;
    wxString m_tty;
    wxString m_realPts;
    long m_pid = wxNOT_FOUND;
    bool m_waitWhenDone = false;
    bool m_autoTerminate = false;
    int m_execExtraFlags = 0;
    bool m_terminalNeeded = true;
    wxProcess* m_callback = nullptr;
    wxEvtHandler* m_sink = nullptr;
    wxString m_callbackUID;

protected:
    /**
     * @brief create an environment list to be used before we execute our terminal
     */
    wxString GetEnvironmentPrefix() const;

    wxString WrapWithQuotesIfNeeded(const wxString& s) const;
    wxString EscapeString(const wxString& str, const wxString& c = "\"") const;
    virtual bool StartProcess(const wxString& command);

public:
    clConsoleBase();
    virtual ~clConsoleBase();

    /**
     * @brief when sink is provided, the terminal will send event when its done
     * The event sent is wxEVT_TERMINAL_EXIT of type clProcessEvent
     * @param sink
     * @param if provided the uid is sent back along with the termination event
     */
    void SetSink(wxEvtHandler* sink, const wxString& uid = "")
    {
        this->m_sink = sink;
        this->m_callbackUID = uid;
    }

    /**
     * @brief split command line arguments taking double quotes and escaping into account
     */
    static wxArrayString SplitArguments(const wxString& args);

    /**
     * @brief add an environment variable to be applied before we start the terminal
     */
    void AddEnvVariable(const wxString& name, const wxString& value);

    /**
     * @brief start terminal with a given command and an optional working directory
     */
    virtual bool Start() = 0;

    /**
     * @brief start terminal for debugger (i.e. launch an empty terminal so we can obtain its tty and return it
     */
    virtual bool StartForDebugger() = 0;

    /**
     * @brief prepare the execution command
     */
    virtual wxString PrepareCommand() = 0;

    /**
     * @brief return the best terminal for the OS. Pass an empty string to return the default temrinal for the OS
     */
    static clConsoleBase::Ptr_t GetTerminal();

    /**
     * @brief return the default name for the OS
     */
    static wxString GetSelectedTerminalName();

    /**
     * @brief return list of known terminals
     */
    static wxArrayString GetAvailaleTerminals();

    // Setters/Getters
    void SetCommand(const wxString& command, const wxString& args)
    {
        this->m_command = command;
        this->m_commandArgs = args;
    }
    void SetEnvironment(const wxStringMap_t& environment) { this->m_environment = environment; }
    void SetEnvironment(const clEnvList_t& environment);
    void SetPid(long pid) { this->m_pid = pid; }
    void SetRealPts(const wxString& realPts) { this->m_realPts = realPts; }
    void SetTty(const wxString& tty) { this->m_tty = tty; }
    void SetWaitWhenDone(bool waitWhenDone) { this->m_waitWhenDone = waitWhenDone; }
    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetCommand() const { return m_command; }
    const wxString& GetCommandArgs() const { return m_commandArgs; }
    const wxStringMap_t& GetEnvironment() const { return m_environment; }
    long GetPid() const { return m_pid; }
    const wxString& GetRealPts() const { return m_realPts; }
    const wxString& GetTty() const { return m_tty; }
    bool IsWaitWhenDone() const { return m_waitWhenDone; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
    void SetExecExtraFlags(int execExtraFlags) { this->m_execExtraFlags = execExtraFlags; }
    int GetExecExtraFlags() const { return m_execExtraFlags; }
    void SetAutoTerminate(bool autoTerminate) { this->m_autoTerminate = autoTerminate; }
    void SetTerminalNeeded(bool terminalNeeded) { this->m_terminalNeeded = terminalNeeded; }
    bool IsAutoTerminate() const { return m_autoTerminate; }
    bool IsTerminalNeeded() const { return m_terminalNeeded; }
    void SetCallback(wxProcess* callback)
    {
        wxDELETE(m_callback);
        this->m_callback = callback;
    }
};

#endif // CLCONSOLEBASE_H
