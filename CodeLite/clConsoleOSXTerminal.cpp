#include "clConsoleOSXTerminal.h"
#include "file_logger.h"
#include "fileutils.h"

clConsoleOSXTerminal::clConsoleOSXTerminal() {}

clConsoleOSXTerminal::~clConsoleOSXTerminal() {}

bool clConsoleOSXTerminal::Start()
{
    wxString commandToExecute = PrepareCommand();
    system(commandToExecute.mb_str(wxConvUTF8).data());
    return true;
}

bool clConsoleOSXTerminal::StartForDebugger()
{
    SetRealPts("");
    SetTty("");
    SetPid(wxNOT_FOUND);
    FileUtils::OSXOpenDebuggerTerminalAndGetTTY(GetWorkingDirectory(), "Terminal", m_tty, m_pid);
    SetRealPts(m_tty);
    return true;
}
wxString clConsoleOSXTerminal::PrepareCommand()
{
    wxString commandToExecute;
    // osascript -e 'tell app "Terminal" to do script "echo hello"'
    wxFileName scriptPath = PrepareExecScript();
    
    if(IsTerminalNeeded()) {
        commandToExecute << GetEnvironmentPrefix() << "/usr/bin/open -a Terminal ";
    }
    commandToExecute << scriptPath.GetFullPath();
    clDEBUG() << commandToExecute;
    return commandToExecute;
}
