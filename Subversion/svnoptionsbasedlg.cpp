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
// C++ code generated with wxFormBuilder (version Apr 16 2008)
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
	bSizer5->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_filePicker = new wxFilePickerCtrl( m_mainPanel, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	bSizer5->Add( m_filePicker, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
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
	
	panelSizer->Add( bSizer8, 0, wxEXPAND, 5 );
	
	m_staticline3 = new wxStaticLine( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	panelSizer->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText12 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("External Diff Viewer:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	fgSizer2->Add( m_staticText12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_diffExe = new wxFilePickerCtrl( m_mainPanel, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	fgSizer2->Add( m_diffExe, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText13 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("Arguments:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	fgSizer2->Add( m_staticText13, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_diffArgs = new wxTextCtrl( m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_diffArgs, 0, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 5, 30 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText4 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("%base"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxLEFT, 20 );
	
	m_staticText5 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("The original file without your changes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, 0, 5 );
	
	m_staticText6 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("%bname"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxLEFT, 20 );
	
	m_staticText7 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("The window title for the base file"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer1->Add( m_staticText7, 0, 0, 5 );
	
	m_staticText8 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("%mine"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer1->Add( m_staticText8, 0, wxLEFT, 20 );
	
	m_staticText9 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("Your own file, with your changes "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer1->Add( m_staticText9, 0, 0, 5 );
	
	m_staticText10 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("%mname"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizer1->Add( m_staticText10, 0, wxLEFT, 20 );
	
	m_staticText11 = new wxStaticText( m_mainPanel, wxID_ANY, wxT("The window title for your file"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer1->Add( m_staticText11, 0, 0, 5 );
	
	fgSizer2->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	bSizer11->Add( fgSizer2, 1, wxEXPAND|wxALL, 5 );
	
	bSizer9->Add( bSizer11, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer12->Add( 20, 0, 0, 0, 5 );
	
	bSizer9->Add( bSizer12, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer10->Add( 45, 0, 0, 0, 5 );
	
	bSizer9->Add( bSizer10, 1, wxEXPAND, 5 );
	
	m_checkBoxCaptureDiffOutput = new wxCheckBox( m_mainPanel, wxID_ANY, wxT("Capture Output From External Diff Viewer"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer9->Add( m_checkBoxCaptureDiffOutput, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );
	
	panelSizer->Add( bSizer9, 0, wxEXPAND, 5 );
	
	m_staticline4 = new wxStaticLine( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	panelSizer->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
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
	bSizer1->Add( m_mainPanel, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	m_buttonSizer = new wxStdDialogButtonSizer();
	m_buttonSizerOK = new wxButton( this, wxID_OK );
	m_buttonSizer->AddButton( m_buttonSizerOK );
	m_buttonSizerCancel = new wxButton( this, wxID_CANCEL );
	m_buttonSizer->AddButton( m_buttonSizerCancel );
	m_buttonSizer->Realize();
	bSizer1->Add( m_buttonSizer, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_checkBoxUseIconsInWorkspace->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnSvnUseIcons ), NULL, this );
	m_checkBoxAutoAddNewFiles->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnAutoAddNewFiles ), NULL, this );
	m_buttonSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnButtonOk ), NULL, this );
}

SvnOptionsBaseDlg::~SvnOptionsBaseDlg()
{
	// Disconnect Events
	m_checkBoxUseIconsInWorkspace->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnSvnUseIcons ), NULL, this );
	m_checkBoxAutoAddNewFiles->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnAutoAddNewFiles ), NULL, this );
	m_buttonSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnOptionsBaseDlg::OnButtonOk ), NULL, this );
}
