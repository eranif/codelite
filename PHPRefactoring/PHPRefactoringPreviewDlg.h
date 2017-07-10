#ifndef PHPREFACTORINGPREVIEWDLG_H
#define PHPREFACTORINGPREVIEWDLG_H
#include "phprefactoringdlgbase.h"

class PHPRefactoringPreviewDlg : public PHPRefactoringPreviewBaseDlg
{

public:
    PHPRefactoringPreviewDlg(wxWindow* parent, const wxString& patchContent);
    virtual ~PHPRefactoringPreviewDlg();
protected:
    virtual void OnApplyChanges(wxCommandEvent& event);
};
#endif // PHPREFACTORINGPREVIEWDLG_H
