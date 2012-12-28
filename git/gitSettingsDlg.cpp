#include "gitSettingsDlg.h"
#include "windowattrmanager.h"

GitSettingsDlg::GitSettingsDlg(wxWindow* parent, const wxColour& tracked,
                               const wxColour& diff, const wxString& pathGIT,
                               const wxString& pathGITK)
    :GitSettingsDlgBase(parent)
{
    m_pathGIT->SetPath(pathGIT);
    m_pathGITK->SetPath(pathGITK);
    WindowAttrManager::Load(this, wxT("GitSettingsDlg"), NULL);
}

GitSettingsDlg::~GitSettingsDlg()
{
    WindowAttrManager::Save(this, wxT("GitSettingsDlg"), NULL);
}

const wxString GitSettingsDlg::GetGITExecutablePath()
{
    return m_pathGIT->GetPath();
}
const wxString GitSettingsDlg::GetGITKExecutablePath()
{
    return m_pathGITK->GetPath();
}
