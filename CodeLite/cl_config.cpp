#include "cl_config.h"
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/log.h>
#include <algorithm>

clConfig::clConfig(const wxString& filename)
{
    if ( wxFileName(filename).IsAbsolute() ) {
        m_filename = filename;
    } else {
        m_filename = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" + wxFileName::GetPathSeparator() + filename;
    }
    
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

void clConfig::Reload()
{
    if ( m_filename.FileExists() == false )
        return;
        
    delete m_root;
    m_root = new JSONRoot(m_filename);
}

wxArrayString clConfig::MergeArrays(const wxArrayString& arr1, const wxArrayString& arr2) const
{
    wxArrayString sArr1, sArr2;
    sArr1.insert(sArr1.end(), arr1.begin(), arr1.end());
    sArr2.insert(sArr2.end(), arr2.begin(), arr2.end());
    
    // Sort the arrays
    std::sort(sArr1.begin(), sArr1.end());
    std::sort(sArr2.begin(), sArr2.end());
    
    wxArrayString output;
    std::set_union(sArr1.begin(), sArr1.end(), sArr2.begin(), sArr2.end(), std::back_inserter(output));
    return output;
}

JSONElement::wxStringMap_t clConfig::MergeStringMaps(const JSONElement::wxStringMap_t& map1, const JSONElement::wxStringMap_t& map2) const
{
    JSONElement::wxStringMap_t output;
    output.insert(map1.begin(), map1.end());
    output.insert(map2.begin(), map2.end());
    return output;
}

void clConfig::Save()
{
    if ( m_root )
        m_root->save(m_filename);
}

void clConfig::Save(const wxFileName& fn)
{
    if ( m_root )
        m_root->save(fn);
}

void clConfig::WriteInt(const wxString& name, int value)
{
    JSONElement general = GetGeneralSetting();
    general.addProperty(name, value);
    Save();
}

JSONElement clConfig::GetGeneralSetting()
{
    if ( m_root->toElement().hasNamedObject("General") ) {
        JSONElement general = JSONElement::createObject("General");
        m_root->toElement().append(general);
    }
    return m_root->toElement().namedObject("General");
}

int clConfig::GetInt(const wxString& name, int defaultValue)
{
    JSONElement general = GetGeneralSetting();
    return general.namedObject(name).toInt(defaultValue);
}
