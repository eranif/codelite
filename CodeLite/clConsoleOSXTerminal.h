#ifndef CLCONSOLEOSXTERMINAL_H
#define CLCONSOLEOSXTERMINAL_H

#include "clConsoleBase.h" // Base class: clConsoleBase

class clConsoleOSXTerminal : public clConsoleBase
{
public:
    clConsoleOSXTerminal();
    virtual ~clConsoleOSXTerminal();

public:
    virtual bool Start();
    virtual bool StartForDebugger();
    virtual wxString PrepareCommand();
};

#endif // CLCONSOLEOSXTERMINAL_H
