#include "fmtXmlLint.hpp"

#include "Platform.hpp"

fmtXmlLint::fmtXmlLint()
{
    SetName("xmllint");
    SetFileTypes({ FileExtManager::TypeXRC, FileExtManager::TypeXml });
    SetDescription(_("xmllint - command line XML tool"));

    wxString xml_lint_exe;
    PLATFORM::Which("xmllint", &xml_lint_exe);
    SetCommand({ xml_lint_exe, "--format", "$(CurrentFileFullPath)" });

    // remote command
    wxString ssh_exe;
    if(GetSSHCommand(&ssh_exe)) {
        wxString remote_command = "\"";
        if(!GetWorkingDirectory().empty()) {
            remote_command << "cd " << GetWorkingDirectory() << " && ";
        }
        remote_command << "xmllint --format $(CurrentFileFullPath)\"";
        SetRemoteCommand({ ssh_exe, "$(SSH_User)@$(SSH_Host)", remote_command });
    }
}

fmtXmlLint::~fmtXmlLint() {}
