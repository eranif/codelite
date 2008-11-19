//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : errorscontainer.cpp
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

#include <wx/button.h>
#include <wx/tglbtn.h>
#include <wx/sizer.h>
#include <wx/panel.h>

#include <wx/log.h>

#include "errorscontainer.h"
#include "errorstab.h"

const wxString ErrorsContainer::ERRORS_WIN = wxT("Errors");

ErrorsContainer::ErrorsContainer( wxWindow *win, wxWindowID id )
		: wxPanel(win, id)
		, m_errorsTab(NULL)
{
	Initialize();
}

ErrorsContainer::~ErrorsContainer()
{
	m_buttonErrors->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ErrorsContainer::OnErrorButton   ), NULL, this );
	m_buttonWarnings->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ErrorsContainer::OnWarningButton ), NULL, this );
}

void ErrorsContainer::Initialize()
{
	wxBoxSizer* sz = new wxBoxSizer( wxVERTICAL );
	wxFlexGridSizer* fsz = new wxFlexGridSizer( 1, 7, 0, 0 );

	fsz->AddGrowableCol        ( 1 );
	fsz->AddGrowableCol        ( 3 );
	fsz->SetFlexibleDirection  ( wxBOTH );
	fsz->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );

	m_buttonErrors   = new wxCheckBox( this, wxID_ANY, wxT("Display errors"));
	m_buttonWarnings = new wxCheckBox( this, wxID_ANY, wxT("Display warnings"));

	// By default, enable both errors and warnings, so make sure buttons are in a pressed state
	m_buttonErrors  ->SetValue( true );
	m_buttonWarnings->SetValue( true );

	m_errorsTab = new ErrorsTab(this, wxID_ANY, ErrorsContainer::ERRORS_WIN);

	fsz->Add( m_buttonErrors,   0, wxALL|wxALIGN_CENTER_VERTICAL,            5 );
	fsz->Add( m_buttonWarnings, 0, wxALL|wxALIGN_CENTER_VERTICAL,            5 );
	sz ->Add( fsz,              0, wxEXPAND,         5 );
	sz ->Add( m_errorsTab,      0, wxALL | wxEXPAND, 5 );

	this->SetSizer( sz );
	this->Layout();

	m_buttonErrors  ->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ErrorsContainer::OnErrorButton   ), NULL, this );
	m_buttonWarnings->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ErrorsContainer::OnWarningButton ), NULL, this );
}

// Error button state is persistent for a session.
void ErrorsContainer::OnErrorButton( wxCommandEvent& event )
{
	wxUnusedVar(event);

	// By default, both errors and warnings are enabled, so the first
	// time the error button is pressed, disable showing errors
	static bool pressed = false;

	m_buttonErrors->SetValue( pressed );
	if ( pressed ) {
		m_errorsTab->SetViewType( m_errorsTab->GetViewType() | ErrorsTab::TYPE_ERROR );
	} else {
		m_errorsTab->SetViewType( m_errorsTab->GetViewType() & (~ErrorsTab::TYPE_ERROR) );
	}
	pressed = !pressed;
}

// Warning button state is persistent for a session.
void ErrorsContainer::OnWarningButton( wxCommandEvent& event )
{
	wxUnusedVar(event);

	// By default, both errors and warnings are enabled, so the first
	// time the warning button is pressed, disable showing warnings
	static bool pressed = false;

	m_buttonWarnings->SetValue( pressed );
	if ( pressed ) {
		m_errorsTab->SetViewType( m_errorsTab->GetViewType() | ErrorsTab::TYPE_WARNING );
	} else {
		m_errorsTab->SetViewType( m_errorsTab->GetViewType() & (~ErrorsTab::TYPE_WARNING) );
	}
	pressed = !pressed;
}

void ErrorsContainer::AppendText( const wxString &text )
{
	if (m_errorsTab != NULL) {
		m_errorsTab->AppendText( text );
	}
}

void ErrorsContainer::Clear()
{
	if (m_errorsTab != NULL) {
		m_errorsTab->Clear();
	}
}
