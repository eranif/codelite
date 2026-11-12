#pragma once
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
