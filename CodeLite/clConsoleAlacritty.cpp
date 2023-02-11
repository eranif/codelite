#include "clConsoleAlacritty.hpp"

#include "Platform.hpp"

clConsoleAlacritty::clConsoleAlacritty()
{
#ifdef __WXMAC__
    // open /Applications/Alacritty.app
    ThePlatform->MacFindApp("Alacritty", &m_terminal);
#else
    ThePlatform->Which("alacritty", &m_terminal);
#endif
}

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

    wxString command_args;
    wxString base_command;

    if(IsTerminalNeeded()) {
#ifdef __WXMAC__
        // m_terminal is already wrapped
        base_command << m_terminal << " --args";
#else
        base_command = WrapWithQuotesIfNeeded(m_terminal);
#endif

        if(IsWaitWhenDone()) {
            command_args << " --hold";
        }

        if(!GetWorkingDirectory().empty()) {
            command_args << " --working-directory " << WrapWithQuotesIfNeeded(GetWorkingDirectory());
        }

        command_args << " -t " << WrapWithQuotesIfNeeded(command);

        // append the command to run
        command_args << " -e " << WrapWithQuotesIfNeeded(command);
        if(!GetCommandArgs().IsEmpty()) {
            command_args << " " << GetCommandArgs();
        }
    } else {
        base_command = WrapWithQuotesIfNeeded(command);
        if(!GetCommandArgs().IsEmpty()) {
            command_args << " " << GetCommandArgs();
        }
    }

    wxString cmd = base_command + command_args;
    clDEBUG() << "Running:" << cmd << endl;
    return cmd;
}
