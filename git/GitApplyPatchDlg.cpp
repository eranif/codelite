#include "GitApplyPatchDlg.h"
#include <wx/filename.h>

GitApplyPatchDlg::GitApplyPatchDlg(wxWindow* parent)
    : GitApplyPatchDlgBase(parent)
{
}

GitApplyPatchDlg::~GitApplyPatchDlg()
{
}

void GitApplyPatchDlg::OnApplyGitPatchUI(wxUpdateUIEvent& event)
{
    wxFileName fnPathFile( m_filePickerPatchFile->GetPath() );
    event.Enable( fnPathFile.IsOk() && fnPathFile.Exists() );
}
