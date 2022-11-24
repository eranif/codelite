#include "fmtPHPCBF.hpp"

#include "Platform.hpp"

fmtPHPCBF::fmtPHPCBF()
{
    SetName("PHPCBF");
    SetFileTypes({ FileExtManager::TypePhp });
    SetDescription(_("PHP Code Beautifier and Fixer"));
    SetShortDescription(_("PHP formatter"));
    SetInplaceFormatter(true);
    SetEnabled(false);

    wxString php_exe = "php";
    ThePlatform->Which("php", &php_exe);
    SetCommand({ php_exe, "$(WorkspacePath)/vendor/bin/phpcbf", "-q", R"#("$(CurrentFileRelPath)")#" });
}

fmtPHPCBF::~fmtPHPCBF() {}
