#include "clConsoleOSXTerminal.h"
#include "file_logger.h"
#include "fileutils.h"

clConsoleOSXTerminal::clConsoleOSXTerminal() { SetTerminalApp("Terminal"); }

clConsoleOSXTerminal::~clConsoleOSXTerminal() {}

bool clConsoleOSXTerminal::Start()
{
    wxString commandToExecute = PrepareCommand();
    int rc = system(commandToExecute.mb_str(wxConvUTF8).data());
    wxUnusedVar(rc);
    return true;
}

bool clConsoleOSXTerminal::StartForDebugger()
{
    SetRealPts("");
    SetTty("");
    SetPid(wxNOT_FOUND);
    FileUtils::OSXOpenDebuggerTerminalAndGetTTY(GetWorkingDirectory(), GetTerminalApp(), m_tty, m_pid);
    SetRealPts(m_tty);
    return true;
}
wxString clConsoleOSXTerminal::PrepareCommand()
{
    wxString commandToExecute;
    // osascript -e 'tell app "Terminal" to do script "echo hello"'
    wxFileName scriptPath = PrepareExecScript();

    if(IsTerminalNeeded()) {
        commandToExecute << "/usr/bin/open -n -a " << GetTerminalApp();
    }

    if(!GetCommand().IsEmpty()) {
        commandToExecute << " " << scriptPath.GetFullPath();
    }
    clDEBUG() << commandToExecute;
    return commandToExecute;
}
