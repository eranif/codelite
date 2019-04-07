#include "clConsoleCodeLiteTerminal.h"
#include "cl_standard_paths.h"
#include "fileutils.h"

clConsoleCodeLiteTerminal::clConsoleCodeLiteTerminal()
{
    wxFileName codeliteTerminal(clStandardPaths::Get().GetBinFolder(), "codelite-terminal");
#ifdef __WXMSW__
    codeliteTerminal.SetExt("exe");
#endif
    wxString cmd = codeliteTerminal.GetFullPath();
    WrapWithQuotesIfNeeded(cmd);
    SetTerminalCommand(wxString() << cmd << " --working-directory=%WD% --file=%COMMANDFILE%");
    SetEmptyTerminalCommand(wxString() << cmd << " --working-directory=%WD%");
}

clConsoleCodeLiteTerminal::~clConsoleCodeLiteTerminal() {}

wxString clConsoleCodeLiteTerminal::PrepareCommand()
{
    // Build the command to execute
    wxString commandToExecute;
    if(IsTerminalNeeded()) {
        wxFileName tmpfile(clStandardPaths::Get().GetTempDir(), "codelite-terminal.txt");
        bool hasCommand = !GetCommand().IsEmpty();
        commandToExecute = hasCommand ? GetTerminalCommand() : GetEmptyTerminalCommand();

        // For testing purposes
        wxString command = WrapWithQuotesIfNeeded(GetCommand());
        if(!command.IsEmpty()) {
            command.Prepend("start /B /WAIT "); // start the application in the foreground
            if(!GetCommandArgs().IsEmpty()) {
                wxString cmdArgs = GetCommandArgs();
                command << " " << GetCommandArgs();
            }

            // Write the content of the command into a file
            FileUtils::WriteFileContent(tmpfile, command);
        }

        wxString wd = GetWorkingDirectory();
        if(wd.IsEmpty() || !wxFileName::DirExists(wd)) { wd = WrapWithQuotesIfNeeded(wxGetCwd()); }
        if(IsWaitWhenDone()) { commandToExecute << " --wait "; }

        wxString commandFile = tmpfile.GetFullPath();
        commandFile = WrapWithQuotesIfNeeded(commandFile);

        commandToExecute.Replace("%COMMANDFILE%", commandFile);
        commandToExecute.Replace("%WD%", wd);

    } else {
        commandToExecute = WrapWithQuotesIfNeeded(GetCommand());
        if(!GetCommandArgs().IsEmpty()) { commandToExecute << " " << GetCommandArgs(); }
    }
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
