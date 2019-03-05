#include "phpoptions.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "cl_standard_paths.h"
#include <wx/filename.h>
#include "globals.h"

PhpOptions::PhpOptions()
    : clConfigItem("PHPConfigurationData")
    , m_phpExe("")
    , m_errorReporting("E_ALL & ~E_NOTICE")
{
    wxFileName newConfigFile = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" +
        wxFileName::GetPathSeparator() + "php-general.conf";
    if(!newConfigFile.FileExists()) {
        wxFileName oldConfigFile = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" +
            wxFileName::GetPathSeparator() + "php.conf";
        // first time, copy the values from the old settings
        JSON root(oldConfigFile);
        JSONItem oldJson = root.toElement().namedObject("PHPConfigurationData");

        m_phpExe = oldJson.namedObject("m_phpExe").toString();
        m_includePaths = oldJson.namedObject("m_includePaths").toArrayString();
        m_errorReporting = oldJson.namedObject("m_errorReporting").toString();

        // Save it

        JSON newRoot(newConfigFile);
        JSONItem e = JSONItem::createObject(GetName());
        e.addProperty("m_phpExe", m_phpExe);
        e.addProperty("m_includePaths", m_includePaths);
        e.addProperty("m_errorReporting", m_errorReporting);
        newRoot.toElement().append(e);
        newRoot.save(newConfigFile);
    }
}

PhpOptions::~PhpOptions() {}

void PhpOptions::FromJSON(const JSONItem& json)
{
    m_phpExe = json.namedObject("m_phpExe").toString(m_phpExe);
    if (m_phpExe.IsEmpty()) {
        wxFileName phpExe;
        clFindExecutable("php", phpExe);
        m_phpExe = phpExe.GetFullPath();
    }

    m_errorReporting = json.namedObject("m_errorReporting").toString(m_errorReporting);
    m_includePaths = json.namedObject("m_includePaths").toArrayString();
}

JSONItem PhpOptions::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
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
