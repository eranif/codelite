#ifndef CLCONSOLEBASE_H
#define CLCONSOLEBASE_H

#include "codelite_exports.h"
#include "macros.h"
#include <wx/arrstr.h>
#include <wx/sharedptr.h>
#include <wx/string.h>

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
    wxString m_tty;
    wxString m_realPts;
    long m_pid = wxNOT_FOUND;
    bool m_waitWhenDone = false;

    /**
     * @brief create an environment list to be used before we execute our terminal
     */
    wxString GetEnvironmentPrefix() const;

    wxString WrapWithQuotesIfNeeded(const wxString& s) const;
    wxString EscapeString(const wxString& str, const wxString& c = "\"") const;

public:
    clConsoleBase();
    ~clConsoleBase();

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
     * @brief return the best terminal for the OS. Pass an empty string to return the default temrinal for the OS
     */
    static clConsoleBase::Ptr_t GetTerminal(const wxString& terminalName = "");

    /**
     * @brief return list of known terminals
     */
    static wxArrayString GetAvailaleTerminals();

    // Setters/Getters
    void SetCommand(const wxString& command) { this->m_command = command; }
    void SetEnvironment(const wxStringMap_t& environment) { this->m_environment = environment; }
    void SetPid(long pid) { this->m_pid = pid; }
    void SetRealPts(const wxString& realPts) { this->m_realPts = realPts; }
    void SetTty(const wxString& tty) { this->m_tty = tty; }
    void SetWaitWhenDone(bool waitWhenDone) { this->m_waitWhenDone = waitWhenDone; }
    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetCommand() const { return m_command; }
    const wxStringMap_t& GetEnvironment() const { return m_environment; }
    long GetPid() const { return m_pid; }
    const wxString& GetRealPts() const { return m_realPts; }
    const wxString& GetTty() const { return m_tty; }
    bool IsWaitWhenDone() const { return m_waitWhenDone; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
};

#endif // CLCONSOLEBASE_H
