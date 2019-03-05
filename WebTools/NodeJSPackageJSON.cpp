#include "NodeJSPackageJSON.h"
#include "JSON.h"
#include "NoteJSWorkspace.h"

NodeJSPackageJSON::NodeJSPackageJSON() {}

NodeJSPackageJSON::~NodeJSPackageJSON() {}

bool NodeJSPackageJSON::Load(const wxString& projectPath)
{
    wxFileName filename(projectPath, "package.json");
    filename.AppendDir(".codelite");
    if(!filename.FileExists()) {
        return false;
    }
    
    JSON root(filename);
    if(!root.isOk()) return false;

    m_name = root.toElement().namedObject("name").toString();
    m_version = root.toElement().namedObject("version").toString();
    m_description = root.toElement().namedObject("description").toString();
    m_script = root.toElement().namedObject("main").toString();
    m_args = root.toElement().namedObject("args").toArrayString();
    return true;
}

bool NodeJSPackageJSON::Create(const wxString& projectPath)
{
    wxFileName filename(projectPath, "package.json");
    if(!filename.FileExists()) {
        return false;
    }
    
    JSON root(filename);
    if(!root.isOk()) return false;

    m_name = root.toElement().namedObject("name").toString();
    m_version = root.toElement().namedObject("version").toString();
    m_description = root.toElement().namedObject("description").toString();
    m_script = root.toElement().namedObject("main").toString();
    // Convert the script into absolute path
    m_script.MakeAbsolute(filename.GetPath());
    
    // Ensure that the folder .codelite exists and "move" the file
    // to that folder
    filename.AppendDir(".codelite");
    filename.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    return Save(projectPath);
}

bool NodeJSPackageJSON::Save(const wxString& projectPath)
{
    wxFileName filename(projectPath, "package.json");
    filename.AppendDir(".codelite");
    
    // Override the previous settings
    JSON root(cJSON_Object);
    JSONItem json = root.toElement();
    
    json.addProperty("name", m_name);
    json.addProperty("version", m_version);
    json.addProperty("description", m_description);
    
    if(!m_script.IsAbsolute()) {
        m_script.MakeAbsolute(filename.GetPath());
    }
    json.addProperty("main", m_script.GetFullPath());
    json.addProperty("args", m_args);
    
    filename.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    root.save(filename);
    return true;
}
