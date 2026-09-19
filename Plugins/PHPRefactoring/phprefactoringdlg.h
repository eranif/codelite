#ifndef PHPREFACTORINGDLG_H
#define PHPREFACTORINGDLG_H
#include "phprefactoringdlgbase.hpp"
#include "phprefactoringoptions.h"

class IManager;
class PHPRefactoringDlg : public PHPRefactoringBaseDlg
{
    PHPRefactoringOptions settings;

public:
    PHPRefactoringDlg(wxWindow* parent);
    ~PHPRefactoringDlg() override = default;

protected:
    void InitDialog() override;
};
#endif // PHPREFACTORINGDLG_H
