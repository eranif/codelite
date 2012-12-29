#include "gitSettingsDlg.h"
#include "windowattrmanager.h"
#include "cl_config.h"
#include "gitentry.h"

GitSettingsDlg::GitSettingsDlg(wxWindow* parent)
    :GitSettingsDlgBase(parent)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    m_pathGIT->SetPath(data.GetGITExecutablePath());
    m_pathGITK->SetPath(data.GetGITKExecutablePath());
    WindowAttrManager::Load(this, wxT("GitSettingsDlg"), NULL);
}

GitSettingsDlg::~GitSettingsDlg()
{
    WindowAttrManager::Save(this, wxT("GitSettingsDlg"), NULL);
}

void GitSettingsDlg::OnOK(wxCommandEvent& event)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    data.SetGITExecutablePath( m_pathGIT->GetPath() );
    data.SetGITKExecutablePath( m_pathGITK->GetPath() );
    conf.WriteItem(&data);
    EndModal(wxID_OK);
}
