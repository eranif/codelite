#include "cl_config.h"
#include <wx/stdpaths.h>
#include <wx/filefn.h>

clConfig::clConfig()
{
    m_filename = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config/codelite.conf");
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
    if ( m_root->toElement().hasNamedObject("workspaceTabOrder") ) {
        JSONElement element = m_root->toElement().namedObject("workspaceTabOrder");
        element.namedObject("tabs").setValue( tabs );
        element.namedObject("selected").setValue( selected );
        
    } else {
        // first time
        JSONElement e = JSONElement::createObject("workspaceTabOrder");
        e.addProperty("tabs", tabs);
        e.addProperty("selected", selected);
        m_root->toElement().append( e );
        
    }
    m_root->save(m_filename);
}

