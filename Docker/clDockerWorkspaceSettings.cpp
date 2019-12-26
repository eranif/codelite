#include "clDockerWorkspaceSettings.h"

#define DOCKER_VERSION wxString("Docker for CodeLite v1.0")

clDockerWorkspaceSettings::clDockerWorkspaceSettings()
    : clConfigItem("Docker")
{
}

clDockerWorkspaceSettings::~clDockerWorkspaceSettings() {}

void clDockerWorkspaceSettings::FromJSON(const JSONItem& json)
{
    m_files.clear();
    m_version = json.namedObject("Version").toString();
    JSONItem files = json.namedObject("files");
    int filesCount = files.arraySize();
    for(int i = 0; i < filesCount; ++i) {
        JSONItem fileJson = files.arrayItem(i);
        clDockerBuildableFile::Ptr_t f = clDockerBuildableFile::New(
            (eDockerFileType)fileJson.namedObject("type").toInt((int)eDockerFileType::kDockerfile));
        if(!f) { continue; }
        f->FromJSON(fileJson, m_workspaceFile.GetPath());
        m_files.insert({ f->GetPath(), f });
    }
}

JSONItem clDockerWorkspaceSettings::ToJSON() const
{
    JSONItem json = JSONItem::createObject(GetName());
    json.addProperty("Version", DOCKER_VERSION);
    JSONItem files = JSONItem::createArray("files");
    json.append(files);
    std::for_each(m_files.begin(), m_files.end(), [&](const clDockerBuildableFile::Map_t::value_type& vt) {
        files.arrayAppend(vt.second->ToJSON(m_workspaceFile.GetPath()));
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

clDockerBuildableFile::Ptr_t clDockerWorkspaceSettings::GetFileInfo(const wxFileName& file) const
{
    if(m_files.count(file.GetFullPath()) == 0) { return clDockerBuildableFile::Ptr_t(new clDockerBuildableFile()); }
    return m_files.find(file.GetFullPath())->second;
}

void clDockerWorkspaceSettings::SetFileInfo(const wxFileName& file, clDockerBuildableFile::Ptr_t info)
{
    if(m_files.count(file.GetFullPath())) { m_files.erase(file.GetFullPath()); }
    m_files.insert({ file.GetFullPath(), info });
}
