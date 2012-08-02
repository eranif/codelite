#include "outline_settings.h"
#include <wx/stdpaths.h>
#include "json_node.h"

OutlineSettings::OutlineSettings()
    : m_tabIndex(wxNOT_FOUND)
{
}

OutlineSettings::~OutlineSettings()
{
}

void OutlineSettings::Load()
{
    wxFileName fn(wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config"), wxT("outline.conf"));
    JSONRoot json(fn);
    if( json.isOk() ) {
        m_tabIndex = json.toElement().namedObject(wxT("m_tabIndex")).toInt();
    }
}

void OutlineSettings::Save()
{
    wxFileName fn(wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config"), wxT("outline.conf"));
    JSONRoot json(cJSON_Object);
    json.toElement().addProperty(wxT("m_tabIndex"), m_tabIndex);
    json.save(fn);
}
