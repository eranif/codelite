//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : editorsettingsfoldingbase.cpp
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

#include "editorsettingsfoldingbase.h"

///////////////////////////////////////////////////////////////////////////

EditorSettingsFoldingBase::EditorSettingsFoldingBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_displayMargin = new wxCheckBox( this, wxID_ANY, _("Display Folding Margin"), wxDefaultPosition, wxDefaultSize, 0 );
	m_displayMargin->SetValue(true); 
	mainSizer->Add( m_displayMargin, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_underlineFolded = new wxCheckBox( this, wxID_ANY, _("Underline Folded Line"), wxDefaultPosition, wxDefaultSize, 0 );
	m_underlineFolded->SetValue(true); 
	fgSizer1->Add( m_underlineFolded, 0, wxALL|wxEXPAND, 5 );
	
	m_foldElse = new wxCheckBox( this, wxID_ANY, _("Fold At Else"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_foldElse, 0, wxALL|wxEXPAND, 5 );
	
	m_foldPreprocessors = new wxCheckBox( this, wxID_ANY, _("Fold Preprocessors"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_foldPreprocessors, 0, wxALL|wxEXPAND, 5 );
	
	m_foldCompact = new wxCheckBox( this, wxID_ANY, _("Fold Compact"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_foldCompact, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Fold Style:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_foldStyleChoices;
	m_foldStyle = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_foldStyleChoices, 0 );
	m_foldStyle->SetSelection( 0 );
	fgSizer1->Add( m_foldStyle, 0, wxALL|wxEXPAND, 5 );
	
	sbSizer1->Add( fgSizer1, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	mainSizer->Add( sbSizer1, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_underlineFolded->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldElse->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldPreprocessors->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldCompact->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_staticText2->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldStyle->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
}

EditorSettingsFoldingBase::~EditorSettingsFoldingBase()
{
	// Disconnect Events
	m_underlineFolded->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldElse->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldPreprocessors->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldCompact->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_staticText2->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldStyle->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	
}
