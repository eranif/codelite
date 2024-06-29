#include "clConsoleLXTerminal.h"

clConsoleLXTerminal::clConsoleLXTerminal()
{
    SetTerminalCommand("lxterminal --working-directory=%WD% -e %COMMAND%");
    SetEmptyTerminalCommand("lxterminal --working-directory=%WD%");
}

clConsoleLXTerminal::~clConsoleLXTerminal() {}
