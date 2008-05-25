//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : svnoptionsbasedlg.cpp              
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
 ///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 20 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "svnoptionsbasedlg.h"

///////////////////////////////////////////////////////////////////////////

SvnOptionsBaseDlg::SvnOptionsBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* panelSizer;
	panelSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("SVN executable:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer5->Add( m_staticText1, 0, wxALL, 5 );
	
	m_filePicker = new FilePicker(m_mainPanel, wxID_ANY);
	bSizer5->Add( m_filePicker, 1, wxALL|wxEXPAND, 5 );
	
	panelSizer->Add( bSizer5, 0, wxEXPAND, 5 );
	
	m_staticline = new wxStaticLine( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	panelSizer->Add( m_staticline, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxUseIconsInWorkspace = new wxCheckBox( m_mainPanel, wxID_ANY, wxT("Use Svn icons in the workspace tree view (Workspace reload is required to remove the icons)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxUseIconsInWorkspace->SetValue(true);
	
	bSizer8->Add( m_checkBoxUseIconsInWorkspace, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer6->Add( 20, 0, 0, 0, 5 );
	
	m_checkBoxKeepIconsAutoUpdate = new wxCheckBox( m_mainPanel, wxID_ANY, wxT("Update Svn icons after adding / removing / deleting item from the workspace tree"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxKeepIconsAutoUpdate->SetValue(true);
	
	bSizer6->Add( m_checkBoxKeepIconsAutoUpdate, 0, wxALL, 5 );
	
	bSizer8->Add( bSizer6, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer61->Add( 20, 0, 0, 0, 5 );
	
	m_checkBoxUpdateAfterSave = new wxCheckBox( m_mainPanel, wxID_ANY, wxT("Update Svn icons after saving a file"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer61->Add( m_checkBoxUpdateAfterSave, 0, wxALL, 5 );
	
	bSizer8->Add( bSizer61, 0, wxEXPAND, 5 );
	
	m_checkBoxAutoAddNewFiles = new wxCheckBox( m_mainPanel, wxID_ANY, wxT("When adding files to project, add them to Svn as well"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxAutoAddNewFiles->SetValue(true);
	
	bSizer8->Add( m_checkBoxAutoAddNewFiles, 0, wxALL, 5 );
	
	panelSizer->Add( bSizer8, 1, wxEXPAND, 5 );
	
	m_staticline3 = new wxStaticLine( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	panelSizer->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText3 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("Svn ignore file patterns:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer7->Add( m_staticText3, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrl1 = new wxTextCtrl( m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_textCtrl1, 1, wxALL|wxEXPAND, 5 );
	
	panelSizer->Add( bSizer7, 0, wxEXPAND, 5 );
	
	m_mainPanel->SetSizer( panelSizer );
	m_mainPanel->Layout();
	panelSizer->Fit( m_mainPanel );
	bSizer1->Add( m_mainPanel, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_checkBoxUseIconsInWorkspace->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnSvnUseIcons ), NULL, this );
	m_checkBoxAutoAddNewFiles->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnAutoAddNewFiles ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnButtonOk ), NULL, this );
}

SvnOptionsBaseDlg::~SvnOptionsBaseDlg()
{
	// Disconnect Events
	m_checkBoxUseIconsInWorkspace->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnSvnUseIcons ), NULL, this );
	m_checkBoxAutoAddNewFiles->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnAutoAddNewFiles ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnButtonOk ), NULL, this );
}
