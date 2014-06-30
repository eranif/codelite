//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : debugcoredumpbase.cpp
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
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "debugcoredumpbase.h"

///////////////////////////////////////////////////////////////////////////

DebugCoreDumpDlgBase::DebugCoreDumpDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Core dump to be opened:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Core = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_Core->SetToolTip( _("Enter the full filepath of the core dump to be examined.\nOr, if you enter the correct working directory below, just the filename will suffice.") );
	
	fgSizer1->Add( m_Core, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseCore = new wxButton( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonBrowseCore, 0, wxALL, 5 );
	
	m_staticText21 = new wxStaticText( this, wxID_ANY, _("Corresponding executable:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	fgSizer1->Add( m_staticText21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_ExeFilepath = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_ExeFilepath->SetToolTip( _("Enter the full filepath of the executable that crashed to cause the core dump.\nOr, if you enter the correct working directory below, just the filename will suffice.") );
	
	fgSizer1->Add( m_ExeFilepath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseExe = new wxButton( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonBrowseExe, 0, wxALL, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Working directory (optional):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_WD = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer1->Add( m_WD, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseWD = new wxButton( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonBrowseWD, 0, wxALL, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Select debugger:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceDebuggersChoices;
	m_choiceDebuggers = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDebuggersChoices, 0 );
	m_choiceDebuggers->SetSelection( 0 );
	fgSizer1->Add( m_choiceDebuggers, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer3->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonDebug = new wxButton( this, wxID_ANY, _("Debug"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonDebug, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_buttonBrowseCore->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpDlgBase::OnButtonBrowseCore ), NULL, this );
	m_buttonBrowseExe->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpDlgBase::OnButtonBrowseExe ), NULL, this );
	m_buttonBrowseWD->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpDlgBase::OnButtonBrowseWD ), NULL, this );
	m_buttonDebug->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpDlgBase::OnButtonDebug ), NULL, this );
	m_buttonDebug->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebugCoreDumpDlgBase::OnDebugBtnUpdateUI ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpDlgBase::OnButtonCancel ), NULL, this );
}

DebugCoreDumpDlgBase::~DebugCoreDumpDlgBase()
{
	// Disconnect Events
	m_buttonBrowseCore->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpDlgBase::OnButtonBrowseCore ), NULL, this );
	m_buttonBrowseExe->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpDlgBase::OnButtonBrowseExe ), NULL, this );
	m_buttonBrowseWD->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpDlgBase::OnButtonBrowseWD ), NULL, this );
	m_buttonDebug->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpDlgBase::OnButtonDebug ), NULL, this );
	m_buttonDebug->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebugCoreDumpDlgBase::OnDebugBtnUpdateUI ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugCoreDumpDlgBase::OnButtonCancel ), NULL, this );
	
}
