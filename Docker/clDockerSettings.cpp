#include "clDockerSettings.h"
#include "cl_standard_paths.h"
#include "globals.h"

clDockerSettings::clDockerSettings()
    : clConfigItem("Docker")
{
    // The defaults
    wxArrayString hints;
    hints.Add("/usr/local/bin");
    hints.Add("/usr/bin");
    ::clFindExecutable("docker", m_docker, hints);
    ::clFindExecutable("docker-compose", m_dockerCompose, hints);
}

clDockerSettings::~clDockerSettings() {}

void clDockerSettings::FromJSON(const JSONElement& json)
{
    m_docker = json.namedObject("docker").toString(m_docker.GetFullPath());
    m_dockerCompose = json.namedObject("docker-compose").toString(m_dockerCompose.GetFullPath());
    m_flags = json.namedObject("flags").toSize_t(m_flags);
}

JSONElement clDockerSettings::ToJSON() const
{
    JSONElement json = JSONElement::createObject(GetName());
    json.addProperty("docker", m_docker.GetFullPath());
    json.addProperty("docker-compose", m_dockerCompose.GetFullPath());
    json.addProperty("flags", m_flags);
    return json;
}

void clDockerSettings::Load()
{
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "docker.conf");
    fn.AppendDir("config");
    clConfig conf(fn.GetFullPath());
    conf.ReadItem(this);
}

void clDockerSettings::Save()
{
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "docker.conf");
    fn.AppendDir("config");
    clConfig conf(fn.GetFullPath());
    conf.WriteItem(this);
}
