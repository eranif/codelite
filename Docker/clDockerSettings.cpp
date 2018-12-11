#include "clDockerSettings.h"
#include "cl_standard_paths.h"
#include "globals.h"

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif

clDockerSettings::clDockerSettings()
    : clConfigItem("Docker")
{
    // The defaults
    wxArrayString hints;
    hints.Add("/usr/local/bin");
    hints.Add("/usr/bin");
#ifdef __WXMSW__
    {
        wxRegKey regkey(wxRegKey::HKCU, "Environment");
        wxString docker_path;
        if(regkey.QueryValue("DOCKER_TOOLBOX_INSTALL_PATH", docker_path) && wxDirExists(docker_path)) {
            hints.Add(docker_path);
        }
    }
#endif
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
