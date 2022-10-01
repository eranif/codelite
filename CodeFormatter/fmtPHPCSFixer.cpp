#include "fmtPHPCSFixer.hpp"

#include "Platform.hpp"

fmtPHPCSFixer::fmtPHPCSFixer()
{
    SetName("PHP-CS-Fixer");
    SetFileTypes({ FileExtManager::TypePhp });
    SetDescription(_("The PHP Coding Standards Fixer"));
    SetInlineFormatter(true);
    SetHasConfigFile(true);
    SetEnabled(false);
    SetConfigFilepath("$(WorkspacePath)/.php-cs-fixer.dist.php");

    wxString php_exe = "php";
    PLATFORM::Which("php", &php_exe);
    SetCommand({ php_exe, "tools/php-cs-fixer/vendor/bin/php-cs-fixer", "fix", "--quiet", "$(CurrentFileFullPath)" });

    // remote command
    wxString ssh_exe;
    if(GetSSHCommand(&ssh_exe)) {
        wxString remote_command = "\"";
        if(!GetWorkingDirectory().empty()) {
            remote_command << "cd " << GetWorkingDirectory() << " && ";
        }
        remote_command << "php tools/php-cs-fixer/vendor/bin/php-cs-fixer fix --quiet $(CurrentFileFullPath)\"";
        SetRemoteCommand({ ssh_exe, "$(SSH_User)@$(SSH_Host)", remote_command });
    }
}

fmtPHPCSFixer::~fmtPHPCSFixer() {}
