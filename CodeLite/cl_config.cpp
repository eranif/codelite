#include "cl_config.h"
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/log.h>
#include <algorithm>
#include "cl_standard_paths.h"

clConfig::clConfig(const wxString& filename)
{
    if ( wxFileName(filename).IsAbsolute() ) {
        m_filename = filename;
    } else {
        m_filename = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" + wxFileName::GetPathSeparator() + filename;
    }
    
    {
        // Make sure that the directory exists
        wxLogNull noLog;
        wxMkdir( clStandardPaths::Get().GetUserDataDir() );
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

bool clConfig::ReadItem(clConfigItem* item, const wxString &differentName)
{
    wxString nameToUse = differentName.IsEmpty() ? item->GetName() : differentName;
    if ( m_root->toElement().hasNamedObject(nameToUse) ) {
        item->FromJSON( m_root->toElement().namedObject(nameToUse));
        return true;
    }
    return false;
}

void clConfig::WriteItem(const clConfigItem* item, const wxString &differentName)
{
    wxString nameToUse = differentName.IsEmpty() ? item->GetName() : differentName;
    DoDeleteProperty(nameToUse);
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

JSONElement clConfig::GetGeneralSetting()
{
    if ( !m_root->toElement().hasNamedObject("General") ) {
        JSONElement general = JSONElement::createObject("General");
        m_root->toElement().append(general);
    }
    return m_root->toElement().namedObject("General");
}

void clConfig::Write(const wxString& name, bool value)
{
    JSONElement general = GetGeneralSetting();
    if (general.hasNamedObject(name)) {
        general.removeProperty(name);
    }

    general.addProperty(name, value);
    Save();
}

bool clConfig::Read(const wxString& name, bool defaultValue)
{
    JSONElement general = GetGeneralSetting();
    if (general.namedObject(name).isBool()) {
        return general.namedObject(name).toBool();
    }

    return defaultValue;
}

void clConfig::Write(const wxString& name, int value)
{
    JSONElement general = GetGeneralSetting();
    if (general.hasNamedObject(name)) {
        general.removeProperty(name);
    }

    general.addProperty(name, value);
    Save();
}

int clConfig::Read(const wxString& name, int defaultValue)
{
    JSONElement general = GetGeneralSetting();
    return general.namedObject(name).toInt(defaultValue);
}

void clConfig::Write(const wxString& name, const wxString& value)
{
    JSONElement general = GetGeneralSetting();
    if (general.hasNamedObject(name)) {
        general.removeProperty(name);
    }

    general.addProperty(name, value);
    Save();
}

wxString clConfig::Read(const wxString& name, const wxString& defaultValue)
{
    JSONElement general = GetGeneralSetting();
    if (general.namedObject(name).isString()) {
        return general.namedObject(name).toString();
    }

    return defaultValue;
}

int clConfig::GetAnnoyingDlgAnswer(const wxString& name, int defaultValue)
{
    if ( m_root->toElement().hasNamedObject("AnnoyingDialogsAnswers") ) {
        
        JSONElement element = m_root->toElement().namedObject("AnnoyingDialogsAnswers");
        if ( element.hasNamedObject(name) ) {
            return element.namedObject(name).toInt(defaultValue);
        }
    }
    return defaultValue;
}

void clConfig::SetAnnoyingDlgAnswer(const wxString& name, int value)
{
    if ( !m_root->toElement().hasNamedObject("AnnoyingDialogsAnswers") ) {
        JSONElement element = JSONElement::createObject("AnnoyingDialogsAnswers");
        m_root->toElement().append(element);
    }
    
    JSONElement element =m_root->toElement().namedObject("AnnoyingDialogsAnswers");
    if ( element.hasNamedObject(name) ) {
        element.removeProperty(name);
    }
    element.addProperty(name, value);
    Save();
}

void clConfig::ClearAnnoyingDlgAnswers()
{
    DoDeleteProperty("AnnoyingDialogsAnswers");
    Save();
    Reload();
}
