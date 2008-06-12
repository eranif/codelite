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
#include "workspace_pane.h"
#include "manager.h"
#include "custom_notebook.h"
#include "fileview.h"
#include "cpp_symbol_tree.h"
#include <wx/xrc/xmlres.h>
#include "frame.h"
#include "checkdirtreectrl.h"
#include "windowstack.h"
#include "openwindowspanel.h"
#include "macros.h"
#include "fileexplorer.h"
#include "workspacetab.h"

const wxString WorkspacePane::SYMBOL_VIEW 	= wxT("Outline");
const wxString WorkspacePane::FILE_VIEW   	= wxT("Workspace");
const wxString WorkspacePane::OPEN_FILES  	= wxT("Tabs");
const wxString WorkspacePane::EXPLORER  	= wxT("Explorer");

extern wxImageList* CreateSymbolTreeImages();

BEGIN_EVENT_TABLE(WorkspacePane, wxPanel)
	EVT_PAINT(WorkspacePane::OnPaint)
	EVT_ERASE_BACKGROUND(WorkspacePane::OnEraseBg)
	EVT_SIZE(WorkspacePane::OnSize)
END_EVENT_TABLE()

WorkspacePane::WorkspacePane(wxWindow *parent, const wxString &caption)
		: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
		, m_caption(caption)
{
	CreateGUIControls();
}


WorkspacePane::~WorkspacePane()
{

}

int WorkspacePane::CaptionToIndex(const wxString &caption)
{
	size_t i = 0;
	for (; i<m_book->GetPageCount(); i++) {
		if (m_book->GetPageText(i) == caption)
			return i;
	}
	return wxNOT_FOUND;
}

void WorkspacePane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	// add the workspace configuration dropdown list
	wxBoxSizer *hsz = new wxBoxSizer(wxHORIZONTAL);
	
	wxArrayString choices;
	m_workspaceConfig = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY|wxALIGN_CENTER_VERTICAL);
	m_workspaceConfig->Enable(false);

	// Connect an event to handle changes in the choice control
	ConnectCombo(m_workspaceConfig, Frame::OnWorkspaceConfigChanged);
	mainSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Selected Configuration:")), 0, wxEXPAND| wxTOP|wxLEFT|wxRIGHT, 5);
	hsz->Add(m_workspaceConfig, 1, wxEXPAND);

	wxButton *btn = new wxButton(this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	hsz->Add(btn, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(hsz, 0, wxEXPAND|wxALL, 5);
	
	// add static line separator
	wxStaticLine *line = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add(line, 0, wxEXPAND);

	ConnectButton(btn, WorkspacePane::OnConfigurationManager);
	btn->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(WorkspacePane::OnConfigurationManagerUI), NULL, this);

	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_LEFT|wxVB_TAB_DECORATION);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 1);

	m_workspaceTab = new WorkspaceTab(m_book);
	m_book->AddPage(m_workspaceTab, WorkspacePane::FILE_VIEW, wxNullBitmap, true);

	m_explorer = new FileExplorer(m_book, wxT("Explorer"));
	m_book->AddPage(m_explorer, WorkspacePane::EXPLORER, wxNullBitmap, false);

	m_winStack = new WindowStack(m_book, wxID_ANY);
	m_book->AddPage(m_winStack, WorkspacePane::SYMBOL_VIEW, wxNullBitmap, false);

	m_openWindowsPane = new OpenWindowsPanel(m_book);
	m_book->AddPage(m_openWindowsPane, WorkspacePane::OPEN_FILES, wxNullBitmap, false);
}

CppSymbolTree *WorkspacePane::GetTreeByFilename(const wxFileName &filename)
{
	wxWindow *win = m_winStack->Find(filename.GetFullPath());
	if (win) {
		return dynamic_cast<CppSymbolTree *>(win);
	}
	return NULL;
}

void WorkspacePane::BuildSymbolTree(const wxFileName &filename)
{
	CppSymbolTree *tree = GetTreeByFilename(filename);
	if (!tree) {
		tree = new CppSymbolTree(m_winStack, wxID_ANY);
		tree->SetSymbolsImages(CreateSymbolTreeImages());
		m_winStack->Add(tree, filename.GetFullPath());
		m_winStack->Select(filename.GetFullPath());
	}
	tree->BuildTree(filename);
}

void WorkspacePane::BuildFileTree()
{
	m_workspaceTab->BuildFileTree();
}

SymbolTree *WorkspacePane::GetSymbolTree()
{
	int id = Frame::Get()->GetNotebook()->GetSelection();
	if (id != wxNOT_FOUND) {
		LEditor *editor = dynamic_cast<LEditor*>( Frame::Get()->GetNotebook()->GetPage((size_t)id));
		if (editor) {
			return GetTreeByFilename(editor->GetFileName());
		}
	}
	return NULL;
}

void WorkspacePane::DisplaySymbolTree(const wxFileName &filename)
{
	m_winStack->Select(filename.GetFullPath());
}

void WorkspacePane::DeleteSymbolTree(const wxFileName &filename)
{
	m_winStack->Delete(filename.GetFullPath());
}

void WorkspacePane::DeleteAllSymbolTrees()
{
	m_winStack->Clear();
}

FileViewTree* WorkspacePane::GetFileViewTree()
{
	return m_workspaceTab->GetFileView();
}

wxComboBox* WorkspacePane::GetConfigCombBox()
{
	return m_workspaceConfig;
}

void WorkspacePane::OnEraseBg(wxEraseEvent &e)
{
	wxUnusedVar(e);
}

void WorkspacePane::OnPaint(wxPaintEvent &e)
{
	wxUnusedVar(e);
	wxBufferedPaintDC dc(this);

	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	dc.DrawRectangle(GetClientSize());
}

void WorkspacePane::OnSize(wxSizeEvent &e)
{
	Refresh();
	e.Skip();
}
void WorkspacePane::CollpaseAll()
{
	m_workspaceTab->CollpaseAll();
}

void WorkspacePane::OnConfigurationManager(wxCommandEvent& e)
{
	wxUnusedVar(e);
	Frame::Get()->ShowBuildConfigurationManager();
}

void WorkspacePane::OnConfigurationManagerUI(wxUpdateUIEvent& e)
{
	e.Enable( ManagerST::Get()->IsWorkspaceOpen() );
}
