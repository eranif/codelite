#include "fmtRustfmt.hpp"

#include "Platform.hpp"

fmtRustfmt::fmtRustfmt()
{
    SetName("rustfmt");
    SetFileTypes({ FileExtManager::TypeRust });
    SetDescription(_("Format Rust code"));
    SetInplaceFormatter(true);
    SetConfigFilepath("$(WorkspacePath)/.rustfmt.toml");

    // local command
    wxString rustfmt_exe = "rustfmt";
    PLATFORM::Which("rustfmt", &rustfmt_exe);
    SetCommand({ rustfmt_exe, "--edition", "2021", "$(CurrentFileRelPath)" });
}

fmtRustfmt::~fmtRustfmt() {}
