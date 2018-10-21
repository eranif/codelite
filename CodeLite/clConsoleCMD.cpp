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
    // Build the command to execute
    wxString commandToExecute;
    commandToExecute << "cmd";

    if(!GetCommand().IsEmpty()) {
        commandToExecute << " /C ";
        if(GetCommand().StartsWith("\"") && !GetCommand().EndsWith("\"")) {
            commandToExecute << "\"" << GetCommand() << "\"";
        } else {
            commandToExecute << GetCommand();
        }
    }
    SetPid(::wxExecute(commandToExecute));
    return (GetPid() > 0);
}

bool clConsoleCMD::StartForDebugger() { return false; }
