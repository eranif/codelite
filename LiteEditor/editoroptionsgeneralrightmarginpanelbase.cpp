//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : editoroptionsgeneralrightmarginpanelbase.cpp
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
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editoroptionsgeneralrightmarginpanelbase.h"

///////////////////////////////////////////////////////////////////////////

EditorOptionsGeneralRightMarginPanelBase::EditorOptionsGeneralRightMarginPanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtnRMDisabled = new wxRadioButton( this, wxID_ANY, _("Disabled"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_radioBtnRMDisabled->SetValue( true ); 
	bSizer4->Add( m_radioBtnRMDisabled, 0, wxALL, 5 );
	
	m_radioBtnRMLine = new wxRadioButton( this, wxID_ANY, _("Line"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_radioBtnRMLine, 0, wxALL, 5 );
	
	m_radioBtnRMBackground = new wxRadioButton( this, wxID_ANY, _("Background"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_radioBtnRMBackground, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer4, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Indicator Column"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer2->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_rightMarginColumn = new wxTextCtrl( this, wxID_ANY, _("80"), wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer2->Add( m_rightMarginColumn, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Indicator Colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer2->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_rightMarginColour = new wxColourPickerCtrl( this, wxID_ANY, wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE|wxCLRP_SHOW_LABEL );
	fgSizer2->Add( m_rightMarginColour, 0, wxALL|wxRIGHT|wxALIGN_RIGHT, 5 );
	
	bSizer3->Add( fgSizer2, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
	
	// Connect Events
	m_radioBtnRMDisabled->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditorOptionsGeneralRightMarginPanelBase::OnRightMarginIndicator ), NULL, this );
	m_radioBtnRMLine->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditorOptionsGeneralRightMarginPanelBase::OnRightMarginIndicator ), NULL, this );
	m_radioBtnRMBackground->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditorOptionsGeneralRightMarginPanelBase::OnRightMarginIndicator ), NULL, this );
}

EditorOptionsGeneralRightMarginPanelBase::~EditorOptionsGeneralRightMarginPanelBase()
{
	// Disconnect Events
	m_radioBtnRMDisabled->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditorOptionsGeneralRightMarginPanelBase::OnRightMarginIndicator ), NULL, this );
	m_radioBtnRMLine->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditorOptionsGeneralRightMarginPanelBase::OnRightMarginIndicator ), NULL, this );
	m_radioBtnRMBackground->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( EditorOptionsGeneralRightMarginPanelBase::OnRightMarginIndicator ), NULL, this );
	
}
