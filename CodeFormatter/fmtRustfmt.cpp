#include "fmtRustfmt.hpp"

#include "Platform.hpp"

fmtRustfmt::fmtRustfmt()
{
    SetName("rustfmt");
    SetFileTypes({ FileExtManager::TypeRust });
    SetDescription(_("Format Rust code"));
    SetShortDescription(_("Rust formatter"));
    SetInplaceFormatter(true);
    SetConfigFilepath("$(WorkspacePath)/.rustfmt.toml");

    // local command
    wxString rustfmt_exe = "rustfmt";
    bool enabeld = ThePlatform->Which("rustfmt", &rustfmt_exe);
    SetCommand({ rustfmt_exe, "--edition", "2021", R"#("$(CurrentFileRelPath)")#" });
    SetEnabled(enabeld);
}

fmtRustfmt::~fmtRustfmt() {}
