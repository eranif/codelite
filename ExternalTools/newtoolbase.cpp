//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : newtoolbase.cpp
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

#include "newtoolbase.h"

///////////////////////////////////////////////////////////////////////////

NewToolBase::NewToolBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	bSizer1->SetMinSize( wxSize( 700,-1 ) ); 
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 5, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("ID:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceIdChoices;
	m_choiceId = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceIdChoices, 0 );
	m_choiceId->SetSelection( 0 );
	fgSizer1->Add( m_choiceId, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlName->SetToolTip( _("The name is used to identify this tool in the 'External Tools' toobar") );
	
	fgSizer1->Add( m_textCtrlName, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Tool path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlPath = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlPath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowsePath = new wxButton( this, wxID_ANY, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonBrowsePath, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Working directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlWd = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlWd, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseWd = new wxButton( this, wxID_ANY, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonBrowseWd, 0, wxALL, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Arguments:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlArguments = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlArguments, 1, wxALL|wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Toolbar:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, _("Toolbar icon (24x24):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer2->Add( m_staticText8, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_textCtrlIcon24 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrlIcon24, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseIcon24 = new wxButton( this, wxID_ANY, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_buttonBrowseIcon24, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, _("Toolbar icon (16x16):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer2->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_textCtrlIcon16 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrlIcon16, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseIcon16 = new wxButton( this, wxID_ANY, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_buttonBrowseIcon16, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	sbSizer1->Add( fgSizer2, 0, wxEXPAND|wxALL, 5 );
	
	bSizer1->Add( sbSizer1, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("General:") ), wxVERTICAL );
	
	m_checkBoxSaveAllFilesBefore = new wxCheckBox( this, wxID_ANY, _("Save all files before executing this tool"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkBoxSaveAllFilesBefore, 0, wxALL, 5 );
	
	m_checkBoxCaptureProcessOutput = new wxCheckBox( this, wxID_ANY, _("Capture process output"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkBoxCaptureProcessOutput, 0, wxALL, 5 );
	
	bSizer1->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	bSizer2->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );
	
	m_buttonHelp = new wxButton( this, wxID_ANY, _("Help..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonHelp, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_choiceId->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( NewToolBase::OnIdSelected ), NULL, this );
	m_buttonBrowsePath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonBrowsePath ), NULL, this );
	m_buttonBrowseWd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonBrowseWD ), NULL, this );
	m_buttonBrowseIcon24->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonBrowseIcon24 ), NULL, this );
	m_buttonBrowseIcon16->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonBrowseIcon16 ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonOk ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonCancel ), NULL, this );
	m_buttonHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonHelp ), NULL, this );
}

NewToolBase::~NewToolBase()
{
	// Disconnect Events
	m_choiceId->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( NewToolBase::OnIdSelected ), NULL, this );
	m_buttonBrowsePath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonBrowsePath ), NULL, this );
	m_buttonBrowseWd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonBrowseWD ), NULL, this );
	m_buttonBrowseIcon24->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonBrowseIcon24 ), NULL, this );
	m_buttonBrowseIcon16->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonBrowseIcon16 ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonOk ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonCancel ), NULL, this );
	m_buttonHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewToolBase::OnButtonHelp ), NULL, this );
	
}
