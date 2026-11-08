#pragma once

#include "clConsoleGnomeTerminal.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clConsoleRXVTTerminal : public clConsoleGnomeTerminal
{
public:
    clConsoleRXVTTerminal();
    ~clConsoleRXVTTerminal() override = default;
};
