#include "fmtBlack.hpp"

#include "Platform/Platform.hpp"

fmtBlack::fmtBlack()
{
    SetName("black");
    SetFileTypes({ FileExtManager::TypePython });
    SetDescription(_("The uncompromising python code formatter"));
    SetShortDescription(_("black - a python formatter"));
    SetConfigFilepath(wxEmptyString);
    SetInplaceFormatter(true);

    // local command
    const auto black_exe = ThePlatform->WhichWithVersion("black", { 20, 19, 18, 17, 16, 15, 14, 13, 12 });
    SetCommand({ black_exe.value_or("black"), "--line-length", "80", R"#("$(CurrentFileRelPath)")#" });
    SetEnabled(black_exe.has_value());
}

fmtBlack::~fmtBlack() {}
