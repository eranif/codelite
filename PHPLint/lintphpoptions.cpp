#include "lintphpoptions.h"

LintPhpOptions::LintPhpOptions()
    : clConfigItem("PHPConfigurationData")
    , m_phpExe("")
{
}

LintPhpOptions::~LintPhpOptions() {}

void LintPhpOptions::FromJSON(const JSONElement& json)
{
    m_phpExe = json.namedObject("m_phpExe").toString(m_phpExe);
}

JSONElement LintPhpOptions::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    return element;
}

LintPhpOptions& LintPhpOptions::Load()
{
    clConfig config("php.conf");
    config.ReadItem(this);
    return *this;
}

LintPhpOptions& LintPhpOptions::Save()
{
    clConfig config("phplint.conf");
    return *this;
}
