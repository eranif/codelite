#ifndef CLDOCKERWORKSPACEVIEW_H
#define CLDOCKERWORKSPACEVIEW_H

#include "clTreeCtrlPanel.h"
#include "cl_command_event.h"
#include "cl_config.h"
#include <wx/menu.h>

class clDockerWorkspaceView : public clTreeCtrlPanel
{
    clConfig m_config;

public:
    clDockerWorkspaceView(wxWindow* parent);
    ~clDockerWorkspaceView();

protected:
    void DoDockerfileContextMenu(wxMenu* menu, const wxString& dockerfile);
    void DoDockerComposeContextMenu(wxMenu* menu, const wxString& docker_compose);

protected:
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnWorkspaceOpened(wxCommandEvent& event);
    void OnFileContextMenu(clContextMenuEvent& event);
    void OnFindInFilesDismissed(clFindInFilesEvent& event);
    void OnFindInFilesShowing(clFindInFilesEvent& event); // override parent's method
};

#endif // CLDOCKERWORKSPACEVIEW_H
