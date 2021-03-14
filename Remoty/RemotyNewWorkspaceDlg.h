#ifndef REMOTYNEWWORKSPACEDLG_H
#define REMOTYNEWWORKSPACEDLG_H
#include "RemotyUI.h"

class RemotyNewWorkspaceDlg : public RemotyNewWorkspaceDlgBase
{
    wxString m_account;

public:
    RemotyNewWorkspaceDlg(wxWindow* parent);
    virtual ~RemotyNewWorkspaceDlg();

    void GetData(wxString& name, wxString& path, wxString& account);

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnBrowse(wxCommandEvent& event);
};
#endif // REMOTYNEWWORKSPACEDLG_H
