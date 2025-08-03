#ifndef CLCONSOLELXTERMINAL_H
#define CLCONSOLELXTERMINAL_H

#include "clConsoleGnomeTerminal.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clConsoleLXTerminal : public clConsoleGnomeTerminal
{
public:
    clConsoleLXTerminal();
    virtual ~clConsoleLXTerminal() = default;
};

#endif // CLCONSOLELXTERMINAL_H
