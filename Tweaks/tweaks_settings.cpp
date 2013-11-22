#include "tweaks_settings.h"
#include <wx/settings.h>
#include "workspace.h"

#define TWEAKS_CONF_FILE "tweaks-settings"

// --------------------------------------------------------
// TweaksSettings
// --------------------------------------------------------

TweaksSettings::TweaksSettings()
    : clConfigItem(TWEAKS_CONF_FILE)
{
}

TweaksSettings::~TweaksSettings()
{
}

void TweaksSettings::FromJSON(const JSONElement& json)
{
    m_projects.clear();
    JSONElement arr = json.namedObject("projects");
    int size = arr.arraySize();
    for(int i=0; i<size; ++i) {
        ProjectTweaks tw;
        tw.FromJSON( arr.arrayItem(i) );
        m_projects.insert( std::make_pair(tw.GetProjectName(), tw) );
    }
}

JSONElement TweaksSettings::ToJSON() const
{
    JSONElement e = JSONElement::createObject(GetName());
    JSONElement arr = JSONElement::createArray("projects");
    e.append( arr );
    
    ProjectTweaks::Map_t::const_iterator iter = m_projects.begin();
    for( ; iter != m_projects.end(); ++iter ) {
        arr.arrayAppend( iter->second.ToJSON() );
    }
    return e;
}

TweaksSettings& TweaksSettings::Load()
{
    // Get the file name
    wxFileName fn(WorkspaceST::Get()->GetPrivateFolder(), "tweaks.conf");
    clConfig conf( fn.GetFullPath() );
    conf.ReadItem( this );
    return *this;
}

void TweaksSettings::Save()
{
    wxFileName fn(WorkspaceST::Get()->GetPrivateFolder(), "tweaks.conf");
    clConfig conf( fn.GetFullPath() );
    conf.WriteItem( this );
}

const ProjectTweaks& TweaksSettings::GetProjectTweaks(const wxString& project) const
{
    if ( m_projects.count(project) ) {
        return m_projects.find(project)->second;
    }
    static ProjectTweaks DUMMY;
    return DUMMY;
}

void TweaksSettings::UpdateProject(const ProjectTweaks& pt)
{
    DeleteProject( pt.GetProjectName() );
    m_projects.insert( std::make_pair(pt.GetProjectName(), pt) );
}

void TweaksSettings::Clear()
{
    m_projects.clear();
}

void TweaksSettings::DeleteProject(const wxString& name)
{
    if ( m_projects.count(name) ) {
        m_projects.erase(name);
    }
}

// --------------------------------------------------------
// ProjectTweaks
// --------------------------------------------------------

ProjectTweaks::ProjectTweaks()
    : clConfigItem("")
{
}

ProjectTweaks::~ProjectTweaks()
{
}

void ProjectTweaks::FromJSON(const JSONElement& json)
{
    m_tabBgColour = json.namedObject("m_tabBgColour").toColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    m_tabFgColour = json.namedObject("m_tabFgColour").toColour(*wxBLACK);
    m_projectName = json.namedObject("m_projectName").toString();
}

JSONElement ProjectTweaks::ToJSON() const
{
    JSONElement e = JSONElement::createObject();
    e.addProperty("m_tabFgColour", m_tabFgColour);
    e.addProperty("m_tabBgColour", m_tabBgColour);
    e.addProperty("m_projectName", m_projectName);
    return e;
}
