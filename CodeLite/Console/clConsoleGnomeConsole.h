#ifndef CLCONSOLEGNOMECONSOLE_H
#define CLCONSOLEGNOMECONSOLE_H

#include "clConsoleBash.h"

class WXDLLIMPEXP_CL clConsoleGnomeConsole : public clConsoleBash
{
    wxString m_terminalCommand;
    wxString m_emptyTerminalCommand;

protected:
    void SetTerminalCommand(const wxString& terminalCommand) { this->m_terminalCommand = terminalCommand; }
    const wxString& GetTerminalCommand() const { return m_terminalCommand; }

    void SetEmptyTerminalCommand(const wxString& emptyTerminalCommand)
    {
        this->m_emptyTerminalCommand = emptyTerminalCommand;
    }
    const wxString& GetEmptyTerminalCommand() const { return m_emptyTerminalCommand; }
    bool FindProcessByCommand(const wxString& name, wxString& tty, long& pid);

public:
    clConsoleGnomeConsole();
    virtual ~clConsoleGnomeConsole() = default;

public:
    virtual bool Start() override;
    virtual bool StartForDebugger() override;
    virtual wxString PrepareCommand() override;
};

#endif // CLCONSOLEGNOMECONSOLE_H
