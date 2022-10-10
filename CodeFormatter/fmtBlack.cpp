#include "fmtBlack.hpp"

#include "Platform.hpp"

fmtBlack::fmtBlack()
{
    SetName("black (python)");
    SetFileTypes({ FileExtManager::TypePython });
    SetDescription(_("The uncompromising python code formatter"));
    SetConfigFilepath(wxEmptyString);
    SetInlineFormatter(true);

    // local command
    wxString black_exe;
    PLATFORM::WhichWithVersion("black", { 20, 19, 18, 17, 16, 15, 14, 13, 12 }, &black_exe);
    SetCommand({ black_exe, "--line-length", "80", "$(CurrentFileRelPath)" });
}

fmtBlack::~fmtBlack() {}
