#pragma once

#include "clConsoleGnomeTerminal.h"
class WXDLLIMPEXP_CL clConsoleXfce4Terminal : public clConsoleGnomeTerminal
{
public:
    clConsoleXfce4Terminal();
    ~clConsoleXfce4Terminal() override = default;
};
