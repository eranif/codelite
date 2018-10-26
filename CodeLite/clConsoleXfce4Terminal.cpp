#include "clConsoleXfce4Terminal.h"

clConsoleXfce4Terminal::clConsoleXfce4Terminal()
{
    SetTerminalCommand("/usr/bin/xfce4-terminal --working-directory=%WD% --execute %COMMAND%");
    SetEmptyTerminalCommand("/usr/bin/xfce4-terminal --working-directory=%WD%");
}

clConsoleXfce4Terminal::~clConsoleXfce4Terminal() {}
