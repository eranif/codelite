//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : workspacetab.h
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
#ifndef __workspacetab__
#define __workspacetab__

#include "cl_command_event.h"
#include "wxcrafter.h"
#include <wx/panel.h>

class clWorkspaceView;
class ProjectSettingsDlg;
class WorkspaceTab : public WorkspaceTabBase
{
    friend class ProjectSettingsDlg;

    wxString m_caption;
    bool m_isLinkedToEditor;
    ProjectSettingsDlg* m_dlg;
    clWorkspaceView* m_view;
    wxColour m_bgColour;
    wxArrayString m_cxxPinnedProjects;
    clTreeCtrl::BitmapVec_t m_bitmaps;
    bool m_buildInProgress = false;
    bool m_runInProgress = false;

protected:
    virtual void OnPinnedCxxProjectSelected(wxDataViewEvent& event);
    virtual void OnPinnedCxxProjectContextMenu(wxDataViewEvent& event);
    void ProjectSettingsDlgClosed();
    void DoGoHome();
    void LoadCxxPinnedProjects();
    void SaveCxxPinnedProjects();
    /**
     * @brief sync the pinned projects view with the file view
     */
    void SyncPinnedProjectsView(const wxDataViewItem& item);
    void ShowPinnedProjectMenu(const wxString& project);

protected:
    virtual void OnWorkspaceOpenUI(wxUpdateUIEvent& event);
    virtual void OnLinkEditorUI(wxUpdateUIEvent& event);

    void OnFolderDropped(clCommandEvent& event);
    void CreateGUIControls();
    void ConnectEvents();

    void OnLinkEditor(wxCommandEvent& e);
    void OnCollapseAll(wxCommandEvent& e);
    void OnCollapseAllUI(wxUpdateUIEvent& e);
    void OnGoHome(wxCommandEvent& e);
    void OnGoHomeUI(wxUpdateUIEvent& e);
    void OnProjectSettingsUI(wxUpdateUIEvent& e);
    void OnProjectSettings(wxCommandEvent& e);
    void OnShowFile(wxCommandEvent& e);
    void OnShowFileUI(wxUpdateUIEvent& e);

    void OnWorkspaceLoaded(clWorkspaceEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& e);
    void OnProjectAdded(clCommandEvent& e);
    void OnProjectRemoved(clCommandEvent& e);
    void OnActiveEditorChanged(wxCommandEvent& e);
    void OnEditorClosing(wxCommandEvent& e);

    void OnBuildStarted(clBuildEvent& event);
    void OnBuildEnded(clBuildEvent& event);
    void OnProgramStarted(clExecuteEvent& event);
    void OnProgramStopped(clExecuteEvent& event);
    void OnBuildActiveProject(wxCommandEvent& event);
    void OnExecuteNoDebug(wxCommandEvent& event);
    void OnBuildActiveProjectDropdown(wxCommandEvent& event);

public:
    WorkspaceTab(wxWindow* parent, const wxString& caption);
    ~WorkspaceTab();

    void OpenProjectSettings(const wxString& project = "");

    FileViewTree* GetFileView() { return m_fileView; }
    const wxString& GetCaption() const { return m_caption; }
    void FreezeThaw(bool freeze = true);

    /**
     * @brief return the workspace view class
     * @return
     */
    clWorkspaceView* GetView() { return m_view; }
    /**
     * @brief
     * @param project
     */
    void AddPinnedProject(const wxString& project);
};
#endif // __workspacetab__
