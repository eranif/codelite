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

#include <wx/panel.h>
#include "theme_handler_helper.h"
#include "wxcrafter.h"
#include "cl_command_event.h"

class clWorkspaceView;
class ProjectSettingsDlg;
class WorkspaceTab : public WorkspaceTabBase
{
    friend class ProjectSettingsDlg;

    wxString m_caption;
    bool m_isLinkedToEditor;
    ThemeHandlerHelper* m_themeHelper;
    ProjectSettingsDlg* m_dlg;
    clWorkspaceView* m_view;
    wxColour m_bgColour;
    wxArrayString m_cxxPinnedProjects;
    clTreeCtrl::BitmapVec_t m_bitmaps;

protected:
    virtual void OnPinnedCxxProjectSelected(wxDataViewEvent& event);
    virtual void OnPinnedCxxProjectContextMenu(wxDataViewEvent& event);
    void ProjectSettingsDlgClosed();
    void DoGoHome();
    void DoConfigChanged(const wxString& newConfigName);
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
    void OnPaint(wxPaintEvent& event);
    void CreateGUIControls();
    void ConnectEvents();
    void DoWorkspaceConfig();
    void DoUpdateChoiceWithProjects();

    void OnLinkEditor(wxCommandEvent& e);
    void OnCollapseAll(wxCommandEvent& e);
    void OnCollapseAllUI(wxUpdateUIEvent& e);
    void OnGoHome(wxCommandEvent& e);
    void OnGoHomeUI(wxUpdateUIEvent& e);
    void OnProjectSettingsUI(wxUpdateUIEvent& e);
    void OnProjectSettings(wxCommandEvent& e);
    void OnShowFile(wxCommandEvent& e);
    void OnShowFileUI(wxUpdateUIEvent& e);

    void OnWorkspaceLoaded(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnProjectAdded(clCommandEvent& e);
    void OnProjectRemoved(clCommandEvent& e);
    void OnActiveEditorChanged(wxCommandEvent& e);
    void OnEditorClosing(wxCommandEvent& e);
    void OnWorkspaceConfig(wxCommandEvent& e);
    void OnConfigurationManager(wxCommandEvent& e);
    void OnConfigChanged(clCommandEvent& e);
    void OnActiveProjectChanged(clProjectSettingsEvent& e);

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
