//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : addoptioncheckdlgbase.cpp
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

#include "addoptioncheckdlgbase.h"

///////////////////////////////////////////////////////////////////////////

AddOptionCheckDialogBase::AddOptionCheckDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Check the command line options you need"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	mainSizer->Add( m_staticText1, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* optionsSizer;
	optionsSizer = new wxBoxSizer( wxVERTICAL );
	
	wxArrayString m_checkListOptionsChoices;
	m_checkListOptions = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListOptionsChoices, wxLB_EXTENDED|wxLB_NEEDED_SB|wxLB_SORT );
	optionsSizer->Add( m_checkListOptions, 2, wxALL|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Enter other options"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	optionsSizer->Add( m_staticText4, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textOptions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	optionsSizer->Add( m_textOptions, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* cmdlnSizer;
	cmdlnSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Command line") ), wxVERTICAL );
	
	m_textCmdLn = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,50 ), wxTE_MULTILINE|wxTE_NO_VSCROLL|wxTE_READONLY|wxNO_BORDER );
	m_textCmdLn->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_textCmdLn->SetMinSize( wxSize( -1,50 ) );
	
	cmdlnSizer->Add( m_textCmdLn, 0, wxEXPAND, 5 );
	
	optionsSizer->Add( cmdlnSizer, 0, wxEXPAND|wxALL, 5 );
	
	mainSizer->Add( optionsSizer, 1, wxEXPAND, 5 );
	
	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();
	mainSizer->Add( m_sdbSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_checkListOptions->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( AddOptionCheckDialogBase::OnOptionToggled ), NULL, this );
	m_textOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AddOptionCheckDialogBase::OnOptionsText ), NULL, this );
}

AddOptionCheckDialogBase::~AddOptionCheckDialogBase()
{
	// Disconnect Events
	m_checkListOptions->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( AddOptionCheckDialogBase::OnOptionToggled ), NULL, this );
	m_textOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AddOptionCheckDialogBase::OnOptionsText ), NULL, this );
	
}
