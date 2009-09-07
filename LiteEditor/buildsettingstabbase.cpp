//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : buildsettingstabbase.cpp              
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
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "buildsettingstabbase.h"

///////////////////////////////////////////////////////////////////////////

BuildTabSettingsBase::BuildTabSettingsBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 4, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableCol( 2 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Background colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	m_staticText4->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Foreground colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	m_staticText5->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Font weight:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	m_staticText6->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Compiler errors colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_colourPickerError = new wxColourPickerCtrl( this, wxID_ANY, wxColour( 255, 98, 98 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_colourPickerError, 0, wxALL|wxEXPAND, 5 );
	
	m_colourPickerErrorFg = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_colourPickerErrorFg, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxBoldErrFont = new wxCheckBox( this, wxID_ANY, wxT("Bold"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxBoldErrFont->SetValue(true);
	
	fgSizer1->Add( m_checkBoxBoldErrFont, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Compiler warnings colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_colourPickerWarnings = new wxColourPickerCtrl( this, wxID_ANY, wxColour( 255, 255, 170 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_colourPickerWarnings, 0, wxALL|wxEXPAND, 5 );
	
	m_colourPickerWarningsFg = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_colourPickerWarningsFg, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxBoldWarnFont = new wxCheckBox( this, wxID_ANY, wxT("Bold"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer1->Add( m_checkBoxBoldWarnFont, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxALL|wxEXPAND, 5 );
	
    wxArrayString choices;
    choices.Add(wxT("When build starts"));
    choices.Add(wxT("When build ends"));
    choices.Add(wxT("Don't automatically show"));
    m_radioBoxShowBuildTab = new wxRadioBox(this, wxID_ANY, wxT("Show Build Pane"), wxDefaultPosition, wxDefaultSize,
                                            choices, 1);
    bSizer1->Add( m_radioBoxShowBuildTab, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxAutoHide = new wxCheckBox( this, wxID_ANY, wxT("Automatically hide the build pane when there are no errors nor warnings"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer1->Add( m_checkBoxAutoHide, 0, wxALL, 5 );
	
	m_checkBoxDisplayAnnotations = new wxCheckBox( this, wxID_ANY, wxT("Display compiler errors / warnings as annotations"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxSkipeWarnings = new wxCheckBox( this, wxID_ANY, wxT("When using the menu to jump to errors, skip warnings"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer1->Add( m_checkBoxDisplayAnnotations, 0, wxALL|wxEXPAND, 5 );
	bSizer1->Add( m_checkBoxSkipeWarnings, 0, wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

BuildTabSettingsBase::~BuildTabSettingsBase()
{
}
