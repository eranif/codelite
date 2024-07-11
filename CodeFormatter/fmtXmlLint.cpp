#include "fmtXmlLint.hpp"

#include "Platform/Platform.hpp"

fmtXmlLint::fmtXmlLint()
{
    SetName("xmllint");
    SetFileTypes({ FileExtManager::TypeXRC, FileExtManager::TypeXml });
    SetDescription(_("xmllint - command line XML tool"));
    SetShortDescription(_("Xml formatter"));
    wxString xml_lint_exe = "xmllint";
    bool enabeld = ThePlatform->Which("xmllint", &xml_lint_exe);
    SetCommand({ xml_lint_exe, "--format", R"#("$(CurrentFileRelPath)")#" });
    SetEnabled(enabeld);
}

fmtXmlLint::~fmtXmlLint() {}
