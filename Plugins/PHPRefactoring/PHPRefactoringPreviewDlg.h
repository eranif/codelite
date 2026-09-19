#ifndef PHPREFACTORINGPREVIEWDLG_H
#define PHPREFACTORINGPREVIEWDLG_H
#include "phprefactoringdlgbase.hpp"

class PHPRefactoringPreviewDlg : public PHPRefactoringPreviewBaseDlg
{

public:
    PHPRefactoringPreviewDlg(wxWindow* parent, const wxString& patchContent);
    ~PHPRefactoringPreviewDlg() override = default;

protected:
    void OnApplyChanges(wxCommandEvent& event) override;
};
#endif // PHPREFACTORINGPREVIEWDLG_H
