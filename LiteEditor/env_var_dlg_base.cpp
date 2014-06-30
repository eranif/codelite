//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : env_var_dlg_base.cpp
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
// C++ code generated with wxFormBuilder (version Mar 22 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "env_var_dlg_base.h"

///////////////////////////////////////////////////////////////////////////

EnvVarsTableDlgBase::EnvVarsTableDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Define here set of environment variables which will be applied by CodeLite before launching processes.\n\nVariables are defined in the format of NAME=VALUE"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	sbSizer1->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Available environment sets:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	
	bSizer1->Add( m_staticText2, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook1 = new wxChoicebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );
	m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrlDefault = new wxStyledTextCtrl(m_panel1);
	bSizer5->Add( m_textCtrlDefault, 1, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer5 );
	m_panel1->Layout();
	bSizer5->Fit( m_panel1 );
	m_notebook1->AddPage( m_panel1, _("Default"), false );
	bSizer2->Add( m_notebook1, 1, wxEXPAND|wxALL, 5 );
	
	bSizer6->Add( bSizer2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNewSet = new wxButton( this, wxID_ANY, _("&New Set..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_buttonNewSet, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonDeleteSet = new wxButton( this, wxID_ANY, _("Delete Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_buttonDeleteSet, 0, wxALL|wxEXPAND, 5 );
	
	m_button5 = new wxButton( this, wxID_ANY, _("&Export"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button5->SetToolTip( _("Export the current set to a platform \nspecific environment file") );
	
	bSizer7->Add( m_button5, 0, wxALL|wxEXPAND, 5 );
	
	bSizer6->Add( bSizer7, 0, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer6, 1, wxEXPAND, 5 );
	
	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	bSizer3->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_buttonNewSet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlgBase::OnNewSet ), NULL, this );
	m_buttonDeleteSet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlgBase::OnDeleteSet ), NULL, this );
	m_buttonDeleteSet->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EnvVarsTableDlgBase::OnDeleteSetUI ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlgBase::OnExport ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlgBase::OnButtonOk ), NULL, this );
}

EnvVarsTableDlgBase::~EnvVarsTableDlgBase()
{
	// Disconnect Events
	m_buttonNewSet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlgBase::OnNewSet ), NULL, this );
	m_buttonDeleteSet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlgBase::OnDeleteSet ), NULL, this );
	m_buttonDeleteSet->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EnvVarsTableDlgBase::OnDeleteSetUI ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlgBase::OnExport ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlgBase::OnButtonOk ), NULL, this );
	
}

EnvVarSetPage::EnvVarSetPage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl = new wxStyledTextCtrl(this);
	bSizer6->Add( m_textCtrl, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer6 );
	this->Layout();
}

EnvVarSetPage::~EnvVarSetPage()
{
}
