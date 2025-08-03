#include "fmtRustfmt.hpp"

#include "Platform/Platform.hpp"

fmtRustfmt::fmtRustfmt()
{
    SetName("rustfmt");
    SetFileTypes({ FileExtManager::TypeRust });
    SetDescription(_("Format Rust code"));
    SetShortDescription(_("Rust formatter"));
    SetInplaceFormatter(true);
    SetConfigFilepath("$(WorkspacePath)/.rustfmt.toml");

    // local command
    const auto rustfmt_exe = ThePlatform->Which("rustfmt");
    SetCommand({ rustfmt_exe.value_or("rustfmt"), "--edition", "2021", R"#("$(CurrentFileRelPath)")#" });
    SetEnabled(rustfmt_exe.has_value());
}
