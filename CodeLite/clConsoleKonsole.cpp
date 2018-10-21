#include "clConsoleKonsole.h"

clConsoleKonsole::clConsoleKonsole() 
{
    SetTerminalCommand("/usr/bin/konsole --separate");
    SetWorkingDirSwitchPattern("--workdir VALUE");
}

clConsoleKonsole::~clConsoleKonsole() {}

