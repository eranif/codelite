#ifndef DIFFSELECTFOLDERSDLG_H
#define DIFFSELECTFOLDERSDLG_H
#include "DiffUI.h"

class DiffSelectFoldersDlg : public DiffSelectFoldersBaseDlg
{
public:
    DiffSelectFoldersDlg(wxWindow* parent);
    virtual ~DiffSelectFoldersDlg();

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // DIFFSELECTFOLDERSDLG_H
