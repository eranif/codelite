#include "clConsoleRXVTerminal.h"

#include "Platform.hpp"

clConsoleRXVTTerminal::clConsoleRXVTTerminal()
{
    wxString executable = "rxvt-unicode";
    wxArrayString commands;
    commands.Add("rxvt-unicode");
    commands.Add("urxvt");
    commands.Add("rxvt");
    ThePlatform->AnyWhich(commands, &executable);

    SetTerminalCommand(executable + " -cd %WD% -e /bin/bash -c '%COMMAND%'");
    SetEmptyTerminalCommand(executable + " -cd %WD%");
}

clConsoleRXVTTerminal::~clConsoleRXVTTerminal() {}
