#include "clConsoleQTerminal.h"

clConsoleQTerminal::clConsoleQTerminal()
{
    SetTerminalCommand("/usr/bin/qterminal -w %WD% -e %COMMAND%");
    SetEmptyTerminalCommand("/usr/bin/qterminal -w %WD%");
}

clConsoleQTerminal::~clConsoleQTerminal() {}
