#include "GitApplyPatchDlg.h"
#include <wx/filename.h>
#include "windowattrmanager.h"

GitApplyPatchDlg::GitApplyPatchDlg(wxWindow* parent)
    : GitApplyPatchDlgBase(parent)
{
    WindowAttrManager::Load(this, "GitPatchDlg", NULL);
}

GitApplyPatchDlg::~GitApplyPatchDlg()
{
    WindowAttrManager::Save(this, "GitPatchDlg", NULL);
}

void GitApplyPatchDlg::OnApplyGitPatchUI(wxUpdateUIEvent& event)
{
    wxFileName fnPathFile( m_filePickerPatchFile->GetPath() );
    event.Enable( fnPathFile.IsOk() && fnPathFile.Exists() );
}
