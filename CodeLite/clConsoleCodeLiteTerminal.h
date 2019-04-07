#ifndef CLCONSOLECODELITETERMINAL_H
#define CLCONSOLECODELITETERMINAL_H

#include "clConsoleGnomeTerminal.h"

class WXDLLIMPEXP_CL clConsoleCodeLiteTerminal : public clConsoleGnomeTerminal
{
public:
    clConsoleCodeLiteTerminal();
    virtual ~clConsoleCodeLiteTerminal();
    wxString PrepareCommand();
    bool StartForDebugger();
};

#endif // CLCONSOLECODELITETERMINAL_H
