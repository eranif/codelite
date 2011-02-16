#include "gitSettingsDlg.h"
#include "windowattrmanager.h"

#include "icons/icon_git.xpm"

GitSettingsDlg::GitSettingsDlg(wxWindow* parent, const wxColour& tracked,
                               const wxColour& diff, const wxString& pathGIT,
                               const wxString& pathGITK)
	:GitSettingsDlgBase(parent)
{
	m_pathGIT->SetPath(pathGIT);
	m_pathGITK->SetPath(pathGITK);
	m_colourDiffFile->SetColour(diff);
	m_colourTrackedFile->SetColour(tracked);
	WindowAttrManager::Load(this, wxT("GitSettingsDlg"), NULL);
}

GitSettingsDlg::~GitSettingsDlg()
{
	WindowAttrManager::Save(this, wxT("GitSettingsDlg"), NULL);
}

const wxColour GitSettingsDlg::GetTrackedFileColour()
{
	return m_colourTrackedFile->GetColour();
}
const wxColour GitSettingsDlg::GetDiffFileColour()
{
	return m_colourDiffFile->GetColour();
}
const wxString GitSettingsDlg::GetGITExecutablePath()
{
	return m_pathGIT->GetPath();
}
const wxString GitSettingsDlg::GetGITKExecutablePath()
{
	return m_pathGITK->GetPath();
}


