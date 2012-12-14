#include "cl_config.h"
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/log.h>

clConfig::clConfig(const wxString& filename)
{
    m_filename = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" + wxFileName::GetPathSeparator() + filename;
    {
        // Make sure that the directory exists
        wxLogNull noLog;
        wxMkdir( wxStandardPaths::Get().GetUserDataDir() );
        wxMkdir( m_filename.GetPath() );
    }
    
    if ( m_filename.FileExists() ) {
        m_root = new JSONRoot(m_filename);
        
    } else {
        m_root = new JSONRoot(cJSON_Object);
    }
}

clConfig::~clConfig()
{
    m_root->save( m_filename );
    wxDELETE(m_root);
}

clConfig& clConfig::Get()
{
    static clConfig config;
    return config;
}

bool clConfig::GetWorkspaceTabOrder(wxArrayString& tabs, int& selected)
{
    if ( m_root->toElement().hasNamedObject("workspaceTabOrder") ) {
        JSONElement element = m_root->toElement().namedObject("workspaceTabOrder");
        tabs = element.namedObject("tabs").toArrayString();
        selected = element.namedObject("selected").toInt();
        return true;
    }
    return false;
}

void clConfig::SetWorkspaceTabOrder(const wxArrayString& tabs, int selected)
{
    DoDeleteProperty("workspaceTabOrder");
    
    // first time
    JSONElement e = JSONElement::createObject("workspaceTabOrder");
    e.addProperty("tabs", tabs);
    e.addProperty("selected", selected);
    m_root->toElement().append( e );
    
    m_root->save(m_filename);
}

void clConfig::DoDeleteProperty(const wxString& property)
{
    if ( m_root->toElement().hasNamedObject(property) ) {
        m_root->toElement().removeProperty(property);
    }
}

bool clConfig::ReadItem(clConfigItem* item)
{
    if ( m_root->toElement().hasNamedObject(item->GetName()) ) {
        item->FromJSON( m_root->toElement().namedObject(item->GetName()));
        return true;
    }
    return false;
}

void clConfig::WriteItem(const clConfigItem* item)
{
    DoDeleteProperty(item->GetName());
    m_root->toElement().append(item->ToJSON());
    m_root->save(m_filename);
}

