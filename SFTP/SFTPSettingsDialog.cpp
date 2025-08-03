#include "SFTPSettingsDialog.h"
#include "windowattrmanager.h"
#include "sftp_settings.h"

SFTPSettingsDialog::SFTPSettingsDialog(wxWindow* parent)
    : SFTPSettingsDialogBase(parent)
{
    CenterOnParent();
    SetName("SFTPSettingsDialog");
    WindowAttrManager::Load(this);
    SFTPSettings s;
    s.Load();
    m_sshClientPath->SetPath(s.GetSshClient());
}

void SFTPSettingsDialog::OnOK(wxCommandEvent& event)
{
    // Save the data
    SFTPSettings settings;
    settings.Load();
    settings.SetSshClient(GetSshClientPath()->GetPath());
    settings.Save();
    event.Skip();
}
