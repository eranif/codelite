//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
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
// C++ code generated with wxFormBuilder (version Mar 19 2008)
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
	
	m_flatNotebook1 = new wxNotebook( this, wxID_ANY );
	m_flatNotebook1->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	m_panel2 = new wxPanel( m_flatNotebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_panelBasicInfo = new wxPanel( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_panelBasicInfo, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlName = new wxTextCtrl( m_panelBasicInfo, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( m_panelBasicInfo, wxID_ANY, wxT("Project Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_dirPicker = new DirPicker(m_panelBasicInfo);
	fgSizer1->Add( m_dirPicker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( m_panelBasicInfo, wxID_ANY, wxT("Application Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceApplicationTypeChoices[] = { wxT("GUI application with Main Frame"), wxT("Simple main with wxWidgets enabled") };
	int m_choiceApplicationTypeNChoices = sizeof( m_choiceApplicationTypeChoices ) / sizeof( wxString );
	m_choiceApplicationType = new wxChoice( m_panelBasicInfo, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceApplicationTypeNChoices, m_choiceApplicationTypeChoices, 0 );
	m_choiceApplicationType->SetSelection( 0 );
	fgSizer1->Add( m_choiceApplicationType, 0, wxALL|wxEXPAND, 5 );
	
	bSizer2->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText4 = new wxStaticText( m_panelBasicInfo, wxID_ANY, wxT("wxWidgets Settings:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer4->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticline2 = new wxStaticLine( m_panelBasicInfo, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer4->Add( m_staticline2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer2->Add( bSizer4, 0, wxEXPAND, 5 );
	
	m_checkBoxUnicode = new wxCheckBox( m_panelBasicInfo, wxID_ANY, wxT("Use Unicode Build of wxWidgets"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxUnicode->SetValue(true);
	
	bSizer2->Add( m_checkBoxUnicode, 0, wxALL, 5 );
	
	m_checkBoxMWindows = new wxCheckBox( m_panelBasicInfo, wxID_ANY, wxT("When creating GUI application, add flag -mwindows to avoid terminal console (Windows Only)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxMWindows->SetValue(true);
	
	bSizer2->Add( m_checkBoxMWindows, 0, wxALL, 5 );
	
	m_panelBasicInfo->SetSizer( bSizer2 );
	m_panelBasicInfo->Layout();
	bSizer2->Fit( m_panelBasicInfo );
	bSizer5->Add( m_panelBasicInfo, 1, wxEXPAND | wxALL, 0 );
	
	m_panel2->SetSizer( bSizer5 );
	m_panel2->Layout();
	bSizer5->Fit( m_panel2 );
	m_flatNotebook1->AddPage( m_panel2, wxT("Basics"), true );
	
	bSizer1->Add( m_flatNotebook1, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer3->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button2, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
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
