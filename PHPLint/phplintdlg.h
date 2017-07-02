#ifndef PHPLINTDLG_H
#define PHPLINTDLG_H
#include "lintoptions.h"
#include "phplintdlgbase.h"

class IManager;
class PHPLintDlg : public PHPLintBaseDlg
{
    LintOptions settings;

public:
    PHPLintDlg(wxWindow* parent);
    virtual ~PHPLintDlg();

protected:
    void InitDialog();
};
#endif // PHPLINTDLG_H
