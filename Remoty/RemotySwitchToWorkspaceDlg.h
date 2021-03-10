#ifndef REMOTYSWITCHTOWORKSPACEDLG_H
#define REMOTYSWITCHTOWORKSPACEDLG_H
#include "RemotyUI.h"

class RemotySwitchToWorkspaceDlg : public RemotySwitchToWorkspaceDlgBase
{
public:
    RemotySwitchToWorkspaceDlg(wxWindow* parent);
    virtual ~RemotySwitchToWorkspaceDlg();
    bool IsRemote() const { return !m_comboBoxRemote->GetValue().empty(); }
    wxString GetPath() const
    {
        if(IsRemote()) {
            return m_comboBoxRemote->GetValue();
        } else {
            return m_comboBoxLocal->GetValue();
        }
    }

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnLocalBrowse(wxCommandEvent& event);
    virtual void OnRemoteBrowse(wxCommandEvent& event);
};
#endif // REMOTYSWITCHTOWORKSPACEDLG_H
