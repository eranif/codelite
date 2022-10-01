#include "fmtRustfmt.hpp"

#include "Platform.hpp"

fmtRustfmt::fmtRustfmt()
{
    SetName("rustfmt");
    SetFileTypes({ FileExtManager::TypeRust });
    SetDescription(_("Format Rust code"));
    SetHasConfigFile(true);
    SetInlineFormatter(true);
    SetConfigFilepath("$(WorkspacePath)/.rustfmt.toml");

    // local command
    wxString rustfmt_exe = "rustfmt";
    PLATFORM::Which("rustfmt", &rustfmt_exe);
    SetCommand({ rustfmt_exe, "$(CurrentFileRelPath)" });

    // remote command
    wxString ssh_exe;
    if(GetSSHCommand(&ssh_exe)) {
        wxString remote_command = "\"";
        if(!GetWorkingDirectory().empty()) {
            remote_command << "cd " << GetWorkingDirectory() << " && ";
        }
        remote_command << "rustfmt $(CurrentFileRelPath)\"";
        SetRemoteCommand({ ssh_exe, "$(SSH_User)@$(SSH_Host)", remote_command });
    }
}

fmtRustfmt::~fmtRustfmt() {}
