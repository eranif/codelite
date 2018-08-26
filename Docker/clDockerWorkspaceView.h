#ifndef CLDOCKERWORKSPACEVIEW_H
#define CLDOCKERWORKSPACEVIEW_H

#include "clTreeCtrlPanel.h"
#include "cl_command_event.h"
#include "cl_config.h"

class clDockerWorkspaceView : public clTreeCtrlPanel
{
    clConfig m_config;
    
public:
    clDockerWorkspaceView(wxWindow* parent);
    ~clDockerWorkspaceView();

protected:
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnWorkspaceOpened(wxCommandEvent& event);
    void OnFileContextMenu(clContextMenuEvent& event);
};

#endif // CLDOCKERWORKSPACEVIEW_H
