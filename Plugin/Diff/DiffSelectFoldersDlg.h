#ifndef DIFFSELECTFOLDERSDLG_H
#define DIFFSELECTFOLDERSDLG_H
#include "DiffUI.hpp"

class DiffSelectFoldersDlg : public DiffSelectFoldersBaseDlg
{
public:
    DiffSelectFoldersDlg(wxWindow* parent, const wxString& left = "", const wxString& right = "");
    ~DiffSelectFoldersDlg() override;

protected:
    void OnOKUI(wxUpdateUIEvent& event) override;
};
#endif // DIFFSELECTFOLDERSDLG_H
