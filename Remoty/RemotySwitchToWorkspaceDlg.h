#ifndef REMOTYSWITCHTOWORKSPACEDLG_H
#define REMOTYSWITCHTOWORKSPACEDLG_H
#include "RemotyConfig.hpp"
#include "RemotyUI.h"

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
    virtual void OnPathChanged(wxCommandEvent& event) override;
    virtual void OnChoiceWorkspaceType(wxCommandEvent& event) override;
    virtual void OnBrowse(wxCommandEvent& event) override;
    virtual void OnRemoteUI(wxUpdateUIEvent& event) override;
    virtual void OnOKUI(wxUpdateUIEvent& event) override;
    void SyncPathToAccount();
    void InitialiseDialog();
};
#endif // REMOTYSWITCHTOWORKSPACEDLG_H
