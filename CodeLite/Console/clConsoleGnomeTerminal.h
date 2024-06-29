#ifndef CLCONSOLEGNOMETERMINAL_H
#define CLCONSOLEGNOMETERMINAL_H

#include "clConsoleBash.h"

class WXDLLIMPEXP_CL clConsoleGnomeTerminal : public clConsoleBash
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
    clConsoleGnomeTerminal();
    virtual ~clConsoleGnomeTerminal();

public:
    virtual bool Start();
    virtual bool StartForDebugger();
    virtual wxString PrepareCommand();
};

#endif // CLCONSOLEGNOMETERMINAL_H
