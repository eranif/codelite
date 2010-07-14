//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : workspace_pane.cpp
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
#include <wx/app.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>
#include "parse_thread.h"
#include "editor_config.h"
#include "configuration_manager_dlg.h"
#include "detachedpanesinfo.h"
#include "dockablepane.h"
#include "manager.h"
#include "frame.h"
#include "cl_editor.h"
#include "notebook_ex.h"
#include "cpp_symbol_tree.h"
#include "windowstack.h"
#include "macros.h"
#include "fileview.h"
#include "openwindowspanel.h"
#include "fileexplorer.h"
#include "workspacetab.h"
#include "tabgroupspane.h"
#include "workspace_pane.h"

#define OPEN_CONFIG_MGR_STR wxT("<Open Configuration Manager...>")

WorkspacePane::WorkspacePane(wxWindow *parent, const wxString &caption, wxAuiManager *mgr)
    : wxPanel(parent)
    , m_caption(caption)
    , m_mgr(mgr)
{
	CreateGUIControls();
    Connect();
}

WorkspacePane::~WorkspacePane()
{
}

#define IS_DETACHED(name) (detachedPanes.Index(name) != wxNOT_FOUND) ? true : false

void WorkspacePane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

    // selected configuration:

	wxBoxSizer *hsz = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(hsz, 0, wxEXPAND|wxTOP|wxBOTTOM, 5);

	wxArrayString choices;
	m_workspaceConfig = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
	m_workspaceConfig->SetToolTip(wxT("Select the workspace build configuration"));

	m_workspaceConfig->Enable(false);
	m_workspaceConfig->Append(OPEN_CONFIG_MGR_STR);
	ConnectChoice(m_workspaceConfig, WorkspacePane::OnConfigurationManagerChoice);
	hsz->Add(m_workspaceConfig, 1, wxEXPAND| wxALL, 1);

#ifdef __WXMAC__
	m_workspaceConfig->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif

    // add notebook for tabs
	long bookStyle = wxVB_LEFT | wxAUI_NB_WINDOWLIST_BUTTON;
	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, bookStyle);

	// Calculate the widthest tab (the one with the 'Workspcae' label)
	int xx, yy;
	wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxWindow::GetTextExtent(wxT("Workspace"), &xx, &yy, NULL, NULL, &fnt);

	mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 0);

    // create tabs (possibly detached)
	DetachedPanesInfo dpi;
	EditorConfigST::Get()->ReadObject(wxT("DetachedPanesList"), &dpi);


	wxArrayString detachedPanes;
	detachedPanes = dpi.GetPanes();

	// Add the workspace tab
	wxString  name;

	name = wxT("Workspace");
	if(IS_DETACHED(name)) {
		DockablePane *cp = new DockablePane(GetParent(), m_book, name, wxNullBitmap, wxSize(200, 200));
		m_workspaceTab = new WorkspaceTab(cp, name);
		cp->SetChildNoReparent(m_workspaceTab);
	} else {
		m_workspaceTab = new WorkspaceTab(m_book, name);
		m_book->AddPage(m_workspaceTab, name, true);
	}

	// Add the explorer tab
	name = wxT("Explorer");
	if(IS_DETACHED(name)) {
		DockablePane *cp = new DockablePane(GetParent(), m_book, name, wxNullBitmap, wxSize(200, 200));
		m_explorer = new FileExplorer(cp, name);
		cp->SetChildNoReparent(m_explorer);
	} else {
		m_explorer = new FileExplorer(m_book, name);
		m_book->AddPage(m_explorer, name, false);
	}

	// Add the Open Windows Panel (Tabs)
	name = wxT("Tabs");
	if(IS_DETACHED(name)) {
		DockablePane *cp = new DockablePane(GetParent(), m_book,  name, wxNullBitmap, wxSize(200, 200));
		m_openWindowsPane = new OpenWindowsPanel(cp, name);
		cp->SetChildNoReparent(m_openWindowsPane);
	} else {
		m_openWindowsPane = new OpenWindowsPanel(m_book, name);
		m_book->AddPage(m_openWindowsPane, name, false);
	}

	// Add the Tabgroups tab
	name = wxT("Tabgroups");
	if(IS_DETACHED(name)) {
		DockablePane *cp = new DockablePane(GetParent(), m_book,  name, wxNullBitmap, wxSize(200, 200));
		m_TabgroupsPane = new TabgroupsPane(cp, name);
		cp->SetChildNoReparent(m_TabgroupsPane);
	} else {
		m_TabgroupsPane = new TabgroupsPane(m_book, name);
		m_book->AddPage(m_TabgroupsPane, name, false);
	}

	if (m_book->GetPageCount() > 0) {
		m_book->SetSelection((size_t)0);
	}
	m_mgr->Update();
}

void WorkspacePane::Connect()
{
	wxTheApp->Connect(wxEVT_WORKSPACE_LOADED,         wxCommandEventHandler(WorkspacePane::OnWorkspaceConfig),     NULL, this);
	wxTheApp->Connect(wxEVT_WORKSPACE_CONFIG_CHANGED, wxCommandEventHandler(WorkspacePane::OnWorkspaceConfig),     NULL, this);
	wxTheApp->Connect(wxEVT_WORKSPACE_CLOSED,         wxCommandEventHandler(WorkspacePane::OnWorkspaceClosed),     NULL, this);
    wxTheApp->Connect(XRCID("configuration_manager"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspacePane::OnConfigurationManager),   NULL, this);
    wxTheApp->Connect(XRCID("configuration_manager"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspacePane::OnConfigurationManagerUI), NULL, this);
}

void WorkspacePane::OnWorkspaceConfig(wxCommandEvent& e)
{
    e.Skip();

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
}

void WorkspacePane::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    m_workspaceConfig->Clear();
    m_workspaceConfig->Enable(false);
}

void WorkspacePane::OnConfigurationManagerUI(wxUpdateUIEvent& e)
{
	e.Enable(ManagerST::Get()->IsWorkspaceOpen());
}

void WorkspacePane::OnConfigurationManagerChoice(wxCommandEvent &event)
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
	LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
	if(editor)
		editor->SetActive();

}

void WorkspacePane::OnConfigurationManager(wxCommandEvent& e)
{
	wxUnusedVar(e);
	ConfigurationManagerDlg dlg(this);
	dlg.ShowModal();

	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
	m_workspaceConfig->SetStringSelection(matrix->GetSelectedConfigurationName());
}
