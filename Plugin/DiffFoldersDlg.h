#ifndef DIFFFOLDERSDLG_H
#define DIFFFOLDERSDLG_H
#include "DiffUI.h"

class DiffFoldersDlg : public DiffFoldersBaseDlg
{
public:
    DiffFoldersDlg(wxWindow* parent);
    virtual ~DiffFoldersDlg();
protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnItemContextMenu(wxDataViewEvent& event);
};
#endif // DIFFFOLDERSDLG_H
