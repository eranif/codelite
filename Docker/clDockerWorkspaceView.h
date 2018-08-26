#ifndef CLDOCKERWORKSPACEVIEW_H
#define CLDOCKERWORKSPACEVIEW_H

#include "clTreeCtrlPanel.h"
#include "cl_command_event.h"

class clDockerWorkspaceView : public clTreeCtrlPanel
{
public:
    clDockerWorkspaceView(wxWindow* parent);
    ~clDockerWorkspaceView();

protected:
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnWorkspaceOpened(wxCommandEvent& event);
    void OnFileContextMenu(clContextMenuEvent& event);
};

#endif // CLDOCKERWORKSPACEVIEW_H
