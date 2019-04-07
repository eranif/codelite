#include "clConsoleCodeLiteTerminal.h"
#include "cl_standard_paths.h"

clConsoleCodeLiteTerminal::clConsoleCodeLiteTerminal()
{
    wxFileName codeliteTerminal(clStandardPaths::Get().GetBinFolder(), "codelite-terminal");
#ifdef __WXMSW__
    codeliteTerminal.SetExt("exe");
#endif
    wxString cmd = codeliteTerminal.GetFullPath();
    WrapWithQuotesIfNeeded(cmd);
    SetTerminalCommand(wxString() << cmd << " --working-directory=\"%WD%\" --command=\"%COMMAND%\"");
    SetEmptyTerminalCommand(wxString() << cmd << " --working-directory=\"%WD%\"");
}

clConsoleCodeLiteTerminal::~clConsoleCodeLiteTerminal() {}

wxString clConsoleCodeLiteTerminal::PrepareCommand()
{
    // Build the command to execute
    wxString commandToExecute;
    bool hasCommand = !GetCommand().IsEmpty();
    commandToExecute = hasCommand ? GetTerminalCommand() : GetEmptyTerminalCommand();

    // For testing purposes
    wxString command = WrapWithQuotesIfNeeded(GetCommand());
    if(!command.IsEmpty()) {
        // We prepend 'call' to the execution to make sure that the execution is always returning 0
        if(!GetCommandArgs().IsEmpty()) { command << " " << GetCommandArgs(); }
    }

    wxString wd = GetWorkingDirectory();
    if(wd.IsEmpty() || !wxFileName::DirExists(wd)) { wd = WrapWithQuotesIfNeeded(wxGetCwd()); }
    if(IsWaitWhenDone()) { commandToExecute << " --wait "; }
    commandToExecute.Replace("%COMMAND%", command);
    commandToExecute.Replace("%WD%", wd);
    return commandToExecute;
}

bool clConsoleCodeLiteTerminal::StartForDebugger()
{
#ifdef __WXMSW__
    return false;
#else
    return clConsoleGnomeTerminal::StartForDebugger();
#endif
}
