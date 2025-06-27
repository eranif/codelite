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
    const auto cmake_format = ThePlatform->Which("cmake-format");
    SetCommand({ cmake_format.value_or("cmake-format"), R"#(--line-width=120 --tab-size=4 -i "$(CurrentFileFullPath)")#" });
    SetEnabled(cmake_format.has_value());
}

fmtCMakeForamt::~fmtCMakeForamt() {}
