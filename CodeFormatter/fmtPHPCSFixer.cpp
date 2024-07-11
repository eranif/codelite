#include "fmtPHPCSFixer.hpp"

#include "Platform/Platform.hpp"

fmtPHPCSFixer::fmtPHPCSFixer()
{
    SetName("PHP-CS-Fixer");
    SetFileTypes({ FileExtManager::TypePhp });
    SetDescription(_("The PHP Coding Standards Fixer"));
    SetShortDescription(_("PHP formatter"));
    SetInplaceFormatter(true);
    SetEnabled(false);
    SetConfigFilepath("$(WorkspacePath)/.php-cs-fixer.dist.php");

    wxString php_exe = "php";
    bool enabeld = ThePlatform->Which("php", &php_exe);
    SetCommand({ php_exe, "$(WorkspacePath)/tools/php-cs-fixer/vendor/bin/php-cs-fixer", "fix", "--quiet",
                 R"#("$(CurrentFileRelPath)")#" });
    SetEnabled(enabeld);
}

fmtPHPCSFixer::~fmtPHPCSFixer() {}
