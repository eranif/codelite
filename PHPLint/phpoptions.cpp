#include "phpoptions.h"
#include "globals.h"

PhpOptions::PhpOptions()
    : clConfigItem("PHPConfigurationData")
    , m_phpExe("")
{
}

PhpOptions::~PhpOptions() {}

void PhpOptions::FromJSON(const JSONElement& json)
{
    m_phpExe = json.namedObject("m_phpExe").toString(m_phpExe);
    if (m_phpExe.IsEmpty()) {
        wxFileName phpExe;
        clFindExecutable("php", phpExe);
        m_phpExe = phpExe.GetFullPath();
    }
}

JSONElement PhpOptions::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    return element;
}

PhpOptions& PhpOptions::Load()
{
    clConfig config("php.conf");
    config.ReadItem(this);
    return *this;
}

PhpOptions& PhpOptions::Save()
{
    return *this;
}
