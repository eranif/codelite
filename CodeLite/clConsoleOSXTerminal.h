#ifndef CLCONSOLEOSXTERMINAL_H
#define CLCONSOLEOSXTERMINAL_H

#include "clConsoleBash.h"

class WXDLLIMPEXP_CL clConsoleOSXTerminal : public clConsoleBash
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
