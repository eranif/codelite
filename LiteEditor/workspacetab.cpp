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
#include <wx/xrc/xmlres.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include "event_notifier.h"
#include <wx/button.h>
#include "pluginmanager.h"
#include "project_settings_dlg.h"
#include "globals.h"
#include "configuration_manager_dlg.h"
#include "manager.h"
#include "fileview.h"
#include "editor_config.h"
#include "frame.h"
#include "macros.h"
#include "workspace_pane.h"
#include "workspacetab.h"
#include "event_notifier.h"
#include "plugin.h"

#define OPEN_CONFIG_MGR_STR _("<Open Configuration Manager...>")

WorkspaceTab::WorkspaceTab(wxWindow *parent, const wxString &caption)
    : WorkspaceTabBase(parent)
    , m_caption(caption)
    , m_isLinkedToEditor(true)
    , m_dlg(NULL)
{
    long link(1);
    EditorConfigST::Get()->GetLongValue(wxT("LinkWorkspaceViewToEditor"), link);
    m_isLinkedToEditor = link ? true : false;

    CreateGUIControls();
    ConnectEvents();
    m_themeHelper = new ThemeHandlerHelper(this);
    int sashPos = clConfig::Get().Read("WorkspaceTabSashPosition", wxNOT_FOUND);
    if ( sashPos != wxNOT_FOUND ) {
        m_splitter->SetSashPosition( sashPos );
    }
}

WorkspaceTab::~WorkspaceTab()
{
    wxDELETE(m_themeHelper);
    wxTheApp->Disconnect(XRCID("show_in_workspace"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnShowFile),   NULL, this);
    wxTheApp->Disconnect(XRCID("show_in_workspace"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnShowFileUI), NULL, this);

    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED,         wxCommandEventHandler(WorkspaceTab::OnWorkspaceLoaded),     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED,         wxCommandEventHandler(WorkspaceTab::OnWorkspaceClosed),     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_ADDED,               wxCommandEventHandler(WorkspaceTab::OnProjectAdded),        NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_REMOVED,             wxCommandEventHandler(WorkspaceTab::OnProjectRemoved),      NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,    wxCommandEventHandler(WorkspaceTab::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_EDITOR_CLOSING,           wxCommandEventHandler(WorkspaceTab::OnEditorClosing),       NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CONFIG_CHANGED, wxCommandEventHandler(WorkspaceTab::OnWorkspaceConfig),     NULL, this);

    wxTheApp->Disconnect(XRCID("configuration_manager"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnConfigurationManager), NULL, this);
    wxTheApp->Disconnect(XRCID("configuration_manager"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler (WorkspaceTab::OnProjectSettingsUI),   NULL, this);
    clConfig::Get().Write("WorkspaceTabSashPosition", m_splitter->GetSashPosition());
}

void WorkspaceTab::CreateGUIControls()
{
    wxSizer *sz = GetSizer();
#ifdef __WXMAC__
    m_workspaceConfig->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
    m_choiceActiveProject->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif
    // Construct the tree
    m_fileView = new FileViewTree(this, wxID_ANY);
    sz->Add(m_fileView, 1, wxEXPAND|wxALL, 2);
}

void WorkspaceTab::FreezeThaw(bool freeze /*=true*/)
{
    // If the selected file is linked to which editor is displayed, the selection changes rapidly when a workspace is loaded
    // Prevent this by temporarily unlinking, then relink after
    static bool frozen = false;
    static bool was_linked;

    wxCHECK_RET(!(freeze && frozen), wxT("Trying to re-freeze a frozen workspace tab"));
    wxCHECK_RET(!(!freeze && !frozen), wxT("Trying to thaw a warm workspace tab"));
    frozen = freeze;

    if (freeze) {
        was_linked = m_isLinkedToEditor;
        m_isLinkedToEditor = false;
    } else {
        m_isLinkedToEditor = was_linked;
        // I expected we'd need to call OnActiveEditorChanged() here, but it doesn't seem necessary (atm)
    }
}

void WorkspaceTab::ConnectEvents()
{
    wxTheApp->Connect(XRCID("show_in_workspace"),     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnShowFile),   NULL, this);
    wxTheApp->Connect(XRCID("show_in_workspace"),     wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnShowFileUI), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED,         wxCommandEventHandler(WorkspaceTab::OnWorkspaceLoaded),     NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED,         wxCommandEventHandler(WorkspaceTab::OnWorkspaceClosed),     NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_ADDED,               wxCommandEventHandler(WorkspaceTab::OnProjectAdded),        NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_REMOVED,             wxCommandEventHandler(WorkspaceTab::OnProjectRemoved),      NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED,    wxCommandEventHandler(WorkspaceTab::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_EDITOR_CLOSING,           wxCommandEventHandler(WorkspaceTab::OnEditorClosing),       NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CONFIG_CHANGED, wxCommandEventHandler(WorkspaceTab::OnWorkspaceConfig),     NULL, this);

    wxTheApp->Connect(XRCID("configuration_manager"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnConfigurationManager),   NULL, this);
    wxTheApp->Connect(XRCID("configuration_manager"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler (WorkspaceTab::OnProjectSettingsUI),   NULL, this);
}

void WorkspaceTab::OnLinkEditor(wxCommandEvent &e)
{
    m_isLinkedToEditor = !m_isLinkedToEditor;
    EditorConfigST::Get()->SaveLongValue(wxT("LinkWorkspaceViewToEditor"), m_isLinkedToEditor ? 1 : 0);
    if (m_isLinkedToEditor) {
        OnActiveEditorChanged(e);
    }
}

void WorkspaceTab::OnCollapseAll(wxCommandEvent &e)
{
    wxUnusedVar(e);
    if (!m_fileView->GetRootItem().IsOk())
        return;
    m_fileView->Freeze();
    m_fileView->CollapseAll();
    m_fileView->Expand(m_fileView->GetRootItem());
    // count will probably be 0 below, so ensure we can at least see the root item
    m_fileView->EnsureVisible(m_fileView->GetRootItem());
    m_fileView->Thaw();

    wxArrayTreeItemIds arr;
    size_t count = m_fileView->GetSelections( arr );

    if ( count == 1 ) {
        wxTreeItemId sel = arr.Item(0);
        if (sel.IsOk()) {
            m_fileView->EnsureVisible(sel);
        }
    }
}

void WorkspaceTab::OnCollapseAllUI(wxUpdateUIEvent &e)
{
    e.Enable(ManagerST::Get()->IsWorkspaceOpen());
}

void WorkspaceTab::OnGoHome(wxCommandEvent &e)
{
    wxUnusedVar(e);
    wxString activeProject = ManagerST::Get()->GetActiveProjectName();
    if (activeProject.IsEmpty())
        return;
    m_fileView->ExpandToPath(activeProject, wxFileName());

    wxArrayTreeItemIds arr;
    size_t count = m_fileView->GetSelections( arr );

    if ( count == 1 ) {
        wxTreeItemId sel = arr.Item(0);
        if (sel.IsOk() && m_fileView->ItemHasChildren(sel))
            m_fileView->Expand(sel);
    }
    ManagerST::Get()->ShowWorkspacePane(m_caption);
}

void WorkspaceTab::OnGoHomeUI(wxUpdateUIEvent &e)
{
    e.Enable(!ManagerST::Get()->GetActiveProjectName().IsEmpty());
}

void WorkspaceTab::OnProjectSettings(wxCommandEvent& e)
{
    wxUnusedVar(e);
    OpenProjectSettings( );
}

void WorkspaceTab::OnProjectSettingsUI(wxUpdateUIEvent& e)
{
    e.Enable(!ManagerST::Get()->GetActiveProjectName().IsEmpty());
}

static int wxStringCmpFunc(const wxString& item1, const wxString& item2)
{
    return item1.CmpNoCase(item2);
}

void WorkspaceTab::OnShowFile(wxCommandEvent& e)
{
    LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if (editor && !editor->GetProject().IsEmpty()) {
        m_fileView->ExpandToPath(editor->GetProject(), editor->GetFileName());
        ManagerST::Get()->ShowWorkspacePane(m_caption);
    }
    e.Skip();
}

void WorkspaceTab::OnShowFileUI(wxUpdateUIEvent& e)
{
    LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    e.Enable(editor && !editor->GetProject().IsEmpty());
}

void WorkspaceTab::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    if (m_isLinkedToEditor) {
        MainBook* mainbook = clMainFrame::Get()->GetMainBook();
        LEditor *editor = mainbook->GetActiveEditor();
        if (editor && !editor->GetProject().IsEmpty()) {
            m_fileView->ExpandToPath(editor->GetProject(), editor->GetFileName());
        }

        Notebook* book = clMainFrame::Get()->GetWorkspacePane()->GetNotebook();
        if (book) {
            size_t index = book->GetPageIndex("wxCrafter");
            if (index == (size_t)book->GetSelection()) {
                book->SetSelection(0); // The most likely to be wanted
            }
        }
    }
}

void WorkspaceTab::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    if(ManagerST::Get()->IsWorkspaceOpen()) {
        DoWorkspaceConfig();
        DoUpdateChoiceWithProjects();

        // Tree construction
        Freeze();
        m_fileView->BuildTree();
        OnGoHome(e);
        Thaw();
        SendCmdEvent(wxEVT_FILE_VIEW_INIT_DONE);
    }
}

void WorkspaceTab::OnEditorClosing(wxCommandEvent& e)
{
    e.Skip();
}

void WorkspaceTab::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    m_workspaceConfig->Clear();
    m_choiceActiveProject->Clear();
    m_workspaceConfig->Enable(false);
    m_fileView->DeleteAllItems();
    SendCmdEvent(wxEVT_FILE_VIEW_INIT_DONE);
}

void WorkspaceTab::OnProjectAdded(wxCommandEvent& e)
{
    e.Skip();
    const wxString *projName = (const wxString *) e.GetClientData();
    m_fileView->BuildTree();
    if (projName && !projName->IsEmpty()) {
        m_fileView->ExpandToPath(*projName, wxFileName());
    }
    DoUpdateChoiceWithProjects();
    SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
}

void WorkspaceTab::OnProjectRemoved(wxCommandEvent& e)
{
    e.Skip();
    Freeze();
    m_fileView->BuildTree();
    OnGoHome(e);
    Thaw();
    DoUpdateChoiceWithProjects();
    SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
}

void WorkspaceTab::DoWorkspaceConfig()
{
    // Update the workspace configuration
    BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
    std::list<WorkspaceConfigurationPtr> confs = matrix->GetConfigurations();

    m_workspaceConfig->Freeze();
    m_workspaceConfig->Enable(true);
    m_workspaceConfig->Clear();
    for (std::list<WorkspaceConfigurationPtr>::iterator iter = confs.begin() ; iter != confs.end(); iter++) {
        m_workspaceConfig->Append((*iter)->GetName());
    }
    if (m_workspaceConfig->GetCount() > 0) {
        m_workspaceConfig->SetStringSelection(matrix->GetSelectedConfigurationName());
    }
    m_workspaceConfig->Append(OPEN_CONFIG_MGR_STR);
    m_workspaceConfig->Thaw();

    clMainFrame::Get()->SelectBestEnvSet();
}

void WorkspaceTab::OnWorkspaceConfig(wxCommandEvent& e)
{
    e.Skip();
    DoWorkspaceConfig();
}

void WorkspaceTab::OnConfigurationManagerChoice(wxCommandEvent& e)
{
    wxString selection = m_workspaceConfig->GetStringSelection();
    if(selection == OPEN_CONFIG_MGR_STR) {
        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("configuration_manager"));
        e.SetEventObject(this);
        ProcessEvent(e);
        return;
    }

    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
    matrix->SetSelectedConfigurationName(selection);
    ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);

    // Set the focus to the active editor if any
    LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if(editor)
        editor->SetActive();

    ManagerST::Get()->UpdateParserPaths(true);
}

void WorkspaceTab::OnConfigurationManager(wxCommandEvent& e)
{
    wxUnusedVar(e);
    ConfigurationManagerDlg dlg(this);
    dlg.ShowModal();

    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
    m_workspaceConfig->SetStringSelection(matrix->GetSelectedConfigurationName());
}

void WorkspaceTab::OnChoiceActiveProject(wxCommandEvent& event)
{
    event.Skip();
    m_fileView->MarkActive(event.GetString());
}

void WorkspaceTab::OnChoiceActiveProjectUI(wxUpdateUIEvent& event)
{
    event.Enable(ManagerST::Get()->IsWorkspaceOpen());
}

void WorkspaceTab::OnLinkEditorUI(wxUpdateUIEvent& event)
{
    event.Enable(ManagerST::Get()->IsWorkspaceOpen());
    event.Check( m_isLinkedToEditor );
}

void WorkspaceTab::DoUpdateChoiceWithProjects()
{
    wxArrayString projects;
    ManagerST::Get()->GetProjectList(projects);
    projects.Sort(wxStringCmpFunc);

    wxWindowUpdateLocker locker(m_choiceActiveProject);
    m_choiceActiveProject->Clear();
    if ( WorkspaceST::Get()->IsOpen() ) {
        m_choiceActiveProject->Append( projects );
        m_choiceActiveProject->SetStringSelection( ManagerST::Get()->GetActiveProjectName() );
    }
}

void WorkspaceTab::OnConfigurationManagerChoiceUI(wxUpdateUIEvent& event)
{
    event.Enable(ManagerST::Get()->IsWorkspaceOpen());
}

void WorkspaceTab::OnWorkspaceOpenUI(wxUpdateUIEvent& event)
{
    event.Enable(ManagerST::Get()->IsWorkspaceOpen());
}

void WorkspaceTab::OpenProjectSettings(const wxString& project)
{
    if ( m_dlg ) {
        m_dlg->Raise();
        return;
    }
    
    wxString projectName = project.IsEmpty() ? ManagerST::Get()->GetActiveProjectName() : project;
    wxString title( projectName );
    title << _( " Project Settings" );
    
    // Allow plugins to process this event first
    clCommandEvent openEvent(wxEVT_CMD_OPEN_PROJ_SETTINGS);
    openEvent.SetString( project );
    if ( EventNotifier::Get()->ProcessEvent( openEvent ) ) {
        return;
    }
    
    //open the project properties dialog
    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();

#ifdef __WXMAC__
    // On OSX we use a modal version of the project settings
    // since otherwise we get some weird focus issues when the project 
    // settings dialog popups helper dialogs
    ProjectSettingsDlg dlg( clMainFrame::Get(), this, matrix->GetProjectSelectedConf( matrix->GetSelectedConfigurationName(), projectName ), projectName, title );
    dlg.ShowModal();
    
#else
    // Find the project configuration name that matches the workspace selected configuration
    m_dlg = new ProjectSettingsDlg( clMainFrame::Get(), this, matrix->GetProjectSelectedConf( matrix->GetSelectedConfigurationName(), projectName ), projectName, title );
    m_dlg->Show();
    
#endif

    // Mark this project as modified
    ProjectPtr proj = ManagerST::Get()->GetProject(projectName);
    if (proj) {
        proj->SetModified(true);
    }
}

void WorkspaceTab::ProjectSettingsDlgClosed()
{
    m_dlg = NULL;
}
