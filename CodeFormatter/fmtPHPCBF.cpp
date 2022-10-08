#include "fmtPHPCBF.hpp"

#include "Platform.hpp"

fmtPHPCBF::fmtPHPCBF()
{
    SetName("PHPCBF");
    SetFileTypes({ FileExtManager::TypePhp });
    SetDescription(_("PHP Code Beautifier and Fixer"));
    SetInlineFormatter(true);
    SetEnabled(false);

    wxString php_exe = "php";
    PLATFORM::Which("php", &php_exe);
    SetCommand({ php_exe, "vendor/bin/phpcbf", "-q", "$(CurrentFileFullPath)" });
}

fmtPHPCBF::~fmtPHPCBF() {}
