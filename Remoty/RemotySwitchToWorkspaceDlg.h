#ifndef REMOTYSWITCHTOWORKSPACEDLG_H
#define REMOTYSWITCHTOWORKSPACEDLG_H
#include "RemotyConfig.hpp"
#include "RemotyUI.h"
#include <unordered_map>
#include <vector>

using namespace std;
class RemotySwitchToWorkspaceDlg : public RemotySwitchToWorkspaceDlgBase
{
    vector<RemoteWorkspaceInfo> m_remoteWorkspaces;

public:
    RemotySwitchToWorkspaceDlg(wxWindow* parent);
    virtual ~RemotySwitchToWorkspaceDlg();
    bool IsRemote() const;
    wxString GetPath() const;
    wxString GetAccount();

protected:
    virtual void OnPathChanged(wxCommandEvent& event);
    virtual void OnChoiceWorkspaceType(wxCommandEvent& event);
    virtual void OnBrowse(wxCommandEvent& event);
    virtual void OnRemoteUI(wxUpdateUIEvent& event);
    void OnOKUI(wxUpdateUIEvent& event) override;
    void SyncPathToAccount();
    void InitialiseDialog();
};
#endif // REMOTYSWITCHTOWORKSPACEDLG_H
