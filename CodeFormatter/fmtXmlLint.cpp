#include "fmtXmlLint.hpp"

#include "Platform.hpp"

fmtXmlLint::fmtXmlLint()
{
    SetName("xmllint");
    SetFileTypes({ FileExtManager::TypeXRC, FileExtManager::TypeXml });
    SetDescription(_("xmllint - command line XML tool"));

    wxString xml_lint_exe;
    Platform::Which("xmllint", &xml_lint_exe);
    SetCommand({ xml_lint_exe, "--format", "$(CurrentFileFullPath)" });

    // remote command
    wxString remote_command = "\"";
    if(!GetWorkingDirectory().empty()) {
        remote_command << "cd " << GetWorkingDirectory() << " && ";
    }
    remote_command << "xmllint --format $(CurrentFileFullPath)\"";
    SetRemoteCommand({ "ssh", "$(SSH_User)@$(SSH_Host)", remote_command });
}

fmtXmlLint::~fmtXmlLint() {}
