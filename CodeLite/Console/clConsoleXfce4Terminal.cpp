#include "clConsoleXfce4Terminal.h"

clConsoleXfce4Terminal::clConsoleXfce4Terminal()
{
    SetTerminalCommand("xfce4-terminal --working-directory=%WD% --execute %COMMAND%");
    SetEmptyTerminalCommand("xfce4-terminal --working-directory=%WD%");
}

clConsoleXfce4Terminal::~clConsoleXfce4Terminal() {}
