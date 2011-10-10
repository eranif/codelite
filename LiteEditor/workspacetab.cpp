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

#define OPEN_CONFIG_MGR_STR _("<Open Configuration Manager...>")

WorkspaceTab::WorkspaceTab(wxWindow *parent, const wxString &caption)
    : wxPanel(parent)
    , m_caption(caption)
    , m_isLinkedToEditor(true)
{
	long link(1);
	EditorConfigST::Get()->GetLongValue(wxT("LinkWorkspaceViewToEditor"), link);
	m_isLinkedToEditor = link ? true : false;

	CreateGUIControls();
    ConnectEvents();
}

WorkspaceTab::~WorkspaceTab()
{
	Disconnect( XRCID("link_editor"),        wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnLinkEditor));
	Disconnect( XRCID("collapse_all"),       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnCollapseAll));
	Disconnect( XRCID("collapse_all"),       wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnCollapseAllUI));
	Disconnect( XRCID("go_home"),            wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnGoHome));
	Disconnect( XRCID("go_home"),            wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnGoHomeUI));
	Disconnect( XRCID("project_properties"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnProjectSettingsUI));
	Disconnect( XRCID("project_properties"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnProjectSettings));
	Disconnect( XRCID("set_project_active"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnShowProjectListPopup));
	Disconnect( XRCID("set_project_active"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnProjectSettingsUI));

    wxTheApp->Disconnect(XRCID("show_in_workspace"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnShowFile),   NULL, this);
    wxTheApp->Disconnect(XRCID("show_in_workspace"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnShowFileUI), NULL, this);

    wxTheApp->Disconnect(wxEVT_WORKSPACE_LOADED,         wxCommandEventHandler(WorkspaceTab::OnWorkspaceLoaded),     NULL, this);
    wxTheApp->Disconnect(wxEVT_WORKSPACE_CLOSED,         wxCommandEventHandler(WorkspaceTab::OnWorkspaceClosed),     NULL, this);
    wxTheApp->Disconnect(wxEVT_PROJ_ADDED,               wxCommandEventHandler(WorkspaceTab::OnProjectAdded),        NULL, this);
    wxTheApp->Disconnect(wxEVT_PROJ_REMOVED,             wxCommandEventHandler(WorkspaceTab::OnProjectRemoved),      NULL, this);
    wxTheApp->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,    wxCommandEventHandler(WorkspaceTab::OnActiveEditorChanged), NULL, this);
    wxTheApp->Disconnect(wxEVT_EDITOR_CLOSING,           wxCommandEventHandler(WorkspaceTab::OnEditorClosing),       NULL, this);
	wxTheApp->Disconnect(wxEVT_WORKSPACE_CONFIG_CHANGED, wxCommandEventHandler(WorkspaceTab::OnWorkspaceConfig),     NULL, this);
	
    wxTheApp->Disconnect(XRCID("configuration_manager"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnConfigurationManager), NULL, this);
	wxTheApp->Disconnect(XRCID("configuration_manager"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler (WorkspaceTab::OnProjectSettingsUI),   NULL, this);
}

void WorkspaceTab::CreateGUIControls()
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer(sz);

	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER);
	tb->AddTool(XRCID("link_editor"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("link_editor")), _("Link Editor"), wxITEM_CHECK);
	tb->ToggleTool(XRCID("link_editor"), m_isLinkedToEditor);
	tb->AddTool(XRCID("collapse_all"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("collapse")), _("Collapse All"), wxITEM_NORMAL);
	tb->AddTool(XRCID("go_home"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("gohome")), _("Goto Active Project"), wxITEM_NORMAL);
	tb->AddSeparator();

	BitmapLoader *bmpLoader = PluginManager::Get()->GetStdIcons();
	tb->AddTool(XRCID("project_properties"), wxEmptyString, bmpLoader->LoadBitmap(wxT("workspace/16/project_settings")),      _("Open Active Project Settings..."), wxITEM_NORMAL);
	tb->AddTool(XRCID("set_project_active"), wxEmptyString, bmpLoader->LoadBitmap(wxT("workspace/16/project_select_active")), _("Select Active Project"),           wxITEM_NORMAL);
	tb->AddSeparator();

	// add the 'multiple/single' tree style
	long val (0);
	if(EditorConfigST::Get()->GetLongValue(wxT("WspTreeMultipleSelection"), val) == false) {val = 0;}

	tb->AddTool(XRCID("set_multi_selection"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("cursor")), _("Toggle Multiple/Single Selection"), wxITEM_CHECK);
	tb->ToggleTool(XRCID("set_multi_selection"), val ? true : false);
	tb->Realize();
	sz->Add(tb, 0, wxEXPAND, 0);
	
	// Add the workspace configuration choice control
	wxArrayString choices;
	m_workspaceConfig = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
	m_workspaceConfig->SetToolTip(_("Select the workspace build configuration"));

	m_workspaceConfig->Enable(false);
	m_workspaceConfig->Append(OPEN_CONFIG_MGR_STR);
	ConnectChoice(m_workspaceConfig, WorkspaceTab::OnConfigurationManagerChoice);
#ifdef __WXMAC__
	m_workspaceConfig->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif
	sz->Add(m_workspaceConfig, 0, wxEXPAND, 2);
	
	// Construct the tree
	m_fileView = new FileViewTree(this, wxID_ANY);
	sz->Add(m_fileView, 1, wxEXPAND|wxTOP, 2);
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
	Connect( XRCID("link_editor"),        wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnLinkEditor));
	Connect( XRCID("set_multi_selection"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnToggleMultiSelection));
	Connect( XRCID("set_multi_selection"),wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnCollapseAllUI));
	Connect( XRCID("collapse_all"),       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnCollapseAll));
	Connect( XRCID("collapse_all"),       wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnCollapseAllUI));
	Connect( XRCID("go_home"),            wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnGoHome));
	Connect( XRCID("go_home"),            wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnGoHomeUI));
	Connect( XRCID("project_properties"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnProjectSettingsUI));
	Connect( XRCID("project_properties"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnProjectSettings));
	Connect( XRCID("set_project_active"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnShowProjectListPopup));
	Connect( XRCID("set_project_active"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnProjectSettingsUI));

    wxTheApp->Connect(XRCID("show_in_workspace"),     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspaceTab::OnShowFile),   NULL, this);
    wxTheApp->Connect(XRCID("show_in_workspace"),     wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspaceTab::OnShowFileUI), NULL, this);
    wxTheApp->Connect(wxEVT_WORKSPACE_LOADED,         wxCommandEventHandler(WorkspaceTab::OnWorkspaceLoaded),     NULL, this);
    wxTheApp->Connect(wxEVT_WORKSPACE_CLOSED,         wxCommandEventHandler(WorkspaceTab::OnWorkspaceClosed),     NULL, this);
    wxTheApp->Connect(wxEVT_PROJ_ADDED,               wxCommandEventHandler(WorkspaceTab::OnProjectAdded),        NULL, this);
    wxTheApp->Connect(wxEVT_PROJ_REMOVED,             wxCommandEventHandler(WorkspaceTab::OnProjectRemoved),      NULL, this);
    wxTheApp->Connect(wxEVT_ACTIVE_EDITOR_CHANGED,    wxCommandEventHandler(WorkspaceTab::OnActiveEditorChanged), NULL, this);
    wxTheApp->Connect(wxEVT_EDITOR_CLOSING,           wxCommandEventHandler(WorkspaceTab::OnEditorClosing),       NULL, this);
	wxTheApp->Connect(wxEVT_WORKSPACE_CONFIG_CHANGED, wxCommandEventHandler(WorkspaceTab::OnWorkspaceConfig),     NULL, this);
	
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
	wxString projectName = ManagerST::Get()->GetActiveProjectName();
	wxString title( projectName );
	title << _( " Project Settings" );

	//open the project properties dialog
	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();

	//find the project configuration name that matches the workspace selected configuration
	ProjectSettingsDlg dlg( clMainFrame::Get(), matrix->GetProjectSelectedConf( matrix->GetSelectedConfigurationName(), projectName ), projectName, title );
	if(dlg.ShowModal() == wxID_OK) {
		ManagerST::Get()->UpdateParserPaths(true);
	}

	//mark this project as modified
	ProjectPtr proj = ManagerST::Get()->GetProject(projectName);
	if (proj) {
		proj->SetModified(true);
	}
}

void WorkspaceTab::OnProjectSettingsUI(wxUpdateUIEvent& e)
{
	e.Enable(!ManagerST::Get()->GetActiveProjectName().IsEmpty());
}

static int wxStringCmpFunc(const wxString& item1, const wxString& item2)
{
	return item1.CmpNoCase(item2);
}

void WorkspaceTab::OnShowProjectListPopup(wxCommandEvent& e)
{
	wxUnusedVar(e);

#ifdef __WXMSW__
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
#endif

	wxMenu popupMenu;

	wxArrayString projects;
	ManagerST::Get()->GetProjectList(projects);
	projects.Sort(wxStringCmpFunc);
	for (size_t i = 0; i < projects.GetCount(); i++) {
		wxString text = projects.Item(i);
		bool selected = ManagerST::Get()->GetActiveProjectName() == text;
		wxMenuItem *item = new wxMenuItem(&popupMenu, i, text, text, wxITEM_CHECK);

		//set the font
#ifdef __WXMSW__
		if (selected) {
			font.SetWeight(wxBOLD);
		}
		item->SetFont(font);
#endif
		popupMenu.Append(item);
		item->Check(selected);

		//restore font
#ifdef __WXMSW__
		font.SetWeight(wxNORMAL);
#endif
	}

    popupMenu.Connect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(WorkspaceTab::OnMenuSelection), NULL, this);
	PopupMenu(&popupMenu);
}

void WorkspaceTab::OnMenuSelection(wxCommandEvent& e)
{
	wxArrayString projects;
	ManagerST::Get()->GetProjectList(projects);
	projects.Sort(wxStringCmpFunc);

	size_t sel = (size_t)e.GetId();
	if(sel < projects.GetCount()) {
		m_fileView->MarkActive(projects.Item(sel));
	}
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
        LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
        if (editor && !editor->GetProject().IsEmpty()) {
            m_fileView->ExpandToPath(editor->GetProject(), editor->GetFileName());
        }
    }
}

void WorkspaceTab::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
	if(ManagerST::Get()->IsWorkspaceOpen()) {
		DoWorkspaceConfig();
		
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
    SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
}

void WorkspaceTab::OnProjectRemoved(wxCommandEvent& e)
{
    e.Skip();
    Freeze();
    m_fileView->BuildTree();
    OnGoHome(e);
    Thaw();
    SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
}

void WorkspaceTab::OnToggleMultiSelection(wxCommandEvent& e)
{
	EditorConfigST::Get()->SaveLongValue(wxT("WspTreeMultipleSelection"), e.IsChecked() ? 1 : 0);
	// Reload the tree
	int answer = wxMessageBox(_("Workspace reload is required\nWould you like to reload workspace now?"), _("CodeLite"), wxICON_INFORMATION|wxYES_NO|wxCANCEL, this);
	if ( answer == wxYES ) {
		wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("reload_workspace"));
		clMainFrame::Get()->GetEventHandler()->AddPendingEvent(e);
	}
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
	if(selection == OPEN_CONFIG_MGR_STR){
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
