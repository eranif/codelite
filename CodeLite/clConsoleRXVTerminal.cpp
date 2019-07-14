#include "clConsoleRXVTerminal.h"

clConsoleRXVTTerminal::clConsoleRXVTTerminal()
{
    SetTerminalCommand("rxvt-unicode -cd %WD% -e /bin/sh -c '%COMMAND%'");
    SetEmptyTerminalCommand("rxvt-unicode -cd %WD%");
}

clConsoleRXVTTerminal::~clConsoleRXVTTerminal() {}
