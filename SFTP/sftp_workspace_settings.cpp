#include "sftp_workspace_settings.h"

SFTPWorkspaceSettings::SFTPWorkspaceSettings()
    : clConfigItem("sftp-workspace-settings")
{
}

SFTPWorkspaceSettings::~SFTPWorkspaceSettings()
{
}

void SFTPWorkspaceSettings::FromJSON(const JSONElement& json)
{
    m_remoteWorkspacePath = json.namedObject("m_remoteWorkspacePath").toString();
    m_account = json.namedObject("m_account").toString();
}

JSONElement SFTPWorkspaceSettings::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("m_account", m_account);
    element.addProperty("m_remoteWorkspacePath", m_remoteWorkspacePath);
    return element;
}

void SFTPWorkspaceSettings::Load(SFTPWorkspaceSettings& settings, const wxFileName& workspaceFile)
{
    wxFileName fn(workspaceFile);
    fn.SetName("sftp-workspace-settings");
    fn.SetExt("conf");
    fn.AppendDir(".codelite");
    clConfig config( fn.GetFullPath() );
    config.ReadItem( &settings );
}

void SFTPWorkspaceSettings::Save(const SFTPWorkspaceSettings& settings, const wxFileName& workspaceFile)
{
    wxFileName fn(workspaceFile);
    fn.SetName("sftp-workspace-settings");
    fn.SetExt("conf");
    fn.AppendDir(".codelite");
    clConfig config( fn.GetFullPath() );
    config.WriteItem( &settings );
}

void SFTPWorkspaceSettings::Clear()
{
    m_account.Clear();
    m_remoteWorkspacePath.Clear();
}
