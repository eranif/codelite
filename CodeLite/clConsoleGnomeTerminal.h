#ifndef CLCONSOLEGNOMETERMINAL_H
#define CLCONSOLEGNOMETERMINAL_H

#include "clConsoleBash.h"

class WXDLLIMPEXP_CL clConsoleGnomeTerminal : public clConsoleBash
{
    wxString m_terminalCommand = "/usr/bin/gnome-terminal";
    wxString m_workingDirSwitchPattern = "--working-directory=VALUE";

protected:
    void SetTerminalCommand(const wxString& terminalCommand) { this->m_terminalCommand = terminalCommand; }
    const wxString& GetTerminalCommand() const { return m_terminalCommand; }
    void SetWorkingDirSwitchPattern(const wxString& workingDirSwitchPattern)
    {
        this->m_workingDirSwitchPattern = workingDirSwitchPattern;
    }
    const wxString& GetWorkingDirSwitchPattern() const { return m_workingDirSwitchPattern; }

public:
    clConsoleGnomeTerminal();
    virtual ~clConsoleGnomeTerminal();

public:
    virtual bool Start();
    virtual bool StartForDebugger();
    virtual wxString PrepareCommand();
};

#endif // CLCONSOLEGNOMETERMINAL_H
