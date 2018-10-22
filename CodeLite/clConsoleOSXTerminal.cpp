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
    wxString strPath = WrapWithQuotesIfNeeded(GetWorkingDirectory());

    // osascript -e 'tell app "Terminal" to do script "echo hello"'
    commandToExecute << GetEnvironmentPrefix() << "osascript -e 'tell app \"Terminal\" to do script \"cd " << strPath;
    if(!GetCommand().IsEmpty()) { commandToExecute << " && " << GetCommand(); }
    commandToExecute << "\"'";
    clDEBUG() << commandToExecute;
    return commandToExecute;
}
