#include "phpoptions.h"
#include "event_notifier.h"
#include "codelite_events.h"

PhpOptions::PhpOptions()
    : clConfigItem("PHPConfigurationData")
    , m_phpExe("")
{
}

PhpOptions::~PhpOptions() {}

void PhpOptions::FromJSON(const JSONElement& json)
{
    m_phpExe = json.namedObject("m_phpExe").toString(m_phpExe);
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
    clConfig config("php.conf");
    config.WriteItem(this);
    
    // Notify that the PHP settings were modified
    clCommandEvent event(wxEVT_PHP_SETTINGS_CHANGED);
    EventNotifier::Get()->AddPendingEvent(event);
    
    return *this;
}
