//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : editoroptionsgeneralindetationpanelbase.cpp
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

#include "editoroptionsgeneralindetationpanelbase.h"

///////////////////////////////////////////////////////////////////////////

EditorOptionsGeneralIndetationPanelBase::EditorOptionsGeneralIndetationPanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxDisableSmartIdent = new wxCheckBox( this, wxID_ANY, _("Disable Smart Indentation"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_checkBoxDisableSmartIdent, 0, wxALL|wxEXPAND, 5 );
	
	m_indentsUsesTabs = new wxCheckBox( this, wxID_ANY, _("Use tabs in indentation"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_indentsUsesTabs, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Columns per indentation level:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_indentWidth = new wxTextCtrl( this, wxID_ANY, _("4"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_indentWidth, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Columns per tab character in document:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_tabWidth = new wxTextCtrl( this, wxID_ANY, _("4"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_tabWidth, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
}

EditorOptionsGeneralIndetationPanelBase::~EditorOptionsGeneralIndetationPanelBase()
{
}
