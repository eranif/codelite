#ifndef NODEJSWORKSPACEVIEW_H
#define NODEJSWORKSPACEVIEW_H

#include "clTreeCtrlPanel.h"
#include "cl_command_event.h"

class NodeJSWorkspaceView : public clTreeCtrlPanel
{
protected:
    void OnContextMenu(clContextMenuEvent& event);
    void OnFolderDropped(clCommandEvent& event);
    void OnShowHiddenFiles(wxCommandEvent& event);
    void OnCloseWorkspace(wxCommandEvent& event);

public:
    NodeJSWorkspaceView(wxWindow* parent, const wxString& viewName);
    virtual ~NodeJSWorkspaceView();

    void RebuildTree();
    void ShowHiddenFiles(bool show);
};

#endif // NODEJSWORKSPACEVIEW_H
