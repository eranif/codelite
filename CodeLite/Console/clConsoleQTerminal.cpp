#include "clConsoleQTerminal.h"

clConsoleQTerminal::clConsoleQTerminal()
{
    SetTerminalCommand("qterminal -w %WD% -e %COMMAND%");
    SetEmptyTerminalCommand("qterminal -w %WD%");
}

clConsoleQTerminal::~clConsoleQTerminal() {}
