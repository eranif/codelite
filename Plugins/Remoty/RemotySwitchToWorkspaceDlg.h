#ifndef REMOTYSWITCHTOWORKSPACEDLG_H
#define REMOTYSWITCHTOWORKSPACEDLG_H
#include "RemotyConfig.hpp"
#include "RemotyUI.hpp"

#include <unordered_map>
#include <vector>

class RemotySwitchToWorkspaceDlg : public RemotySwitchToWorkspaceDlgBase
{
    std::vector<RemoteWorkspaceInfo> m_remoteWorkspaces;

public:
    RemotySwitchToWorkspaceDlg(wxWindow* parent);
    virtual ~RemotySwitchToWorkspaceDlg();
    bool IsRemote() const;
    wxString GetPath() const;
    wxString GetAccount();

protected:
    void OnPathChanged(wxCommandEvent& event) override;
    void OnChoiceWorkspaceType(wxCommandEvent& event) override;
    void OnBrowse(wxCommandEvent& event) override;
    void OnRemoteUI(wxUpdateUIEvent& event) override;
    void OnOKUI(wxUpdateUIEvent& event) override;
    void SyncPathToAccount();
    void InitialiseDialog();
};
#endif // REMOTYSWITCHTOWORKSPACEDLG_H
