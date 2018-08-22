#include "clDockerWorkspaceSettings.h"

#define DOCKER_VERSION wxString("Docker for CodeLite v1.0")

clDockerWorkspaceSettings::clDockerWorkspaceSettings()
    : clConfigItem("Docker")
{
}

clDockerWorkspaceSettings::~clDockerWorkspaceSettings() {}

void clDockerWorkspaceSettings::FromJSON(const JSONElement& json)
{
    m_files.clear();
    m_version = json.namedObject("Version").toString();
    JSONElement files = json.namedObject("files");
    int filesCount = files.arraySize();
    for(int i = 0; i < filesCount; ++i) {
        clDockerfile f;
        f.FromJSON(files.arrayItem(i), m_workspaceFile.GetPath());
        m_files.insert({ f.GetPath(), f });
    }
}

JSONElement clDockerWorkspaceSettings::ToJSON() const
{
    JSONElement json = JSONElement::createObject(GetName());
    json.addProperty("Version", DOCKER_VERSION);
    JSONElement files = JSONElement::createArray("files");
    json.append(files);
    std::for_each(m_files.begin(), m_files.end(), [&](const clDockerfile::Map_t::value_type& vt) {
        files.arrayAppend(vt.second.ToJSON(m_workspaceFile.GetPath()));
    });
    return json;
}

clDockerWorkspaceSettings& clDockerWorkspaceSettings::Load(const wxFileName& filename)
{
    m_workspaceFile = filename;
    clConfig conf(filename.GetFullPath());
    conf.ReadItem(this);
    return *this;
}

clDockerWorkspaceSettings& clDockerWorkspaceSettings::Save(const wxFileName& filename)
{
    m_workspaceFile = filename;
    clConfig conf(filename.GetFullPath());
    conf.WriteItem(this);
    return *this;
}

bool clDockerWorkspaceSettings::IsOk() const { return m_version.Contains("Docker for CodeLite"); }

void clDockerWorkspaceSettings::Clear()
{
    m_files.clear();
    m_version.clear();
}

bool clDockerWorkspaceSettings::GetFileInfo(const wxFileName& file, clDockerfile& info) const
{
    if(m_files.count(file.GetFullPath()) == 0) { return false; }
    info = m_files.find(file.GetFullPath())->second;
    return true;
}

void clDockerWorkspaceSettings::SetFileInfo(const wxFileName& file, const clDockerfile& info)
{
    if(m_files.count(file.GetFullPath())) { m_files.erase(file.GetFullPath()); }
    m_files.insert({ file.GetFullPath(), info });
}

void clDockerfile::FromJSON(const JSONElement& json, const wxString& workspaceDir)
{
    m_path = json.namedObject("path").toString();
    // The file path is relative, make it absolute
    wxFileName fn(m_path);
    fn.MakeAbsolute(workspaceDir);
    m_path = fn.GetFullPath();

    m_buildOptions = json.namedObject("buildOptions").toString();
    m_runOptions = json.namedObject("runOptions").toString();
}

JSONElement clDockerfile::ToJSON(const wxString& workspaceDir) const
{
    JSONElement json = JSONElement::createObject();

    // m_path is absolute, convert to relative before we save it
    wxFileName fn(m_path);
    fn.MakeRelativeTo(workspaceDir); // Use Unix style paths
    wxString path = fn.GetFullPath();
    path.Replace("\\", "/");
    json.addProperty("path", path);
    json.addProperty("buildOptions", m_buildOptions);
    json.addProperty("runOptions", m_runOptions);
    return json;
}
