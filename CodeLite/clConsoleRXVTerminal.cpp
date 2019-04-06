#include "clConsoleRXVTerminal.h"

clConsoleRXVTTerminal::clConsoleRXVTTerminal()
{
    SetTerminalCommand("/usr/bin/rxvt-unicode -cd %WD% -e /bin/bash -c '%COMMAND%'");
    SetEmptyTerminalCommand("/usr/bin/rxvt-unicode -cd %WD%");
}

clConsoleRXVTTerminal::~clConsoleRXVTTerminal() {}
