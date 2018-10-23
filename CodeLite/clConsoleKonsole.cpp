#include "clConsoleKonsole.h"

clConsoleKonsole::clConsoleKonsole()
{
    SetTerminalCommand("/usr/bin/konsole --separate --workdir %WD% -e '%COMMAND%'");
    SetEmptyTerminalCommand("/usr/bin/konsole --separate --workdir %WD%");
}

clConsoleKonsole::~clConsoleKonsole() {}
