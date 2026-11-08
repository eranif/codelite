#pragma once

#include "clConsoleGnomeTerminal.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clConsoleLXTerminal : public clConsoleGnomeTerminal
{
public:
    clConsoleLXTerminal();
    ~clConsoleLXTerminal() override = default;
};
