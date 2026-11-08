#pragma once

#include "clConsoleGnomeTerminal.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clConsoleQTerminal : public clConsoleGnomeTerminal
{
public:
    clConsoleQTerminal();
    ~clConsoleQTerminal() override = default;
};
