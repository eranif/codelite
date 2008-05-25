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
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
#include "globals.h"
#include "workspacetab.h"
#include "manager.h"
#include "fileview.h"
#include "wx/combobox.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "frame.h"
#include "macros.h"

WorkspaceTab::WorkspaceTab(wxWindow *parent)
: wxPanel(parent)
{
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
	
	wxBoxSizer *hsz = new wxBoxSizer(wxHORIZONTAL);
	
	//add toolbar on top of the workspace tab that includes a single button that collapse all 
	//tree items
	
	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL);
	
	tb->AddTool(XRCID("collapse_all"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("collapse")), wxT("Collapse All"), wxITEM_NORMAL);
	tb->Realize();
	
	Connect( XRCID("collapse_all"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WorkspaceTab::OnCollapseAll ));
	Connect( XRCID("collapse_all"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceTab::OnCollapseAllUI ));
	
	//add the workspace configuration combobox
	wxArrayString choices;
	m_workspaceConfig = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY|wxALIGN_CENTER_VERTICAL);
	m_workspaceConfig->Enable(false);
	
	// Connect an event to handle changes in the choice control
	ConnectCombo(m_workspaceConfig, Frame::OnWorkspaceConfigChanged);
	sz->Add(new wxStaticText(this, wxID_ANY, wxT("Configuration:")), 0, wxEXPAND| wxTOP | wxLEFT, 5);
	hsz->Add(m_workspaceConfig, 1, wxEXPAND);
	
	wxButton *btn = new wxButton(this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	hsz->Add(btn, 0, wxALIGN_CENTER_VERTICAL);
	sz->Add(hsz, 0, wxEXPAND|wxTOP, 5);
	
	ConnectButton(btn, WorkspaceTab::OnConfigurationManager);
	btn->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(WorkspaceTab::OnConfigurationManagerUI), NULL, this);
	
	//add the fileview tab
	m_fileView = new FileViewTree(this, wxID_ANY);
	sz->Add(m_fileView, 1, wxEXPAND|wxTOP, 2);
	sz->Add(tb, 0, wxEXPAND, 0);
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
if(ManagerST::Get()->IsWorkspaceOpen() == false) {
		return;
	}
	
	wxTreeItemId root = m_fileView->GetRootItem();
	if(root.IsOk() == false) {
		return;
	}
	
	if(m_fileView->ItemHasChildren(root) == false) {
		return;
	}
	
	//iterate over all the projects items and collapse them all
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_fileView->GetFirstChild(root, cookie);
	while( child.IsOk() ) {
		m_fileView->CollapseAllChildren(child);
		child = m_fileView->GetNextChild(root, cookie);
	}
}

void WorkspaceTab::CollpaseAll()
{
	DoCollpaseAll();
}

void WorkspaceTab::OnConfigurationManager(wxCommandEvent &e)
{
	wxUnusedVar(e);
	Frame::Get()->ShowBuildConfigurationManager();
}
void WorkspaceTab::OnConfigurationManagerUI(wxUpdateUIEvent &e)
{
	e.Enable( ManagerST::Get()->IsWorkspaceOpen() );
}
