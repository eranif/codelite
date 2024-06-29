#include "clConsoleMateTerminal.h"

clConsoleMateTerminal::clConsoleMateTerminal()
{
    SetTerminalCommand("mate-terminal --working-directory=%WD% -e '%COMMAND%'");
    SetEmptyTerminalCommand("mate-terminal --working-directory=%WD%");
}

clConsoleMateTerminal::~clConsoleMateTerminal()
{
}
