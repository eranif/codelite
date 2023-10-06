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
    wxString full_command;
    wxString command = GetCommand();
    command.Trim().Trim(false);

    if(IsTerminalNeeded()) {
        if(m_terminal.empty()) {
            // no terminal, but a terminal is required...
            return wxEmptyString;
        }

        // on mac, add `--args` here
        full_command = m_terminal;
        MacAddArgsIfNeeded(&full_command);
        if(!GetWorkingDirectory().empty()) {
            full_command << " --working-directory " << WrapWithQuotesIfNeeded(GetWorkingDirectory());
        }

        if(IsWaitWhenDone()) {
            full_command << " --hold";
        }

        // set the title
        if(!command.empty()) {
            full_command << " -t " << WrapWithQuotesIfNeeded(command);
            full_command << " -e " << command;
        }
    } else {
        full_command << " " << command;
    }

    if(!GetCommandArgs().IsEmpty()) {
        full_command << " " << GetCommandArgs();
    }

    full_command.Trim().Trim(false);
    clDEBUG() << "Running:" << full_command << endl;
    return full_command;
}
