#include "fmtPHPCBF.hpp"

#include "Platform.hpp"

fmtPHPCBF::fmtPHPCBF()
{
    SetName("PHPCBF");
    SetFileTypes({ FileExtManager::TypePhp });
    SetDescription(_("PHP Code Beautifier and Fixer"));
    SetInlineFormatter(true);
    SetEnabled(false);

    wxString php_exe = "php";
    PLATFORM::Which("php", &php_exe);
    SetCommand({ php_exe, "vendor/bin/phpcbf", "-q", "$(CurrentFileFullPath)" });

    // remote command
    wxString ssh_exe;
    if(GetSSHCommand(&ssh_exe)) {
        wxString remote_command = "\"";
        if(!GetWorkingDirectory().empty()) {
            remote_command << "cd " << GetWorkingDirectory() << " && ";
        }
        remote_command << "php vendor/bin/phpcbf -q $(CurrentFileFullPath)\"";
        SetRemoteCommand({ ssh_exe, "$(SSH_User)@$(SSH_Host)", remote_command });
    }
}

fmtPHPCBF::~fmtPHPCBF() {}
