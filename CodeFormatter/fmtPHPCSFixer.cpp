#include "fmtPHPCSFixer.hpp"

#include "Platform.hpp"

fmtPHPCSFixer::fmtPHPCSFixer()
{
    SetName("PHP-CS-Fixer");
    SetFileTypes({ FileExtManager::TypePhp });
    SetDescription(_("The PHP Coding Standards Fixer"));
    SetInlineFormatter(true);
    SetHasConfigFile(true);
    SetEnabled(false);
    SetConfigFilepath("$(WorkspacePath)/.php-cs-fixer.dist.php");

    wxString php_exe = "php";
    PLATFORM::Which("php", &php_exe);
    SetCommand({ php_exe, "tools/php-cs-fixer/vendor/bin/php-cs-fixer", "fix", "--quiet", "$(CurrentFileFullPath)" });
}

fmtPHPCSFixer::~fmtPHPCSFixer() {}
