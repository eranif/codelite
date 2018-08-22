#ifndef NEWDOCKERWORKSPACEDLG_H
#define NEWDOCKERWORKSPACEDLG_H
#include "UI.h"
#include <wx/filename.h>

class NewDockerWorkspaceDlg : public NewDockerWorkspaceDlgBase
{
public:
    NewDockerWorkspaceDlg(wxWindow* parent);
    virtual ~NewDockerWorkspaceDlg();
    wxFileName GetWorkspaceFile() const;

protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
};
#endif // NEWDOCKERWORKSPACEDLG_H
