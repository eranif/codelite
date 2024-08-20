#include "clConsoleRXVTerminal.h"

#include "Platform/Platform.hpp"
#include "StdToWX.h"

clConsoleRXVTTerminal::clConsoleRXVTTerminal()
{
    wxString executable = "rxvt-unicode";
    const wxArrayString commands = StdToWX::ToArrayString({"rxvt-unicode", "urxvt", "rxvt"});
    ThePlatform->AnyWhich(commands, &executable);

    SetTerminalCommand(executable + " -cd %WD% -e /bin/bash -c '%COMMAND%'");
    SetEmptyTerminalCommand(executable + " -cd %WD%");
}

clConsoleRXVTTerminal::~clConsoleRXVTTerminal() {}
