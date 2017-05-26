#ifndef PHPLINTDLG_H
#define PHPLINTDLG_H
#include "phplintdlgbase.h"

class PHPLintDlg : public PHPLintBaseDlg
{
public:
    PHPLintDlg(wxWindow* parent);
    virtual ~PHPLintDlg();
protected:
    virtual void OnCancel(wxCommandEvent& event);
    virtual void OnLintOnLoad(wxCommandEvent& event);
    virtual void OnLintOnSave(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnPhpFileSelected(wxFileDirPickerEvent& event);
    virtual void OnPhpcsFileSelected(wxFileDirPickerEvent& event);
    virtual void OnPhpmdFileSelected(wxFileDirPickerEvent& event);
    virtual void OnPhpmdRulesFileSelected(wxFileDirPickerEvent& event);
};
#endif // PHPLINTDLG_H
