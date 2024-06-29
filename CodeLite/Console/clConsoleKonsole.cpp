#include "clConsoleKonsole.h"

clConsoleKonsole::clConsoleKonsole()
{
    SetTerminalCommand("konsole --separate --workdir %WD% -e '%COMMAND%'");
    SetEmptyTerminalCommand("konsole --separate --workdir %WD%");
}

clConsoleKonsole::~clConsoleKonsole() {}
