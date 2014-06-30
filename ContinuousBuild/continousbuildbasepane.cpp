//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : continousbuildbasepane.cpp
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

#include "continousbuildbasepane.h"

///////////////////////////////////////////////////////////////////////////

ContinousBuildBasePane::ContinousBuildBasePane( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_checkBox1 = new wxCheckBox( this, wxID_ANY, _("Enable continous build"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_checkBox1, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	
	bSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_ANY, _("&Clear All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonCancel->SetDefault(); 
	bSizer5->Add( m_buttonCancel, 0, wxEXPAND|wxALL, 5 );
	
	mainSizer->Add( bSizer5, 0, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText31 = new wxStaticText( this, wxID_ANY, _("Queued Files:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	m_staticText31->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer6->Add( m_staticText31, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_listBoxQueue = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer6->Add( m_listBoxQueue, 1, wxEXPAND|wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer51->Add( bSizer6, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Failed Files:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	m_staticText4->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	m_staticText4->SetForegroundColour( wxColour( 255, 0, 0 ) );
	m_staticText4->Hide();
	
	bSizer9->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_listBoxFailedFiles = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_listBoxFailedFiles->Hide();
	
	bSizer9->Add( m_listBoxFailedFiles, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer51->Add( bSizer9, 1, wxEXPAND, 5 );
	
	mainSizer->Add( bSizer51, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_checkBox1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ContinousBuildBasePane::OnEnableCB ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ContinousBuildBasePane::OnStopAll ), NULL, this );
	m_buttonCancel->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnStopUI ), NULL, this );
	m_staticText31->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnEnableContBuildUI ), NULL, this );
	m_listBoxQueue->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnEnableContBuildUI ), NULL, this );
	m_staticText4->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnEnableContBuildUI ), NULL, this );
	m_listBoxFailedFiles->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnEnableContBuildUI ), NULL, this );
}

ContinousBuildBasePane::~ContinousBuildBasePane()
{
	// Disconnect Events
	m_checkBox1->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ContinousBuildBasePane::OnEnableCB ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ContinousBuildBasePane::OnStopAll ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnStopUI ), NULL, this );
	m_staticText31->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnEnableContBuildUI ), NULL, this );
	m_listBoxQueue->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnEnableContBuildUI ), NULL, this );
	m_staticText4->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnEnableContBuildUI ), NULL, this );
	m_listBoxFailedFiles->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ContinousBuildBasePane::OnEnableContBuildUI ), NULL, this );
	
}
