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
#include <wx/xrc/xmlres.h>
#include "parse_thread.h"
#include "editor_config.h"
#include "configuration_manager_dlg.h"
#include "detachedpanesinfo.h"
#include "dockablepane.h"
#include "manager.h"
#include "frame.h"
#include "cl_editor.h"
#include "custom_notebook.h"
#include "cpp_symbol_tree.h"
#include "windowstack.h"
#include "macros.h"
#include "fileview.h"
#include "openwindowspanel.h"
#include "fileexplorer.h"
#include "workspacetab.h"
#include "workspace_pane.h"


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

#define ADD_WORKSPACE_PAGE(win, name) \
	if( detachedPanes.Index(name) != wxNOT_FOUND ) {\
		new DockablePane(GetParent(), m_book, win, name, wxNullBitmap, wxSize(200, 200));\
	} else {\
		m_book->AddPage(win, name, name, wxNullBitmap, true);\
	}

void WorkspacePane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

    // selected configuration:

	mainSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Selected Configuration:")), 0, wxEXPAND| wxTOP|wxLEFT|wxRIGHT, 5);

	wxBoxSizer *hsz = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(hsz, 0, wxEXPAND|wxALL, 5);

	wxArrayString choices;
	m_workspaceConfig = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY|wxALIGN_CENTER_VERTICAL);
	m_workspaceConfig->Enable(false);
	ConnectCombo(m_workspaceConfig, WorkspacePane::OnConfigurationManagerChoice);
	hsz->Add(m_workspaceConfig, 1, wxEXPAND);

	wxButton *btn = new wxButton(this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	ConnectButton(btn, WorkspacePane::OnConfigurationManager);
	btn->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(WorkspacePane::OnConfigurationManagerUI), NULL, this);
	hsz->Add(btn, 0, wxALIGN_CENTER_VERTICAL);

	// add static line separator
	wxStaticLine *line = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add(line, 0, wxEXPAND);

    // add notebook for tabs
	long bookStyle = wxVB_LEFT|wxVB_FIXED_WIDTH;
	EditorConfigST::Get()->GetLongValue(wxT("WorkspaceView"), bookStyle);
	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, bookStyle);

	// Calculate the widthest tab (the one with the 'Workspcae' label)
	int xx, yy;
	wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxWindow::GetTextExtent(wxT("Workspace"), &xx, &yy, NULL, NULL, &fnt);
	m_book->SetFixedTabWidth(xx + 20);

    m_book->SetAuiManager(m_mgr, m_caption);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 1);

    // create tabs (possibly detached)
	DetachedPanesInfo dpi;
	EditorConfigST::Get()->ReadObject(wxT("DetachedPanesList"), &dpi);
	wxArrayString detachedPanes = dpi.GetPanes();

	m_workspaceTab = new WorkspaceTab(m_book, wxT("Workspace"));
	ADD_WORKSPACE_PAGE(m_workspaceTab, m_workspaceTab->GetCaption());

	m_explorer = new FileExplorer(m_book, wxT("Explorer"));
	ADD_WORKSPACE_PAGE(m_explorer, m_explorer->GetCaption());

	m_winStack = new WindowStack(m_book, wxID_ANY);
	ADD_WORKSPACE_PAGE(m_winStack, wxT("Outline"));

	m_openWindowsPane = new OpenWindowsPanel(m_book, wxT("Tabs"));
	ADD_WORKSPACE_PAGE(m_openWindowsPane, m_openWindowsPane->GetCaption());

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
	wxTheApp->Connect(wxEVT_PROJ_FILE_ADDED,          wxCommandEventHandler(WorkspacePane::OnProjectFileAdded),    NULL, this);
	wxTheApp->Connect(wxEVT_PROJ_FILE_REMOVED,        wxCommandEventHandler(WorkspacePane::OnProjectFileRemoved),  NULL, this);
	wxTheApp->Connect(wxEVT_SYNBOL_TREE_UPDATE_ITEM,  wxCommandEventHandler(WorkspacePane::OnSymbolsUpdated),      NULL, this);
	wxTheApp->Connect(wxEVT_SYNBOL_TREE_DELETE_ITEM,  wxCommandEventHandler(WorkspacePane::OnSymbolsDeleted),      NULL, this);
	wxTheApp->Connect(wxEVT_SYNBOL_TREE_ADD_ITEM,     wxCommandEventHandler(WorkspacePane::OnSymbolsAdded),        NULL, this);
	wxTheApp->Connect(wxEVT_FILE_RETAGGED,            wxCommandEventHandler(WorkspacePane::OnFileRetagged),        NULL, this);
	wxTheApp->Connect(wxEVT_ACTIVE_EDITOR_CHANGED,    wxCommandEventHandler(WorkspacePane::OnActiveEditorChanged), NULL, this);
	wxTheApp->Connect(wxEVT_EDITOR_CLOSING,           wxCommandEventHandler(WorkspacePane::OnEditorClosing),       NULL, this);
	wxTheApp->Connect(wxEVT_ALL_EDITORS_CLOSED,       wxCommandEventHandler(WorkspacePane::OnAllEditorsClosed),    NULL, this);

    wxTheApp->Connect(XRCID("configuration_manager"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler (WorkspacePane::OnConfigurationManager),   NULL, this);
    wxTheApp->Connect(XRCID("configuration_manager"), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(WorkspacePane::OnConfigurationManagerUI), NULL, this);
}


extern wxImageList* CreateSymbolTreeImages();

void WorkspacePane::ShowCurrentOutline()
{
    LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
    if (!editor || editor->GetProjectName().IsEmpty()) {
        m_winStack->SelectNone();
        return;
    }
    wxString path = editor->GetFileName().GetFullPath();
    if (m_winStack->GetSelectedKey() != path) {
        m_winStack->Freeze();
        if (m_winStack->Find(path) == NULL) {
            CppSymbolTree *tree = new CppSymbolTree(m_winStack, wxID_ANY);
            tree->SetSymbolsImages(CreateSymbolTreeImages());
            tree->BuildTree(path);
            m_winStack->Add(tree, path);
        }
        m_winStack->Select(path);
        m_winStack->Thaw();
    }
}

void WorkspacePane::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    ShowCurrentOutline();
}

void WorkspacePane::OnAllEditorsClosed(wxCommandEvent& e)
{
    e.Skip();
    m_winStack->Clear();
}

void WorkspacePane::OnEditorClosing(wxCommandEvent& e)
{
    e.Skip();
    IEditor *editor = (IEditor*) e.GetClientData();
    if (editor && !editor->GetProjectName().IsEmpty()) {
        m_winStack->Delete(editor->GetFileName().GetFullPath());
    }
}

void WorkspacePane::OnFileRetagged(wxCommandEvent& e)
{
    e.Skip();
    std::vector<wxFileName> *files = (std::vector<wxFileName>*) e.GetClientData();
	if (files && !files->empty()) {
        m_winStack->Freeze();
        // toss out any out-of-date outlines
        for (size_t i = 0; i < files->size(); i++) {
            m_winStack->Delete(files->at(i).GetFullPath());
        }
        ShowCurrentOutline(); // in case active editor's file was one of them
        m_winStack->Thaw();
    }
}

void WorkspacePane::OnProjectFileAdded(wxCommandEvent& e)
{
    e.Skip();
    ShowCurrentOutline(); // in case the active editor's file is now tagged
}

void WorkspacePane::OnProjectFileRemoved(wxCommandEvent& e)
{
    e.Skip();
    wxArrayString *files = (wxArrayString*) e.GetClientData();
	if (files && !files->IsEmpty()) {
        for (size_t i = 0; i < files->Count(); i++) {
            m_winStack->Delete(files->Item(i));
        }
        ShowCurrentOutline(); // in case active editor's file is no longer tagged
    }
}

void WorkspacePane::OnSymbolsAdded(wxCommandEvent& e)
{
    e.Skip();
    ParseThreadEventData *data = (ParseThreadEventData*) e.GetClientData();
	if (data && !data->GetItems().empty()) {
        CppSymbolTree *tree = (CppSymbolTree*) m_winStack->Find(data->GetFileName());
        if (tree) {
            tree->AddSymbols(data->GetItems());
        }
    }
}

void WorkspacePane::OnSymbolsDeleted(wxCommandEvent& e)
{
    e.Skip();
    ParseThreadEventData *data = (ParseThreadEventData*) e.GetClientData();
	if (data && !data->GetItems().empty()) {
        CppSymbolTree *tree = (CppSymbolTree*) m_winStack->Find(data->GetFileName());
        if (tree) {
            tree->DeleteSymbols(data->GetItems());
        }
    }
}

void WorkspacePane::OnSymbolsUpdated(wxCommandEvent& e)
{
    e.Skip();
    ParseThreadEventData *data = (ParseThreadEventData*) e.GetClientData();
	if (data && !data->GetItems().empty()) {
        CppSymbolTree *tree = (CppSymbolTree*) m_winStack->Find(data->GetFileName());
        if (tree) {
            tree->UpdateSymbols(data->GetItems());
        }
    }
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
	m_workspaceConfig->Thaw();
}

void WorkspacePane::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    m_workspaceConfig->Clear();
    m_workspaceConfig->Enable(false);
    m_winStack->Clear();
}

void WorkspacePane::OnConfigurationManager(wxCommandEvent& e)
{
	ConfigurationManagerDlg *dlg = new ConfigurationManagerDlg(this);
	dlg->ShowModal();
	dlg->Destroy();
}

void WorkspacePane::OnConfigurationManagerUI(wxUpdateUIEvent& e)
{
	e.Enable(ManagerST::Get()->IsWorkspaceOpen());
}

void WorkspacePane::OnConfigurationManagerChoice(wxCommandEvent &event)
{
	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
	matrix->SetSelectedConfigurationName(event.GetString());
	ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);
}
