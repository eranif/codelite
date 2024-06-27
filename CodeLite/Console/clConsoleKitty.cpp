#include "clConsoleKitty.hpp"

#include "Platform.hpp"

clConsoleKitty::clConsoleKitty()
{
#ifdef __WXMAC__
    // open /Applications/Kitty.app
    ThePlatform->MacFindApp("Kitty", &m_terminal);
#else
    ThePlatform->Which("kitty", &m_terminal);
#endif
}

clConsoleKitty::~clConsoleKitty() {}

bool clConsoleKitty::Start() { return StartProcess(PrepareCommand()); }

bool clConsoleKitty::StartForDebugger() { return false; }

wxString clConsoleKitty::PrepareCommand()
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
            full_command << " -d " << WrapWithQuotesIfNeeded(GetWorkingDirectory());
        }

        if(IsWaitWhenDone()) {
            full_command << " --hold";
        }

        // set the title
        if(!command.empty()) {
            full_command << " -T " << WrapWithQuotesIfNeeded(command);
        }
    }

    full_command << " " << command;
    if(!GetCommandArgs().IsEmpty()) {
        full_command << " " << GetCommandArgs();
    }

    full_command.Trim().Trim(false);
    clDEBUG() << "Running:" << full_command << endl;
    return full_command;
}
