#include "clConsoleLXTerminal.h"

clConsoleLXTerminal::clConsoleLXTerminal()
{
    SetTerminalCommand("/usr/bin/lxterminal --working-directory=%WD% -e %COMMAND%");
    SetEmptyTerminalCommand("/usr/bin/lxterminal --working-directory=%WD%");
}

clConsoleLXTerminal::~clConsoleLXTerminal() {}
