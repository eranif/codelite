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
#include "wx/xrc/xmlres.h"
#include "project_settings_dlg.h"
#include "globals.h"
#include "workspacetab.h"
#include "manager.h"
#include "fileview.h"
#include "editor_config.h"
#include "wx/combobox.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "frame.h"
#include "macros.h"
#include "workspace_pane.h"

static int wxStringCmpFunc(const wxString& item1, const wxString& item2) 
{
	return item1.CmpNoCase(item2);
}

WorkspaceTab::WorkspaceTab(wxWindow *parent)
		: wxPanel(parent)
		, m_isLinkedToEditor(true)
{
	long link(1);
	EditorConfigST::Get()->GetLongValue(wxT("LinkWorkspaceViewToEditor"), link);
	m_isLinkedToEditor = link ? true : false;
	CreateGUIControls();
}

WorkspaceTab::~WorkspaceTab()
{
}

void WorkspaceTab::OnCollapseAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	DoCollpaseAll();
}

void WorkspaceTab::CreateGUIControls()
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer(sz);


	//add toolbar on top of the workspace tab that includes a single button that collapse all
	//tree items
	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER);

	tb->AddTool(XRCID("link_editor"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("link_editor")), wxT("Link Editor"), wxITEM_CHECK);
	tb->ToggleTool(XRCID("link_editor"), m_isLinkedToEditor);
	tb->AddTool(XRCID("collapse_all"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("collapse")), wxT("Collapse All"), wxITEM_NORMAL);
	tb->AddTool(XRCID("go_home"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("gohome")), wxT("Goto Active Project"), wxITEM_NORMAL);
	tb->AddSeparator();
	tb->AddTool(XRCID("project_properties"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("project_settings")), wxT("Open Active Project Settings..."), wxITEM_NORMAL);
	tb->AddTool(XRCID("set_project_active"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("set_active")), wxT("Select Active Project"), wxITEM_NORMAL);
	tb->Realize();

	Connect( XRCID("link_editor"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WorkspaceTab::OnLinkEditor ));
	Connect( XRCID("collapse_all"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WorkspaceTab::OnCollapseAll ));
	Connect( XRCID("collapse_all"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceTab::OnCollapseAllUI ));
	Connect( XRCID("go_home"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WorkspaceTab::OnGoHome ));
	Connect( XRCID("go_home"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceTab::OnGoHomeUI ));
	Connect( XRCID("project_properties"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceTab::OnProjectSettingsUI ));
	Connect( XRCID("project_properties"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WorkspaceTab::OnProjectSettings) );
	Connect( XRCID("set_project_active"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WorkspaceTab::OnShowProjectListPopup) );
	Connect( XRCID("set_project_active"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceTab::OnProjectSettingsUI ));
	
	//add the fileview tab
	m_fileView = new FileViewTree(this, wxID_ANY);
	sz->Add(tb, 0, wxEXPAND, 0);
	sz->Add(m_fileView, 1, wxEXPAND|wxTOP, 2);
}

void WorkspaceTab::BuildFileTree()
{
	m_fileView->BuildTree();
}

void WorkspaceTab::OnCollapseAllUI(wxUpdateUIEvent &event)
{
	event.Enable(ManagerST::Get()->IsWorkspaceOpen());
}
void WorkspaceTab::DoCollpaseAll()
{

	if (ManagerST::Get()->IsWorkspaceOpen() == false) {
		return;
	}

	wxTreeItemId root = m_fileView->GetRootItem();
	if (root.IsOk() == false) {
		return;
	}

	if (m_fileView->ItemHasChildren(root) == false) {
		return;
	}

	m_fileView->Freeze();

	//iterate over all the projects items and collapse them all
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_fileView->GetFirstChild(root, cookie);
	while ( child.IsOk() ) {
		m_fileView->CollapseAllChildren(child);
		child = m_fileView->GetNextChild(root, cookie);
	}

	m_fileView->Thaw();

	wxTreeItemId sel = m_fileView->GetSelection();
	if (sel.IsOk())
		m_fileView->EnsureVisible(sel);
}

void WorkspaceTab::CollpaseAll()
{
	DoCollpaseAll();
}

void WorkspaceTab::OnLinkEditor(wxCommandEvent &e)
{
	wxUnusedVar(e);
	m_isLinkedToEditor = !m_isLinkedToEditor;
	// save the value
	EditorConfigST::Get()->SaveLongValue(wxT("LinkWorkspaceViewToEditor"), m_isLinkedToEditor ? 1 : 0);
	if (m_isLinkedToEditor) {
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("show_in_workspace"));
		Frame::Get()->AddPendingEvent(event);
	}
}

void WorkspaceTab::OnGoHome(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxString activeProject = ManagerST::Get()->GetActiveProjectName();
	if (activeProject.IsEmpty()) {
		return;
	}
	ManagerST::Get()->ShowWorkspacePane(WorkspacePane::FILE_VIEW);
	m_fileView->ExpandToPath(activeProject, wxFileName());
	wxTreeItemId sel = m_fileView->GetSelection();
	if (sel.IsOk() && m_fileView->ItemHasChildren(sel))
		m_fileView->Expand(sel);
}

void WorkspaceTab::OnGoHomeUI(wxUpdateUIEvent &event)
{
	event.Enable(!ManagerST::Get()->GetActiveProjectName().IsEmpty());
}

void WorkspaceTab::OnProjectSettings(wxCommandEvent& e)
{
	wxString projectName = ManagerST::Get()->GetActiveProjectName();
	wxString title( projectName );
	title << wxT( " Project Settings" );

	//open the project properties dialog
	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();

	//find the project configuration name that matches the workspace selected configuration
	ProjectSettingsDlg dlg( Frame::Get(), matrix->GetProjectSelectedConf( matrix->GetSelectedConfigurationName(), projectName ), projectName, title );
	dlg.ShowModal();

	//mark this project as modified
	ProjectPtr proj = ManagerST::Get()->GetProject(projectName);
	if (proj) {
		proj->SetModified(true);
	}
}

void WorkspaceTab::OnProjectSettingsUI(wxUpdateUIEvent& event)
{
	event.Enable(!ManagerST::Get()->GetActiveProjectName().IsEmpty());
}

void WorkspaceTab::OnShowProjectListPopup(wxCommandEvent& e)
{
	wxUnusedVar(e);
	DoShowPopupMenu();
}

void WorkspaceTab::DoShowPopupMenu()
{
	wxMenu popupMenu;

#ifdef __WXMSW__
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
#endif
	
	
	wxArrayString projects;
	ManagerST::Get()->GetProjectList(projects);
	projects.Sort(wxStringCmpFunc);
	
	for (size_t i=0; i<projects.GetCount(); i++) {
		wxString text = projects.Item(i);
		bool selected = ManagerST::Get()->GetActiveProjectName() == text;

		wxMenuItem *item = new wxMenuItem(&popupMenu, static_cast<int>(i), text, text, wxITEM_CHECK);

		//set the font
#ifdef __WXMSW__
		if (selected) {
			font.SetWeight(wxBOLD);
		}
		item->SetFont(font);
#endif
		popupMenu.Append( item );

		//mark the selected item
		item->Check(selected);

		//restore font
#ifdef __WXMSW__
		font.SetWeight(wxNORMAL);
#endif
	}
	
	// connect an event handler to our menu
    popupMenu.Connect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(WorkspaceTab::OnMenuSelection), NULL, this);
	PopupMenu( &popupMenu );
}

void WorkspaceTab::OnMenuSelection(wxCommandEvent& e)
{
	wxArrayString projects;
	size_t sel = (size_t)e.GetId();
	ManagerST::Get()->GetProjectList(projects);
	
	if(sel < projects.GetCount()) {
		GetFileView()->MarkActive(projects.Item(sel));
	}
}
