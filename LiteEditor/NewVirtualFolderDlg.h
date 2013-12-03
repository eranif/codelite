#ifndef NEWVIRTUALFOLDERDLG_H
#define NEWVIRTUALFOLDERDLG_H

#include "new_virtual_folder.h"

class NewVirtualFolderDlg : public NewVirtualFolderDlgBase
{
public:
    NewVirtualFolderDlg(wxWindow* parent);
    virtual ~NewVirtualFolderDlg();
    wxString GetName() const {
        return m_textCtrlName->GetValue();
    }
protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
    virtual void OnCreateOnDiskUI(wxUpdateUIEvent& event);
    virtual void OnNameUpdated(wxCommandEvent& event);
};
#endif // NEWVIRTUALFOLDERDLG_H
