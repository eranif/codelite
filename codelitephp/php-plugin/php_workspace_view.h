//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_workspace_view.h
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

#ifndef __php_workspace_view__
#define __php_workspace_view__

#include "clFileSystemEvent.h"
#include "clTreeKeyboardInput.h"
#include "php_event.h"
#include "php_ui.h"
#include "php_workspace.h"
#include "wxStringHash.h"
#include "wx_ordered_map.h"
#include <bitmap_loader.h>
#include <cl_command_event.h>
#include <set>

class IManager;
class ItemData;

class PHPWorkspaceView : public PHPWorkspaceViewBase
{
    IManager* m_mgr;
    BitmapLoader::BitmapMap_t m_bitmaps;
    wxOrderedMap<wxTreeItemId, bool> m_itemsToSort;
    std::unordered_map<wxString, wxTreeItemId> m_filesItems;
    std::unordered_map<wxString, wxTreeItemId> m_foldersItems;
    clTreeKeyboardInput::Ptr_t m_keyboardHelper;
    bool m_scanInProgress;
    std::unordered_set<wxString> m_pendingSync;
    wxArrayString m_draggedFiles;
    
private:
    enum {
        ID_TOGGLE_AUTOMATIC_UPLOAD = wxID_HIGHEST + 1,
    };

protected:
    virtual void OnCollapse(wxCommandEvent& event);
    virtual void DoCollapseItem(wxTreeItemId& item);
    void OnFolderDropped(clCommandEvent& event);

    virtual void OnCollapseUI(wxUpdateUIEvent& event);
    virtual void OnStartDebuggerListenerUI(wxUpdateUIEvent& event);
    virtual void OnSetupRemoteUploadUI(wxUpdateUIEvent& event);
    virtual void OnItemActivated(wxTreeEvent& event);
    virtual void OnMenu(wxTreeEvent& event);
    void OnWorkspaceLoaded(PHPEvent& event);
#if USE_SFTP
    virtual void OnSetupRemoteUpload(wxAuiToolBarEvent& event);
#endif
    virtual void OnWorkspaceOpenUI(wxUpdateUIEvent& event);
    virtual void OnActiveProjectSettings(wxCommandEvent& event);
    virtual void OnProjectSettings(wxCommandEvent& event);
    virtual void OnActiveProjectSettingsUI(wxUpdateUIEvent& event);
    void DoGetSelectedFiles(wxArrayString& files);
    void DoGetSelectedFolders(wxArrayString& paths);
    void DoSetStatusBarText(const wxString& text, int timeOut);

    // Helpers
    void DoSortItems();
    wxTreeItemId DoAddFolder(const wxString& project, const wxString& path);
    wxTreeItemId DoCreateFile(const wxTreeItemId& parent, const wxString& fullpath, const wxString& content = "");
    wxTreeItemId DoGetProject(const wxString& project);
    PHPProject::Ptr_t DoGetProjectForItem(const wxTreeItemId& item);
    wxTreeItemId DoGetSingleSelection();
    void DoGetSelectedItems(wxArrayTreeItemIds& items);

    wxString DoGetSelectedProject();
    ItemData* DoGetItemData(const wxTreeItemId& item);
    const ItemData* DoGetItemData(const wxTreeItemId& item) const;
    bool IsFolderItem(const wxTreeItemId& item);
    int DoGetItemImgIdx(const wxString& filename);
    wxBitmap DoGetBitmapForExt(const wxString& ext) const;
    void DoDeleteSelectedFileItem();
#if USE_SFTP
    void DoOpenSSHAccountManager();
#endif

    /**
     * @brief construct the project in the tree view
     * @param projectItem
     * @param project
     */
    void DoBuildProjectNode(const wxTreeItemId& projectItem, PHPProject::Ptr_t project);

    /**
     * @brief return list of files and folders for a given project
     */
    void DoGetFilesAndFolders(const wxString& projectName, wxArrayString& folders, wxArrayString& files);
    void DoGetFilesAndFolders(const wxTreeItemId& parent, wxArrayString& folders, wxArrayString& files);

    /**
     * @brief open an item into an editor
     */
    void DoOpenFile(const wxTreeItemId& item);

    void DoSetProjectActive(const wxString& projectName);

    /**
     * @brief expand the tree view to highlight the active editor
     */
    void DoExpandToActiveEditor();

protected:
    // Handlers for PHPWorkspaceViewBase events.

    // Menu handlers
    DECLARE_EVENT_TABLE()

    void OnCloseWorkspace(wxCommandEvent& e);
    void OnReloadWorkspace(wxCommandEvent& e);
    void OnNewFolder(wxCommandEvent& e);
    void OnRenameFolder(wxCommandEvent& e);
    void OnNewClass(wxCommandEvent& e);
    void OnDeleteProject(wxCommandEvent& e);
    void OnSetProjectActive(wxCommandEvent& e);
    void OnNewFile(wxCommandEvent& e);
    void OnDeleteFolder(wxCommandEvent& e);
    void OnOpenInExplorer(wxCommandEvent& e);
    void OnOpenShell(wxCommandEvent& e);
    void OnRetagWorkspace(wxCommandEvent& e);
    void OnAddExistingProject(wxCommandEvent& e);
    void OnFindInFiles(wxCommandEvent& e);
    void OnRemoveFile(wxCommandEvent& e);
    void OnOpenFile(wxCommandEvent& e);
    void OnRenameFile(wxCommandEvent& e);
    void OnRenameWorkspace(wxCommandEvent& e);
    void OnRunProject(wxCommandEvent& e);
    void OnMakeIndexPHP(wxCommandEvent& e);
    void OnSyncWorkspaceWithFileSystem(wxCommandEvent& e);
    void OnSyncWorkspaceWithFileSystemUI(wxUpdateUIEvent& e);
    void OnSyncProjectWithFileSystem(wxCommandEvent& e);
    void OnSyncFolderWithFileSystem(wxCommandEvent& e);
    void OnOpenWithDefaultApp(wxCommandEvent& e);
    void OnRunActiveProject(clExecuteEvent& e);
    void OnStopExecutedProgram(clExecuteEvent& e);
    void OnIsProgramRunning(clExecuteEvent& e);
    void OnEditorChanged(wxCommandEvent& e);
    void OnFileRenamed(PHPEvent& e);
    void OnWorkspaceRenamed(PHPEvent& e);
    void OnFindInFilesShowing(clCommandEvent& e);
    void OnToggleAutoUpload(wxCommandEvent& e);
    void OnStartDebuggerListener(wxCommandEvent& e);
    void OnProjectSyncCompleted(clCommandEvent& event);

    // Php parser events
    void OnPhpParserStarted(clParseEvent& event);
    void OnPhpParserProgress(clParseEvent& event);
    void OnPhpParserDone(clParseEvent& event);

    // PHP Workspace events
    void OnWorkspaceSyncStart(clCommandEvent& event);
    void OnWorkspaceSyncEnd(clCommandEvent& event);
    void OnFileSaveAs(clFileSystemEvent& event);
    
    // DnD
    void OnDragBegin(wxTreeEvent& event);
    void OnDragEnd(wxTreeEvent& event);
    
public:
    /** Constructor */
    PHPWorkspaceView(wxWindow* parent, IManager* mgr);
    virtual ~PHPWorkspaceView();

    /**
     * @brief create a new project
     * @param name
     */
    void CreateNewProject(PHPProject::CreateData cd);

    void LoadWorkspaceView();
    void UnLoadWorkspaceView();

    void ReportParseThreadProgress(size_t curIndex, size_t total);
    void ReportParseThreadDone();
    void ReloadWorkspace(bool saveBeforeReload);
};

#endif // __php_workspace_view__
