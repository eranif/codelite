//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : newwxprojectbasedlg.cpp
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

#include "newwxprojectbasedlg.h"

///////////////////////////////////////////////////////////////////////////

NewWxProjectBaseDlg::NewWxProjectBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_bitmapPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_bitmapPanel->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_bitmap1 = new wxStaticBitmap( m_bitmapPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_bitmap1, 0, wxEXPAND, 0 );
	
	m_bitmapPanel->SetSizer( bSizer7 );
	m_bitmapPanel->Layout();
	bSizer7->Fit( m_bitmapPanel );
	bSizer1->Add( m_bitmapPanel, 0, wxEXPAND, 0 );
	
	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline4, 0, wxEXPAND|wxBOTTOM, 5 );
	
	m_flatNotebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_flatNotebook1->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	m_panelBasicInfo = new wxPanel( m_flatNotebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* basicSizer;
	basicSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_panelBasicInfo, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlName = new wxTextCtrl( m_panelBasicInfo, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlName->SetToolTip( _("Choose a name to give the project") );
	
	fgSizer1->Add( m_textCtrlName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( m_panelBasicInfo, wxID_ANY, _("Project Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_dirPicker = new DirPicker(m_panelBasicInfo);
	fgSizer1->Add( m_dirPicker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( m_panelBasicInfo, wxID_ANY, _("Application Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceApplicationTypeChoices[] = { _("Simple main with wxWidgets enabled"), _("GUI application with Main Frame"), _("GUI frame-based application (wxFormBuilder)"), _("GUI dialog-based application (wxFormBuilder)") };
	int m_choiceApplicationTypeNChoices = sizeof( m_choiceApplicationTypeChoices ) / sizeof( wxString );
	m_choiceApplicationType = new wxChoice( m_panelBasicInfo, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceApplicationTypeNChoices, m_choiceApplicationTypeChoices, 0 );
	m_choiceApplicationType->SetSelection( 0 );
	m_choiceApplicationType->SetToolTip( _("The normal choice will be 'GUI application', but choose 'Simple main' for a wx console app") );
	m_choiceApplicationType->SetMinSize( wxSize( -1,30 ) );
	
	fgSizer1->Add( m_choiceApplicationType, 0, wxALL|wxEXPAND, 5 );
	
	basicSizer->Add( fgSizer1, 1, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText4 = new wxStaticText( m_panelBasicInfo, wxID_ANY, _("Advanced Settings:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer4->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticline2 = new wxStaticLine( m_panelBasicInfo, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer4->Add( m_staticline2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	basicSizer->Add( bSizer4, 0, wxEXPAND|wxALL, 5 );
	
	m_checkBoxCreateSeparateDir = new wxCheckBox( m_panelBasicInfo, wxID_ANY, _("Create the project under a separate directory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxCreateSeparateDir->SetValue(true); 
	basicSizer->Add( m_checkBoxCreateSeparateDir, 0, wxALL, 5 );
	
	m_checkBoxPCH = new wxCheckBox( m_panelBasicInfo, wxID_ANY, _("Use precompiled headers"), wxDefaultPosition, wxDefaultSize, 0 );
	basicSizer->Add( m_checkBoxPCH, 0, wxALL, 5 );
	
	m_checkBoxWinRes = new wxCheckBox( m_panelBasicInfo, wxID_ANY, _("Use MS Windows resources"), wxDefaultPosition, wxDefaultSize, 0 );
	basicSizer->Add( m_checkBoxWinRes, 0, wxALL, 5 );
	
	m_checkBoxMWindows = new wxCheckBox( m_panelBasicInfo, wxID_ANY, _("Add -mwindows flag to avoid a terminal console in MSWin GUI apps"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxMWindows->SetToolTip( _("Without this flag, there will be an unnecessary, visible terminal window when your app runs on MSWindows") );
	
	basicSizer->Add( m_checkBoxMWindows, 0, wxALL, 5 );
	
	m_panelBasicInfo->SetSizer( basicSizer );
	m_panelBasicInfo->Layout();
	basicSizer->Fit( m_panelBasicInfo );
	m_flatNotebook1->AddPage( m_panelBasicInfo, _("Basics"), true );
	m_panelAdvancedInfo = new wxPanel( m_flatNotebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* advancedSizer;
	advancedSizer = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxUnicode = new wxCheckBox( m_panelAdvancedInfo, wxID_ANY, _("Use Unicode Build of wxWidgets"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxUnicode->SetValue(true); 
	m_checkBoxUnicode->SetToolTip( _("Uses an unicode configuration if found.") );
	
	advancedSizer->Add( m_checkBoxUnicode, 0, wxALL, 5 );
	
	m_checkBoxStatic = new wxCheckBox( m_panelAdvancedInfo, wxID_ANY, _("Use Static wxWidgets libraries"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxStatic->SetToolTip( _("Uses a static configuration if found.") );
	
	advancedSizer->Add( m_checkBoxStatic, 0, wxALL, 5 );
	
	m_checkBoxUniversal = new wxCheckBox( m_panelAdvancedInfo, wxID_ANY, _("Use Universal wxWidgets libraries"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxUniversal->SetToolTip( _("Uses an universal configuration if found.") );
	
	advancedSizer->Add( m_checkBoxUniversal, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText6 = new wxStaticText( m_panelAdvancedInfo, wxID_ANY, _("Version:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer2->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceVersionChoices;
	m_choiceVersion = new wxChoice( m_panelAdvancedInfo, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceVersionChoices, 0 );
	m_choiceVersion->SetSelection( 0 );
	m_choiceVersion->SetToolTip( _("Use selected wxWidgets version.") );
	
	fgSizer2->Add( m_choiceVersion, 0, wxALL, 5 );
	
	m_staticText7 = new wxStaticText( m_panelAdvancedInfo, wxID_ANY, _("Prefix:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer2->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlPrefix = new wxTextCtrl( m_panelAdvancedInfo, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlPrefix->SetToolTip( _("Path of the wxWidgets installation (optional).") );
	
	fgSizer2->Add( m_textCtrlPrefix, 0, wxALL|wxEXPAND, 5 );
	
	advancedSizer->Add( fgSizer2, 0, wxEXPAND|wxLEFT, 5 );
	
	m_panelAdvancedInfo->SetSizer( advancedSizer );
	m_panelAdvancedInfo->Layout();
	advancedSizer->Fit( m_panelAdvancedInfo );
	m_flatNotebook1->AddPage( m_panelAdvancedInfo, _("wxWidgets settings"), false );
	
	bSizer1->Add( m_flatNotebook1, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer3->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button2, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_choiceApplicationType->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( NewWxProjectBaseDlg::OnChoiceChanged ), NULL, this );
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewWxProjectBaseDlg::OnButtonOK ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewWxProjectBaseDlg::OnButtonCancel ), NULL, this );
}

NewWxProjectBaseDlg::~NewWxProjectBaseDlg()
{
	// Disconnect Events
	m_choiceApplicationType->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( NewWxProjectBaseDlg::OnChoiceChanged ), NULL, this );
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewWxProjectBaseDlg::OnButtonOK ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewWxProjectBaseDlg::OnButtonCancel ), NULL, this );
	
}
