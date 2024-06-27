#ifndef CLCONSOLEQTERMINAL_H
#define CLCONSOLEQTERMINAL_H

#include "clConsoleGnomeTerminal.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clConsoleQTerminal : public clConsoleGnomeTerminal
{
public:
    clConsoleQTerminal();
    virtual ~clConsoleQTerminal();
};

#endif // CLCONSOLEQTERMINAL_H
