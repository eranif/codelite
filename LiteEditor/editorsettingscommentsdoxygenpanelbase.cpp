//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editorsettingscommentsdoxygenpanelbase.cpp              
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

#include "editorsettingscommentsdoxygenpanelbase.h"

///////////////////////////////////////////////////////////////////////////

EditorSettingsCommentsDoxygenPanelBase::EditorSettingsCommentsDoxygenPanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxUseSlash2Stars = new wxCheckBox( this, wxID_ANY, wxT("Use '/**' as doxygen block start (else use '/*!')"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxUseSlash2Stars->SetValue(true);
	
	bSizer2->Add( m_checkBoxUseSlash2Stars, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxUseShtroodel = new wxCheckBox( this, wxID_ANY, wxT("Use '@' as doxygen keyword prefix"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer2->Add( m_checkBoxUseShtroodel, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer2->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("CodeLite contains a built-in doxygen documentation generator which\nadds doxygen comments to your code. Here you can set the prefix \nthat will be placed on top of the dynamic content of the comment:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer2->Add( m_staticText3, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer2->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Class prefix:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL, 5 );
	
	m_textCtrlClassPattern = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	fgSizer1->Add( m_textCtrlClassPattern, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Function prefix:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL, 5 );
	
	m_textCtrlFunctionPattern = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	fgSizer1->Add( m_textCtrlFunctionPattern, 1, wxALL|wxEXPAND, 5 );
	
	bSizer2->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("The following macros are available:\n$(CurrentFileName), $(CurrentFilePath), $(User), $(Date), $(Name)\n$(CurrentFileFullPath), $(CurrentFileExt), $(ProjectName), $(WorksapceName)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer2->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_checkBoxUseShtroodel->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( EditorSettingsCommentsDoxygenPanelBase::OnUseAtPrefix ), NULL, this );
}

EditorSettingsCommentsDoxygenPanelBase::~EditorSettingsCommentsDoxygenPanelBase()
{
	// Disconnect Events
	m_checkBoxUseShtroodel->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( EditorSettingsCommentsDoxygenPanelBase::OnUseAtPrefix ), NULL, this );
}
