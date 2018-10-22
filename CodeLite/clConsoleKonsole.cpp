#include "clConsoleKonsole.h"

clConsoleKonsole::clConsoleKonsole()
{
    SetTerminalCommand("/usr/bin/konsole --separate --working %WD% -e '%COMMAND%'");
    SetEmptyTerminalCommand("/usr/bin/konsole --separate --working %WD%");
}

clConsoleKonsole::~clConsoleKonsole() {}
