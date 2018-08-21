#ifndef CLDOCKERWORKSPACEVIEW_H
#define CLDOCKERWORKSPACEVIEW_H

#include "clTreeCtrlPanel.h"

class clDockerWorkspaceView : public clTreeCtrlPanel
{
public:
    clDockerWorkspaceView(wxWindow* parent);
    ~clDockerWorkspaceView();

protected:
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnWorkspaceOpened(wxCommandEvent& event);
};

#endif // CLDOCKERWORKSPACEVIEW_H
