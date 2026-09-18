#ifndef CLCONSOLEQTERMINAL_H
#define CLCONSOLEQTERMINAL_H

#include "clConsoleGnomeTerminal.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clConsoleQTerminal : public clConsoleGnomeTerminal
{
public:
    clConsoleQTerminal();
    ~clConsoleQTerminal() override = default;
};

#endif // CLCONSOLEQTERMINAL_H
