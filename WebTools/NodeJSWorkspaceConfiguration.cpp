#include "NodeJSWorkspaceConfiguration.h"
#include <algorithm>

NodeJSWorkspaceConfiguration::NodeJSWorkspaceConfiguration(const wxFileName& filename)
    : clConfigItem("NodeJS")
    , m_isOk(false)
    , m_showHiddenFiles(false)
    , m_filename(filename)
{
}

void NodeJSWorkspaceConfiguration::FromJSON(const JSONItem& json)
{
    m_folders = json.namedObject("folders").toArrayString();
    ConvertToRelative(m_folders);
    
    m_showHiddenFiles = json.namedObject("m_showHiddenFiles").toBool(m_showHiddenFiles);

    m_isOk = false;
    if(json.hasNamedObject("metadata")) {
        JSONItem e = json.namedObject("metadata");
        if(e.hasNamedObject("type")) { m_isOk = (e.namedObject("type").toString() == "NodeJS"); }
    }
}

JSONItem NodeJSWorkspaceConfiguration::ToJSON() const
{
    // add the metadata
    JSONItem json = JSONItem::createObject(GetName());
    JSONItem metadata = JSONItem::createObject("metadata");
    json.append(metadata);
    metadata.addProperty("version", "1.0");
    metadata.addProperty("ide", "CodeLite");
    metadata.addProperty("type", "NodeJS");

    // now add some real properties
    json.addProperty("folders", m_folders);
    json.addProperty("m_showHiddenFiles", m_showHiddenFiles);
    return json;
}

NodeJSWorkspaceConfiguration& NodeJSWorkspaceConfiguration::Load()
{
    clConfig conf(m_filename.GetFullPath());
    conf.ReadItem(this);
    return *this;
}

NodeJSWorkspaceConfiguration& NodeJSWorkspaceConfiguration::Save()
{
    clConfig conf(m_filename.GetFullPath());
    conf.WriteItem(this);
    return *this;
}

NodeJSWorkspaceConfiguration& NodeJSWorkspaceConfiguration::SetFolders(const wxArrayString& folders)
{
    this->m_folders = folders;
    ConvertToRelative(this->m_folders);
    return *this;
}

wxArrayString NodeJSWorkspaceConfiguration::GetFolders() const
{
    // Convert the folders to absolute path
    wxArrayString folders;
    for (const wxString& folder : this->m_folders) {
        wxFileName fnFolder(folder, "dummy.txt");
        fnFolder.MakeAbsolute(m_filename.GetPath());
        folders.Add(fnFolder.GetPath());
    }
    return folders;
}

void NodeJSWorkspaceConfiguration::ConvertToRelative(wxString& folder) const
{
    wxFileName fnFolder(folder, "dummy.txt");
    if(fnFolder.IsAbsolute()) { fnFolder.MakeRelativeTo(m_filename.GetPath()); }
    folder = fnFolder.GetPath(wxPATH_GET_VOLUME, wxPATH_UNIX);
    if(folder.IsEmpty()) { folder = "."; }
}

void NodeJSWorkspaceConfiguration::ConvertToRelative(wxArrayString& folders) const
{
    for (auto& folder : folders) {
        ConvertToRelative(folder);
    }
}
