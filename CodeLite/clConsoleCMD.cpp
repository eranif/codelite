#include "clConsoleCMD.h"
#include "dirsaver.h"
#include <wx/filename.h>
#include <wx/utils.h>

clConsoleCMD::clConsoleCMD()
{
}

clConsoleCMD::~clConsoleCMD()
{
}

bool clConsoleCMD::Start()
{
    DirSaver ds; // restore the directory upon exiting this function
    wxFileName fn(GetWorkingDirectory(), "");
    if(fn.DirExists()) {
        ::wxSetWorkingDirectory(fn.GetPath());
    }
    return StartProcess(PrepareCommand());
}

bool clConsoleCMD::StartForDebugger()
{
    return false;
}

wxString clConsoleCMD::PrepareCommand()
{
    // no args? just return the cmd command
    wxString command = WrapWithQuotesIfNeeded(GetCommand());
    if(command.empty()) {
        return "cmd";
    }

    // Build the command to execute
    wxString commandToExecute;

    wxString rest_cmd;
    // if "wait-when-done" enabled, wrap the command with codelite-exec
    if(IsWaitWhenDone()) {
        wxFileName fnCodeliteExec(clStandardPaths::Get().GetBinaryFullPath("codelite-exec"));
        wxString strCodeLiteExec = WrapWithQuotesIfNeeded(fnCodeliteExec.GetFullPath());
        rest_cmd << strCodeLiteExec << " ";
    }

    // append the command to run
    rest_cmd << command;
    if(!GetCommandArgs().IsEmpty()) {
        rest_cmd << " " << GetCommandArgs();
    }
    return commandToExecute + rest_cmd;
}
