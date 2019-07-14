#include "clConsoleRXVTerminal.h"

clConsoleRXVTTerminal::clConsoleRXVTTerminal()
{
    SetTerminalCommand("rxvt-unicode -cd %WD% -e /bin/bash -c '%COMMAND%'");
    SetEmptyTerminalCommand("rxvt-unicode -cd %WD%");
}

clConsoleRXVTTerminal::~clConsoleRXVTTerminal() {}
