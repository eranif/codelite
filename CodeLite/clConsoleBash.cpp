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
#endif

    scriptPath.AppendDir("tmp");
    scriptPath.AppendDir(::wxGetUserId());
    scriptPath.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    if(!GetCommand().IsEmpty()) {
        wxString fileContent;
        fileContent << "#!/bin/bash\n";
        fileContent << "command=\"" << GetCommand() << "\"\n";
        if(!GetCommandArgs().IsEmpty()) {
            // first, we split the input string by double quotes
            wxArrayString tmparr = ::wxSplit(GetCommandArgs(), '"', '\\');
            // remove empty entries
            wxArrayString arr;
            for(size_t i = 0; i < tmparr.size(); ++i) {
                if(!tmparr[i].IsEmpty()) { arr.Add(tmparr[i].Trim(false).Trim(true)); }
            }

            // Create an array in the script
            for(size_t i = 0; i < arr.size(); ++i) {
                fileContent << "args[" << i << "]=\"" << arr[i] << "\"\n";
            }
            fileContent << "${command} \"${args[@]}\"\n";
        } else {
            fileContent << "${command}\n";
        }
        if(IsWaitWhenDone()) { fileContent << "echo Hit any key to continue...\nread"; }
        FileUtils::WriteFileContent(scriptPath, fileContent);
    }
#ifdef __WXOSX__
    // The script must have exec permissions
    system("chown +x /tmp/codelite-exec.sh");
#endif
    return scriptPath;
}
