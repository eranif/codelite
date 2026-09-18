#ifndef CLCONSOLEOSXTERMINAL_H
#define CLCONSOLEOSXTERMINAL_H

#include "clConsoleBash.h"

class WXDLLIMPEXP_CL clConsoleOSXTerminal : public clConsoleBash
{
    wxString m_terminalApp;

public:
    clConsoleOSXTerminal();
    ~clConsoleOSXTerminal() override = default;

    void SetTerminalApp(const wxString& terminalApp) { this->m_terminalApp = terminalApp; }
    const wxString& GetTerminalApp() const { return m_terminalApp; }

public:
    bool Start() override;
    bool StartForDebugger() override;
    wxString PrepareCommand() override;
};

#endif // CLCONSOLEOSXTERMINAL_H
