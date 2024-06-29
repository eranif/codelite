#include "clConsoleBash.h"
#include "cl_standard_paths.h"
#include "fileutils.h"
#include <wx/arrstr.h>
#include <wx/utils.h>

clConsoleBash::clConsoleBash() {}

clConsoleBash::~clConsoleBash() {}

wxFileName clConsoleBash::PrepareExecScript() const
{
// Create a script
#ifdef __WXOSX__
    wxFileName scriptPath("/tmp", "codelite-exec.sh");
#else
    wxFileName scriptPath(clStandardPaths::Get().GetUserDataDir(), "codelite-exec.sh");
    scriptPath.AppendDir("tmp");
    scriptPath.AppendDir(::wxGetUserId());
    scriptPath.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
#endif

    if(!GetCommand().IsEmpty()) {
        wxString fileContent;
        fileContent << "#!/bin/bash\n";
        fileContent << "command=\"" << GetCommand() << "\"\n";
        wxString cdCommand;
#ifdef __WXOSX__
        // on OSX, we need to set the working directory via the script
        if(!GetWorkingDirectory().IsEmpty()) {
            wxString workingDir = GetWorkingDirectory();
            if(workingDir.Contains(" ")) {
                workingDir.Prepend("\"").Append("\"");
            }
            cdCommand << "cd " << workingDir << " && ";
        }
#endif
        if(!GetCommandArgs().IsEmpty()) {
            // Split the arguments line
            wxArrayString arr = SplitArguments(GetCommandArgs());
            // Create an array in the script
            for(size_t i = 0; i < arr.size(); ++i) {
                fileContent << "args[" << i << "]=\"" << arr[i] << "\"\n";
            }
            fileContent << cdCommand << "\"${command}\" \"${args[@]}\"\n";
        } else {
            fileContent << cdCommand << "\"${command}\"\n";
        }
        if(IsWaitWhenDone()) {
            fileContent << "echo Hit any key to continue...\nread";
        }
        FileUtils::WriteFileContent(scriptPath, fileContent);
    }
#ifdef __WXOSX__
    // The script must have exec permissions
    system("chmod +x /tmp/codelite-exec.sh");
#endif
    return scriptPath;
}
