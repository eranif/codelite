#ifndef NEWVIRTUALFOLDERDLG_H
#define NEWVIRTUALFOLDERDLG_H

#include "new_virtual_folder.h"

class NewVirtualFolderDlg : public NewVirtualFolderDlgBase
{
    wxString m_basePath;
protected:
    void DoUpdatePath();
    
public:
    NewVirtualFolderDlg(wxWindow* parent, const wxString& currentVD);
    virtual ~NewVirtualFolderDlg();
    wxString GetName() const {
        return m_textCtrlName->GetValue();
    }
    bool GetCreateOnDisk() const {
        return m_checkBoxCreateOnDisk->IsChecked();
    }
    wxString GetDiskPath() const {
        return m_textCtrlPath->GetValue();
    }
protected:
    virtual void OnCreateOnFolderChecked(wxCommandEvent& event);
    virtual void OnOkUI(wxUpdateUIEvent& event);
    virtual void OnCreateOnDiskUI(wxUpdateUIEvent& event);
    virtual void OnNameUpdated(wxCommandEvent& event);
};
#endif // NEWVIRTUALFOLDERDLG_H
