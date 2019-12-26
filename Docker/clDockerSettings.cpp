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
    if(!::clFindExecutable("docker", m_docker, hints)) { m_docker = "docker"; }
    if(!::clFindExecutable("docker-compose", m_dockerCompose, hints)) { m_dockerCompose = "docker-compose"; }
}

clDockerSettings::~clDockerSettings() {}

void clDockerSettings::FromJSON(const JSONItem& json)
{
    wxString v;
    v = json.namedObject("docker").toString();
    if(!v.empty()) { m_docker = v; }
    v = json.namedObject("docker-compose").toString();
    if(!v.empty()) { m_dockerCompose = v; }
    m_flags = json.namedObject("flags").toSize_t(m_flags);
}

JSONItem clDockerSettings::ToJSON() const
{
    JSONItem json = JSONItem::createObject(GetName());
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
