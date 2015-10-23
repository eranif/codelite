#ifndef NODEJSWORKSPACEVIEW_H
#define NODEJSWORKSPACEVIEW_H

#include "clTreeCtrlPanel.h"
#include "cl_command_event.h"
#include "NodeJSDebuggerDlg.h"

class NodeJSWorkspaceView : public clTreeCtrlPanel
{
protected:
    void OnContextMenu(clContextMenuEvent& event);
    void OnContextMenuFile(clContextMenuEvent& event);
    void OnFolderDropped(clCommandEvent& event);
    void OnShowHiddenFiles(wxCommandEvent& event);
    void OnCloseWorkspace(wxCommandEvent& event);

    void OnOpenPackageJsonFile(wxCommandEvent& event);
    void OnProjectDebug(wxCommandEvent& event);
    void OnProjectRun(wxCommandEvent& event);
    
    void DoExecuteProject(NodeJSDebuggerDlg::eDialogType type);
    
    /**
     * @brief return the selected project path
     * A "selected project" means the entry which is highlighted on the tree
     * view. If no project is selected, return an empty string
     */
    bool GetSelectProjectPath(wxString& path, wxTreeItemId& item);

public:
    NodeJSWorkspaceView(wxWindow* parent, const wxString& viewName);
    virtual ~NodeJSWorkspaceView();

    void RebuildTree();
    void ShowHiddenFiles(bool show);
};

#endif // NODEJSWORKSPACEVIEW_H
