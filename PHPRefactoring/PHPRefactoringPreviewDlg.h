#ifndef PHPREFACTORINGPREVIEWDLG_H
#define PHPREFACTORINGPREVIEWDLG_H
#include "phprefactoringdlgbase.h"

class PHPRefactoringPreviewDlg : public PHPRefactoringPreviewBaseDlg
{
    wxFileName m_patchFile;

public:
    PHPRefactoringPreviewDlg(wxWindow* parent, const wxString& patchFile, const wxString& patchContent);
    virtual ~PHPRefactoringPreviewDlg();

protected:
    virtual void OnApplyChanges(wxCommandEvent& event);
};
#endif // PHPREFACTORINGPREVIEWDLG_H
