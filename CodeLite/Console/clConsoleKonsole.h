#pragma once

#include "clConsoleGnomeTerminal.h"

class WXDLLIMPEXP_CL clConsoleKonsole : public clConsoleGnomeTerminal
{
public:
    clConsoleKonsole();
    ~clConsoleKonsole() override = default;
};

