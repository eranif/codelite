#ifndef GITAPPLYPATCHDLG_H
#define GITAPPLYPATCHDLG_H
#include "gitui.h"

class GitApplyPatchDlg : public GitApplyPatchDlgBase
{
public:
    GitApplyPatchDlg(wxWindow* parent);
    virtual ~GitApplyPatchDlg();
    
    wxString GetExtraFlags() const {
        return m_textCtrlExtraFlags->GetValue().Trim();
    }
    wxString GetPatchFile() const {
        return m_filePickerPatchFile->GetPath().Trim();
    }
    
protected:
    virtual void OnApplyGitPatchUI(wxUpdateUIEvent& event);
};
#endif // GITAPPLYPATCHDLG_H
