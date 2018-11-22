#ifndef CLCONSOLEOSXTERMINAL_H
#define CLCONSOLEOSXTERMINAL_H

#include "clConsoleBash.h"

class WXDLLIMPEXP_CL clConsoleOSXTerminal : public clConsoleBash
{
    wxString m_terminalApp;

public:
    clConsoleOSXTerminal();
    virtual ~clConsoleOSXTerminal();

    void SetTerminalApp(const wxString& terminalApp) { this->m_terminalApp = terminalApp; }
    const wxString& GetTerminalApp() const { return m_terminalApp; }

public:
    virtual bool Start();
    virtual bool StartForDebugger();
    virtual wxString PrepareCommand();
};

#endif // CLCONSOLEOSXTERMINAL_H
