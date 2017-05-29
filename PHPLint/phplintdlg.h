#ifndef PHPLINTDLG_H
#define PHPLINTDLG_H
#include "phplintdlgbase.h"
#include "lintoptions.h"

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
