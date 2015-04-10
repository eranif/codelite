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

SFTPSettingsDialog::~SFTPSettingsDialog()
{
    
}
