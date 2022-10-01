#include "fmtJQ.hpp"

#include "Platform.hpp"

fmtJQ::fmtJQ()
{
    SetName("jq");

    // handle all known JSON files
    SetFileTypes({ FileExtManager::TypeJSON, FileExtManager::TypeWorkspaceFileSystem,
                   FileExtManager::TypeWorkspaceDocker, FileExtManager::TypeWxCrafter,
                   FileExtManager::TypeWorkspaceNodeJS, FileExtManager::TypeWorkspacePHP });

    SetDescription(_("commandline JSON processor"));

    wxString jq_exe = "jq";
    PLATFORM::Which("jq", &jq_exe);

    SetCommand({ jq_exe, ".", "-S", "$(CurrentFileFullPath)" });

    // remote command
    wxString remote_command = "\"";
    if(!GetWorkingDirectory().empty()) {
        remote_command << "cd " << GetWorkingDirectory() << " && ";
    }
    remote_command << "jq . -S $(CurrentFileFullPath)\"";
    SetRemoteCommand({ GetSSHCommand(), "$(SSH_User)@$(SSH_Host)", remote_command });
}

fmtJQ::~fmtJQ() {}
