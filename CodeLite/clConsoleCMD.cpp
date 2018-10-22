#include "clConsoleCMD.h"
#include "dirsaver.h"
#include <wx/filename.h>
#include <wx/utils.h>

clConsoleCMD::clConsoleCMD() {}

clConsoleCMD::~clConsoleCMD() {}

bool clConsoleCMD::Start()
{
    DirSaver ds; // restore the directory upon exiting this function
    wxFileName fn(GetWorkingDirectory(), "");
    if(fn.DirExists()) { ::wxSetWorkingDirectory(fn.GetPath()); }
    return StartProcess(PrepareCommand());
}

bool clConsoleCMD::StartForDebugger() { return false; }

wxString clConsoleCMD::PrepareCommand()
{
    // Build the command to execute
    wxString commandToExecute;
    commandToExecute << "cmd";
    
    wxString command = WrapWithQuotesIfNeeded(GetCommand());
    if(!command.IsEmpty()) {
        commandToExecute << " /C ";
        // We prepend 'call' to the execution to make sure that the execution is always returning 0
        if(IsWaitWhenDone()) { commandToExecute << " call "; }
        commandToExecute << command;
        if(!GetCommandArgs().IsEmpty()) {
            commandToExecute << " " << GetCommandArgs();
        }
        if(IsWaitWhenDone()) { commandToExecute << " && pause"; }
    }
    return commandToExecute;
}
