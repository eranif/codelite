#include "clConsoleRXVTerminal.h"

#include "Platform/Platform.hpp"

clConsoleRXVTTerminal::clConsoleRXVTTerminal()
{
    const wxArrayString commands = {"rxvt-unicode", "urxvt", "rxvt"};
    const auto executable = ThePlatform->AnyWhich(commands);

    SetTerminalCommand(executable.value_or("rxvt-unicode") + " -cd %WD% -e /bin/bash -c '%COMMAND%'");
    SetEmptyTerminalCommand(executable.value_or("rxvt-unicode") + " -cd %WD%");
}
