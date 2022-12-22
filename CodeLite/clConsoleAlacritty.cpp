#include "clConsoleAlacritty.hpp"

#include "Platform.hpp"

clConsoleAlacritty::clConsoleAlacritty() { ThePlatform->Which("alacritty", &m_terminal); }

clConsoleAlacritty::~clConsoleAlacritty() {}

bool clConsoleAlacritty::StartForDebugger() { return false; }
bool clConsoleAlacritty::Start() { return StartProcess(PrepareCommand()); }
wxString clConsoleAlacritty::PrepareCommand()
{
    // no args? just return the cmd command
    const wxString& command = GetCommand();
    if(command.empty() || m_terminal.empty()) {
        return m_terminal;
    }
    wxString commandToExecute;
    if(IsTerminalNeeded()) {
        // Build the command to execute
        commandToExecute = WrapWithQuotesIfNeeded(m_terminal);

        // if "wait-when-done" enabled, wrap the command with codelite-exec
        if(IsWaitWhenDone()) {
            commandToExecute << " --hold";
        }

        if(!GetWorkingDirectory().empty()) {
            commandToExecute << " --working-directory " << WrapWithQuotesIfNeeded(GetWorkingDirectory());
        }

        commandToExecute << " --title " << WrapWithQuotesIfNeeded(command);

        // append the command to run
        commandToExecute << " --command " << WrapWithQuotesIfNeeded(command);
        if(!GetCommandArgs().IsEmpty()) {
            commandToExecute << " " << GetCommandArgs();
        }
    } else {
        commandToExecute = WrapWithQuotesIfNeeded(command);
        if(!GetCommandArgs().IsEmpty()) {
            commandToExecute << " " << GetCommandArgs();
        }
    }
    return commandToExecute;
}
