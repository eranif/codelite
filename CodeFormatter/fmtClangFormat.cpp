#include "fmtClangFormat.hpp"

#include "Platform.hpp"

fmtClangFormat::fmtClangFormat()
{
    SetName("clang-format");
    SetFileTypes({ FileExtManager::TypeSourceCpp, FileExtManager::TypeSourceC, FileExtManager::TypeHeader,
                   FileExtManager::TypeJava, FileExtManager::TypeJS });
    SetDescription(_("A tool to format C/C++/Java/JavaScript/JSON/Objective-C/Protobuf/C# code"));
    SetShortDescription(_("clang-format - a C/C++ formatter"));
    SetConfigFilepath("$(WorkspacePath)/.clang-format");

    // local command
    wxString clang_format_exe;
    ThePlatform->WhichWithVersion("clang-format", { 20, 19, 18, 17, 16, 15, 14, 13, 12 }, &clang_format_exe);
    SetCommand({ clang_format_exe, R"#("$(CurrentFileRelPath)")#" });
}

fmtClangFormat::~fmtClangFormat() {}
