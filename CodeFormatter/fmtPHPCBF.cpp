#include "fmtPHPCBF.hpp"

#include "Platform/Platform.hpp"

fmtPHPCBF::fmtPHPCBF()
{
    SetName("PHPCBF");
    SetFileTypes({ FileExtManager::TypePhp });
    SetDescription(_("PHP Code Beautifier and Fixer"));
    SetShortDescription(_("PHP formatter"));
    SetInplaceFormatter(true);
    SetEnabled(false);

    const auto php_exe = ThePlatform->Which("php");
    SetCommand({ php_exe.value_or("php"), "$(WorkspacePath)/vendor/bin/phpcbf", "-q", R"#("$(CurrentFileRelPath)")#" });
    SetEnabled(php_exe.has_value());
}

fmtPHPCBF::~fmtPHPCBF() {}
