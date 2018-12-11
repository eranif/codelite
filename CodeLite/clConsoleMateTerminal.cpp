#include "clConsoleMateTerminal.h"

clConsoleMateTerminal::clConsoleMateTerminal()
{
    SetTerminalCommand("/usr/bin/mate-terminal --working-directory=%WD% -e '%COMMAND%'");
    SetEmptyTerminalCommand("/usr/bin/mate-terminal --working-directory=%WD%");
}

clConsoleMateTerminal::~clConsoleMateTerminal()
{
}
