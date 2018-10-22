#include "clConsoleLXTerminal.h"

clConsoleLXTerminal::clConsoleLXTerminal()
{
    SetTerminalCommand("/usr/bin/lxterminal");
    SetWorkingDirSwitchPattern("--working-directory=VALUE");
}

clConsoleLXTerminal::~clConsoleLXTerminal()
{
}

