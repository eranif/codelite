#ifndef SWITCHTOWORKSPACEDLG_H
#define SWITCHTOWORKSPACEDLG_H
#include "SwitchToWorkspaceBase.hpp"

class WXDLLIMPEXP_SDK SwitchToWorkspaceDlg : public SwitchToWorkspaceBaseDlg
{
public:
    SwitchToWorkspaceDlg(wxWindow* parent);
    ~SwitchToWorkspaceDlg() override;
    wxString GetPath() const;

protected:
    void OnBrowse(wxCommandEvent& event) override;
    void OnOKUI(wxUpdateUIEvent& event) override;
};
#endif // SWITCHTOWORKSPACEDLG_H
