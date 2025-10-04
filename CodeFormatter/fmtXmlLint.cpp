#include "fmtXmlLint.hpp"

#include "Platform/Platform.hpp"

fmtXmlLint::fmtXmlLint()
{
    SetName("xmllint");
    SetFileTypes({ FileExtManager::TypeXRC, FileExtManager::TypeXml });
    SetDescription(_("xmllint - command line XML tool"));
    SetShortDescription(_("Xml formatter"));
    const auto xml_lint_exe = ThePlatform->Which("xmllint");
    SetCommand({ xml_lint_exe.value_or("xmllint"), "--format", R"#("$(CurrentFileRelPath)")#" });
    SetEnabled(xml_lint_exe.has_value());
}
