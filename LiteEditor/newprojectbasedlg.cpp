//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : newprojectbasedlg.cpp              
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
 ///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "newprojectbasedlg.h"

///////////////////////////////////////////////////////////////////////////

NewProjectBaseDlg::NewProjectBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	m_projTypes = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer15->Add( m_projTypes, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("Project Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer18->Add( m_staticText11, 0, wxALL, 5 );
	
	m_textCtrlProjName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_textCtrlProjName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, wxT("Project Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer18->Add( m_staticText12, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlProjectPath = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer19->Add( m_textCtrlProjectPath, 1, wxALL, 5 );
	
	m_buttonProjectDirPicker = new wxButton( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer19->Add( m_buttonProjectDirPicker, 0, wxALL, 5 );
	
	bSizer18->Add( bSizer19, 0, wxEXPAND, 5 );
	
	m_staticText14 = new wxStaticText( this, wxID_ANY, wxT("Compiler Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	bSizer18->Add( m_staticText14, 0, wxALL, 5 );
	
	wxArrayString m_choiceCompilerTypeChoices;
	m_choiceCompilerType = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceCompilerTypeChoices, 0 );
	m_choiceCompilerType->SetSelection( 0 );
	bSizer18->Add( m_choiceCompilerType, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxCreateSeparateDir = new wxCheckBox( this, wxID_ANY, wxT("Create the project under a separate directory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxCreateSeparateDir->SetValue(true);
	
	bSizer18->Add( m_checkBoxCreateSeparateDir, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("File Name:") ), wxVERTICAL );
	
	m_staticTextProjectFileFullPath = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextProjectFileFullPath->Wrap( -1 );
	sbSizer2->Add( m_staticTextProjectFileFullPath, 0, wxALL|wxEXPAND, 5 );
	
	bSizer18->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	m_textCtrlDescription = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
	m_textCtrlDescription->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	bSizer18->Add( m_textCtrlDescription, 1, wxALL|wxEXPAND, 5 );
	
	bSizer15->Add( bSizer18, 2, wxEXPAND, 5 );
	
	bSizer14->Add( bSizer15, 1, wxEXPAND, 5 );
	
	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer14->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonCreateProject = new wxButton( this, wxID_ANY, wxT("&Create"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonCreateProject->SetDefault(); 
	bSizer17->Add( m_buttonCreateProject, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer17->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer14->Add( bSizer17, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer14 );
	this->Layout();
	
	// Connect Events
	m_projTypes->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( NewProjectBaseDlg::OnItemSelected ), NULL, this );
	m_textCtrlProjName->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectPathUpdated ), NULL, this );
	m_textCtrlProjectPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectPathUpdated ), NULL, this );
	m_buttonProjectDirPicker->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectPathPicker ), NULL, this );
	m_checkBoxCreateSeparateDir->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectPathUpdated ), NULL, this );
	m_buttonCreateProject->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewProjectBaseDlg::OnButtonCreate ), NULL, this );
}

NewProjectBaseDlg::~NewProjectBaseDlg()
{
	// Disconnect Events
	m_projTypes->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( NewProjectBaseDlg::OnItemSelected ), NULL, this );
	m_textCtrlProjName->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectPathUpdated ), NULL, this );
	m_textCtrlProjectPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectPathUpdated ), NULL, this );
	m_buttonProjectDirPicker->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectPathPicker ), NULL, this );
	m_checkBoxCreateSeparateDir->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectPathUpdated ), NULL, this );
	m_buttonCreateProject->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewProjectBaseDlg::OnButtonCreate ), NULL, this );
}
