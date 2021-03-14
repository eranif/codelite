#ifndef SWITCHTOWORKSPACEDLG_H
#define SWITCHTOWORKSPACEDLG_H
#include "SwitchToWorkspaceBase.h"

class WXDLLIMPEXP_SDK SwitchToWorkspaceDlg : public SwitchToWorkspaceBaseDlg
{
public:
    SwitchToWorkspaceDlg(wxWindow* parent);
    virtual ~SwitchToWorkspaceDlg();
    wxString GetPath() const;

protected:
    virtual void OnBrowse(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // SWITCHTOWORKSPACEDLG_H
