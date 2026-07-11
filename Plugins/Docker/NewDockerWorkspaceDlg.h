#ifndef NEWDOCKERWORKSPACEDLG_H
#define NEWDOCKERWORKSPACEDLG_H
#include "UI.hpp"

#include <wx/filename.h>

class NewDockerWorkspaceDlg : public NewDockerWorkspaceDlgBase
{
public:
    NewDockerWorkspaceDlg(wxWindow* parent);
    virtual ~NewDockerWorkspaceDlg() = default;
    wxFileName GetWorkspaceFile() const;

protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
};
#endif // NEWDOCKERWORKSPACEDLG_H
