#ifndef PHPLINTDLG_H
#define PHPLINTDLG_H
#include "lintoptions.h"
#include "phplintdlgbase.hpp"

class IManager;
class PHPLintDlg : public PHPLintBaseDlg
{
    LintOptions settings;

public:
    PHPLintDlg(wxWindow* parent);
    ~PHPLintDlg() override = default;

protected:
    void InitDialog();
};
#endif // PHPLINTDLG_H
