#include "fmtBlack.hpp"

#include "Platform.hpp"

fmtBlack::fmtBlack()
{
    SetName("black");
    SetFileTypes({ FileExtManager::TypePython });
    SetDescription(_("The uncompromising python code formatter"));
    SetShortDescription(_("black - a python formatter"));
    SetConfigFilepath(wxEmptyString);
    SetInplaceFormatter(true);

    // local command
    wxString black_exe;
    ThePlatform->WhichWithVersion("black", { 20, 19, 18, 17, 16, 15, 14, 13, 12 }, &black_exe);
    SetCommand({ black_exe, "--line-length", "80", R"#("$(CurrentFileRelPath)")#" });
}

fmtBlack::~fmtBlack() {}
