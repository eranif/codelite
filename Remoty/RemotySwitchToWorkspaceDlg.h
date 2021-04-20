#ifndef REMOTYSWITCHTOWORKSPACEDLG_H
#define REMOTYSWITCHTOWORKSPACEDLG_H
#include "RemotyUI.h"
#include <unordered_map>

class RemotySwitchToWorkspaceDlg : public RemotySwitchToWorkspaceDlgBase
{
    std::unordered_map<wxString, wxString> m_displayToRemotePath;

public:
    RemotySwitchToWorkspaceDlg(wxWindow* parent);
    virtual ~RemotySwitchToWorkspaceDlg();
    bool IsRemote() const
    {
        return !m_comboBoxRemote->GetValue().empty();
    }
    wxString GetPath() const;

protected:
    void OnOKUI(wxUpdateUIEvent& event) override;
    void OnLocalBrowse(wxCommandEvent& event) override;
    void OnRemoteBrowse(wxCommandEvent& event) override;
    wxString GetDisplayStringFromFullPath(const wxString& full_path) const;
};
#endif // REMOTYSWITCHTOWORKSPACEDLG_H
