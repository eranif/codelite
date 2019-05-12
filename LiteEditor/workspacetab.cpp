//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : workspacetab.cpp
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
#include "DefaultWorkspacePage.h"
#include "clFileOrFolderDropTarget.h"
#include "clTreeCtrlPanel.h"
#include "clWorkspaceView.h"
#include "configuration_manager_dlg.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "fileview.h"
#include "frame.h"
#include "globals.h"
#include "macros.h"
#include "manager.h"
#include "plugin.h"
#include "pluginmanager.h"
#include "project_settings_dlg.h"
#include "workspace_pane.h"
#include "workspacetab.h"
#include <algorithm>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/xrc/xmlres.h>
#include <wx/dcbuffer.h>
#include "drawingutils.h"
#include <wx/dcbuffer.h>

static bool SortPinnedProjects(clRowEntry* a, clRowEntry* b)
{
    wxString a_label = a->GetLabel(0);
    wxString b_label = b->GetLabel(0);
    return a_label.CmpNoCase(b_label) < 0;
}

WorkspaceTab::WorkspaceTab(wxWindow* parent, const wxString& caption)
    : WorkspaceTabBase(parent)
    , m_caption(caption)
    , m_isLinkedToEditor(true)
    , m_dlg(NULL)
    , m_view(NULL)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_panelCxx->SetBackgroundStyle(wxBG_STYLE_PAINT);

    long link = EditorConfigST::Get()->GetInteger(wxT("LinkWorkspaceViewToEditor"), 1);
    m_isLinkedToEditor = link ? true : false;

    CreateGUIControls();
    ConnectEvents();
    m_themeHelper = NULL; // new ThemeHandlerHelper(this);
    SetDropTarget(new clFileOrFolderDropTarget(this));
    Bind(wxEVT_DND_FOLDER_DROPPED, &WorkspaceTab::OnFolderDropped, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_BUILD_CONFIG_CHANGED, &WorkspaceTab::OnConfigChanged, this);

    m_panelCxx->Bind(wxEVT_PAINT, [&](wxPaintEvent& e) {
        wxAutoBufferedPaintDC dc(m_panelCxx);
        dc.SetBrush(m_bgColour);
        dc.SetPen(m_bgColour);
        dc.DrawRectangle(m_panelCxx->GetClientRect());
    });

    m_bgColour = DrawingUtils::GetPanelBgColour();
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, [&](clCommandEvent& event) {
        event.Skip();
        bool useCustomColour = clConfig::Get().Read("UseCustomBaseColour", false);
        m_bgColour = DrawingUtils::GetPanelBgColour();
        if(useCustomColour) { m_bgColour = clConfig::Get().Read("BaseColour", m_bgColour); }
        Refresh();
        m_panelCxx->Refresh();
    });
    m_bitmaps.push_back(clGetManager()->GetStdIcons()->LoadBitmap("project"));
    m_dvListCtrlPinnedProjects->SetBitmaps(&m_bitmaps);
}

WorkspaceTab::~WorkspaceTab()
{
    wxDELETE(m_themeHelper);
    wxTheApp->Disconnect(XRCID("show_in_workspace"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(WorkspaceTab::OnShowFile), NULL, this);
    wxTheApp->Disconnect(XRCID("show_in_workspace"), wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(WorkspaceTab::OnShowFileUI), NULL, this);

    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(WorkspaceTab::OnWorkspaceLoaded),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(WorkspaceTab::OnWorkspaceClosed),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_ADDED, clCommandEventHandler(WorkspaceTab::OnProjectAdded), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_REMOVED, clCommandEventHandler(WorkspaceTab::OnProjectRemoved), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                     wxCommandEventHandler(WorkspaceTab::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(WorkspaceTab::OnEditorClosing), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CONFIG_CHANGED,
                                     wxCommandEventHandler(WorkspaceTab::OnWorkspaceConfig), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_PROJECT_CHANGED,
                                     clProjectSettingsEventHandler(WorkspaceTab::OnActiveProjectChanged), NULL, this);

    wxTheApp->Disconnect(XRCID("configuration_manager"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(WorkspaceTab::OnConfigurationManager), NULL, this);
    wxTheApp->Disconnect(XRCID("configuration_manager"), wxEVT_UPDATE_UI,
                         wxUpdateUIEventHandler(WorkspaceTab::OnProjectSettingsUI), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_BUILD_CONFIG_CHANGED, &WorkspaceTab::OnConfigChanged, this);
    m_toolbar580->Unbind(wxEVT_TOOL, &WorkspaceTab::OnCollapseAll, this, XRCID("ID_TOOL_COLLAPSE_ALL"));
    m_toolbar580->Unbind(wxEVT_UPDATE_UI, &WorkspaceTab::OnCollapseAllUI, this, XRCID("ID_TOOL_COLLAPSE_ALL"));
    m_toolbar580->Unbind(wxEVT_TOOL, &WorkspaceTab::OnLinkEditor, this, XRCID("ID_TOOL_LINK_EDITOR"));
    m_toolbar580->Unbind(wxEVT_UPDATE_UI, &WorkspaceTab::OnLinkEditorUI, this, XRCID("ID_TOOL_LINK_EDITOR"));
    m_toolbar580->Unbind(wxEVT_TOOL, &WorkspaceTab::OnProjectSettings, this, XRCID("ID_TOOL_ACTIVE_PROJECT_SETTINGS"));
    m_toolbar580->Unbind(wxEVT_UPDATE_UI, &WorkspaceTab::OnProjectSettingsUI, this,
                         XRCID("ID_TOOL_ACTIVE_PROJECT_SETTINGS"));
    m_toolbar580->Unbind(wxEVT_TOOL, &WorkspaceTab::OnGoHome, this, XRCID("ID_TOOL_GOTO_ACTIVE_PROJECT"));
    m_toolbar580->Unbind(wxEVT_UPDATE_UI, &WorkspaceTab::OnGoHomeUI, this, XRCID("ID_TOOL_GOTO_ACTIVE_PROJECT"));
}

void WorkspaceTab::CreateGUIControls()
{
    int index = m_simpleBook->FindPage(m_panelCxx);
    if(index != wxNOT_FOUND) {
        // set the C++ workspace view name to fit its workspace type
        //// Construct the tree
        m_simpleBook->SetPageText(index, clCxxWorkspaceST::Get()->GetWorkspaceType());
    }
    m_view = new clWorkspaceView(m_simpleBook);
    m_view->AddPage(new DefaultWorkspacePage(m_simpleBook), _("Default"));
    m_view->SelectPage(_("Default"));
    m_view->SetDefaultPage(_("Default"));
    BitmapLoader* bmps = clGetManager()->GetStdIcons();
    m_toolbar580->AddTool(XRCID("ID_TOOL_LINK_EDITOR"), _("Link Editor"), bmps->LoadBitmap("link_editor"), "",
                          wxITEM_CHECK);
    m_toolbar580->AddTool(XRCID("ID_TOOL_COLLAPSE_ALL"), _("Collapse All"), bmps->LoadBitmap("fold"));
    m_toolbar580->AddTool(XRCID("ID_TOOL_GOTO_ACTIVE_PROJECT"), _("Goto Active Project"), bmps->LoadBitmap("home"));
    m_toolbar580->AddTool(XRCID("ID_TOOL_ACTIVE_PROJECT_SETTINGS"),
                          _("Open selected project settings. If there is no project selected, open the parent project "
                            "of the selected item in the tree"),
                          bmps->LoadBitmap("cog"));
    m_toolbar580->AddSpacer();
    m_toolbar580->AddTool(XRCID("execute_no_debug"), _("Run Active Project"), bmps->LoadBitmap("execute"),
                          _("Run Active Project"));
    m_toolbar580->AddTool(XRCID("build_active_project"), _("Build Active Project"), bmps->LoadBitmap("build"),
                          _("Build Active Project"), wxITEM_DROPDOWN);
    m_toolbar580->AddTool(XRCID("stop_active_project_build"), _("Stop Current Build"), bmps->LoadBitmap("stop"),
                          _("Stop Current Build"));
    m_toolbar580->Realize();
}

void WorkspaceTab::FreezeThaw(bool freeze /*=true*/)
{
    // If the selected file is linked to which editor is displayed, the selection changes rapidly when a workspace is
    // loaded
    // Prevent this by temporarily unlinking, then relink after
    static bool frozen = false;
    static bool was_linked;

    wxCHECK_RET(!(freeze && frozen), wxT("Trying to re-freeze a frozen workspace tab"));
    wxCHECK_RET(!(!freeze && !frozen), wxT("Trying to thaw a warm workspace tab"));
    frozen = freeze;

    if(freeze) {
        was_linked = m_isLinkedToEditor;
        m_isLinkedToEditor = false;
    } else {
        m_isLinkedToEditor = was_linked;
        // I expected we'd need to call OnActiveEditorChanged() here, but it doesn't seem necessary (atm)
    }
}

void WorkspaceTab::ConnectEvents()
{
    wxTheApp->Connect(XRCID("show_in_workspace"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(WorkspaceTab::OnShowFile), NULL, this);
    wxTheApp->Connect(XRCID("show_in_workspace"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(WorkspaceTab::OnShowFileUI),
                      NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(WorkspaceTab::OnWorkspaceLoaded), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(WorkspaceTab::OnWorkspaceClosed), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_ADDED, clCommandEventHandler(WorkspaceTab::OnProjectAdded), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_REMOVED, clCommandEventHandler(WorkspaceTab::OnProjectRemoved), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                  wxCommandEventHandler(WorkspaceTab::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(WorkspaceTab::OnEditorClosing), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CONFIG_CHANGED,
                                  wxCommandEventHandler(WorkspaceTab::OnWorkspaceConfig), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_PROJECT_CHANGED,
                                  clProjectSettingsEventHandler(WorkspaceTab::OnActiveProjectChanged), NULL, this);
    wxTheApp->Connect(XRCID("configuration_manager"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(WorkspaceTab::OnConfigurationManager), NULL, this);
    wxTheApp->Connect(XRCID("configuration_manager"), wxEVT_UPDATE_UI,
                      wxUpdateUIEventHandler(WorkspaceTab::OnProjectSettingsUI), NULL, this);
    m_toolbar580->Bind(wxEVT_TOOL, &WorkspaceTab::OnCollapseAll, this, XRCID("ID_TOOL_COLLAPSE_ALL"));
    m_toolbar580->Bind(wxEVT_UPDATE_UI, &WorkspaceTab::OnCollapseAllUI, this, XRCID("ID_TOOL_COLLAPSE_ALL"));
    m_toolbar580->Bind(wxEVT_TOOL, &WorkspaceTab::OnLinkEditor, this, XRCID("ID_TOOL_LINK_EDITOR"));
    m_toolbar580->Bind(wxEVT_UPDATE_UI, &WorkspaceTab::OnLinkEditorUI, this, XRCID("ID_TOOL_LINK_EDITOR"));
    m_toolbar580->Bind(wxEVT_TOOL, &WorkspaceTab::OnProjectSettings, this, XRCID("ID_TOOL_ACTIVE_PROJECT_SETTINGS"));
    m_toolbar580->Bind(wxEVT_UPDATE_UI, &WorkspaceTab::OnProjectSettingsUI, this,
                       XRCID("ID_TOOL_ACTIVE_PROJECT_SETTINGS"));
    m_toolbar580->Bind(wxEVT_TOOL, &WorkspaceTab::OnGoHome, this, XRCID("ID_TOOL_GOTO_ACTIVE_PROJECT"));
    m_toolbar580->Bind(wxEVT_UPDATE_UI, &WorkspaceTab::OnGoHomeUI, this, XRCID("ID_TOOL_GOTO_ACTIVE_PROJECT"));
}

void WorkspaceTab::OnLinkEditor(wxCommandEvent& e)
{
    m_isLinkedToEditor = e.IsChecked();
    EditorConfigST::Get()->SetInteger(wxT("LinkWorkspaceViewToEditor"), m_isLinkedToEditor ? 1 : 0);
    if(m_isLinkedToEditor) { OnActiveEditorChanged(e); }
}

void WorkspaceTab::OnCollapseAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(!m_fileView->GetRootItem().IsOk()) return;
    m_fileView->CollapseAll();

    // count will probably be 0 below, so ensure we can at least see the root item
    m_fileView->EnsureVisible(m_fileView->GetRootItem());
    m_fileView->SelectItem(m_fileView->GetRootItem());
    // Expand the workspace
    m_fileView->Expand(m_fileView->GetRootItem());

    wxArrayTreeItemIds arr;
    size_t count = m_fileView->GetSelections(arr);
    if(count == 1) {
        wxTreeItemId sel = arr.Item(0);
        if(sel.IsOk()) { m_fileView->EnsureVisible(sel); }
    }
}

void WorkspaceTab::OnCollapseAllUI(wxUpdateUIEvent& e) { e.Enable(ManagerST::Get()->IsWorkspaceOpen()); }

void WorkspaceTab::OnGoHome(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxString activeProject = ManagerST::Get()->GetActiveProjectName();
    if(activeProject.IsEmpty()) return;
    m_fileView->ExpandToPath(activeProject, wxFileName());

    wxArrayTreeItemIds arr;
    size_t count = m_fileView->GetSelections(arr);

    if(count == 1) {
        wxTreeItemId sel = arr.Item(0);
        if(sel.IsOk() && m_fileView->ItemHasChildren(sel)) m_fileView->Expand(sel);
    }
    ManagerST::Get()->ShowWorkspacePane(m_caption);
}

void WorkspaceTab::OnGoHomeUI(wxUpdateUIEvent& e) { e.Enable(!ManagerST::Get()->GetActiveProjectName().IsEmpty()); }

void WorkspaceTab::OnProjectSettings(wxCommandEvent& e)
{
    wxUnusedVar(e);
    ProjectPtr proj = m_fileView->GetSelectedProject();
    OpenProjectSettings(proj ? proj->GetName() : "");
}

void WorkspaceTab::OnProjectSettingsUI(wxUpdateUIEvent& e)
{
    e.Enable(!ManagerST::Get()->GetActiveProjectName().IsEmpty());
}

void WorkspaceTab::OnShowFile(wxCommandEvent& e)
{
    clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if(editor && !editor->GetProject().IsEmpty()) {
        m_fileView->ExpandToPath(editor->GetProject(), editor->GetFileName());
        ManagerST::Get()->ShowWorkspacePane(m_caption);
    }
    e.Skip();
}

void WorkspaceTab::OnShowFileUI(wxUpdateUIEvent& e)
{
    clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    e.Enable(editor && !editor->GetProject().IsEmpty());
}

void WorkspaceTab::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    if(m_isLinkedToEditor) {
        MainBook* mainbook = clMainFrame::Get()->GetMainBook();
        clEditor* editor = mainbook->GetActiveEditor();
        if(editor && !editor->GetProject().IsEmpty()) {
            m_fileView->ExpandToPath(editor->GetProject(), editor->GetFileName());
        }

        Notebook* book = clMainFrame::Get()->GetWorkspacePane()->GetNotebook();
        if(book) {
            size_t index = book->GetPageIndex("wxCrafter");
            if(index == (size_t)book->GetSelection()) {
                book->SetSelection(0); // The most likely to be wanted
            }
        }
    }
}

void WorkspaceTab::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    if(ManagerST::Get()->IsWorkspaceOpen()) {
        m_configChangeCtrl->Enable(true);
        DoWorkspaceConfig();
        DoUpdateChoiceWithProjects();

        // Tree construction
        m_fileView->BuildTree();
        CallAfter(&WorkspaceTab::DoGoHome);
        SendCmdEvent(wxEVT_FILE_VIEW_INIT_DONE);

        // Load the C++ pinned projects list
        LoadCxxPinnedProjects();
    }
}

void WorkspaceTab::OnEditorClosing(wxCommandEvent& e) { e.Skip(); }

void WorkspaceTab::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    m_configChangeCtrl->Clear();
    m_configChangeCtrl->Enable(false);
    m_fileView->DeleteAllItems();

    // Clear the pinned projects view
    m_dvListCtrlPinnedProjects->DeleteAllItems();
    SaveCxxPinnedProjects();
    m_cxxPinnedProjects.clear();

    SendCmdEvent(wxEVT_FILE_VIEW_INIT_DONE);
}

void WorkspaceTab::OnProjectAdded(clCommandEvent& e)
{
    e.Skip();
    const wxString& projName = e.GetString();
    m_fileView->BuildTree();
    if(!projName.IsEmpty()) { m_fileView->ExpandToPath(projName, wxFileName()); }
    DoUpdateChoiceWithProjects();
    SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
}

void WorkspaceTab::OnProjectRemoved(clCommandEvent& e)
{
    e.Skip();
    m_fileView->BuildTree();
    CallAfter(&WorkspaceTab::DoGoHome);
    DoUpdateChoiceWithProjects();
    SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
}

struct wxStringSorter {
    bool operator()(WorkspaceConfigurationPtr one, WorkspaceConfigurationPtr two) const
    {
        return one->GetName().Lower().CmpNoCase(two->GetName().Lower()) < 0;
    }
};

void WorkspaceTab::DoWorkspaceConfig()
{
    // Update the workspace configuration
    BuildMatrixPtr matrix = clCxxWorkspaceST::Get()->GetBuildMatrix();
    std::list<WorkspaceConfigurationPtr> confs = matrix->GetConfigurations();

    confs.sort(wxStringSorter());
    wxArrayString configurations;
    std::for_each(confs.begin(), confs.end(),
                  [&](WorkspaceConfigurationPtr conf) { configurations.push_back(conf->GetName()); });

    m_configChangeCtrl->SetConfigurations(configurations);
    wxString activeConfig = configurations.IsEmpty() ? "" : matrix->GetSelectedConfigurationName();
    m_configChangeCtrl->SetActiveConfiguration(activeConfig);

    clMainFrame::Get()->SelectBestEnvSet();
}

void WorkspaceTab::OnWorkspaceConfig(wxCommandEvent& e)
{
    e.Skip();
    DoWorkspaceConfig();
}

void WorkspaceTab::OnConfigurationManager(wxCommandEvent& e)
{
    wxUnusedVar(e);
    ConfigurationManagerDlg dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();

    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
    m_configChangeCtrl->SetActiveConfiguration(matrix->GetSelectedConfigurationName());
}

void WorkspaceTab::OnLinkEditorUI(wxUpdateUIEvent& event)
{
    event.Enable(ManagerST::Get()->IsWorkspaceOpen());
    event.Check(m_isLinkedToEditor);
}

void WorkspaceTab::DoUpdateChoiceWithProjects()
{
    if(clCxxWorkspaceST::Get()->IsOpen()) {
        m_configChangeCtrl->SetActiveProject(ManagerST::Get()->GetActiveProjectName());
    }
}

void WorkspaceTab::OnWorkspaceOpenUI(wxUpdateUIEvent& event) { event.Enable(ManagerST::Get()->IsWorkspaceOpen()); }

void WorkspaceTab::OpenProjectSettings(const wxString& project)
{
    if(m_dlg) {
        m_dlg->Raise();
        return;
    }

    wxString projectName = project.IsEmpty() ? ManagerST::Get()->GetActiveProjectName() : project;
    wxString title(projectName);
    title << _(" Project Settings");

    // Allow plugins to process this event first
    clCommandEvent openEvent(wxEVT_CMD_OPEN_PROJ_SETTINGS);
    openEvent.SetString(project);
    if(EventNotifier::Get()->ProcessEvent(openEvent)) { return; }

    // open the project properties dialog
    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();

#ifdef __WXMAC__
    // On OSX we use a modal version of the project settings
    // since otherwise we get some weird focus issues when the project
    // settings dialog popups helper dialogs
    ProjectSettingsDlg dlg(clMainFrame::Get(), this,
                           matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), projectName),
                           projectName, title);
    dlg.ShowModal();

#else
    // Find the project configuration name that matches the workspace selected configuration
    m_dlg = new ProjectSettingsDlg(clMainFrame::Get(), this,
                                   matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), projectName),
                                   projectName, title);
    m_dlg->Show();

#endif

    // Mark this project as modified
    ProjectPtr proj = ManagerST::Get()->GetProject(projectName);
    if(proj) { proj->SetModified(true); }
}

void WorkspaceTab::ProjectSettingsDlgClosed() { m_dlg = NULL; }

void WorkspaceTab::OnActiveProjectChanged(clProjectSettingsEvent& e)
{
    e.Skip();

    // Update the choice control
    m_configChangeCtrl->SetActiveProject(e.GetProjectName());
}

void WorkspaceTab::DoGoHome()
{
    wxString activeProject = ManagerST::Get()->GetActiveProjectName();
    if(activeProject.IsEmpty()) return;
    m_fileView->ExpandToPath(activeProject, wxFileName());

    wxArrayTreeItemIds arr;
    size_t count = m_fileView->GetSelections(arr);

    if(count == 1) {
        wxTreeItemId sel = arr.Item(0);
        if(sel.IsOk() && m_fileView->ItemHasChildren(sel)) m_fileView->Expand(sel);
    }
    // ManagerST::Get()->ShowWorkspacePane(m_caption);
}

void WorkspaceTab::DoConfigChanged(const wxString& newConfigName)
{
    wxBusyCursor bc;
    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
    matrix->SetSelectedConfigurationName(newConfigName);
    ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);

    // Set the focus to the active editor if any
    clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if(editor) { editor->SetActive(); }

    ManagerST::Get()->UpdateParserPaths(true);
}

void WorkspaceTab::OnFolderDropped(clCommandEvent& event)
{
    // pass it on to the tree view
    m_fileView->CallAfter(&FileViewTree::FolderDropped, event.GetStrings());
}

void WorkspaceTab::OnConfigChanged(clCommandEvent& e)
{
    e.Skip();
    wxString selection = e.GetString();
    if(selection == OPEN_CONFIG_MGR_STR) {
        e.Skip(false);
        wxCommandEvent e(wxEVT_MENU, XRCID("configuration_manager"));
        e.SetEventObject(this);
        ProcessEvent(e);
        return;
    }
    DoConfigChanged(selection);
}

void WorkspaceTab::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    dc.SetBrush(m_bgColour);
    dc.SetPen(m_bgColour);
    dc.DrawRectangle(GetClientRect());
}

void WorkspaceTab::LoadCxxPinnedProjects()
{
    m_cxxPinnedProjects.clear();
    clCxxWorkspaceST::Get()->GetLocalWorkspace()->GetPinnedProjects(m_cxxPinnedProjects);

    // We got the pinned projects loaded, update the view
    if(m_cxxPinnedProjects.empty()) {
        // hide the to view
        if(m_splitter->IsSplit()) { m_splitter->Unsplit(m_splitterPagePinnedProjects); }
    } else {
        // ensure the view is visible
        if(!m_splitter->IsSplit()) {
            m_splitter->SplitHorizontally(m_splitterPagePinnedProjects, m_splitterPageTreeView, 150);
            m_splitter->CallAfter(&wxSplitterWindow::UpdateSize);
        }
        
        m_dvListCtrlPinnedProjects->DeleteAllItems();
        m_dvListCtrlPinnedProjects->SetSortFunction(nullptr);
        for(const wxString& project : m_cxxPinnedProjects) {
            wxVector<wxVariant> V;
            V.push_back(::MakeBitmapIndexText(project, 0));
            m_dvListCtrlPinnedProjects->AppendItem(V);
        }
        // apply the sort
        m_dvListCtrlPinnedProjects->SetSortFunction(SortPinnedProjects);
    }
}

void WorkspaceTab::SaveCxxPinnedProjects()
{
    clCxxWorkspaceST::Get()->GetLocalWorkspace()->SetPinnedProjects(m_cxxPinnedProjects);
}

void WorkspaceTab::OnPinnedCxxProjectContextMenu(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();
    if(!item.IsOk()) { return; }
    SyncPinnedProjectsView(event.GetItem());

    wxString project = m_dvListCtrlPinnedProjects->GetItemText(item);
    ShowPinnedProjectMenu(project);
}

void WorkspaceTab::OnPinnedCxxProjectSelected(wxDataViewEvent& event)
{
    // we MUST sync the views, otherwise, the context menu wont work
    // since it reliese on the current selection in the tree
    SyncPinnedProjectsView(event.GetItem());
}

void WorkspaceTab::AddPinnedProject(const wxString& project)
{
    if(m_cxxPinnedProjects.Index(project) != wxNOT_FOUND) { return; }
    m_cxxPinnedProjects.Add(project);

    // Store the pinned projehcts
    SaveCxxPinnedProjects();

    // Refresh the view
    LoadCxxPinnedProjects();
}

void WorkspaceTab::SyncPinnedProjectsView(const wxDataViewItem& item)
{
    if(!item.IsOk()) { return; }

    wxString project = m_dvListCtrlPinnedProjects->GetItemText(item);
    m_fileView->ExpandToPath(project, wxFileName()); // Select the project
}

void WorkspaceTab::ShowPinnedProjectMenu(const wxString& project)
{
    wxMenu menu;
    menu.Append(XRCID("unpin_project"), _("Unpin Project"));
    menu.AppendSeparator();
    // Build the rest of the menu
    m_fileView->CreateProjectContextMenu(menu, project, false);
    menu.Bind(wxEVT_MENU, [&](wxCommandEvent& menuEvent) {
        if(menuEvent.GetId() == XRCID("unpin_project")) {
            wxDataViewItem item = m_dvListCtrlPinnedProjects->GetSelection();
            if(item.IsOk()) {
                m_dvListCtrlPinnedProjects->DeleteItem(m_dvListCtrlPinnedProjects->ItemToRow(item));
                int where = m_cxxPinnedProjects.Index(project);
                if(where != wxNOT_FOUND) {
                    m_cxxPinnedProjects.RemoveAt(where);
                    SaveCxxPinnedProjects();
                    CallAfter(&WorkspaceTab::LoadCxxPinnedProjects);
                }
            }
        } else {
            // Pass all the context menu events to the tree tree view
            m_fileView->GetEventHandler()->ProcessEvent(menuEvent);
        }
    });
    m_dvListCtrlPinnedProjects->PopupMenu(&menu);
}
