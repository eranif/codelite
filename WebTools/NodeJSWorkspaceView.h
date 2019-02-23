//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSWorkspaceView.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef NODEJSWORKSPACEVIEW_H
#define NODEJSWORKSPACEVIEW_H

#include "clTreeCtrlPanel.h"
#include "cl_command_event.h"
#include "NodeJSDebuggerDlg.h"
#include "TerminalEmulator.h"
#include "clFileSystemEvent.h"

class NodeJSWorkspaceView : public clTreeCtrlPanel
{
    clConfig m_config;

protected:
    void OnContextMenu(clContextMenuEvent& event);
    void OnContextMenuFile(clContextMenuEvent& event);
    void OnFolderDropped(clCommandEvent& event);
    void OnShowHiddenFiles(wxCommandEvent& event);
    void OnCloseWorkspace(wxCommandEvent& event);
    void OnFileSystemUpdated(clFileSystemEvent& event);
    void OnFindInFilesDismissed(clFindInFilesEvent& event);
    void OnFindInFilesShowing(clFindInFilesEvent& event);
    void OnOpenPackageJsonFile(wxCommandEvent& event);
    void OnProjectDebug(wxCommandEvent& event);
    void OnProjectRun(wxCommandEvent& event);
    void OnNpmInit(wxCommandEvent& event);
    void OnNpmInstall(wxCommandEvent& event);
    void OnTerminalClosed(clProcessEvent& event);
    void DoExecuteProject(NodeJSDebuggerDlg::eDialogType type);
    void OnItemExpanding(wxTreeEvent& event);
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
