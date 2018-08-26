#ifndef PHPREFACTORINGDLG_H
#define PHPREFACTORINGDLG_H
#include "phprefactoringdlgbase.h"
#include "phprefactoringoptions.h"

class IManager;
class PHPRefactoringDlg : public PHPRefactoringBaseDlg
{
    PHPRefactoringOptions settings;

public:
    PHPRefactoringDlg(wxWindow* parent);
    virtual ~PHPRefactoringDlg();

protected:
    void InitDialog();
};
#endif // PHPREFACTORINGDLG_H
