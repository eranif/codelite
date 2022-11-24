#include "fmtXmlLint.hpp"

#include "Platform.hpp"

fmtXmlLint::fmtXmlLint()
{
    SetName("xmllint");
    SetFileTypes({ FileExtManager::TypeXRC, FileExtManager::TypeXml });
    SetDescription(_("xmllint - command line XML tool"));
    SetShortDescription(_("Xml formatter"));
    wxString xml_lint_exe;
    ThePlatform->Which("xmllint", &xml_lint_exe);
    SetCommand({ xml_lint_exe, "--format", R"#("$(CurrentFileRelPath)")#" });
}

fmtXmlLint::~fmtXmlLint() {}
