#include "phpoptions.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "cl_standard_paths.h"
#include <wx/filename.h>

PhpOptions::PhpOptions()
    : clConfigItem("PHPConfigurationData")
    , m_phpExe("")
    , m_errorReporting("E_ALL & ~E_NOTICE")
{
    wxFileName oldConfigFile = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" +
        wxFileName::GetPathSeparator() + "php.conf";
    wxFileName newConfigFile = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" +
        wxFileName::GetPathSeparator() + "php-general.conf";
    if(!newConfigFile.FileExists()) {
        // first time, copy the values from the old settings
        JSONRoot root(oldConfigFile);
        JSONElement oldJson = root.toElement().namedObject("PHPConfigurationData");

        m_phpExe = oldJson.namedObject("m_phpExe").toString();
        m_includePaths = oldJson.namedObject("m_includePaths").toArrayString();
        m_errorReporting = oldJson.namedObject("m_errorReporting").toString();

        // Save it

        JSONRoot newRoot(newConfigFile);
        JSONElement e = JSONElement::createObject(GetName());
        e.addProperty("m_phpExe", m_phpExe);
        e.addProperty("m_includePaths", m_includePaths);
        e.addProperty("m_errorReporting", m_errorReporting);
        newRoot.toElement().append(e);
        newRoot.save(newConfigFile);
    }
}

PhpOptions::~PhpOptions() {}

void PhpOptions::FromJSON(const JSONElement& json)
{
    m_phpExe = json.namedObject("m_phpExe").toString(m_phpExe);
    m_errorReporting = json.namedObject("m_errorReporting").toString(m_errorReporting);
    m_includePaths = json.namedObject("m_includePaths").toArrayString();
}

JSONElement PhpOptions::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("m_phpExe", m_phpExe);
    element.addProperty("m_errorReporting", m_errorReporting);
    element.addProperty("m_includePaths", m_includePaths);
    return element;
}

PhpOptions& PhpOptions::Load()
{
    clConfig config("php-general.conf");
    config.ReadItem(this);
    return *this;
}

PhpOptions& PhpOptions::Save()
{
    clConfig config("php-general.conf");
    config.WriteItem(this);

    // Notify that the PHP settings were modified
    clCommandEvent event(wxEVT_PHP_SETTINGS_CHANGED);
    EventNotifier::Get()->AddPendingEvent(event);

    return *this;
}
