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
        File f;
        f.FromJSON(files.arrayItem(i));
        m_files.push_back(f);
    }
}

JSONElement clDockerWorkspaceSettings::ToJSON() const
{
    JSONElement json = JSONElement::createObject(GetName());
    json.addProperty("Version", DOCKER_VERSION);
    JSONElement files = JSONElement::createArray("files");
    json.append(files);
    for(size_t i = 0; i < m_files.size(); ++i) {
        files.arrayAppend(m_files[i].ToJSON());
    }
    return json;
}

clDockerWorkspaceSettings& clDockerWorkspaceSettings::Load(const wxFileName& filename)
{
    clConfig conf(filename.GetFullPath());
    conf.ReadItem(this);
    return *this;
}

clDockerWorkspaceSettings& clDockerWorkspaceSettings::Save(const wxFileName& filename)
{
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

void clDockerWorkspaceSettings::File::FromJSON(const JSONElement& json)
{
    m_path = json.namedObject("path").toString();
    m_buildOptions = json.namedObject("buildOptions").toString();
    m_runOptions = json.namedObject("runOptions").toString();
}

JSONElement clDockerWorkspaceSettings::File::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("path", m_path);
    json.addProperty("buildOptions", m_buildOptions);
    json.addProperty("runOptions", m_runOptions);
    return json;
}
