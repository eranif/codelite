#include "fmtCMakeForamt.hpp"

#include "Platform/Platform.hpp"

fmtCMakeForamt::fmtCMakeForamt()
{
    SetName("cmake-format");
    SetFileTypes({ FileExtManager::TypeCMake });
    SetDescription(_("Parse cmake listfiles and format them nicely"));
    SetShortDescription(_("cmake-format - a CMake formatter"));
    SetConfigFilepath("$(WorkspacePath)/.cmake-format");
    SetInplaceFormatter(true);

    // local command
    wxString cmake_format = "cmake-format";
    bool enabeld = ThePlatform->Which("cmake-format", &cmake_format);
    SetCommand({ cmake_format, R"#(--line-width=120 --tab-size=4 -i "$(CurrentFileFullPath)")#" });
    SetEnabled(enabeld);
}

fmtCMakeForamt::~fmtCMakeForamt() {}
