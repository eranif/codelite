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

    const auto php_exe = ThePlatform->Which("php");
    SetCommand({ php_exe.value_or("php"),
                 "$(WorkspacePath)/tools/php-cs-fixer/vendor/bin/php-cs-fixer",
                 "fix",
                 "--quiet",
                 R"#("$(CurrentFileRelPath)")#" });
    SetEnabled(php_exe.has_value());
}
