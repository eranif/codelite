#include "ssh_workspace_settings.h"
#include "php_workspace.h"

SSHWorkspaceSettings::SSHWorkspaceSettings()
    : clConfigItem("sftp")
    , m_remoteUploadEnabled(true)
{
}

SSHWorkspaceSettings::~SSHWorkspaceSettings()
{
}

void SSHWorkspaceSettings::FromJSON(const JSONItem& json)
{
    m_account = json.namedObject("m_account").toString();
    m_remoteFolder = json.namedObject("m_remoteFolder").toString();
    m_remoteUploadEnabled = json.namedObject("m_remoteUploadEnabled").toBool();
}

JSONItem SSHWorkspaceSettings::ToJSON() const
{
    JSONItem json = JSONItem::createObject(GetName());
    json.addProperty("m_account", m_account);
    json.addProperty("m_remoteFolder", m_remoteFolder);
    json.addProperty("m_remoteUploadEnabled", m_remoteUploadEnabled);
    return json;
}

void SSHWorkspaceSettings::Load()
{
    wxFileName fn(PHPWorkspace::Get()->GetPrivateFolder(), "php-sftp.conf" );
    clConfig conf(fn.GetFullPath());
    conf.ReadItem( this );
}

void SSHWorkspaceSettings::Save()
{
    wxFileName fn(PHPWorkspace::Get()->GetPrivateFolder(), "php-sftp.conf" );
    clConfig conf(fn.GetFullPath());
    conf.WriteItem( this );
}

bool SSHWorkspaceSettings::IsRemoteUploadSet() const
{
    return !m_remoteFolder.IsEmpty() && !m_account.IsEmpty();
}

void SSHWorkspaceSettings::Reset()
{
    m_remoteFolder.clear();
    m_account.clear();
    m_remoteUploadEnabled = false;
}
