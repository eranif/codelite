#ifndef CLCONSOLEKONSOLE_H
#define CLCONSOLEKONSOLE_H

#include "clConsoleGnomeTerminal.h"

class WXDLLIMPEXP_CL clConsoleKonsole : public clConsoleGnomeTerminal
{
public:
    clConsoleKonsole();
    virtual ~clConsoleKonsole();
};

#endif // CLCONSOLEKONSOLE_H
