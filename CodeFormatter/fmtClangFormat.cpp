#include "fmtClangFormat.hpp"

#include "Platform/Platform.hpp"

fmtClangFormat::fmtClangFormat()
{
    SetName("clang-format");
    SetFileTypes({ FileExtManager::TypeSourceCpp, FileExtManager::TypeSourceC, FileExtManager::TypeHeader,
                   FileExtManager::TypeJava, FileExtManager::TypeJS });
    SetDescription(_("A tool to format C/C++/Java/JavaScript/JSON/Objective-C/Protobuf/C# code"));
    SetShortDescription(_("clang-format - a C/C++ formatter"));
    SetConfigFilepath("$(WorkspacePath)/.clang-format");

    // local command
    const auto clang_format_exe = ThePlatform->WhichWithVersion("clang-format", { 20, 19, 18, 17, 16, 15, 14, 13, 12 });
    SetCommand({ clang_format_exe.value_or("clang-format"), R"#("$(CurrentFileRelPath)")#" });
    SetEnabled(clang_format_exe.has_value());
}
