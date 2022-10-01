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
    wxString rustfmt_exe;
    PLATFORM::Which("rustfmt", &rustfmt_exe);
    SetCommand({ rustfmt_exe, "$(CurrentFileRelPath)" });

    // remote command
    wxString remote_command = "\"";
    if(!GetWorkingDirectory().empty()) {
        remote_command << "cd " << GetWorkingDirectory() << " && ";
    }
    remote_command << "rustfmt $(CurrentFileRelPath)\"";
    SetRemoteCommand({ GetSSHCommand(), "$(SSH_User)@$(SSH_Host)", remote_command });
}

fmtRustfmt::~fmtRustfmt() {}
