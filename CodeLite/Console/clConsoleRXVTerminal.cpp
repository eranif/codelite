#include "clConsoleRXVTerminal.h"

#include "Platform/Platform.hpp"
#include "StdToWX.h"

clConsoleRXVTTerminal::clConsoleRXVTTerminal()
{
    const wxArrayString commands = StdToWX::ToArrayString({"rxvt-unicode", "urxvt", "rxvt"});
    const auto executable = ThePlatform->AnyWhich(commands);

    SetTerminalCommand(executable.value_or("rxvt-unicode") + " -cd %WD% -e /bin/bash -c '%COMMAND%'");
    SetEmptyTerminalCommand(executable.value_or("rxvt-unicode") + " -cd %WD%");
}
